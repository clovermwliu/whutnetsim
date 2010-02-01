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
#ifndef WIN32
//#include <unistd.h>
#endif

#ifdef HAVE_QT
#include "GUI_Defs.h"
#endif

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "node-real.h"
#include "protograph.h"
#include "portdemux.h"
#include "ipaddr.h"
#include "duplexlink.h"
#include "application.h"
#include "mobility.h"
//#include "link-rti.h"  v1.01
#include "mask.h"
#include "interface-real.h"
//#include "interface-wireless.h"  v1.01
//#include "interface-ghost.h"
//#include "interface-ethernet.h"

using namespace std;

NodeImpl::NodeImpl(Node* n)
    : pNode(n), ipAddr(IPADDR_NONE), neighborCount(0), down(false)
{
}

NodeImpl::~NodeImpl()
{
	//�ͷŽڵ��ÿһ��Ӧ�ã���Ӧ�õ����ͷ��˵��Ĳ�Э�� 2010-1-29

	ApplicationVec_t::iterator iter; 
	for ( iter=pappvec.begin(); iter!=pappvec.end(); ++iter )
	{
		delete *iter;
	}
	pappvec.clear(); //Ӧ����������

	//�ڵ�ӿڵ��ͷ� 2010-1-29
	IFVec_t::iterator iterinterface;
	for ( iterinterface=interfaces.begin(); iterinterface!=interfaces.end(); iterinterface++ )
	{
		delete *iterinterface;
	}
	int test = interfaces.size();
	interfaces.clear();

}

// IP Address Management
IPAddr_t    NodeImpl::GetIPAddr()
//���ظý�����ĵ�һ��IP��ַ����Ϊһ�������������ӿڰ󶨣���˿��ܴ��ڶ��IP��ַ
{ 
  return ipAddr;  
}

void NodeImpl::SetIPAddr(IPAddr_t i)
//���øý���IP��ַΪ�������i
{ 
  ipAddr = i;
  if (interfaces.size() == 1)
    { // Only one iface, set the same IPAddr there
      interfaces[0]->SetIPAddr(i);
    }
}

void NodeImpl::IPAddrs(IPMaskVec_t& IPMaskVect)
/*
������Ϊ�ý�㴴��һ��IP��ַ������
������[OUT]IPMaskVect�����������µ�IP��ַ��������IP��ַ�б�
��ע��һ�������ж���ӿڣ�ÿ���ӿڶ���һ���������IP��ַ�����롣
*/
{ 
  bool localFound = false;  //��֤����IP��ַ�Ѿ���������
  for (IFVec_t::size_type j = 0; j < interfaces.size(); ++j)
    {
      Interface* i = interfaces[j];
      if (i->GetIPAddr() != IPADDR_NONE)
        {
          IPMaskVect.push_back(IpMask(i->GetIPAddr(), i->GetIPMask()));
          if ((ipAddr & i->GetIPMask()) == (i->GetIPAddr() & i->GetIPMask()))
            {
              localFound = true;
            }
        }
    }
  if (!localFound && (ipAddr != IPADDR_NONE))
    { // ��Ҫ�����㱾��
      IPMaskVect.push_back(IpMask(ipAddr, MASK_ALL));
    }
}

bool NodeImpl::LocalIP( IPAddr_t ip)
/*
�������ж�ip�Ƿ�Ϊ�����IP��ַ��
����ֵ��true ���� ip�Ǳ��ؽ��IP��ַ
        false ����ip���Ǳ��ؽ��IP��ַ
*/
{ 
  if (ipAddr == IPADDR_NONE) return false; // ���ؽ��û��IP��ַ
  if (ip == ipAddr) return true; //�����Ǳ��ؽ��IP��ַ
  NodeReal::count4 += interfaces.size();
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if ((IPAddr_t)interfaces[i]->GetIPAddr() == ip)
        {
          return true; //�ڱ��ؽ��IP��ַ���ҵ�
        }
    }
  return false; //û���ҵ������Ǳ��ؽ��IP��ַ
}

