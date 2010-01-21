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
/* 2010.1.4  ����Ƽ                                                     */
/***********************************************************************/

#include <iostream>

#include "link.h"
#include "linkp2p.h"
#include "ratetimeparse.h"
#include "interface-basic.h"

using namespace std;

Rate_t   Link::defaultRate   = Rate("1Mb"); //Ĭ����·��������
Time_t   Link::defaultDelay  = Time("1ms"); //Ĭ����·�ӳ�
Link*    Link::defaultLink;
Random*  Link::defaultJitter;
Random*  Link::berRng;
bool     Link::useSeqEvents = true;  // True �������ϵͳ�����¼��б�

Link::Link()
    : localIf(nil)
//����һ��Ĭ����·
{
}

Link::Link(Rate_t, Time_t)
    : localIf(nil)
//����һ����·��ָ���������ʱ
{
}

Link::Link(const Link& l)
    : localIf(nil)
//����ָ����·���󣬴���һ������·
{
}

InterfaceBasic* Link::GetLocalIf() const
//���ر��ؽӿڵ�ָ��
{
  return localIf;
}

void         Link::SetLocalIf(InterfaceBasic* lif)
//���ñ��ش���ӿ�
{
  localIf = lif;
}

LinkMonitor* Link::AddLinkMonitor(const LinkMonitor&)
{ // Over-ridden by real links as needed, ignored for ghosts
  return nil;
}
  
Interface* Link::GetNewInterface(IPAddr_t, Mask_t) const
/*
������Ϊ����·����һ��ָ���ӿ�
����һ�������ӿڵ�IP��ַ
�������������ӿڵ�����
����ֵ�������ӿڵ�ָ��
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
//��ȡ��·��Ĭ�ϴ���
{ 
  defaultRate = r;
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  defaultLink->Bandwidth(r); // And change in the default link
  Linkp2p::Default().Bandwidth(r); // and for default p2p
}

void     Link::DefaultDelay(Time_t t)
//��ȡ��·��Ĭ���ӳ�
{
  defaultDelay = t;
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  defaultLink->Delay(t);
  Linkp2p::Default().Delay(t); // Change for default p2p
}

void     Link::DefaultJitter(const Random& j)
//������·��Ĭ��jitter
{
  defaultJitter = j.Copy();
  defaultLink->Jitter(j);
  Linkp2p::Default().Jitter(j); // Change for default p2p
}

Random*  Link::DefaultJitter()
//������·��Ĭ��jitter
{
  if (!defaultJitter) defaultJitter = new Constant(0);
  return defaultJitter;
}

void     Link::Default(const Link& l)
//Ϊ����·����һ��ָ������·����
{
  if (defaultLink) delete defaultLink;
  defaultLink = l.Copy();
}

const Link& Link::Default()
//���ر���·����·����
{
  if (!defaultLink) defaultLink = new Linkp2p(defaultRate, defaultDelay);
  return *defaultLink;
}

