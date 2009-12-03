#include "simulator.h"
Simulator* Simulator::instance;         // Points to single Simulator instance
PHVec_t    Simulator::progressHooks;    // List of callbacks on progress
RectRegion Simulator::boundingBox;      // Min/Max X/Y positions
string     Simulator::simName;          // Name of this sim

// Custom background callback
CustomBackground_t Simulator::pCustomBackground = nil;
NodeSelected_t     Simulator::pNodeSelected = nil;
bool       Simulator::recorderEnabled = false;
Mult_t     Simulator::recorderSpeedup = 10.0;

Simulator::Simulator() : 
verbose(false), verbosemod(1000), cleanUp(false), 
lastTopologyChange(0),
supercomputerInterconnect(false),
simtime(0)
//totevs(0), totrm(0), totevp(0), totevc(0),
//evlSize(0), maxSize(0), 
{
	instance = this;
	//  hint = eventList.end();
	// Call the initializers for various protocols
	//TCPDemux::Initialize();
	//UDPDemux::Initialize();

	//Stats::initialMemory = ReportMemoryUsage();

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
//	DEBUG0((cout << "Hello from Simulator Destructor" << endl));
//	instance = nil;
//	const NodeVec_t& nodes = Node::GetNodes();
//	if (cleanUp)
//	{
//		for (NodeVec_t::size_type i = 0; i < nodes.size(); ++i)
//		{
//			DEBUG0((nodes[i]->DumpNV())); // Print debug nixvec info
//			delete nodes[i];
//		}
//		DEBUG0((ReuseBase::DumpQ())); // Print dbg info from memory reuse buffers
//	}
//	Node::Clear(); // Remove all entries
//	Trace::Instance()->Close();
//#ifdef CTRLFIFO
//	unlink(name.c_str());
//#endif
}

