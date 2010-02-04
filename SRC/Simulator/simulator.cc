//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information: Simulator object
//File Name: simulator.cc
//File Purpose: Event Control
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Spring 2002
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010

#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#else
#include <time.h>
#endif



#include <math.h>


#ifdef HAVE_PSINFO
// Added for PSC
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#endif




//#define DEBUG_MASK 0x10
#include "G_debug.h"
#include "simulator.h"
#include "timer.h"
#include "routing.h"
#include "node.h"
#include "G_globalstats.h"
#include "tcpdemux.h"
#include "udpdemux.h"
#include "ipv4.h"
//#include "link-rti.h"
//#include "application-tcpsend.h"
#include "memreuse.h"
//#include "backplane.h"
#include "stringtok.h"
#include "link-real.h"
#include "eventcq.h"
#include "node.h"
#include "routing-nixvector.h"
#include "interface.h" //v1.01
#include "protograph.h"
//#include "interface-interconnect-remote.h"

#ifdef CTRLFIFO

#include <sys/signal.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ctldefs.h"
#endif


#ifdef  HAVE_RTI
extern "C" {
#include "rticore.h"
#include "brti.h"
}
#endif


using namespace std;

#ifdef CTRLFIFO
string      Simulator::name = getenv("HOME");
int         Simulator::sockfd = 0;
#endif


Simulator* Simulator::instance;         // Points to single Simulator instance
PHVec_t    Simulator::progressHooks;    // List of callbacks on progress
RectRegion Simulator::boundingBox;      // Min/Max X/Y positions
string     Simulator::simName;          // Name of this sim

// Custom background callback
CustomBackground_t Simulator::pCustomBackground = nil;
NodeSelected_t     Simulator::pNodeSelected = nil;
bool       Simulator::recorderEnabled = false;
Mult_t     Simulator::recorderSpeedup = 10.0;

void ProgressTimer::Timeout(TimerEvent* ev)
{ // Process the progress timer expiration
  ProgressEvent* pe = (ProgressEvent*)ev;
  if  (Simulator::progressHooks.empty())
    { // No hooks, just print simple progress message
      cout << "Progress to " << Simulator::Now()  << endl;
    }
  else
    { // Call each hook
      for (PHVec_t::iterator i = Simulator::progressHooks.begin();
           i != Simulator::progressHooks.end(); ++i)
        {
          (*i)(Simulator::Now()); // Call the user progress function
        }
    }
  Time_t interval = pe->interval;
  Simulator::instance->progressEvent = nil;
  delete pe;
  // If only remaining event is progress, we are out of events and should exit
  // othersize reschedule the progress event
  Count_t evlSize = Scheduler::Instance()->EventListSize();
  bool empty = (evlSize == 0) ||
      (evlSize == 1 && Simulator::instance->haltEventSched);
  
  if (!empty || Simulator::instance->DistributedSim())
    { // Not empty (or distributed sim), schedule new one
      Simulator::instance->ProgressResched(interval);
    }
  else
    {
      if(!Simulator::instance->silent)
        cout << "Simulator halting, event list exhausted" << endl;
      Simulator::instance->Halt();
    }
}


Simulator::Simulator() : 
  verbose(false), verbosemod(1000), cleanUp(false), 
  lastTopologyChange(0),
  supercomputerInterconnect(false),
  simtime(0),
  //totevs(0), totrm(0), totevp(0), totevc(0),
  //evlSize(0), maxSize(0), 
  silent(false),
  halted(false), uid(0),
  progressInterval(0), serverEnabled(false), progressEvent(0),
  startRouteTime(0), startRunTime(0),
  //qtWin(nil), 
  animate(false), playbackAnimate(false), animateWirelessTx(false),
  animateBasebandTx(false), 
  basebandTxStart(false), 
  pauseWirelessTx(false),
  haltEventSched(false),
  smallestX(0), smallestY(0),
  largestX(0), largestY(0),
  firstLocation(true), hasMobility(false)
{
  instance = this;
  //  hint = eventList.end();
  //调用不同协议的构造方法
  TCPDemux::Initialize();
  UDPDemux::Initialize();

  Stats::initialMemory = ReportMemoryUsage();

#ifdef CTRLFIFO
  name.append ("/.gtsock");
  unlink(name.c_str());
  if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
    perror("socket");
  struct sockaddr_un saun;
  memset(&saun, 0, sizeof(saun));
  saun.sun_family = PF_UNIX;
  strcpy(saun.sun_path, name.c_str());

  if (bind (sockfd, (struct sockaddr*)&saun, sizeof(saun)) < 0)
    perror("bind");

#endif
  
}

