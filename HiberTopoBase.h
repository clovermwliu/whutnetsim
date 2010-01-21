//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
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


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:


//更改人：李玉
//更改时间：2010-1-15

#ifndef __HIBERTOPOBASE_H__   
#define __HIBERTOPOBASE_H__ 

#pragma once
#include "node.h"
#include "linkp2p.h"
#include "PlatTopoBase.h"
#include "G_common_defs.h"
#include "ErrorHandler.h"
#include <string>
using namespace std;

#define  SUCCESS_HIBERTOPO                ERROR_NO_ERROR
#define  ERROR_ID_OUT_OF_LAY1NODECOUNT_FAIL                0x00050201 //要找的节点的ID超出了第一层nodecount的范围
#define  ERROR_ID_OUT_OF_LAY2NODECOUNT_FAIL                0x00050202 //要找的节点的ID超出了第二层nodecount的范围
#define  ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL                0x00050203 //要找的节点的ID超出了第三层nodecount的范围

//#define ERROR_HIBERTOPO_NOT_EXSITING	 0x00050001
//#define ERROR_FILE_WRITE_FAIL            0x00050002

class CHiberTopoBase  : public CErrorHandler
{
public:
	CHiberTopoBase(IPAddr_t i= IPADDR_NONE,
				   const Linkp2p& = Linkp2p::Default(),
				   SystemId_t = 0);
	typedef enum  {DEFAULT, CIRCLE, GRID, CIRCLES} BoxType;
public:
	~CHiberTopoBase(void);
	 Error_str GetLastErrorEx(){return "hello";}
	 Interface* AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l = Linkp2p::Default());

	 virtual void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										 const Location& BoundBoxRightUpper,
									     BoxType  type = DEFAULT) {};

	 virtual bool GenerateTopo() = 0;
	
	 //对platTopoVec的操作
	 bool  AddOneTopo(CPlatTopoBase*,Count_t layer);//将已经新建的平面拓扑的指针一个个压入相量保存
	
	 //layer1层的numOfTopo1拓扑的第topo1NodeId个节点，和layer2层的numOfTopo2拓扑的第topo2NodeId个拓扑相连
	 bool  ConnectDomain(Count_t layer1,
						 Count_t  numOfTopo1,
						 Count_t topo1NodeId,
						 Count_t layer2,
						 Count_t  numOfTopo2,
						 Count_t topo2NodeId);
	
	 //第numOfTopo1拓扑随机选一个节点，和第numOfTopo2拓扑随机选一个节点相连
	 bool  ConnectDomain(Count_t layer1,
						 Count_t  numOfTopo1,
						 Count_t layer2,
						 Count_t  numOfTopo2);
	

	 Count_t  GetLay1Num(){return transitTopoVec.size();}
	 Count_t  GetLay2Num(){return stubTopoVec.size();}
	 Count_t  GetLay3Num(){return lanTopoVec.size();}

protected:
	 IPAddr_t ip;
	 const Linkp2p&  link;
	 SystemId_t sid;
	 string configFile;  //配置文件的路径

	// vector<CPlatTopoBase*>  platTopoVec;//
	 vector<CPlatTopoBase*>  transitTopoVec;
	 vector<CPlatTopoBase*>  stubTopoVec;
	 vector<CPlatTopoBase*>  lanTopoVec;
	
	 Count_t  transit;
	 Count_t  stub;
	 Count_t  lan;
};
#endif