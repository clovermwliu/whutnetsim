#include "debug.h"
#include "common-defs.h"
#include "globalstats.h"
#include <iostream>
using namespace std;
class Simulator {
	
public:
	friend class ProgressTimer; // Allow timer access to "ProgressResched"
	
	Simulator();
	virtual ~Simulator();
public:
	bool            verbose;           // True if verbose debug
	unsigned long   verbosemod;  // Mod factor for verbosity
	bool            cleanUp;     // Cleanup on delete
	Time_t          lastTopologyChange; // Time of last failure or recovery
	bool            supercomputerInterconnect; // True if modeling SC/IC
protected:
	Time_t          simtime;     // Current Simulation Time
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
	bool            silent;          // True if no printouts from Simulator
	bool            halted;          // True if halted
protected:
	SimulatorUid_t  uid;              // Current UID
	Time_t          progressInterval; // Interval for progress messages
	bool            serverEnabled;    // True if HTTP server enabled
//public:
¡¡¡¡//ProgressEvent*  progressEvent;    // Pending progress event (if any);
protected:
	//ProgressTimer   progressTimer;    // Progress timer
	Time_t          startRouteTime;   // CPU time used before Route computations
	Time_t          startRunTime;     // CPU time used before the "Run" call

	//QTWindow*       qtWin;            // qt display window
	bool            animate;          // True if animation active
	bool            playbackAnimate;  // True if animate from trace file
	std::ifstream   playbackFile;     // Input stream for playback file
	bool            animateWirelessTx;// True if wireless tx animation desired
	bool            animateBasebandTx;// True if Bluetooth tx animation desired
	bool            basebandTxStart;  // True if Bluetooth tx animation started
	bool            pauseWirelessTx;  // True if pause on wireless transmit 
	bool            haltEventSched;   // True if the halt event is scheduled

	// Location min/max
	Meters_t        smallestX;
	Meters_t        smallestY;
	Meters_t        largestX;
	Meters_t        largestY;
	bool            firstLocation;    // True if first call to set location
	bool            hasMobility;      // True if any mobility models used
public:
	static Simulator* instance;
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

	static Size_t  ReportMemoryUsage(); 
};