Simulator::~Simulator()
{
  DEBUG0((cout << "Hello from Simulator Destructor" << endl));
  instance = nil;
  //eventlist的清理2010-2-2
  Scheduler::ClearEventlist();

  const NodeVec_t& nodes = Node::GetNodes();
  //if (cleanUp)
  //  {
      for (NodeVec_t::size_type i = 0; i < nodes.size(); ++i)
      {

          DEBUG0((nodes[i]->DumpNV())); //打印节点的调试信息
          delete nodes[i];
      }
      DEBUG0((ReuseBase::DumpQ())); //打印内存应用的调试信息
    //}
  Node::Clear(); //清理所有的节点
  //ProtocolGraph::ClearCommon();
  //mac类static变量的重新初始化
  MACAddr::next = 0;
  MACAddr::NONE = 0;
  //link类static变量的重新初始化
  Link::useSeqEvents = true;  // True 代表进入系统仿真事件列表


  Trace::Instance()->Close();
#ifdef CTRLFIFO
  unlink(name.c_str());
#endif
}

void Simulator::Handle(Event* e, Time_t t)
{
  SimulatorEvent* ev = (SimulatorEvent*)e;
  switch(e->event)
    {
      case SimulatorEvent::HALT :
        halted = true;
        if (!silent) cout <<  "Simulator halting at time " << t << endl;
        break;
      case SimulatorEvent::DELETE_OBJECT :
        delete ev->object; //删掉调用者指定的一个对象
        break;
      case SimulatorEvent::NOTIFY:
        ((NotifyHandler*)ev->object)->Notify(ev->c); //调用通知
        break;
      case SimulatorEvent::NODE_DOWN:
        ev->node->Down();
        break;
      case SimulatorEvent::NODE_UP:
        ev->node->Up();
        break;
      case SimulatorEvent::INTERFACE_DOWN:
        ev->iFace->Down();
        break;
      case SimulatorEvent::INTERFACE_UP:
        ev->iFace->Up();
        break;
      case SimulatorEvent::BASEBAND_TX:
        basebandTxStart = true;
        break;
    }
  delete e;
}
#ifdef MOVED_TO_SCHEDULER
void Simulator::Cancel(Event* p)
{
  EventMap_t::iterator i = eventList.find(KeyPair(p->Time(), p->uid));
  //EventSet_t::iterator i = eventList.find(p);
  if (i != eventList.end())
    {
      eventList.erase(i);
      //      hint = eventList.end(); 
      totrm++;
      totevc++;
      evlSize--;
    }
}

void Simulator::Schedule(Event* p)
{
  Schedule(p, p->Time() - Now());
}

void Simulator::Schedule(Event* p, Time_t t)
{
  Schedule(p, t, p->handler);
}

void Simulator::Schedule(Event* p, Time_t t, Handler* h)
{ // Schedule event for t secs in future
  p->uid = uid++;
  p->Time(t + Now());
  p->handler = h;
  //hint = eventList.insert(
  //           hint,
  //           EventMap_t::value_type(KeyPair(p->time, p->uid), p));
  eventList.insert(
             EventMap_t::value_type(KeyPair(p->Time(), p->uid), p));
  //eventList.insert(p);
  evlSize++;
  totevs++;  
  Count_t temp = evlSize;
  if (verbose && ((totevs % verbosemod) == 0))
    {
      cout << "STLSched::total " << totevs
           << " current size " << temp << endl;
    }
  if ( maxSize < temp)
     maxSize=temp;
}

void Simulator::Schedule(Event* p, Time_t t, Handler& h)
{
  Schedule(p, t, &h);
}

void Simulator::ScheduleEarly(Event* p, Time_t t, Handler* h)
{ // Schedule event for t secs in future, using STL hint
  p->uid = uid++;
  p->Time(t + Now());
  p->handler = h;
  // For now no difference from normal schedule
  eventList.insert(
             EventMap_t::value_type(KeyPair(p->Time(), p->uid), p));
  //eventList.insert(eventList.begin(), p);
  //eventList.insert(eventList.end(), p); // Just testing!!!!!!!!!!!
  evlSize++;
  totevs++;  
}
#endif

