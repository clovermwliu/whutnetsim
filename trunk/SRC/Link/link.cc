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


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:

/************************************************************************/
/* 2010.1.4  关怡萍                                                     */
/***********************************************************************/

#include <iostream>

#include "link.h"
#include "linkp2p.h"
#include "ratetimeparse.h"
#include "interface-basic.h"

using namespace std;

Rate_t   Link::defaultRate   = Rate("1Mb"); //默认链路传输速率
Time_t   Link::defaultDelay  = Time("1ms"); //默认链路延迟
Link*    Link::defaultLink;
Random*  Link::defaultJitter;
Random*  Link::berRng;
bool     Link::useSeqEvents = true;  // True 代表进入系统仿真事件列表

Link::Link()
    : localIf(nil)
//创建一条默认链路
{
}

Link::Link(Rate_t, Time_t)
    : localIf(nil)
//创建一条链路，指定带宽和延时
{
}

Link::Link(const Link& l)
    : localIf(nil)
//根据指定链路对象，创建一条新链路
{
}

InterfaceBasic* Link::GetLocalIf() const
//返回本地接口的指针
{
  return localIf;
}

void         Link::SetLocalIf(InterfaceBasic* lif)
//设置本地传输接口
{
  localIf = lif;
}

LinkMonitor* Link::AddLinkMonitor(const LinkMonitor&)
{ // Over-ridden by real links as needed, ignored for ghosts
  return nil;
}
  
Interface* Link::GetNewInterface(IPAddr_t, Mask_t) const
/*
描述：为本链路新增一个指定接口
参数一：新增接口的IP地址
参数二：新增接口的掩码
返回值：新增接口的指针
*/
{
  return nil;
}


  
void Link::AnimateLink()
{
}

void Link::ReAnimateLink()
{ 
}
void Link::AnimatePackets()
{ 
}

void     Link::DefaultRate(Rate_t r)
//获取链路的默认带宽
{ 
  defaultRate = r;
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  defaultLink->Bandwidth(r); // And change in the default link
  Linkp2p::Default().Bandwidth(r); // and for default p2p
}

void     Link::DefaultDelay(Time_t t)
//获取链路的默认延迟
{
  defaultDelay = t;
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  defaultLink->Delay(t);
  Linkp2p::Default().Delay(t); // Change for default p2p
}

void     Link::DefaultJitter(const Random& j)
//设置链路的默认jitter
{
  defaultJitter = j.Copy();
  defaultLink->Jitter(j);
  Linkp2p::Default().Jitter(j); // Change for default p2p
}

Random*  Link::DefaultJitter()
//返回链路的默认jitter
{
  if (!defaultJitter) defaultJitter = new Constant(0);
  return defaultJitter;
}

void     Link::Default(const Link& l)
//为本链路设置一个指定的链路对象
{
  if (defaultLink) delete defaultLink;
  defaultLink = l.Copy();
}

const Link& Link::Default()
//返回本链路的链路对象
{
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  return *defaultLink;
}

