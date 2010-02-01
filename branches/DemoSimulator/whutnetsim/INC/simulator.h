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


//File Information: Simulator class
//File Name: simulator.h
//File Purpose: Event Control
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Spring 2002
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010

#ifndef __simulator_h__
#define __simulator_h__

#include <iostream>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>
#include "qtwindow.h"
//#define	CTRLFIFO true

#include "G_common_defs.h"
#include "GUI_Defs.h"
#include "object.h"
#include "handler.h"
#include "event.h"
#include "timer.h"
#include "location.h"
#include "rng.h"
// For testing, include scheduler.h..remove later //delete
#include "scheduler.h" //delete
using namespace std;
class Interface;
class Node;
class NotifyHandler;
class QTWindow;

// 定义一个动画背景的回调
typedef void(*CustomBackground_t)(MyCanvas*);

typedef void (*ProgressHook_t)(Time_t);
typedef std::vector<ProgressHook_t> PHVec_t;  // 进度回调列表
typedef std::vector<std::string> StringVec_t; // String列表


// 对动画中鼠标选择节点动作定义一个回调
typedef void (*NodeSelected_t)(Node*);

const double DegreesToMeters = 111319.5; // 度数到长度的转化
const double DegreesToRadians = (2.0 * M_PI) / 360.0; //度数到弧度的转化

/*
Simulator事件
作用：指定了Simulator对象处理事件的方法
*/
class SimulatorEvent : public Event {
public:
  typedef enum { HALT,           //仿真停止
	            DELETE_OBJECT,   //删除绑定对象
				  NOTIFY,         //通知
                NODE_DOWN,      //使节点失效
				  NODE_UP,        //恢复节点
				  BASEBAND_TX,    //
                INTERFACE_DOWN,  //使interface失效
				  INTERFACE_UP    //恢复interface
  } SimulatorEvent_t;
  SimulatorEvent() 
      : object(nil), c(nil), node(nil), iFace(nil) { }
  SimulatorEvent(Event_t ev) 
      : Event(ev), object(nil), c(nil), node(nil), iFace(nil) { }
  virtual ~SimulatorEvent() { }
public:
  Object*    object;
  void*      c;
  Node*      node;   
  Interface* iFace;
};
/*
事件组
作用：将事件和事件触发时间绑定
*/
class EventPair {  
public:
  EventPair() : time(0.0), event(nil) { }
  EventPair(Time_t t, Event* e) : time(t), event(e) { }
  //添加eventpair类析构函数
  ~EventPair() { delete event;}
public:
  Time_t time;
  Event* event;
};

#ifndef __scheduler_h__
class KeyPair {
public:
  KeyPair(Time_t t, SimulatorUid_t u) : time(t), uid(u) { }
public:
  Time_t         time;
  SimulatorUid_t uid;
};

// Definitions for event list managemet
typedef std::pair<Time_t,SimulatorUid_t> KeyPair_t; // The key for the map
//typedef std::pair<Time_t,Event*> EventPair_t;       // For non-sorted ev lists (obsolete)
class key_less
{
public:
  key_less() { }
  bool operator()(const KeyPair& l, const KeyPair& r) const {
    return (l.time < r.time ||
            l.time == r.time && l.uid < r.uid);
    //bool operator()(const KeyPair& l, const KeyPair& r) const {
    //if (l.time > r.time) return false;
    //if (l.time < r.time) return true;
    //return (l.uid < r.uid);
  }
};

class event_less
{
public:
  event_less() { }
  inline bool operator()(Event* const & l, const Event* const & r) const {
    return (l->Time() < r->Time() ||
            l->Time() == r->Time() && l->uid < r->uid);
  }
};

// Define a sorted event list (EventMap_t) and fifo list (EventDeq_t)
//typedef std::map<KeyPair, Event*, std::less<KeyPair> > EventMap_t;
#endif
typedef std::map<KeyPair, Event*, key_less> EventMap_t;
//typedef std::set<Event*, event_less>        EventSet_t;
typedef std::list<EventPair >               EventList_t;