void Simulator::Progress(Time_t interval)
{
  if (progressEvent)
    {
      Scheduler::Cancel(progressEvent); // Cancel any pending
      delete progressEvent;  // And remove the event
      progressEvent = nil;
    }
  // Schedule new event for "interval" secs in future
  progressInterval = interval;
  ProgressResched(interval);
}

//安排node和interface的失效事件
SimulatorEvent* Simulator::NodeDownAt(Node* n, const Random& r)
{
  Random& r1 = (Random&)r;   //绕过const的小trick
  Time_t t = r1.Value();
  if (t < Now()) return nil; //不能在以前的时间处理
  SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::NODE_DOWN);
  e->node = n;
  Scheduler::Schedule(e, t - Now(), this);
  return e;
}

SimulatorEvent* Simulator::NodeUpAt(Node* n, const Random& r)
{
  Random& r1 = (Random&)r;   
  Time_t t = r1.Value();
  if (t < Now()) return nil; 
  SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::NODE_UP);
  e->node = n;
  Scheduler::Schedule(e, t - Now(), this);
  return e;
}


SimulatorEvent* Simulator::InterfaceDownAt(Interface* i, const Random& r)
{
  Random& r1 = (Random&)r;   
  Time_t t = r1.Value();
  if (t < Now()) return nil;
  SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::INTERFACE_DOWN);
  e->iFace = i;
  Scheduler::Schedule(e, t - Now(), this);
  return e;
}


SimulatorEvent* Simulator::InterfaceUpAt(Interface* i, const Random& r)
{
  Random& r1 = (Random&)r;   
  Time_t t = r1.Value();
  if (t < Now()) return nil; 
  SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::INTERFACE_UP);
  e->iFace = i;
  Scheduler::Schedule(e, t - Now(), this);
  return e;
}

#ifdef MOVED_TO_SCHEDULER
Event* Simulator::DeQueue()		// next event (removes from q)
{
  EventMap_t::iterator i = eventList.begin();
  //EventSet_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; // Empty on deque
  Event* p = i->second;
  //Event* p = *i;
  eventList.erase(i);
  //  hint = eventList.end();
  totrm++;
  evlSize--;
  return p;
}

Event* Simulator::PeekEvent()
{ // Return next event without removal
  EventMap_t::iterator i = eventList.begin();
  //EventSet_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; // Empty on deque
  return eventList.begin()->second;
  //return *(eventList.begin());
}
#endif

void Simulator::PrintStats()
{
  if(!silent)
    {
      Scheduler* sch = Scheduler::Instance();
      cout << "Simulator::PrintStats, totevs " << sch->totevs
           << " totrm " << sch->totrm 
           << " totevp " << sch->totevp 
           << " totevc " << sch->totevc 
           << " size " << sch->EventListSize()
           << " maxsize " << sch->maxSize
           << endl;
    }
  
}

void Simulator::CommonRunInit()
{ 
#ifdef CTRLFIFO
  fcntl(sockfd, F_SETOWN, getpid());
  toUnblock(sockfd);
  signal(SIGIO, &Simulator::io_signal_handler);
  listen(sockfd, 2);
#endif
  
  halted = false;
  Stats::modelMemory = ReportMemoryUsage();
  startRouteTime = GetSec(); //记录路由计算之前的CPU时间
  DEBUG0((cout << "Simulator::Run, needInit() " 
          << Routing::Default()->NeedInit() << endl));
  if (Routing::Default()->NeedInit())
    { //需要在每个节点上初始化路由信息
      const NodeVec_t& n = Node::GetNodes(); //取得全局的nodes信息
      for (NodeVec_t::size_type i = 0; i < n.size(); ++i)
        { //调用所有结点的初始化
          n[i]->InitializeRoutes();
        }
    }
  DEBUG0((cout << "Done initializing routes..." << endl));
  Stats::routingMemory = ReportMemoryUsage();
  startRunTime = GetSec(); //记录Run函数执行前的CPU时间
}

  
void Simulator::Run()
{
  CommonRunInit();
  	int test=0;
  while(!halted)
    {
      test++;

      Event* e = Scheduler::DeQueue();
      if (!e) break; //没有事件
      if (e->handler)
        {
          if (e->Time() < simtime) 
            cout << "HuH? Time backing up from " << simtime
                 << " to " << e->Time() << endl;
          simtime = e->Time();
          Scheduler::Instance()->totevp++;
          DEBUG(4,(cout << "Handling ev at time " << e->Time() << endl));
          e->handler->Handle(e, e->Time()); //执行事件
        }
      else
        {
          if (!silent)
            {
              cout << "Skipping event " << e->uid
                   << "  at time " << e->Time()
                   << ", no handler " << endl;
            }
        }
    }
  DEBUG0((cout << "Simulator::Run exit event loop" << endl));
  if (!Scheduler::Instance()->EventListSize() && !halted)
    if(!silent)cout << "Simulator::Run, event list exhausted" << endl;
  Stats::finalMemory = ReportMemoryUsage();
}

