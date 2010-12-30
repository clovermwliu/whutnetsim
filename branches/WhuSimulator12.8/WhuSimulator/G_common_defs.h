
#ifndef __common_defs_h__
#define __common_defs_h__

#include <string>

typedef double         Time_t;         // Simulation time
typedef unsigned long  Event_t;        // Idenifies events in handler
typedef unsigned long  SimulatorUid_t; // Unique ID for each event
typedef size_t         Count_t;
typedef size_t  NodeId_t; //节点ID类型

typedef unsigned long   Error_t;
typedef std::string     Error_str;
typedef std::string     Handler_status_str;
typedef size_t          Packet_t;
typedef size_t			HandlerType;
#endif