bool NodeImpl::IPKnown()
//�ж��Ƿ������һ����Ĭ�ϵ�IP��ַ�����򷵻�true
{ 
  return ipAddr != IPADDR_NONE;
}

Interface* NodeImpl::AddInterface(Interface* pif)
/*
���������һ���Ѿ����úõĽӿ�pif
����ֵ:���뱾�ؽ���Ժ�Ľӿڵ�ָ��
*/
{ 
  Interface::Add(pif, pif->GetMACAddr());
  pif->SetNode(pNode);
  interfaces.push_back(pif);
  if (!IPKnown() && (pif->GetIPAddr() != IPADDR_NONE))
    { //��һ���ӿڷ���Ĭ��IP��ַ
      SetIPAddr(pif->GetIPAddr());
    }
  return pif;
}

  
Interface* NodeImpl::AddInterface(const L2Proto& l2, bool bootstrap)
/*
����������µĿսӿڣ��ýӿڷ���ڶ�������Э�飬û��IP��ַ������
������l2����Ϊ�����ӿڷ���ĵڶ�������Э�����
	  bootstrap������MAC��ַ��صĲ����������Ѿ���Ϊfalse
����ֵ:���뱾�ؽ���Ժ�Ľӿڵ�ָ��
��ע�����������Ľӿڷ���һ���ڶ�������Э������������������ͨ�����Ʋ����ģ���˿��Թ�����ӿ�ʹ��
*/
{
  return AddInterface(l2, IPADDR_NONE, MASK_ALL, MACAddr::Allocate(),
		      bootstrap);
}

Interface* NodeImpl::AddInterface(const L2Proto& l2,
                                  IPAddr_t ip, 
								  Mask_t mask,
                                  MACAddr mac, 
								  bool bootstrap)
/*
������Ϊ�ý������ָ��IP��ַ������Ľӿڣ�MAC��ַĬ�ϣ�������ڶ�������Э��
������l2����Ϊ�����ӿڷ���ĵڶ�������Э�����
	  ip����Ϊ�����ӿڷ����IP��ַ
	  mask����Ϊ�����ӿڷ��������
	  mac����Ϊ�����ӿڷ����MAC��ַ���õ�ַ��Ĭ�ϵ�
	  bootstrap������MAC��ַ��صĲ����������Ѿ���Ϊfalse
����ֵ�����뱾�ؽ���Ժ�Ľӿڵ�ָ��
*/
{
  Interface* pif;

  if (IsReal())
    { // Real���͵Ľӿ�, use either wired or wireless interface v1.01
      //if (l2.IsWireless())
      //  {
      //    DEBUG(2,(cout << "Node " << pNode->Id()
      //              << " adding wireless interface" << endl));
      //    pif = new InterfaceWireless(l2, ip, mask, mac, bootstrap);
      //  }
      //else
        {
          DEBUG(2,(cout << "Node " << pNode->Id()
                   << " adding real interface" << hex << ip << endl));
          pif = new InterfaceReal(l2, ip, mask, mac, bootstrap);
        }
    }
  //else v1.01
  //  { // Use a ghost interface
  //    DEBUG(2,(cout << "Node " << pNode->Id()
  //             << " adding ghost interface" << endl));
  //    pif = new InterfaceGhost(l2, ip, mask, mac, bootstrap);
  //  }

  // If firstbitrx node, note first bit rx interface
  if (FirstBitRx())
    {
      pif->FirstBitRx(true);
    }
  
  
  // Add to list  of interface by Mac addresses
  Interface::Add(pif, mac.macAddr);
  pif->SetNode(pNode);
  interfaces.push_back(pif);
  if (!IPKnown())
    { // Default IP address to first interface
      SetIPAddr(ip);
    }
  return pif;
}

Interface* NodeImpl::AddInterface(const L2Proto& l2, 
								  const Interface& iface,
                                  IPAddr_t ip, 
								  Mask_t mask,
                                  MACAddr mac, 
								  bool bootstrap)