void Simulator::StopAt(Time_t t)
{
SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::HALT);

 //安排一个终止事件
 Scheduler::Schedule(e, t, this);
 haltEventSched = true;
}

void Simulator::Halt()
{
	halted = true;
}

void Simulator::Name(const string& n)
{
  simName = n;
}


void Simulator::DeleteObject(Object* o)
//C++中，对象不能删除自身，所以我们定义了一个接口，使得一些对象（如协议的对象
//）可以通过这个接口调用Simulator的方法来删除自身
{
  SimulatorEvent* ev = new SimulatorEvent(SimulatorEvent::DELETE_OBJECT);
  ev->object = o;
  Scheduler::Schedule(ev, 0.0, this); //安排在当前时间执行
}

void Simulator::ProgressHook(ProgressHook_t ph)
{ // Add a new progress callback
  progressHooks.push_back(ph);
}

void Simulator::AddNotify(NotifyHandler* n, Time_t t, void* c)
//在未来的指定时间安排一个通知事件
{ 
  SimulatorEvent* ev = new SimulatorEvent(SimulatorEvent::NOTIFY);
  ev->object = (Object*)n;
  ev->c = c;
  Scheduler::Schedule(ev, t, this);
}

void Simulator::TopologyChanged(bool u)
//记录最近拓扑变更的时间
{ 
  lastTopologyChange = Now();
  //通知路由对象这些变更
  Routing::Default()->TopologyChanged(u);
}

bool Simulator::IsScheduled(Event* p )
{ // Shouldn't this be removed?
  return Scheduler::Instance()->IsScheduled(p);
}

void Simulator::DisplayTopology( )
{
	gui_ctrl::qtWin->DisplayTopology();
}

void Simulator::UpdateTopology( )
{
    gui_ctrl::qtWin->UpdateTopology();

}

void Simulator::StartAnimation(Time_t when,bool initialPause, bool fullScreen)
{

  animate = true;
  gui_ctrl::qtWin->DisplayTopology();

  QTEvent* ev;
  if (initialPause)
    {
      ev = new QTEvent(QTEvent::PAUSE);
    }
  else
    {
      ev = new QTEvent(QTEvent::START);
    }
  Scheduler::Schedule(ev, when - Simulator::Now(), gui_ctrl::qtWin);

}

void Simulator::StopAnimation(Time_t when)
{
  QTEvent* ev = new QTEvent(QTEvent::STOP);
  Scheduler::Schedule(ev, when - Simulator::Now(), gui_ctrl::qtWin);
}

void Simulator::PauseAnimation(Time_t when)
{
  QTEvent* ev = new QTEvent(QTEvent::PAUSE);
  Scheduler::Schedule(ev, when - Simulator::Now(),gui_ctrl::qtWin);
}

void Simulator::AnimationUpdateInterval(Time_t t)
//设置初始的动画更新频率
{ 

  gui_ctrl::qtWin->AnimationUpdateRate(t);
}

void Simulator::AnimateWirelessTx(bool a)
{
  animateWirelessTx = a;
}

bool Simulator::AnimateWirelessTx()
{
  if (!animate) return false; // False if no animation selected
  return animateWirelessTx;
}

void Simulator::AnimateBasebandTx(Time_t when, bool a)
{
#ifdef HAVE_QT
  animateBasebandTx = a;
  if(animate && animateBasebandTx) {
  	// Schedule the Tx animation event
  	SimulatorEvent* ev;
  	ev = new SimulatorEvent(SimulatorEvent::BASEBAND_TX);
  	Scheduler::Schedule(ev, when - Simulator::Now(), this);
  }
#endif
}