class ProgressTimer : public Timer {
public:
  virtual void Timeout(TimerEvent*);  // 重载Timeout方法
};

class ProgressEvent : public TimerEvent {
public:
  ProgressEvent(Time_t i) 
    : TimerEvent(), interval(i) { }
public:
  Time_t   interval;
};

class Simulator : public Handler {
  //所有GTNS仿真都需要Simulator类的对象。当仿真开始执行时，这个Simulator的对象就控制着
  //所有的动作。每个GTNS仿真都需要声明一个Simulator对象，它通常作为主函数里的本地变量出现。
public:
  friend class ProgressTimer; // 让Timer重新调度progress
  Simulator();//除非使用分布式仿真，否则Simulator的构造函数没有参数

  virtual ~Simulator();
public:   
  virtual void Handle(Event*, Time_t);//Simulator类是Handler类的子类，因为Simulator的对象必须要处理事件，尤其是进度和停止事件

#ifdef MOVED_TO_SCHEDULER
  //Doc:Method
  virtual void Cancel(Event*);	     // cancel event
    //Doc:Desc Cancels a pending event.
    //Doc:Arg1 A pointer to the event to be canceled.  When scheduling events
    //Doc:Arg1 that will later be canceled, a pointer to the event must
    //Doc:Arg1 be retained to allow cancellation.

  //Doc:Method
  virtual void Schedule(Event*);     // schedule event
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event must be stored in the event class, member
    //Doc:Desc {\tt time}, by the caller prior to calling Schedule.  The
    //Doc:Desc handler for the event is caller, which must be a subclass
    //Doc:Desc of  {\tt Handler}
    //Doc:Arg1 A pointer to the event being scheduled.

  //Doc:Method
  virtual void Schedule(Event*, Time_t);  // schedule event
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The
    //Doc:Desc handler for the event must be already specified
    //Doc:Desc in the passed {\tt Event}.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.

  //Doc:Method
  virtual void Schedule(Event*, Time_t, Handler*);
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The 
    //Doc:Desc handler for the event is also specified in the
    //Doc:Desc argument list.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.
    //Doc:Arg3 A pointer to any object that is a subclass of class
    //Doc:Arg3 {\tt Handler}, specifying the event handler for this event.

  //Doc:Method
  virtual void Schedule(Event*, Time_t, Handler&);
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The 
    //Doc:Desc handler for the event is also specified in the
    //Doc:Desc argument list.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.
    //Doc:Arg3 A reference to any object that is a subclass of class
    //Doc:Arg3 {\tt Handler}, specifying the event handler for this event.

  // Schedule an "early" event (use STL hint at beginning of queue)
  // Not documented in manual at this point.
  virtual void ScheduleEarly(Event*, Time_t, Handler*);
#endif
  Scheduler* SetScheduler(const Scheduler&); //没有实现
  virtual void Progress(Time_t);//定义请求一条进度信息并在标准输出中打印出来
  virtual SimulatorEvent* NodeDownAt(Node*, const Random&);//令一个节点在特定时间的时候失效
  virtual SimulatorEvent* NodeUpAt(Node*, const Random&);//令一个节点在特定时间的时候恢复
  virtual SimulatorEvent* InterfaceDownAt(Interface*, const Random&);//令一个interface在特定时间的时候失效
  virtual SimulatorEvent* InterfaceUpAt(Interface*, const Random&);//令一个interface在特定时间的时候恢复
#ifdef MOVED_TO_SCHEDULER
  //Doc:Method
  virtual Event* DeQueue();	     // get next event
    //Doc:Desc Removes the earliest pending event from the event queue
    //Doc:Desc and returns a pointer to it.
    //Doc:Return A pointer to the earliest pending event in the
    //Doc:Return event queue.