/*
������Ϊ���ؽ�������ӿڣ�IP��ַ�����롢�ӿ�����ָ����MAC��ַĬ�ϣ�������ڶ�������Э��
������l2����Ϊ�����ӿڷ���ĵڶ�������Э�����Ĭ��Ϊ802.3
	  iface���������ӿ�ָ���Ľӿ�����	
	  ip����Ϊ�����ӿڷ����IP��ַ
	  mask����Ϊ�����ӿڷ��������
	  mac����Ϊ�����ӿڷ����MAC��ַ���õ�ַ��Ĭ�ϵ�
	  bootstrap������MAC��ַ��صĲ����������Ѿ���Ϊfalse
*/
{  //v1.01
  //Interface* pif;
  //// if the interface parameter is of type interface ethernet
  //if(iface.IsEthernet())
  //   {
  //      DEBUG(2,(cout << "Node " << pNode->Id()
  //                 << " adding Ethernet interface" << hex << ip << endl));
  //     pif = new InterfaceEthernet(l2, ip, mask, mac, bootstrap);
  //  }
  //else
  //  {
      return AddInterface(l2, ip, mask, mac, bootstrap);
  //  }

  //pif->SetNode(pNode);
  //interfaces.push_back(pif);
  //if (!IPKnown())
  //  { // Default IP address to first interface
  //    SetIPAddr(ip);
  //  }
  //return pif;
}

Count_t NodeImpl::InterfaceCount()
//��ȡ��ý��󶨵Ľӿڵ���Ŀ������ֵ�ǽӿ���Ŀ
{
  return interfaces.size();
}

const IFVec_t& NodeImpl::Interfaces()
/*
��������ȡ�뱾�ؽ��󶨵����нӿڵ���Ϣ
����ֵ��Vector����ָ�룬���а�������ý��󶨵����нӿڵ�ָ��
*/
{ 
  return interfaces;
}

Interface* NodeImpl::GetIfByLink(Link* l)
/*
���������ݱ߲��Ҷ�Ӧ�ӿ�
������l�����뱾�ؽ��������ĳ����
����ֵ�����ؽ����l��Ӧ�Ľӿڵ�ָ�롣��û���ҵ������ؿ�
*/
{
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i]->GetLink() == l) return interfaces[i]; //�ҵ���Ӧ�Ľӿ�
    }
  return nil;  // Does not exist
}

////void NodeImpl::UseWormContainment(bool b) 
////{
////  usewormcontainment = b;
////}

Interface* NodeImpl::GetIfByNode(Node* n)
/*
���������ݵ���Ҷ�Ӧ�ӿ�
������n�����뱾�ؽ����ͨ��Զ�̽��
����ֵ�����ؽ����n��Ӧ�Ľӿڵ�ָ�룬��û���ҵ������ؿ�
*/
{ 
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      Link* pLink = interfaces[i]->GetLink();
      if (!pLink) continue;
      if (pLink->NodeIsPeer(n)) return interfaces[i];//�ҵ���Ӧ�Ľӿ�
    }
  return nil;
}

Interface* NodeImpl::GetIfByIP(IPAddr_t ip)
/*
���������ݱ��ؽӿ�IP��ַ���Ҷ�Ӧ�ӿ�
������n�����뱾�ؽ����ͨ��Զ�̽��
����ֵ�����ؽ����n��Ӧ�Ľӿڵ�ָ�룬��û���ҵ������ؿ�
*/
{
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if ((IPAddr_t)interfaces[i]->GetIPAddr() == ip)
        {
          return interfaces[i]; //�ҵ���Ӧ�Ľӿ�
        }
    }
  return nil;
}


Interface* NodeImpl::AddDuplexLink(Node* remotenode)
/*
����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ΪĬ�ϡ�
������remotenode����Զ�̽��ָ��
����ֵ������˫����·����Ӧ�ı��ؽӿ�ָ��
*/
{
  DuplexLink dl(pNode, remotenode);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* remotenode, const Linkp2p& l)
