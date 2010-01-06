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


/************************************************************************/
/* 2010.1.4  关怡萍                                                     */
/***********************************************************************/
#ifndef __duplexlink_h__
#define __duplexlink_h__

//在两个结点之间创建一条双向链路（本质上是新建两条边）
#include "linkp2p.h"

class Interface;

class DuplexLink {
public:
  DuplexLink(Node* ln, Node* rn);//根据本地结点和远程结点添加默认的双向链路
  DuplexLink(Node* ln, const Linkp2p& ll,
             Node* rn, const Linkp2p& rl); //根据本地结点、待添加链路和远程结点、待添加链路添加双向链路
  DuplexLink(Interface* li, Interface* ri);//根据本地结点接口和远程结点接口添加默认的双向链路
  DuplexLink(Interface* li, 
	         const Linkp2p& ll,
             Interface* ri, 
			 const Linkp2p& rl);//根据本地结点接口、待添加链路和远程结点接口、待添加链路添加双向链路
  DuplexLink(Node* ln, 
	         IPAddr_t la, 
			 Mask_t lm,  
             Node* rn, 
			 IPAddr_t ra, 
			 Mask_t rm); //根据本地结点、IP地址、掩码和远程结点、IP地址、掩码添加双向链路
  DuplexLink(Node* ln, 
			 IPAddr_t la, 
			 Mask_t lm,  
             const Linkp2p& ll, 
             Node* rn, 
			 IPAddr_t ra, 
			 Mask_t rm,
             const Linkp2p& rr);//根据本地结点、IP地址、掩码、待添加链路以及远程结点、IP地址、掩码、待添加链路添加双向链路
  ~DuplexLink(); //析构函数
public:
  Link*      local;//本地结点的链路
  Link*      remote;//远程结点的链路
  Interface* localif;//本地结点的接口
  Interface* remoteif;//远程结点的接口
private:
  void AddSimplexLinks(Node* ln, 
	                   IPAddr_t la, 
					   Mask_t lm, 
					   const Link& ll,
                       Node* rn, 
					   IPAddr_t ra, 
					   Mask_t rm, 
					   const Link& rl);//创建两条链路，本地结点、IP地址、掩码、待添加链路以及远程结点、IP地址、掩码、待添加链路已知。
};

#endif