Size_t  Simulator::ReportMemoryUsage()
{
#ifdef WIN32
	return 0;
#else

//	pid_t  pid;
//	char   work[4096];
//	FILE*  f;
//	char*  pCh;
//
//	pid = getpid();
//	sprintf(work, "/proc/%d/stat", (int)pid);
//	f = fopen(work, "r");
//	if (f == NULL)
//	{
//#ifdef HAVE_PSINFO
//		DEBUG(0,(cout << "Can't open " << work 
//			<<" trying ioctl method" << endl));
//		if (!pagesize)
//		{
//			pagesize = getpagesize();
//		}
//		char w[255];
//		sprintf(w, "/proc/%d", pid);
//		int p = open(w, O_RDONLY, 0);
//		if (p < 0)
//		{
//			DEBUG(0,(cout << "Huh? Can't open "
//				<< string(w) << endl));
//			return(0);
//		}
//		struct prpsinfo prps;
//		int i = ioctl(p, PIOCPSINFO, &prps);
//		if (i)
//		{
//			DEBUG(0,(cout << "HuH? Can't get ioctl in memusage" << endl));
//			return(0);
//		}
//		return(prps.pr_rssize * pagesize);
//#else
//		DEBUG(0,(cout << "Can't open " << work << endl));
//#endif
//		return(0);
//	}
//	fgets(work, sizeof(work), f);
//	fclose(f);
//	strtok(work, " ");
//	for (int i = 1; i < 23; i++) 
//	{
//		pCh = strtok(NULL, " ");
//	}
//	//return(atoll(pCh));
//	return(atol(pCh));
return 1;
#endif // WIN32
}
//
//void Simulator::StartAnimation(Time_t when, bool initialPause, bool fullScreen)
//{
//#ifdef HAVE_QT
//	if (!qtWin) qtWin = new QTWindow(fullScreen);
//	animate = true;
//	qtWin->DisplayTopologyAndReturn();
//	// Schedule the start animation event
//	QTEvent* ev;
//	if (initialPause)
//	{
//		ev = new QTEvent(QTEvent::PAUSE);
//	}
//	else
//	{
//		ev = new QTEvent(QTEvent::START);
//	}
//	Scheduler::Schedule(ev, when - Simulator::Now(), qtWin);
//#endif
//}
//
//Time_t Simulator::Now()
//{
//	if (!instance) return(0.0);  // Simulation has not yet started
//	return instance->simtime;
//}
//
//void Simulator::Progress(Time_t interval)
//{
//	if (progressEvent)
//	{
//		Scheduler::Cancel(progressEvent); // Cancel any pending
//		delete progressEvent;  // And remove the event
//		progressEvent = nil;
//	}
//	// Schedule new event for "interval" secs in future
//	progressInterval = interval;
//	ProgressResched(interval);
//}
//
//void Simulator::ProgressResched(Time_t interval)
//{
//	progressEvent = new ProgressEvent(interval);;
//	progressEvent->event = 0; // ! HuH? why do I need this..
//	progressTimer.Schedule(progressEvent, interval);
//}
//
//void Simulator::StopAt(Time_t t)
//{
//	SimulatorEvent* e = new SimulatorEvent(SimulatorEvent::HALT);
//
//	// Schedule the halt event
//	Scheduler::Schedule(e, t, this);
//	haltEventSched = true;
//}
//
//void Simulator::Run()
//{
//	CommonRunInit();
//	while(!halted)
//	{
//		Event* e = Scheduler::DeQueue();
//		if (!e) break; // No more events
//		if (e->handler)
//		{
//			if (e->Time() < simtime) 
//				cout << "HuH? Time backing up from " << simtime
//				<< " to " << e->Time() << endl;
//			simtime = e->Time();
//			Scheduler::Instance()->totevp++;
//			DEBUG(4,(cout << "Handling ev at time " << e->Time() << endl));
//			e->handler->Handle(e, e->Time()); // Process the event
//		}
//		else
//		{
//			if (!silent)
//			{
//				cout << "Skipping event " << e->uid
//					<< "  at time " << e->Time()
//					<< ", no handler " << endl;
//			}
//		}
//	}
//	DEBUG0((cout << "Simulator::Run exit event loop" << endl));
//	if (!Scheduler::Instance()->EventListSize() && !halted)
//		if(!silent)cout << "Simulator::Run, event list exhausted" << endl;
//	Stats::finalMemory = ReportMemoryUsage();
//}
//
//void Simulator::CommonRunInit()
//{ // Used by both Simulator::Run and DistributedSimulator::Run
//#ifdef CTRLFIFO
//	fcntl(sockfd, F_SETOWN, getpid());
//	toUnblock(sockfd);
//	signal(SIGIO, &Simulator::io_signal_handler);
//	listen(sockfd, 2);
//#endif
//
//	halted = false;
//	Stats::modelMemory = ReportMemoryUsage();
//	startRouteTime = GetSec(); // Log CPU time used before route computations
//	DEBUG0((cout << "Simulator::Run, needInit() " 
//		<< Routing::Default()->NeedInit() << endl));
//	//if (Routing::Default()->NeedInit())
//	//{ // Need to initialize routes at each node
//	//	const NodeVec_t& n = Node::GetNodes(); // Get the global list of nodes
//	//	for (NodeVec_t::size_type i = 0; i < n.size(); ++i)
//	//	{ // Call the initializer for each node
//	//		n[i]->InitializeRoutes();
//	//	}
//	//}
//	DEBUG0((cout << "Done initializing routes..." << endl));
//	Stats::routingMemory = ReportMemoryUsage();
//	startRunTime = GetSec(); // Log CPU time used before RUN
//}
//Time_t Simulator::GetSec()
//{ 
//#ifndef WIN32	
//	// Get cpu usage time, rounded to seconds
//	struct rusage u;
//	getrusage (RUSAGE_SELF, &u);
//	Time_t us = u.ru_utime.tv_usec + u.ru_stime.tv_usec;
//	Time_t s  = u.ru_utime.tv_sec + u.ru_stime.tv_sec;
//	return s + us / 1000000.0;
//#else
//	return 0;
//#endif
//}