/*
����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ָ��
������remotenode����Զ�̽��ָ��
      l��������˫����·������
����ֵ������˫����·����Ӧ�ı��ؽӿ�ָ��
*/
{ 
  DuplexLink dl(pNode, l, remotenode, l);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Interface* li, Interface* ri)
/*
�������½�һ����������ָ���ӿڵ�˫����·��˫����·����Ĭ��
������li�������ؽ��ӿ�ָ��
	  ri����Զ�̽��ӿ�ָ��
����ֵ�����������ӿ�ָ��
*/
{ 
  DuplexLink dl(li, ri);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Interface* li, 
								   Interface* ri,
                                   const Linkp2p& l)
/*
�������½�һ����������ָ���ӿڵ�˫����·��˫����·����Ĭ��
������li�������ؽ��ӿ�ָ��
      ri����Զ�̽��ӿ�ָ��
      l��������˫����·����
����ֵ�����������ӿ�ָ��
*/
{
  DuplexLink dl(li, l, ri, l);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* remotenode,
                                   IPAddr_t li, 
								   Mask_t lm,
                                   IPAddr_t ri, 
								   Mask_t rm)
/*
����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ΪĬ�ϡ�Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
������remotenode����Զ�̽��ָ��
      li����Ϊ���ؽ�������ӿڷ����IP��ַ
      lm����Ϊ���ؽ�������ӿڷ��������
      ri����ΪԶ�̽�������ӿڷ����IP��ַ
      rm����ΪԶ�̽�������ӿڷ��������
����ֵ�����������ӿ�ָ��
*/
{
  DuplexLink dl(pNode, li, lm, remotenode, ri, rm);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* rn, 
								   const Linkp2p& l,
                                   IPAddr_t li, 
								   Mask_t lm,
                                   IPAddr_t ri, 
								   Mask_t rm)
/*
����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ΪĬ�ϡ�Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
������remotenode����Զ�̽��ָ��
	  l��������˫����·����
	  li����Ϊ���ؽ�������ӿڷ����IP��ַ
	  lm����Ϊ���ؽ�������ӿڷ��������
	  ri����ΪԶ�̽�������ӿڷ����IP��ַ
	  rm����ΪԶ�̽�������ӿڷ��������
*/
{
  DuplexLink dl(pNode, li, lm, l, rn, ri, rm, l);
  return dl.localif;
}

Link* NodeImpl::GetLink(Node* remotenode)
/*
����:������ָ�������ͨ��˫����·
������n����Զ�̽��
����ֵ�������ҵ�˫����·���ڣ��򷵻�����ָ�룬���򷵻ؿ�
*/
{
  Interface* i = GetIfByNode(remotenode); //��Զ�̽����ͨ�ı��ؽ��ӿ�
  if (!i) return nil;            //�ӿڲ�����
  return i->GetLink();           //�ӿڴ��ڣ����ض�Ӧ��˫����·ָ��
}

//v1.01
//Interface* NodeImpl::AddRemoteLink(IPAddr_t ip, Mask_t mask)
///*
//���������ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á�˫����·�Ĵ�����ӳ�Ĭ��
//������ip����Ϊ���ؽ�������ӿڷ����IP��ַ
//      mask����Ϊ���ؽ�������ӿڷ��������
//����ֵ�����������ӿ�ָ��
//*/
//{
//  Interface* iFace = AddInterface(L2Proto802_3(), ip, mask);
//  iFace->SetLink(new LinkRTI(ip, mask, iFace));
//  return iFace;
//}
//
//Interface* NodeImpl::AddRemoteLink(IPAddr_t ip, Mask_t mask, Rate_t rate, Time_t delay)
///*
//���������ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á�˫����·�Ĵ�����ӳ�ָ��
//������ip����Ϊ���ؽ�������ӿڷ����IP��ַ
//	  mask����Ϊ���ؽ�������ӿڷ��������
//	  rate��������˫����·�Ĵ���
//	  delay��������˫����·�Ĵ����ӳ�
//����ֵ�����������ӿ�ָ��
//*/
//{
//  Interface* iFace = AddInterface(L2Proto802_3(), ip, mask);
//  iFace->SetLink(new LinkRTI(ip, mask, iFace, rate, delay, 0));
//  return iFace;
//}


