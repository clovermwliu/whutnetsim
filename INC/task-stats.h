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


//File Information: TaskStats class
//File Name: task-stats.h
//File Purpose: Record global status of simulator
//Original Author: YueYuntao
//Author Organization: WHU
//Construct Data:  JAN, 2010
//Modify Author: 
//Author Organization: 
//Modify Data: 


#ifndef __task_stats_h__
#define __task_stats_h__
#include <iostream>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>

using namespace std;

class Node;
class Interface;

class TaskStats{
public:
	static DCount_t GetCurrentTime();//获得当前的时间
	static DCount_t GetPktsSentByNode(Node*);//获得结点发送数据包总数
	static DCount_t GetPktsRecievedByNode(Node*);//获得结点接受数据包总数
	static DCount_t GetBytesSentByNode(Node*);//获得结点发送数据总量
	static DCount_t GetBytesRecievedByNode(Node*);//获得结点接受数据总数
	static DCount_t GetQueueSizeByNode(Node*);//获得结点的所有队列中数据包个数
	static DCount_t GetDropPktsCountByNode(Node*);//获得结点主动丢弃的数据包总数
	static DCount_t GetDropPktsRateByNode(Node*);//获得结点的丢包率
	static DCount_t GetPktsSentByNodes(Node*, Node*);//获得结点向另一结点发送的数据包总数
	static DCount_t GetPktsRecievedByNodes(Node*, Node*);//获得结点从另一结点接受的数据包总数
	static DCount_t GetBytesSentByNodes(Node*, Node*);//获得结点向另一结点发送的数据总量
	static DCount_t GetBytesRecievedByNodes(Node*, Node*);//获得结点从另一结点接受的数据总量
	static DCount_t GetUtilizationByNodes(Node*, Node*);//获得结点到另一结点的链路利用率
	static DCount_t GetJitterByNodes(Node*, Node*);//to be modify，获得结点到另一结点的链路传输延迟抖动
};

#endif