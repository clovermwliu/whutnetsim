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


//File Information: TaskStats object
//File Name: task-stats.cc
//File Purpose: Record global status of simulator
//Original Author: YueYuntao
//Author Organization: WHU
//Construct Data:  JAN, 2010
//Modify Author: 
//Author Organization: 
//Modify Data: 

#include "node.h"
#include "link-real.h"
#include "queue.h"
#include "interface.h"
#include "task-stats.h"

using namespace std;

DCount_t TaskStats::GetPktsSentByNode(Node* n)
//获得结点发送数据包总数
{
	NodeWeightVec_t nwv;
	n->Neighbors(nwv);
	DCount_t sum = 0.0;
	for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
	{
		Node* nn = nwv[j].node;
		Interface* ifaceSrc = n->GetIfByNode(nn);
		Link* linkSrc = ifaceSrc->GetLink();
		sum += linkSrc->PktsSent();
	}
	return sum;
}

DCount_t TaskStats::GetPktsRecievedByNode(Node* n)
//获得结点接受数据包总数
{
	NodeWeightVec_t nwv;
	n->Neighbors(nwv);
	DCount_t sum = 0.0;
	for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
	{
		Node* nn = nwv[j].node;
		Interface* ifaceSrc = nn->GetIfByNode(n);
		Link* linkSrc = ifaceSrc->GetLink();
		sum += linkSrc->PktsSent();
	}
	return sum;
}

DCount_t TaskStats::GetBytesSentByNode(Node* n)
//获得结点发送数据总量
{
	NodeWeightVec_t nwv;
	n->Neighbors(nwv);
	DCount_t sum = 0.0;
	for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
	{
		Node* nn = nwv[j].node;
		Interface* ifaceSrc = n->GetIfByNode(nn);
		Link* linkSrc = ifaceSrc->GetLink();
		sum += linkSrc->BytesSent();
	}
	return sum;
}

DCount_t TaskStats::GetBytesRecievedByNode(Node* n)
//获得结点接受数据总数
{
	NodeWeightVec_t nwv;
	n->Neighbors(nwv);
	DCount_t sum = 0.0;
	for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
	{
		Node* nn = nwv[j].node;
		Interface* ifaceSrc = nn->GetIfByNode(n);
		Link* linkSrc = ifaceSrc->GetLink();
		sum += linkSrc->BytesSent();
	}
	return sum;
}

DCount_t TaskStats::GetQueueSizeByNode(Node* n)
//获得结点的所有队列中数据包个数
{
	IFVec_t ifv = n->Interfaces();
	DCount_t sum = 0.0;
	for (IFVec_t::size_type j = 0; j < ifv.size(); ++j)
	{
		Interface* i = ifv[j];
		Queue* q = i->GetQueue();
		sum += DCount_t(q->LengthPkts());
	}
	return sum;
}

DCount_t TaskStats::GetDropPktsCountByNode(Node* n)
//获得结点主动丢弃的数据包总数
{
	IFVec_t ifv = n->Interfaces();
	DCount_t sum = 0.0;
	for (IFVec_t::size_type j = 0; j < ifv.size(); ++j)
	{
		Interface* i = ifv[j];
		Queue* q = i->GetQueue();
		sum += DCount_t(q->DropCount());
	}
	return sum;
}

DCount_t TaskStats::GetDropPktsRateByNode(Node* n)
//获得结点的丢包率
{
	return GetDropPktsCountByNode(n) / GetPktsSentByNode(n);
}

DCount_t TaskStats::GetPktsSentByNodes(Node* n1, Node* n2)
//获得结点向另一结点发送的数据包总数
{
	Interface* ifaceSrc = n1->GetIfByNode(n2);
	Link* linkSrc = ifaceSrc->GetLink();
	return linkSrc->PktsSent();
}

DCount_t TaskStats::GetPktsRecievedByNodes(Node* n1, Node* n2)
//获得结点从另一结点接受的数据包总数
{
	Interface* ifaceSrc = n2->GetIfByNode(n1);
	Link* linkSrc = ifaceSrc->GetLink();
	return linkSrc->PktsSent();
}

DCount_t TaskStats::GetBytesSentByNodes(Node* n1, Node* n2)
//获得结点向另一结点发送的数据总量
{
	Interface* ifaceSrc = n1->GetIfByNode(n2);
	Link* linkSrc = ifaceSrc->GetLink();
	return linkSrc->BytesSent();
}

DCount_t TaskStats::GetBytesRecievedByNodes(Node* n1, Node* n2)
//获得结点从另一结点接受的数据总量
{
	Interface* ifaceSrc = n2->GetIfByNode(n1);
	Link* linkSrc = ifaceSrc->GetLink();
	return linkSrc->BytesSent();
}

DCount_t TaskStats::GetUtilizationByNodes(Node* n1, Node* n2)
//获得结点到另一结点的链路利用率
{
	Interface* ifaceSrc = n1->GetIfByNode(n2);
	Link* linkSrc = ifaceSrc->GetLink();
	return linkSrc->Utilization();
}