Interface* NodeImpl::AddSimplexLink(Node* remotenode)
/*
����������Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ΪĬ�ϡ�
������remotenode����Զ�̽��ָ��
����ֵ��������·����Ӧ�ı��ؽӿ�ָ��
*/
{
  return AddSimplexLink(remotenode, Linkp2p::Default(), IPADDR_NONE, Mask(32));
}

Interface* NodeImpl::AddSimplexLink(Node* remotenode, const Linkp2p& l)
/*
����������Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ��
������remotenode����Զ�̽��ָ��
	  l����������·������
����ֵ��������·����Ӧ�ı��ؽӿ�ָ��
*/
{
  return AddSimplexLink(remotenode, l, IPADDR_NONE, Mask(32));
}

Interface* NodeImpl::AddSimplexLink(Node* remotenode, 
									const Linkp2p& l,
                                    IPAddr_t ip,
									Mask_t mask)
/*
����������Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ�������ؽӿ�IP��ַ������ָ����
������remotenode����Զ�̽��ָ��
	  l����������·������
   	  ip����������·��Ӧ���ؽӿڵ�ip��ַ
	  mask����������·��Ӧ���ؽӿڵ�����
����ֵ��������·����Ӧ�ı��ؽӿ�ָ��
*/
{
  Interface* localif = AddInterface(L2Proto802_3(), ip, mask);
  Interface* remoteif = remotenode->AddInterface(L2Proto802_3(), IPADDR_NONE, Mask(32));
  Linkp2p* lp = (Linkp2p*)l.Copy();
  localif->SetLink(lp);
  lp->SetPeer(remoteif);
  lp->SetLocalIf(localif);
  return localif;
}

void NodeImpl::Neighbors(NodeWeightVec_t& neighbornodes, bool noLeaf)
/*
��������ȡ���е��ڽ���㣬���巽����ͨ��ѯ�ʱ��ؽ���ÿһ���ӿ�
������[OUT]neighbornodes���������˱��ؽ�����е��ٽ���㡢��֮��صĽӿں���·��Ȩ��
      noLeaf�����Ƿ��ڲ��ҹ����к��Է�·�ɽ�㣬Ĭ��Ϊfalse��������
*/
{ 
  NodeReal::count3++;
  for (IFVec_t::size_type i = 0; i != interfaces.size(); ++i)
    {
      Interface* iface = interfaces[i];
  //    if (iface->IsWireless()) continue; // ? Is this right?  v1.01
      if (noLeaf)
        { //��鵱ǰ·���еķ�·�ɽ�㣬true����ԣ�false�򲻺���
          if (iface->NeighborCount(pNode) <= 1)
            { // Only one neighbor, find it
              // ?? Why is this so hard?
              Link* l = iface->GetLink();
              if (l)
                {
                  InterfaceBasic* pPeer = l->GetPeer((Packet*)nil);
                  if (pPeer)
                    {
                      Node* neighbornodes = pPeer->GetNode();
                      if (neighbornodes)
                        {
                          if (neighbornodes->NeighborCount() <= 1)
                            { // ���鵽��һ����·�ɵĽ��
                              IPAddr_t ip = neighbornodes->GetIPAddr();
                              // ����Ǳ���·�ɣ�����
                              if (iface->IsLocalIP(ip)) continue;
                            }
                        }
                    }
                }
            }
        }
      DEBUG(1,(cout << "Node " << pNode->Id() 
               << " asking if about neighbors " << endl));
      iface->Neighbors(neighbornodes);
    }
}