bool Simulator::AnimateBasebandTx()
{
  if (!animate) return false; // False if no animation selected
  return animateBasebandTx;
}

bool Simulator::BasebandTxStart()
{
  if ((!animate)||(!animateBasebandTx)) return false; // False if no animation selected
  return basebandTxStart;
}

bool Simulator::AddBackgroundMap(const string& mapName, const RectRegion& r)
{ // Create a one entry vector and use the overloaded AddBackgroundMap function
#ifdef HAVE_QT
  StringVec_t sv;
  sv.push_back(mapName);
  return AddBackgroundMap(sv, r);
#endif
#ifndef HAVE_QT
  return false;//v1.01
#endif
}

bool Simulator::AddBackgroundMap(const StringVec_t& maps, const RectRegion& r)
{
#ifdef HAVE_QT
  LocationVecVec_t locVecVec;
  for (StringVec_t::size_type i = 0; i < maps.size(); ++i)
    {
      //ProcessOneMap(maps[i], r, locVecVec, minX, maxX, minY, maxY);
      ProcessOneMap(maps[i], r, locVecVec);
    }
  
  boundingBox = FindBoundingBox(locVecVec);
  
  // Now we adjust the longitude value by cos(latitude) for correct
  // projections on the map.  This is done by taking a centerline
  // of the bounding box, and adjusting the offsets from the centerline
  // by cos(lat).
  Meters_t centerLine = boundingBox.LowerLeft().X() + 
      (boundingBox.UpperRight().X() - boundingBox.LowerLeft().X()) / 2;
  
  for (LocationVecVec_t::size_type i = 0; i < locVecVec.size(); ++i)
    {
      LocationVec_t& lv = locVecVec[i];
      for (LocationVec_t::size_type j = 0; j < lv.size(); ++j)
        {
          Meters_t x = lv[j].X();
          Meters_t y = lv[j].Y();
          Meters_t delta = x - centerLine;
          delta *= cos(y * DegreesToRadians);
          DEBUG0((cout << "orig x " << x 
                  << " y " << y
                  << " cl " << centerLine
                  << " adjusted x " << delta + centerLine << endl));
          lv[j].X(centerLine + delta);
        }
    }
  
  // Bounding box has changed, so find it again
  boundingBox = FindBoundingBox(locVecVec);

  // Specify min/max and then display the lines
  NewLocation(boundingBox.LowerLeft());
  NewLocation(boundingBox.UpperRight());
  
  if (!silent)
    {
      cout << "minX "  << boundingBox.LowerLeft().X()
           << " minY " << boundingBox.LowerLeft().Y()
           << " maxX " << boundingBox.UpperRight().X()
           << " maxY " << boundingBox.UpperRight().Y()
           << " locVecVec.size " << locVecVec.size() 
           << endl;
    }
  for (Count_t i = 0; i < locVecVec.size(); ++i)
    {
      gui_ctrl::qtWin->AddBackgroundLines(locVecVec[i]);
    }
  return true;
#else
  return false;
#endif
}

bool Simulator::PlaybackTraceFile(const char* fn)
{  // Specify the name of the playback trace file
  //  This should be called before the topology is
  // created, since it sets the Interface Mac to IF logging
  // which is neede by the trace file playback
#ifdef HAVE_QT
  playbackFile.open(fn, ios::in);
  if (!playbackFile)
    {
      cout << "Can't open playback trace file " << fn << endl;
      return false;
    }
  playbackAnimate = true;
  Interface::recordMacAddresses = true;
  return true;
#else
  return false;
#endif
}

// Definitions for Playback mode
bool    Simulator::StartPlayback()
{
#ifdef HAVE_QT
  simtime = 0.0;
  Time_t msPerFrame = 1000/24; // 24 fps

  if (!playbackAnimate)
    {
      cout << "No playback file specified, can't start playback" << endl;
      return false;
    }
  DisplayTopology( );
  gui_ctrl::qtWin->PlaybackMode(true); // Notify qtwindow we are in playback mode
  gui_ctrl::qtWin->PlaybackPause();

  while(!gui_ctrl::qtWin->HasQuit())
    {
      FillEventsFromTrace(simtime + 0.1);
	  gui_ctrl::qtWin->UpdateTopology(false);
	  simtime += gui_ctrl::qtWin->PlaybackRate() * (double)msPerFrame / 1000.0;
      DEBUG0((cout << "Simtime updated to " << simtime << endl));
    }
  return true;
#else
  return false;
#endif
}