  //Doc:Method
  virtual Event* PeekEvent();        // Peek at next event (do not remove)
    //Doc:Desc   Returns a pointer to the earliest pending event in the event
    //Doc:Desc   queue, but does not remove it from the queue.
    //Doc:Return A pointer to the earliest pending event in the
    //Doc:Return event queue.
#endif

  void    PrintStats();//打印一些仿真中的细节信息，通常在Run方法退出，仿真结束的时候调用
  void CommonRunInit();// Run()之前的公共代码
  virtual bool DistributedSim()  {return false; }
  virtual void Run(); //启动仿真
  void    StopAt(Time_t t);//在一个特定时间设定一个Stop事件，这个事件可以使Run方法在这个特定时间退出
  void    Halt();//立即执行Stop事件并会导致Run方法退出
  void    Silent(bool s) { silent = s; }//设置静默模式的开启或关闭，当开启时，Simulator不会在标准输出中打印任何信息
  void    Name(const std::string&);//对这次仿真指定一个名字，被http网页interface调用来给simulation编号
  void    DeleteObject(Object*);//在特定的仿真时间设定一个事件删除一个指定的对象
  void    ProgressHook(ProgressHook_t);//每当一个进展事件被处理的时候指定调用一个子程序
  void    AddNotify(NotifyHandler*, Time_t, void*);//在未来的指定时间调用一个通知

  //CleanupOnExit是当Simulator对象被销毁时删除所有结点的，只在内存泄露的debug中使用
  void    CleanupOnExit(bool c = true) { cleanUp = c;}//指明了当Simulator对象被销毁时需要清理掉所有已开辟的内存对象。
  void TopologyChanged(bool);//通知仿真器拓扑改变事件已经发生
  bool    IsScheduled(Event*);// Debug用，来检查event是否已经放入事件队列

  // QTWindow接口
  void    DisplayTopology( );//打开一个动画显示窗口并可视化的显示拓扑
  //void    DisplayTopologyAndReturn(); //打开一个动画显示窗口并可视化的显示拓扑，立即返回
  void    UpdateTopology();//使一个已经存在的动画窗口更新所有的动画对象的状态
  void    StartAnimation(Time_t,bool = true, bool = false);// 在一个特定时间开始动画的演示
  void    StopAnimation(Time_t);//在一个特定时间停止动画演示并关闭动画窗口
  void    PauseAnimation(Time_t);//在一个特定时间暂停动画演示
  void    AnimationUpdateInterval(Time_t);//为动画演示初始化更新频率。更新频率可以在动画窗口的控制条上进行调节。

  //静态方法
  void    AnimateWirelessTx(bool a);
    //Doc:Desc Specify  detailed  animiation of wireless transmissions
    //Doc:Arg1 true if wireless animation desired

  //Doc:Method
  bool    AnimateWirelessTx();
    //Doc:Desc Determine  if wireless transmit animation selected
    //Doc:Return true if wireless animation selected.

  //Doc:Method
  void    AnimateBasebandTx(Time_t when, bool a);
    //Doc:Desc Specify  detailed  animiation of Bluetoth transmissions
    //Doc:Arg1 time to start Bluetooth Tx animation
    //Doc:Arg2 true if Bluetooth animation desired

  //Doc:Method
  bool    AnimateBasebandTx();
    //Doc:Desc Determine  if Bluetooth transmit animation selected
    //Doc:Return true if Bluetooth animation selected.
	
  //Doc:Method
  bool    BasebandTxStart();
    //Doc:Desc Determine  if Bluetooth transmit animation started
    //Doc:Return true if Bluetooth animation started.

  //Doc:Method
  void    PauseOnWirelessTx(bool p) { pauseWirelessTx = p;}
    //Doc:Desc The wireless transmit animations are usually fast,
    //Doc:Desc and can easily
    //Doc:Desc be missed.  Setting TRUE will pause the animation on each
    //Doc:Desc transmit, to allow a pause--based stepping through
    //Doc:Desc the tranmissions for debugging.
    //Doc:Arg1 TRUE if pause before each wireless transmit is desired.
  