Count_t NodeImpl::NeighborCount()
/*
��������ȡ·�ɱ����ڽ�������Ŀ��ͨ����ÿ���ӿڵ��ڽӽ�����
����ֵ���ڽӽ��ĸ���
*/
{ 
  if (neighborCount) return neighborCount;//���ǰ��������ֱ�ӷ���
   
  Count_t t = 0;
  for (IFVec_t::iterator i = interfaces.begin(); i != interfaces.end(); ++i)
    {
      t += (*i)->NeighborCount(pNode);
    }
  neighborCount = t;
  return t;
}

void NodeImpl::NeighborsByIf(Interface* i, IPAddrVec_t& v)
/*
��������ȡ�뱾�ؽ��ָ���ӿ��������ڽӽ��IP��ַ
������i�������ؽ��ָ���ӿ�
	  [OUT]v��������ȡ��һ���ڽӽ��IP��ַ
*/
{
  NodeReal::count2++;
  NodeWeightVec_t nwv;
  //����ͨ���ýӿ����ӵ��ڽӽ��
  i->Neighbors(nwv, true); //�������н�㣬��������̫������
  for (NodeWeightVec_t::iterator j = nwv.begin(); j != nwv.end(); ++j)
    {
      v.push_back(j->node->GetIPAddr()); //����һ���ڽӽ���IP��ַ
    }
}


Interface* NodeImpl::LocalRoute(IPAddr_t ip)
//�ж�ĳIP��ַ�Ƿ���ֱ���뱾�ؽ��ĳ�ӿ������������򷵻ظýӿ�
{ 
	DEBUG0((cout << "NodeImpl::LocalRoute, id " << pNode->Id() << endl));
	NodeReal::count1 += interfaces.size();
	for (IFVec_t::iterator i = interfaces.begin(); i != interfaces.end(); ++i)
	{
		if ((*i)->IsLocalIP(ip)) return *i; // found it
	}
	return nil; // Not local
}

Count_t NodeImpl::GetNix(Node* n) const
//����ָ���������ڽӽ�������
{
	Count_t nix = 0;
	// Scan the interfaces looking for specified neighbor
	for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
	{
		if (interfaces[i]->NodeIsPeer(n))
		{ // Found it
			return nix + interfaces[i]->NodePeerIndex(n);
		}
		nix += interfaces[i]->NeighborCount(pNode); // Advance neighbor count
	}
	return MAX_COUNT; // Not found
}

#ifdef HAVE_QT

CustomShape_t NodeImpl::CustomShape()
//����һ��ͼ�εĻص���������δ�����أ��ɺ���
{
  return nil; 
}

void NodeImpl::CustomShape(CustomShape_t)
//��δ�����أ��ɺ���
{ 
}

bool NodeImpl::CustomShapeFile(const char*)
{ 
  return true;
}

bool NodeImpl::CustomShapeImage(const Image&)
{
  return true;
}

#endif

////bool NodeImpl::ICMPEnabled() const
////// ��鱾�ؽ���Ƿ�֧��ICMP,��node��real������
////{
////  return false; 
////}
////
////void NodeImpl::DisableICMP()
//////�趨���ؽ�㲻֧��ICMP����node��real������
////{
////  
////}

// Failure management
void NodeImpl::Down()
//�趨�ý����Ч���޷�������ת�����ݰ�
{ //�趨ÿ���ӿ���Ч
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Down(true);
    }
  down = true;
  Simulator::instance->TopologyChanged(false);
#ifdef HAVE_QT  
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

void NodeImpl::Up()
//�趨�ý���Ѿ��ָ���������Ч
{ //�趨ÿ���ӿ�״̬Ϊup
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Up();
    }
  down = false;
  Simulator::instance->TopologyChanged(true);
#ifdef HAVE_QT
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

bool NodeImpl::IsDown()
//�鿴����Ƿ���Ч������Ч�򷵻�true���������򷵻�false
{
  return down;
}



  