void    Simulator::CustomBackground(CustomBackground_t cb)
{
#ifdef HAVE_QT
  pCustomBackground = cb;
#endif
}

void    Simulator::NodeSelectedCallback(NodeSelected_t ns)
{
  pNodeSelected = ns;
}

void    Simulator::EnableAnimationRecorder(bool e)
{
  recorderEnabled = e;
}

void    Simulator::RecorderMPEGSpeedup(Mult_t sp)
{
  recorderSpeedup = sp;
}

void    Simulator::EnableHTTPServer()
{
  // not implemented yet.
}

void    Simulator::FillEventsFromTrace(Time_t t)
{ // Read the trace file to the first event with time > t
  // and populate the pending events list
#ifdef HAVE_QT
  while(playbackFile)
    {
      string s;

      getline(playbackFile, s);
      DEBUG0((cout << "Playback line " << s << endl));
      vector<string> tokens;
      stringtok(tokens, s, " "); // Break into tokens
      if (tokens.size() < 2) continue; // ? empty ?
      Time_t thisTime = atof(tokens[0].c_str());
      string node = tokens[1];
      for (vector<string>::size_type i = 2; i < tokens.size(); ++i)
        {
          if (tokens[i].find("L2") != string::npos)
            {
              DEBUG0((cout << "Found l2, i " << i
                      << " token " << tokens[i] << endl));
              Count_t size = atol(tokens[i+1].c_str());
              MACAddr_t src = atol(tokens[i+2].c_str());
              MACAddr_t dst = atol(tokens[i+3].c_str());
              //Count_t            uid = atol(tokens[i+4].c_str());
              bool isRx;
              Interface* iface;
              if (tokens[i][0] == '+')
                {  // This is a rx
                  iface = Interface::GetByMac(dst);
                  isRx = true;
                  if (!iface)
                    cout << "HuH? Can't find iface for mac " << dst << endl;
                  else if(iface->GetMACAddr() != MACAddr(dst))
                    cout << "HuH?  MAC Mismatch, dst " << dst
                         << " ifmac " << iface->GetMACAddr() << endl;
                }
              else
                {
                  iface = Interface::GetByMac(src);
                  isRx = false;
                }
              if (iface)
                { // Interface exists, process event if target node displayed
                  bool nodeDisplayed = false;
                  Node* n = iface->GetNode();
                  if (n) nodeDisplayed = n->Show();
                  if (isRx && nodeDisplayed && thisTime >= Now())
                    { // Put in pending rx queue if node displayed
                      // and not in the past
                      Packet* p = new Packet();
                      p->size = size; // Set size
                      LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p);
                      iface->ScheduleRx(evRx, thisTime);
                      DEBUG0((cout << "Scheduling future rx event time "
                              << thisTime
                              << " iface " << iface << endl));
                    }
                }
              i += 3;
            }

        }
      if (thisTime > t) return;
    }
#endif
}


// Statistics
Time_t  Simulator::SetupTime()
{ // Wall clock time for initialization
  return startRouteTime; // This is CPU time used before the "Run" was called
}

Time_t Simulator::RouteTime()
{ // Time to calculate routes
  return startRunTime - startRouteTime;
}

Time_t  Simulator::RunTime()
{// CPU clock time for Run()
  return GetSec() - startRunTime;
}

Time_t  Simulator::TotalTime()
{ // Total time of execution
  return RunTime() + RouteTime() + SetupTime();
}

#ifdef HAVE_PSINFO
static int pagesize = 0;
#endif