  //Doc:Method
  bool    PauseOnWirelessTx() { return pauseWirelessTx; }
    //Doc:Desc Query of pauseOnWirelessTransmit is set.  Used by qtwindow.cc.
    //Doc:Return TRUE if pause on wireless transmit set.
  
  //Doc:Method
  bool    AddBackgroundMap( const std::string&,
                            const RectRegion& = RectRegion()
						   );
    //Doc:Desc Add background map lines from the specified map file.  The
    //Doc:Desc maps can be either in the CIA World Databank II format,
    //Doc:Desc or  a simple file of lat/lon coordinates with a single
    //Doc:Desc header line specifying the number of points.  The CIA
    //Doc:Desc database files  are available on the  GTNetS web page.
    //Doc:Arg1 Name of map database file.
    //Doc:Arg2 Bounding Region
    //Doc:Return True if successfully loaded

  bool    AddBackgroundMap(const StringVec_t&,
                           const RectRegion& = RectRegion()
                          );
    //Doc:Desc Add background map lines from the specified map files.  The
    //Doc:Desc maps can be either in the CIA World Databank II format,
    //Doc:Desc or  a simple file of lat/lon coordinates with a single
    //Doc:Desc header line specifying the number of points.  The CIA
    //Doc:Desc database files  are available on the  GTNetS web page.
    //Doc:Arg1 Vector of map file  names
    //Doc:Arg2 Bounding Region
    //Doc:Return True if successfully loaded

  //Doc:Method
  bool    PlaybackTraceFile(const char*);
    //Doc:Desc GTNetS can animate a previously run simulation, from the trace
    //Doc:Desc file logged by that simulation.  The topology MUST be identical
    //Doc:Desc to that of the original simulation.
    //Doc:Arg1 Trace file name
    //Doc:Return True if trace file successfully opened

  //Doc:Method
  bool    StartPlayback();
    //Doc:Desc Starts a trace file playback animation
    //Doc:Return True if successful.

  //Doc:Method
  void    CustomBackground(CustomBackground_t);
    //Doc:Desc Specifies a custom background callback
    //Doc:Arg1 Pointer to custom background callback function

  //Doc:Method
  void    NodeSelectedCallback(NodeSelected_t);
    //Doc:Desc Specifies a method to call when nodes are selected
    //Doc:Desc on the animation
    //Doc:Desc display.
    //Doc:Arg1 Callback function pointer.

  //Doc:Method
  void    EnableAnimationRecorder(bool);
    //Doc:Desc Enables or Disables the recorder icon on the animation display.
    //Doc:Desc     Recording takes huge amounts of disk space, so we only
    //Doc:Desc enable this when requested.
    //Doc:Arg1 True if recorder icon enabled.

  //Doc:Method
  void    RecorderMPEGSpeedup(Mult_t);
    //Doc:Desc When recording the MPEG file, the normal frame rate is 25fps.
    //Doc:Desc However, in a network simulation, this means 40ms
    //Doc:Desc between frames, and is much too long to make a 
    //Doc:Desc meaningful simulation animation.  Specifying the speedup
    //Doc:Desc increases the frames per second rate by the specified
    //Doc:Desc value.  For example, a speedup of 10.0 (the default value)
    //Doc:Desc will result in  250 fps.
    //Doc:Arg1 Desired animation recorder speedup.

  //Doc:Method
  void    EnableHTTPServer();
    //Doc:Desc GTNetS has an optional HTTP interface to query the
    //Doc:Desc state of the simulation.  Enabling this causes a
    //Doc:Desc separate thread to be spawned that will service
    //Doc:Desc HTTP request on port 8081. (If port 8081
    //Doc:Desc is not availble, consecutive ports are tried until
    //Doc:Desc one is found.

private:
  // Used internally by the Playback
  void FillEventsFromTrace(Time_t);

public:

  // Various statistics
  //Doc:Method
  Time_t  SetupTime();               // Wall clock time for initializatoin
    //Doc:Desc Returns the amount of wall clock (CPU) time used to initialize
    //Doc:Desc the simulation. This includes all actions from the start of
    //Doc:Desc the {\tt main} function and the call to {\tt Simulator::Run()}.
    //Doc:Return The CPU time used to initialize the simulation.

  //Doc:Method
  Time_t  RouteTime();               // Wall clock time for route computations
    //Doc:Desc Returns the amount of wall clock (CPU) time used to calculate
    //Doc:Desc routing information for the simulation.
    //Doc:Return The CPU time used to calculate routing information.

  //Doc:Method
  Time_t  RunTime();                 // Wall clock time for Run()
    //Doc:Desc Returns the amount of wall clock (CPU) time used by
    //Doc:Desc the event processing phase of the simulation.
    //Doc:Return The CPU time used to by the event processing.

  //Doc:Method
  Time_t  TotalTime();               // Total time of execution
    //Doc:Desc Returns the amount of wall clock (CPU) time used by
    //Doc:Desc the simulation.
    //Doc:Return The CPU time used to by the entire simulation.
#ifdef MOVED_TO_SCHEDULER
  //Doc:Method
  Count_t TotalEventsProcessed() { return totevp;} // Events processed
    //Doc:Desc Returns the count of total events processed by the simulation.
    //Doc:Return The total number of events processed.

  //Doc:Method
  Count_t TotalEventsScheduled() { return totevs;} // Events scheduled
    //Doc:Desc Returns the count of total events scheduled by the simulation.
    //Doc:Return The total number of events scheduled.

  //Doc:Method
  Count_t EventListSize() { return eventList.size();} // Number pending events
    //Doc:Desc Returns the current size of the event list, which is the
    //Doc:Desc number of scheduled, but unprocessed, events.
    //Doc:Return The current size of the pending events list.
#endif

  // X-Y Positions
  //Doc:Method
  void     AmimationRegion(const Location&, const Location&);
    //Doc:Desc Specifies the lower left and upper right corner for the
    //Doc:Desc animation display.
    //Doc:Arg1 Lower left corner
    //Doc:Arg1 Upper right corner.

  //Doc:Method
  void     NewLocation(Meters_t, Meters_t); // Set new x/y location
    //Doc:Desc Informs the animation processor of a new location.
    //Doc:Desc Will expand the AnimationRegion (See above) if needed.
    //Doc:Arg1 x-coordinate
    //Doc:Arg2 y-coordinate

  //Doc:Method
  void     NewLocation(const Location&);    // Set new x/y location
    //Doc:Desc Informs the animation processor of a new location.
    //Doc:Desc     Will expand the AnimationRegion (See above) if needed.
    //Doc:Arg1 x-y location to add.

  Meters_t SmallestX();              // Get the smallest X coordinate
  Meters_t SmallestY();              // Get the smallest Y coordinate
  Meters_t LargestX();               // Get the smallest X coordinate
  Meters_t LargestY();               // Get the smallest Y coordinate
  bool     HasMobility() { return hasMobility;} // True if mobility enabled
  void     HasMobility(bool m) { hasMobility = m;}

protected:
  virtual void ProgressResched(Time_t);
  Time_t       GetSec();             // Get CPU time used (rounded to seconds)

  bool         ProcessOneMap(std::string, const RectRegion&,
                             LocationVecVec_t&);
  RectRegion   FindBoundingBox(LocationVecVec_t&);

public:
  bool            verbose;           // True if verbose debug
  unsigned long   verbosemod;  // Mod factor for verbosity
  bool            cleanUp;     //指示是否清理内存
  Time_t          lastTopologyChange; //上次拓扑变更的时间
  bool            supercomputerInterconnect; // True if modeling SC/IC
protected:
  Time_t          simtime;     // 当前仿真的时间
  //EventMap_t      eventList;   // The actual event list (moved to scheduler)
  //EventSet_t      eventList;   // The actual event list
public:
#ifdef MOVED_TO_SCHEDULER
  //Doc:Member
  Count_t         totevs;      // Total events scheduled (debug)
    //Doc:Desc Count of the total number of events that have been scheduled