Size_t  Simulator::ReportMemoryUsage()
{
#ifdef WIN32
	return 0;
#else

  pid_t  pid;
  char   work[4096];
  FILE*  f;
  char*  pCh;

  pid = getpid();
  sprintf(work, "/proc/%d/stat", (int)pid);
  f = fopen(work, "r");
  if (f == NULL)
    {
#ifdef HAVE_PSINFO
      DEBUG(0,(cout << "Can't open " << work 
               <<" trying ioctl method" << endl));
      if (!pagesize)
        {
          pagesize = getpagesize();
        }
      char w[255];
      sprintf(w, "/proc/%d", pid);
      int p = open(w, O_RDONLY, 0);
      if (p < 0)
        {
          DEBUG(0,(cout << "Huh? Can't open "
                   << string(w) << endl));
          return(0);
        }
      struct prpsinfo prps;
      int i = ioctl(p, PIOCPSINFO, &prps);
      if (i)
        {
          DEBUG(0,(cout << "HuH? Can't get ioctl in memusage" << endl));
          return(0);
        }
      return(prps.pr_rssize * pagesize);
#else
      DEBUG(0,(cout << "Can't open " << work << endl));
#endif
      return(0);
    }
  fgets(work, sizeof(work), f);
  fclose(f);
  strtok(work, " ");
  for (int i = 1; i < 23; i++) 
    {
      pCh = strtok(NULL, " ");
    }
  //return(atoll(pCh));
  return(atol(pCh));

#endif // WIN32
}


#ifdef OLD_ONE
Size_t  Simulator::ReportMemoryUsage()
{
  pid_t  pid;
  char   work[4096];
  FILE*  f;
  char*  pCh;

  pid = getpid();
  sprintf(work, "/proc/%d/stat", (int)pid);
  f = fopen(work, "r");
  if (f == NULL)
    {
      cout << "Can't open " << work << endl;
      return(0);    
    }
  fgets(work, sizeof(work), f);
  fclose(f);
  strtok(work, " ");
  for (int i = 1; i < 23; i++) 
    {
      pCh = strtok(NULL, " ");
    }
  return(atol(pCh));
}
#endif

Size_t Simulator::ReportMemoryUsageMB()
{ // Returns usage in MB!
  long long u = ReportMemoryUsage();
  return((u + 500000) / 1000000 );
}

// Grid Location Management
void Simulator::AmimationRegion(const Location& ll, const Location& ur)
{
  NewLocation(ll);
  NewLocation(ur);
}

void Simulator::NewLocation(Meters_t x, Meters_t y)
{
  if (firstLocation)
    {
      smallestX = x;
      smallestY = y;
      largestX  = x;
      largestY  = y;
      firstLocation = false;
      return;
    }
  smallestX = min(smallestX, x);
  smallestY = min(smallestY, y);
  largestX  = max(largestX, x);
  largestY  = max(largestY, y);
}

void Simulator::NewLocation(const Location& l)
{
  NewLocation(l.X(), l.Y());
}

Meters_t  Simulator::SmallestX()
{
  return smallestX;
}

Meters_t  Simulator::SmallestY()
{
  return smallestY;
}

Meters_t  Simulator::LargestX()
{
  return largestX;
}

Meters_t  Simulator::LargestY()
{
  return largestY;
}

// Private Methods
void Simulator::ProgressResched(Time_t interval)
{
  progressEvent = new ProgressEvent(interval);;
  progressEvent->event = 0; // ! HuH? why do I need this..
  progressTimer.Schedule(progressEvent, interval);
}

Time_t Simulator::GetSec()
{ 
#ifndef WIN32	
  // Get cpu usage time, rounded to seconds
  struct rusage u;
  getrusage (RUSAGE_SELF, &u);
  Time_t us = u.ru_utime.tv_usec + u.ru_stime.tv_usec;
  Time_t s  = u.ru_utime.tv_sec + u.ru_stime.tv_sec;
  return s + us / 1000000.0;
#else
	return 0;
#endif
}