  //Doc:Member
  Count_t         totrm;       // Total events removed (debug)
    //Doc:Desc Count of the total number of events that have been removed

  //Doc:Member
  Count_t         totevp;      // Total events processed (debug)
    //Doc:Desc Count  of the total number of event processed

  //Doc:Member
  Count_t         totevc;      // Total events cancelled (debug)
    //Doc:Desc Count of total number of event cancelled

  //Doc:Member
  Count_t         evlSize;     // Number of events in list
    //Doc:Desc Current size of pending event list

  Count_t         maxSize;
#endif
  //  EventMap_t::iterator hint;   // Hint for insertions (right after prior)
  bool            silent;          // true为静默模式，不输出调试信息
  bool            halted;          // true的时候终止仿真
protected:
  SimulatorUid_t  uid;              // Current UID
  Time_t          progressInterval; // Interval for progress messages
  bool            serverEnabled;    // True if HTTP server enabled
public:
  ProgressEvent*  progressEvent;    // Pending progress event (if any);
protected:
  ProgressTimer   progressTimer;    // Progress timer
  Time_t          startRouteTime;   // CPU time used before Route computations
  Time_t          startRunTime;     // CPU time used before the "Run" call

  //QTWindow*       qtWin;            // QTWindow的对象，用来显示仿真的动画过程
  bool            animate;          //动画开始演示时为真
  bool            playbackAnimate;  // True if animate from trace file
  std::ifstream   playbackFile;     // Input stream for playback file
  bool            animateWirelessTx;// True if wireless tx animation desired
  bool            animateBasebandTx;// True if Bluetooth tx animation desired
  bool            basebandTxStart;  // True if Bluetooth tx animation started
  bool            pauseWirelessTx;  // True if pause on wireless transmit 
  bool            haltEventSched;   // 如果终止事件已经加入事件队列则为true

  // Location min/max
  Meters_t        smallestX;
  Meters_t        smallestY;
  Meters_t        largestX;
  Meters_t        largestY;
  bool            firstLocation;    //第一次设置窗口位置时为真
  bool            hasMobility;      // True if any mobility models used
public:
  static Simulator* instance;        //Simulator的实例
  static PHVec_t    progressHooks;  // List of callbacks on progress
  static RectRegion boundingBox;    // Min/Max X/Y positions
  static std::string simName;       // Name of this simulation for display
  static CustomBackground_t pCustomBackground; // Custom background callback
  static NodeSelected_t     pNodeSelected;     // Node selected callback
  static bool       recorderEnabled;// True if animation recorder icon enabled
  static Mult_t     recorderSpeedup;// Animation recorder speedup rate

#ifdef CTRLFIFO
  static std::string     name;
  static int             sockfd;
  static void            io_signal_handler(int);
  static void            toBlock(int);
  static void            toUnblock(int);
#endif
  
  //Doc:Method
  static Time_t   Now();            // 返回仿真的当前时间

  //Doc:Method
  static Size_t  ReportMemoryUsage();  // Memory usage in bytes
    //Doc:Desc Returns the total amount of memory (bytes) used by the
    //Doc:Desc simulation.
    //Doc:Return The total number of memory bytes used by the simulation.

  //Doc:Method
  static Size_t  ReportMemoryUsageMB();// Memory usage in megabytes
    //Doc:Desc Returns the total amount of memory (MegaBbytes) used by the
    //Doc:Desc simulation.
    //Doc:Return The total number of memory MegaBytes used by the simulation.
};

#endif