bool  Simulator::ProcessOneMap(string n, const RectRegion& r,
                               LocationVecVec_t& lv)
{
  ifstream is(n.c_str());
  if (!is)
    {
      cout << "Can't open map file " << n << endl;
      return false;
    }
  LocationVec_t locs;

  // Keep a vector of the vectors.  We need to find then
  // entire set of points to set the min/max points
  // for meters to pixels conversions.  So we first
  // build the entire set, set the min/max points,
  // then display the lines.
  string s;
  getline(is, s);
  vector<string> tokens;

  while(is)
    {
      int lineCount = 0;
      int rank = 1;
      // process the header line
      DEBUG0((cout << "Hdr line is " << s << endl));
      tokens.clear();
      stringtok(tokens, s);
      
      if (s.find("segment")  != string::npos)
        { // CIA World Databank format
          lineCount = atol(tokens[5].c_str());
          rank = atol(tokens[3].c_str());
          DEBUG0((cout << "CIA lineCount " << lineCount
                  << " rank " << rank << endl));
        }
      else
        { // Simple format, just a line count on first line
          if (tokens.size() != 1)
            {
              cout <<  "Unrecognized map file format" << endl;
              return false;
            }
          lineCount = atol(tokens[0].c_str());
        }
      bool skipAll = false;
  
      for (int i = 0; i < lineCount; ++i)
        {
          double lat;
          double lon;
          getline(is, s);
          tokens.clear();
          stringtok(tokens, s);
          if (tokens.size() != 2)
            {
              cout <<  "Unrecognized map file format, token size" 
                   << tokens.size() << endl;
              return false;
            }
          lat = atof(tokens[0].c_str());
          lon = atof(tokens[1].c_str());
          if (!r.Empty() && !r.InRegion(Location(lon, lat)))
            { // Not in bounding rect, terminate this polyline
              skipAll = true;
            }
          if (lat == 0 && lon == 0)
            { // Discontinuous,  save and clear the vector
              lv.push_back(locs);
              DEBUG0((cout << "size " << locs.size() << endl));
              locs.clear();
            }
          else
            {
              // Convert to meters from the 0,0 location
              // (equator, zero meridian)
              //lon *= (DegreesToMeters / cos(lat * DegreesToRadians));
              // Need a better adjustment for longitude..code later
              //lon *= DegreesToMeters;
              //lat *= DegreesToMeters;
              if (rank == 1 && !skipAll)
                { // Only process rank 1 entries from CIA Database
                  //mnx = min(lon, mnx);
                  //mny = min(lat, mny);
                  //mxx = max(lon, mxx);
                  //mxy = max(lat, mxy);
              
                  DEBUG0((cout << "Lat " << lat << " lon  " << lon << endl));
                  locs.push_back(Location(lon, lat));
                }
            }
        }
      
      if (skipAll) locs.clear();
      if (locs.size()) lv.push_back(locs);
      locs.clear();
      if(is)getline(is, s); // Get Next Header Line
      if (s.length() == 0) break; // allow for empty last line
    }
  is.close();
  return true;
}

RectRegion Simulator::FindBoundingBox(LocationVecVec_t& lvv)
{
  Meters_t minX = 1e307;
  Meters_t minY = 1e307;
  Meters_t maxX = -1e307;
  Meters_t maxY = -1e307;
  for (LocationVecVec_t::size_type i = 0; i < lvv.size(); ++i)
    {
      LocationVec_t& lv = lvv[i];
      for (LocationVec_t::size_type j = 0; j < lv.size(); ++j)
        {
          Meters_t x = lv[j].X();
          Meters_t y = lv[j].Y();
          minX = min(x, minX);
          minY = min(y, minY);
          maxX = max(x, maxX);
          maxY = max(y, maxY);
        }
    }
  return RectRegion(Location(minX, minY), Location(maxX, maxY));
}

// Static Methods
Time_t Simulator::Now()
{
  if (!instance) return(0.0);  // Simulation has not yet started
  return instance->simtime;
}

#ifdef CTRLFIFO


void Simulator::toBlock(int sockfd)
{
  fcntl (sockfd, F_SETFL,
         fcntl(sockfd, F_GETFL) & (~(O_ASYNC  | O_NONBLOCK)));
  return;
}

void Simulator::toUnblock(int sockfd)
{
  fcntl (sockfd, F_SETFL,
         fcntl(sockfd, F_GETFL) | O_ASYNC | O_NONBLOCK);
  return;
}

void Simulator::io_signal_handler(int sig)
{
  char buffer;
  bool paused = false;

  printf("Got a signal\n");

  while (1) {
    if (read(sockfd, &buffer, 1) > 0)
      printf("Read %x \n", buffer);
    switch(buffer) {
    case GT_PAUSE:
      if (paused) {
        cout << "Received Resume" << endl;
        signal(SIGIO, &Simulator::io_signal_handler);
        toUnblock(sockfd);
        return;
      } else {
        cout << "Received Pause" << endl;
        paused = true;
        toBlock(sockfd);
      }
      break;
    case GT_DUMP:
      cout << "Received Dump" << endl;
      return;
    case GT_DEBUG:
      cout << "Received Debug" << endl;
      return;
    case GT_STOP:
      cout << "Received Stop" << endl;
      return;
    default:
      cout << "Unknown Code" << endl;
      return;
    }
  }
  return;
}

#endif

