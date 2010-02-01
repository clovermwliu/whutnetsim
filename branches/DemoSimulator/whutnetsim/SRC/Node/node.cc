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
#include <math.h>

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "node.h"
#include "node-impl.h"
#include "node-real.h"
//#include "node-ghost.h"
#include "protograph.h"
#include "portdemux.h"
#include "ipaddr.h"
#include "duplexlink.h"
#include "application.h"
#include "mobility.h"
#include "routing-nixvector.h"
#include "interface-real.h"  //v1.01
//#include "interface-wireless.h"
//#include "distributed-simulator.h" v1.01

using namespace std;

NodeId_t  Node::nextId = 0;  //Ҫ�������һ�����Id
NodeVec_t Node::nodes;       //Vector���������������еĽ��
#ifdef HAVE_QT //v1.01
Count_t   Node::defaultPixelSize = 10;  //�����У��������ش�С
Node::Shape_t Node::defaultShape = Node::SQUARE;  //���н���Ĭ����״:����
#endif
double    Node::defaultMaxSpeed = 20;  //���Ĭ�ϵ�������ʣ�20

bool operator==(const Location& l, const Location& r)
/*
������λ�ñȽ������"=="���ж�Location l��r��xyz����ϵ���Ƿ���ȣ�����ȷ���true�����򷵻�false
*/
{
  return (l.X() == r.X() && l.Y() == r.Y() && l.Z() == r.Z());
}

Node::Node(SystemId_t sysId) : id(nextId++),
  proxyIP(IPADDR_NONE), proxyMask(0)
/*
����������һ��ģ�����˽�㡣��ѡ����ֻ�Էֲ�ʽģ�������ã���ָ��ά���ý��״
̬�ķ������������ָ��id��ָ��ϵͳid��ƥ�䣬��ô�ᴴ��һ��Real��㡣������ǣ�
�򴴽�һ��Ghost���             
������SystemId_t sysId ���� �ֲ�ʽϵͳ��ʶ��                                                      
����ֵ����                                                                                       
��ע���洢���н���Vector�����£��������½��Ľ�㡣                                              
*/
{
  ////if (sysId != DistributedSimulator::systemId)
  ////  { //ӳ�䵽��ͬ���߼������У������Ƿַ�ģ�����е�Id���򴴽�����ghost���
  ////    pImpl = new NodeGhost(this);
  ////  }
  ////else
  ////  { //������Real���
      pImpl = new NodeReal(this);
    ////}
  nodes.push_back(this);   //���ý����ӵ���֪������ڵ�Vector��
}

Node::Node(NodeImpl* ni, SystemId_t sysId)
    : id(nextId++), proxyIP(IPADDR_NONE), proxyMask(MASK_NONE)
/*
����������һ��ָ����ģ�����˽�㡣��ѡ����ֻ�Էֲ�ʽģ�������ã���ָ��ά���ý��״̬��ģ�����������ָ��id��
      ��������ָ��ϵͳid��ƥ�䣬��ô�ᴴ��һ��Real��㡣������ǣ��򴴽�һ��Ghost��㡣             
������ni����ָ���Ľ�����
      SystemId_t sysId ���� �ֲ�ʽϵͳ��ʶ��                                                     
����ֵ����                                                                                      
��ע���洢���н���Vector�����£��������½��Ľ�㡣                                              
*/
{
  ////if (sysId != DistributedSimulator::systemId)
  ////  { 
  ////    pImpl = new NodeGhost(this);
  ////    delete ni; 
  ////  }
  ////else
  ////  { 
      pImpl = ni;
	////}
  nodes.push_back(this);  
}

Node::~Node()
//ɾ���ý��
{
  nextId = 0;
  
  delete pImpl;                    
}

IPAddr_t  Node::GetIPAddr()
//���ظý�����ĵ�һ��IP��ַ����Ϊһ�������������ӿڰ󶨣���˿��ܴ��ڶ��IP��ַ
{ 
  return pImpl->GetIPAddr(); 
}

void  Node::SetIPAddr(IPAddr_t i)
//���øý���IP��ַΪ�������i
{ 
  pImpl->SetIPAddr(i);
}

void Node::IPAddrs(IPMaskVec_t& IPMaskVect)
/*
������Ϊ�ý�㴴��һ��IP��ַ������
������[OUT]IPMaskVect�����������µ�IP��ַ��������IP��ַ�б�
��ע��һ�������ж���ӿڣ�ÿ���ӿڶ���һ���������IP��ַ�����롣
*/
{ 
  pImpl->IPAddrs(IPMaskVect);
}

bool Node::LocalIP( IPAddr_t ip)
//�ж�ip�Ƿ�Ϊ�����IP��ַ�����򷵻�true
{ 
  return pImpl->LocalIP(ip);
}

bool Node::IPKnown()
//�ж��Ƿ������һ����Ĭ�ϵ�IP��ַ�����򷵻�true
{ 
  return pImpl->IPKnown();
}

bool Node::IsReal()
//�ж��Ƿ���һ��Real���͵Ľ�㣬���򷵻�true
{
  return pImpl->IsReal();
}

void Node::IsSwitchNode(bool b)
//�Ƿ񽫸ý����Ϊ�����������򷵻�true
{ 
  return pImpl->IsSwitchNode(b);
}

bool Node::IsSwitchNode()
//�жϸý���Ƿ���һ��������
{
  return pImpl->IsSwitchNode();
}

void Node::PacketRX(Packet* p, Interface* iface)
//������������յ�һ�����ݰ�����ú�����ִ��
{ 
  return pImpl->PacketRX(p, iface);
}

Interface* Node::AddInterface(Interface* pif)
/*
���������һ���Ѿ����úõĽӿ�pif
����ֵ:���뱾�ؽ���Ժ�Ľӿڵ�ָ��
*/
{
  return pImpl->AddInterface(pif);
}

Interface* Node::AddInterface(const L2Proto& l2, bool bootstrap)
/*
����������µĿսӿڣ��ýӿڷ���ڶ�������Э�飬û��IP��ַ������
������l2����Ϊ�����ӿڷ���ĵڶ�������Э�����Ĭ��Ϊ802.3
      bootstrap������MAC��ַ��صĲ����������Ѿ���Ϊfalse
����ֵ:���뱾�ؽ���Ժ�Ľӿڵ�ָ��
*/
{
  return pImpl->AddInterface(l2, bootstrap);
}

Interface* Node::AddInterface(const L2Proto& l2,
                              IPAddr_t ip, 
							  Mask_t mask,
			                  MACAddr mac, 
							  bool bootstrap)
/*
������Ϊ�ý������ָ��IP��ַ������Ľӿڣ�MAC��ַĬ�ϣ�������ڶ�������Э��
������l2����Ϊ�����ӿڷ���ĵڶ�������Э�����Ĭ��Ϊ802.3
      ip����Ϊ�����ӿڷ����IP��ַ
	  mask����Ϊ�����ӿڷ��������
      mac����Ϊ�����ӿڷ����MAC��ַ���õ�ַ��Ĭ�ϵ�
      bootstrap������MAC��ַ��صĲ����������Ѿ���Ϊfalse
����ֵ�����뱾�ؽ���Ժ�Ľӿڵ�ָ��
*/
{
  return pImpl->AddInterface(l2, ip, mask, mac, bootstrap);
}

Interface* Node::AddInterface(const L2Proto& l2,
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
{
  return pImpl->AddInterface(l2, iface, ip, mask, mac, bootstrap);
}

Count_t Node::InterfaceCount()
//��ȡ��ý��󶨵Ľӿڵ���Ŀ������ֵ�ǽӿ���Ŀ
{
  return pImpl->InterfaceCount();
}

const IFVec_t& Node::Interfaces()
/*
��������ȡ�뱾�ؽ��󶨵����нӿڵ���Ϣ��
����ֵ��Vector����ָ�룬���а�������ý��󶨵����нӿڵ�ָ��
*/
{ 
  return pImpl->Interfaces();
}

Interface* Node::GetIfByLink(Link* l)
/*
���������ݱ߲��Ҷ�Ӧ�ӿ�
������l�����뱾�ؽ��������ĳ����
����ֵ�����ؽ����l��Ӧ�Ľӿڵ�ָ�룬��û���ҵ������ؿ�
*/
{
  return pImpl->GetIfByLink(l);
}

Interface* Node::GetIfByNode(Node* n)
/*
���������ݵ���Ҷ�Ӧ�ӿ�
������n�����뱾�ؽ����ͨ��Զ�̽��
����ֵ�����ؽ����n��Ӧ�Ľӿڵ�ָ�룬��û���ҵ������ؿ�
*/
{ 
  return pImpl->GetIfByNode(n);
}

Interface* Node::GetIfByIP(IPAddr_t ip)
/*
���������ݱ��ؽӿ�IP��ַ���Ҷ�Ӧ�ӿ�
������n�����뱾�ؽ����ͨ��Զ�̽��
����ֵ�����ؽ����n��Ӧ�Ľӿڵ�ָ�룬��û���ҵ������ؿ�
*/
{
  return pImpl->GetIfByIP(ip);
}

void Node::Broadcast(Packet* pack, Proto_t proto)
/*
������ͨ���ý������нӿڹ㲥���ݰ���
������pack-��Ҫ�㲥�����ݰ���ָ��
      proto-�㲥���ݰ���ͷ��ӵ�Э������
��ע�������ݰ��İ�ͷ��ӵ�����Э�顣ÿ���ӿڸ���һ�ݶ��������ݰ������н������ݰ���Ȼ��Ч��������Ҫ���ݰ�ʱ��ɾ����
*/
{ 
  return pImpl->Broadcast(pack, proto);
}

// Links.
// All of the following are just syntactic shortcuts to the DuplexLink object.
// Each creates a local DuplexLink object, which is destroyed on exit.
Interface* Node::AddDuplexLink(Node* remotenode)
 /*
 ����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ΪĬ�ϡ�
 ������remotenode����Զ�̽��ָ��
 ����ֵ������˫����·����Ӧ�ı��ؽӿ�ָ��
 */
{
  return pImpl->AddDuplexLink(remotenode);
}

Interface* Node::AddDuplexLink(Node* remotenode, const Linkp2p& l)
/*
����������Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ�˫����·����ָ��
������remotenode����Զ�̽��ָ��
      l��������˫����·������
����ֵ������˫����·����Ӧ�ı��ؽӿ�ָ��
*/
{ 
  return pImpl->AddDuplexLink(remotenode, l);
}

Interface* Node::AddDuplexLink(Interface* li, Interface* ri)
/*
�������½�һ����������ָ���ӿڵ�˫����·��˫����·����Ĭ��
������li�������ؽ��ӿ�ָ��
      ri����Զ�̽��ӿ�ָ��
����ֵ�����������ӿ�ָ��
*/
{ 
  return pImpl->AddDuplexLink(li, ri);
}

Interface* Node::AddDuplexLink(Interface* li, 
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
  return pImpl->AddDuplexLink(li, ri, l);
}

// Interface* Node::AddDuplexLink(Interface* li, Interface* ri,
//                                const Linkp2p& l, Node* rn)
// {
//   return pImpl->AddDuplexLink(li, ri, l, rn);
// }

Interface* Node::AddDuplexLink(Node* remotenode,
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
  return pImpl->AddDuplexLink(remotenode, li, lm, ri, rm);
}

Interface* Node::AddDuplexLink(Node* remotenode, 
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
����ֵ�����������ӿ�ָ��
*/
{
  return pImpl->AddDuplexLink(remotenode, l, li, lm, ri, rm);
}

Link* Node::GetLink(Node* remotenode)
/*
����:������ָ�������ͨ��˫����·
������n����Զ�̽��
����ֵ�������ҵ�˫����·���ڣ��򷵻�����ָ�룬���򷵻ؿ�
*/
{
  return pImpl->GetLink(remotenode);
}
//v1.01
//Interface* Node::AddRemoteLink(IPAddr_t ip, Mask_t mask)
///*
//���������ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á�˫����·�Ĵ�����ӳ�Ĭ��
//������ip����Ϊ���ؽ�������ӿڷ����IP��ַ
//      mask����Ϊ���ؽ�������ӿڷ��������
//����ֵ�����������ӿ�ָ��
//*/
//{
//  return pImpl->AddRemoteLink(ip, mask);
//}
//
//Interface* Node::AddRemoteLink(IPAddr_t ip, Mask_t mask, Rate_t rate, Time_t delay)
///*
//���������ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á�˫����·�Ĵ�����ӳ�ָ��
//������ip����Ϊ���ؽ�������ӿڷ����IP��ַ
//      mask����Ϊ���ؽ�������ӿڷ��������
//	  rate��������˫����·�Ĵ���
//	  delay��������˫����·�Ĵ����ӳ�
//����ֵ�����������ӿ�ָ��
//*/
//{
//  return pImpl->AddRemoteLink(ip, mask, rate, delay);
//}

Interface* Node::AddSimplexLink(Node* remotenode)
/*
����������Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ΪĬ�ϡ�
������remotenode����Զ�̽��ָ��
����ֵ��������·����Ӧ�ı��ؽӿ�ָ��
*/
{
  return pImpl->AddSimplexLink(remotenode);
}

Interface* Node::AddSimplexLink(Node* remotenode, const Linkp2p& l)
/*
����������Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ��
������remotenode����Զ�̽��ָ��
	  l����������·������
����ֵ��������·����Ӧ�ı��ؽӿ�ָ��
*/
{
  return pImpl->AddSimplexLink(remotenode, l);
}

Interface* Node::AddSimplexLink(Node* remotenode, 
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
  return pImpl->AddSimplexLink(remotenode, l, ip, mask);
}

Queue* Node::GetQueue()
/*
��������ȡ���ؽ��Ķ��У����Ե��ӿڵĽ����Ч��
����ֵ:���ر��ص��ӿڽ��Ķ��У������û�нӿڣ������ж���ӿڣ��򷵻ؿ�
*/
{ 
  return pImpl->GetQueue();
}

Queue* Node::GetQueue(Node* targetnode)
/*
��������ȡ���ؽ��ͨ��ָ��Ŀ����Ķ���
����ֵ:�����ؽ����Ŀ������ͨ���ö��д��ڣ��򷵻�����ָ�룬���򷵻ؿ�
*/
{ 
  return pImpl->GetQueue(targetnode);
}


Application* Node::AddApplication(const Application& app)
/*
�����������ؽ�����ָ��Ӧ��
������app����Ӧ�ö�������ȷ������Ӧ�õ�����
����ֵ�����ظ�����Ӧ�ö���ָ��
*/
{
  return pImpl->AddApplication(app);
}

ApplicationVec_t* Node::GetApplication()
/*
��������ѯ���ڵ�Ӧ��
*/
{
	return pImpl->GetApplicationVec(); 
}
////
////
////void Node::UseWormContainment(bool b) 
////{
////  return pImpl->UseWormContainment(b);
////}
////
////
////bool Node::UseWormContainment()
////{ 
////  return pImpl->UseWormContainment();
////}
////
////void Node::SetWormContainment(WormContainment* cont)
////{
////  return pImpl->SetWormContainment(cont);
////}
////
////WormContainment* Node::GetWormContainment()
////{
////  return pImpl->GetWormContainment();
////}
////
////bool Node::FirstBitRx()
////{
////  return pImpl->FirstBitRx();
////}
////
////void Node::FirstBitRx(bool is)
////{
////  pImpl->FirstBitRx(is);
////}
////
////void Node::UseARP(bool b)
////{
////  return pImpl->UseARP(b);
////}

void Node::Neighbors(NodeWeightVec_t& neighbornodes, bool noLeaf)
/*
��������ȡ���е��ڽ���㣬���巽����ͨ��ѯ�ʱ��ؽ���ÿһ���ӿ�
������[OUT]neighbornodes���������˱��ؽ�����е��ٽ���㡢��֮��صĽӿں���·��Ȩ��
      noLeaf�����Ƿ��ڲ��ҹ����к��Է�·�ɽ�㣬Ĭ��Ϊfalse��������
*/
{ 
  pImpl->Neighbors(neighbornodes, noLeaf);
}

void Node::AddNeighbor(Node*, Weight_t)
{//��ghost�ͽ�㲻�ᱣ���ڽӽ���

}

Count_t Node::NeighborCount()
/*
��������ȡ·�ɱ����ڽ�������Ŀ��ͨ����ÿ���ӿڵ��ڽӽ����ӣ�
����ֵ���ڽӽ��ĸ���
*/
{ 
  return pImpl->NeighborCount();
}


void Node::NeighborsByIf(Interface* i, IPAddrVec_t& v)
/*
��������ȡ�뱾�ؽ��ָ���ӿ��������ڽӽ��IP��ַ
������i�������ؽ��ָ���ӿ�
      [OUT] v��������ȡ��һ���ڽӽ��IP��ַ
*/
{
  return pImpl->NeighborsByIf(i, v);
}

void Node::DefaultRoute( RoutingEntry r)
// ָ���������ݰ���Ĭ��·����
{
	pImpl->DefaultRoute(r);
}

void Node::DefaultRoute( Node* n)
//ָ���������ݰ���Ĭ����һ�����
{ 
	pImpl->DefaultRoute(n);
}

void Node::AddRoute( IPAddr_t ip, Count_t mask, Interface* i, IPAddr_t ip1)
/*
����������һ��·���Ŀ��IP�������롢���ؽӿڡ���һ��IP��ַȷ��
������ip������·�����Ŀ��IP
	mask������·����Ŀ��IP��Ӧ������
	i�������ؽ�㵽Ŀ�ĵ���һ��ʹ�õ��ı��ؽӿ�
	ip1������һ������IP��ַ
*/
{ 
	pImpl->AddRoute(ip, mask, i, ip1);
}

RoutingEntry Node::LookupRoute(IPAddr_t ip)
//����Ŀ��IP��ַ���ұ���·����������򷵻�
{ 
	return pImpl->LookupRoute(ip);
}

RoutingEntry Node::LookupRouteNix(Count_t nix)
//�����ڽӽ���������ұ���·����������򷵻�
{
	return pImpl->LookupRouteNix(nix);
}
////
////AODVRoutingEntry *
////Node::LookupRouteAODV(IPAddr_t ip) {
////	return pImpl->LookupRouteAODV(ip);
////}
////
////void
////Node::SetRoutingAODV(void *pRouting){
////	return pImpl->SetRoutingAODV(pRouting);
////}
////
////RoutingAODV *
////Node::GetRoutingAODV(){
////	return pImpl->GetRoutingAODV();
////}

Routing::RType_t Node::RoutingType()
//��ȡ���ؽ�㵱ǰ��ʹ�õ�·������
{
	return pImpl->RoutingType();
}

Interface* Node::LocalRoute(IPAddr_t ip)
//�ж�ĳIP��ַ�Ƿ���ֱ���뱾�ؽ��ĳ�ӿ������������򷵻ظýӿ�
{ 
	return pImpl->LocalRoute(ip);
}

void       Node::InitializeRoutes()
//·�ɳ�ʼ����Ϊ·��Э����׼��
{ 
	pImpl->InitializeRoutes();
}

void       Node::ReInitializeRoutes(bool  status)
/*
�����������˷����仯����Ҫ���³�ʼ��·��
������status��������ӿ��Ƿ�����
*/
{
	pImpl->ReInitializeRoutes(status);
}

Count_t    Node::RoutingFibSize() const
//ͳ�Ʊ��ؽ��·�ɵĴ�С
{ 
	return pImpl->RoutingFibSize();
}

Count_t    Node::GetNix(Node* n) const
//����ָ���������ڽӽ�������
{ 
	return pImpl->GetNix(n);
}

Routing* Node::GetRouting()
//���ر��ؽ����ʹ�õ�·�ɶ����ָ��
{
	return pImpl->GetRouting();
}

RoutingNixVector* Node::GetNixRouting()
//�����ؽ�����NixVector����·�ɣ��򷵻ظ�·�ɶ����ָ�룬���򷵻ؿ�
{ 
  return pImpl->GetNixRouting();
}

void Node::DumpNV()
//����ʹ��
{
  RoutingNixVector* r = pImpl->GetNixRouting();
  if (!r) return;
  r->DBDump(this);
}

// Protocol Graph Interface
#ifdef COMPACT
// No private protocol graphs in compact mode
Protocol* Node::LookupProto(Layer_t l, Proto_t p) // Lookup protocol by layer
{
  return pImpl->LookupProto(l, p);
}

void      Node::InsertProto(Layer_t l, Proto_t p, Protocol* pp) // Insert proto
{
  pImpl->InsertProto(l, p, pp);
}
#else
Protocol* Node::LookupProto(Layer_t l, Proto_t proto)
//�ڱ��ؽ�㣬����Э���ͱ�Ų��Ҷ�Ӧ��Э�����
{
  return pImpl->LookupProto(l, proto);
}

void Node::InsertProto(Layer_t l, Proto_t p, Protocol* pp)
//�ڱ��ؽ�����һ��Э�����.Э���ͱ��ָ��
{ 
  pImpl->InsertProto(l, p, pp);
}
#endif

// Port Demux Interface
bool Node::Bind(Proto_t proto, PortId_t port , Protocol* p)
/*
������Ϊָ���˿ڰ�ָ��Э��
������proto����Э���
	  port����ָ���Ķ˿�
	  p�������󶨵�Э��
���أ��Ƿ�󶨳ɹ�
*/
{
  return pImpl->Bind(proto, port, p);
}

bool Node::Bind(Proto_t proto,
                PortId_t lport, 
				IPAddr_t laddr,
                PortId_t rport, 
				IPAddr_t raddr,
                Protocol* p)
/*
������Ϊ���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
������proto��������Э���
	  lport�������ض˿ں�
	  laddr��������IP��ַ
	  rport����Զ�̶˿ں�
	  raddr����Զ��IP��ַ
	  p�������󶨵�Э��
����ֵ��true�����󶨳ɹ�
	    false������ʧ��
*/
{ 
  return pImpl->Bind(proto, lport, laddr, rport, raddr, p);
}

PortId_t Node::Bind(Proto_t proto, Protocol* p)
/*
��������ָ��Э�飬��Ӧ���ض˿����
������proto����Э���
      p��������Э��
����ֵ�����󶨵ı��ؽ��Ķ˿�
*/
{ 
  return pImpl->Bind(proto, p);
}

bool  Node::Unbind(Proto_t proto, PortId_t port, Protocol* p)
/*
������ɾ�����ؽ��ָ���ӿڰ󶨵�ָ��Э��
������proto������ɾ��Э���Э���
      port������ǰ�󶨵ı��ض˿ں�
      p������ɾ����Э��
*/
{
  return pImpl->Unbind(proto, port, p);
}

bool  Node::Unbind(Proto_t proto,
                   PortId_t lport, 
				   IPAddr_t laddr,
                   PortId_t rport, 
				   IPAddr_t raddr,
                   Protocol* p)
/*
���������ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
������proto��������Э���
	  lport�������ض˿ں�
	  laddr��������IP��ַ
	  rport����Զ�̶˿ں�
	  raddr����Զ��IP��ַ
	  p�������󶨵�Э��
����ֵ��true�����󶨳ɹ�
	    false������ʧ��
*/
{
  return pImpl->Unbind(proto, lport, laddr, rport, raddr, p);
}

Protocol* Node::LookupByPort(Proto_t proto, PortId_t port)
/*
���������ݱ��ض˿ں�Э��Ų������󶨵�Э��
������proto����Э���
	  port�������ض˿ں�
����ֵ�����ҵ���Ӧ��Э�飬���䷵�أ����򷵻ؿ�
*/
{
  return pImpl->LookupByPort(proto, port);
}

Protocol* Node::LookupByPort(Proto_t proto,
                             PortId_t sport, 
							 IPAddr_t sip,
                             PortId_t dport, 
							 IPAddr_t dip)
/*
����������Э�飬����Э��š����ؽ��˿ڡ�IP��ַ��Զ�̽��˿ڡ�IP��ַ
������proto��������Э���
	  sport�������ض˿ں�
	  sip��������IP��ַ
	  dport����Զ�̶˿ں�
	  dip����Զ��IP��ַ
����ֵ�����ҵ���Ӧ��Э�飬���䷵�أ����򷵻ؿ�
*/
{
  return pImpl->LookupByPort(proto, sport, sip, dport, dip);
}

bool Node::TracePDU(Protocol* proto, PDU* h, Packet* p, char* s)
/*
��������ָ��PDU���ݴ�ָ��Э���¼�������ļ�
������proto��������·����ָ��Э��ָ��
	  h����PDU��Ԫ
	  p����������PDU��Ԫ�����ݰ�
	  s���������ļ������ı���Ϣ
����ֵ�����ø����ļ����ڣ�����true�����򷵻�false
*/
{
  return pImpl->TracePDU(proto, h, p, s);
}

void Node::SetTrace(Trace::TraceStatus t)
/*
���������ñ��ؽ��ĸ���״̬������״̬�����֣�
	1.ENABLED�����ý������������ٵ����ݰ������ᱻ���٣�
	2.DISABLED�����ý���������ݰ������ᱻ����
	3.DEFAULT������״̬ȡ���ڱ��ؽ��󶨵�Э����
����:t��������״̬
*/
{
  pImpl->SetTrace(t);
}

void Node::SetLocation(Meters_t xl, Meters_t yl, Meters_t zl)
/*
������ֱ����xyz����ϵ�����ñ��ؽ�������
������xl����x������
      yl����y������
	  zl����z������
*/
{
  pImpl->SetLocation(xl, yl, zl);
}

void Node::SetLocation(const Location& l)
//����location�������ñ��ؽ����xyz����ϵ�е�����
{
  pImpl->SetLocation(l);
}

void Node::SetLocationLongLat(const Location& l)
{
  pImpl->SetLocationLongLat(l);
}

bool Node::HasLocation()
//��鱾�ؽ���Ƿ��ѷ����ַ�����ѷ��䣬�����棬���򷵻ؼ�
{
  return pImpl->HasLocation();
}

Meters_t Node::LocationX()
//���ر��ؽ���x����
{
  return pImpl->LocationX();
}

Meters_t Node::LocationY()
//���ر��ؽ���y����
{
  return pImpl->LocationY();
}

Meters_t Node::LocationZ()
//���ر��ؽ���z����
{
  return pImpl->LocationZ();
}

Location Node::GetLocation()
//���ر��ؽ�㵱ǰλ��
{
  return pImpl->GetLocation();
}

Location Node::UpdateLocation()
/*
���������±��ؽ���λ�á������λ�ò��ǳ������µģ�ֻ�иú���������ʱ�Ž��и���
����ֵ�����º�ı��ؽ���λ��
*/
{
  return pImpl->UpdateLocation();
}

Mobility* Node::AddMobility(const Mobility& m)
//Ϊ���ؽ������һ��mobilityģ�ͣ�����ֵΪ������mobility����ָ��
{
  return pImpl->AddMobility(m);
}

Mobility* Node::GetMobility()
//��ȡ���ؽ�㵱ǰ��mobilityģ��
{
  return pImpl->GetMobility();
}

bool Node::IsMoving()
//�жϱ��ؽ��ĵ�ǰ״̬�Ƿ�����������
{
  return pImpl->IsMoving();
}

bool Node::IsMobile()
//�жϱ��ؽ���Ƿ���mobilityģ��
{
  return pImpl->IsMobile();
}
#ifdef HAVE_QT //v1.01
void Node::Show(bool s)
//���ݲ���ȷ���Ƿ�Ҫ��ʾ����
{
  pImpl->Show(s);
}

bool Node::Show()
//������ʾ�Ƿ�Ҫ������
{
  return pImpl->Show();
}

MyCanvasItem* Node::Display(QTWindow* qc)
//ȷ������ʾ����״����С�ȣ���ʾλ��Ϊ�㵱ǰ��λ��
{
  return pImpl->Display(qc);
}

MyCanvasItem* Node::Display(const MyPoint& qp, QTWindow* qc)
//ȷ������ʾ����״����С�ȣ���ʾλ�ò������ṩ��λ��
{
  return pImpl->Display(qp, qc);
}

void Node::PixelSize(Count_t nps) 
//���õ�����ʾʱ�Ĵ�С
{
  pImpl->PixelSize(nps);
}

Count_t Node::PixelSizeX()
//���ص�����ʾʱ�Ŀ��
{
  return pImpl->PixelSizeX();
}

Count_t Node::PixelSizeY()
//���ص�����ʾʱ�ĸ߶�
{
  return pImpl->PixelSizeY();
}

void Node::Shape(Shape_t ns) 
//���õ�����ʾʱ����״
{
  pImpl->Shape(ns);
}

Node::Shape_t Node::Shape()
//���ص�����ʾʱ����״
{
  return pImpl->Shape();
}

CustomShape_t Node::CustomShape()
//����һ��ͼ�εĻص�����
{
  return pImpl->CustomShape();
}

void Node::CustomShape(CustomShape_t cs)
//����һ��ͼ�εĻص�����
{
  pImpl->CustomShape(cs);
}

bool Node::CustomShapeFile(const char* fn)
//����һ������һ��ͼ�ε��ļ������ɹ�����true�����򷵻�false
{
  return pImpl->CustomShapeFile(fn);
}

bool Node::CustomShapeImage(const Image& im)
//����һ��һ��ͼ�Σ����ɹ�����true�����򷵻�false
{
  return pImpl->CustomShapeImage(im);
}

void Node::Color(const MyColor& c)
//ָ�����ؽ�����ɫ
{
  pImpl->Color(c);
}

bool Node::HasColor()
//��鱾�ؽ���Ƿ��Ѿ�������ɫ
{
  return pImpl->HasColor();
}

MyColor& Node::Color()
//���ر��ؽ�����ɫ
{
  return pImpl->Color();
}

NodeAnimation* Node::GetNodeAnimation() const
//����ָ�����н���㶯����Ϣ���ָ��
{
  return pImpl->GetNodeAnimation();
}
#endif

////bool Node::ICMPEnabled() const
//////��鱾�ؽ���Ƿ�֧��ICMP
////{
////  return pImpl->ICMPEnabled();
////}
////
////void Node::DisableICMP()
//////�趨���ؽ�㲻֧��ICMP
////{
////  pImpl->DisableICMP();
////}



void Node::Down()
//�趨�ý����Ч���޷�������ת�����ݰ�
{
  pImpl->Down();
}

void Node::Up()
//�趨�ý���Ѿ��ָ���������Ч
{
  pImpl->Up();
}

bool Node::IsDown()
//�鿴����Ƿ���Ч������Ч�򷵻�true���������򷵻�false
{
  return pImpl->IsDown();
}

void Node::AddCallback(Layer_t l, 
					   Proto_t p,
                       PacketCallbacks::Type_t t,
                       Interface* i,
                       PacketCallbacks::Function_t f)
/*
�����������������һ���ص��������ص������Ȿ�����ջ��������Э�����������ݰ�
������l������ӻص���Э��㡣ֵΪ0�����ص������е�Э������Ч
	  p�����ص���Э��š�ֵΪ0�����ص���ָ��Э��������Э�����Ч
	  t�����ص������͡�RX�������ݰ����յĻص���TX�������ݰ�����Ļص���
	  i�����ӿ�ָ�롣�ڶ�Э����ܶ�������ָ��Ψһһ���ӿ�
	  f��������ӵĻص����������ص���������true��������������򶪵����ݰ����������������
	       Layer_t�������ص�������Э��㣩��Type_t ���ص��������ͣ���Packet*���������
		   ���ݰ�����Node*���������ݰ��Ľ�㣩��Interface*��ֻ��Եڶ�Э��㣬��������
		   ���Ľӿڣ�
*/
{
  pImpl->AddCallback(l, p, t, i, f);
}

void Node::AddCallbackHead(Layer_t l, 
						   Proto_t p,
                           PacketCallbacks::Type_t t,
                           Interface* i,
                           PacketCallbacks::Function_t f)
/*
�����������ؽ�����һ���ص��������ú����ص����ȼ����
������l������ӻص���Э��㡣ֵΪ0�����ص������е�Э������Ч
	  p�����ص���Э��š�ֵΪ0�����ص���ָ��Э��������Э�����Ч
	  t�����ص������͡�RX�������ݰ����յĻص���TX�������ݰ�����Ļص���
	  i�����ӿ�ָ�롣�ڶ�Э����ܶ�������ָ��Ψһһ���ӿ�
	  f��������ӵĻص����������ص���������true��������������򶪵����ݰ����������������
		   Layer_t�������ص�������Э��㣩��Type_t ���ص��������ͣ���Packet*���������
		   ���ݰ�����Node*���������ݰ��Ľ�㣩��Interface*��ֻ��Եڶ�Э��㣬��������
		   ���Ľӿڣ�
*/
{
  pImpl->AddCallbackHead(l, p, t, i, f);
}

void Node::DeleteCallback(Layer_t l, 
						  Proto_t p, 
						  PacketCallbacks::Type_t t,
                          Interface* i)
/*
�������ӱ��ؽ��ɾ��һ���ص�����
������l����ɾ���ص���Э��㡣ֵΪ0�����ص������е�Э������Ч
	  p�����ص���Э��š�ֵΪ0�����ص���ָ��Э��������Э�����Ч
	  t�����ص������͡�RX�������ݰ����յĻص���TX�������ݰ�����Ļص���
	  i�����ӿ�ָ�롣�ڶ�Э����ܶ�������ָ��Ψһһ���ӿ�
*/
{
  pImpl->DeleteCallback(l, p, t, i);
}

bool Node::CallCallbacks(Layer_t l, 
						 Proto_t p,
                         PacketCallbacks::Type_t t, 
						 Packet* pkt,
                         Interface* i)
/*
���������ñ��ؽ��Ļص�����
������l�������õĻص���Э��㡣ֵΪ0�����ص������е�Э������Ч
	  p�����ص���Э��š�ֵΪ0�����ص���ָ��Э��������Э�����Ч
	  t�����ص������͡�RX�������ݰ����յĻص���TX�������ݰ�����Ļص���
	  pkt��������������ݰ�
	  i�����ӿ�ָ�롣�ڶ�Э����ܶ�������ָ��Ψһһ���ӿ�
*/
{
  return pImpl->CallCallbacks(l, p, t, pkt, i);
}

#ifdef HAVE_QT

void Node::UserInformation(void* p)
/*
�������洢�û�����������κ�������Ϣ
������p�������ؽ��ָ���û�������Ϣ��ָ��
*/
{
  pImpl->UserInformation(p);
}

void* Node::UserInformation()
//����֮ǰָ�����û���Ϣָ�룬��û�з��ؿ�
{
  return pImpl->UserInformation();
}


////bool Node::WirelessTx()
////{
////  return pImpl->WirelessTx();
////}
////
////bool Node::WirelessRx()
////{
////  return pImpl->WirelessRx();
////}
////
////bool Node::WirelessCx()
////{
////  return pImpl->WirelessCx();
////}
////
////bool Node::WirelessRxMe()
////{
////  return pImpl->WirelessRxMe();
////}
////
////bool Node::WirelessRxZz()
////{
////  return pImpl->WirelessRxZz();
////}

#endif

Meters_t Node::Distance(Node* n)
/*
���������ر��ؽ�㵽ָ���ԵȽ��ľ���
������n�����ԵȽ��
����ֵ������֮��ľ���
*/
{
  return pImpl->Distance(n);
}

////void Node::BuildRadioInterfaceList(WirelessLink* l)
////{
////  pImpl->BuildRadioInterfaceList(l);
////}
////
////const RadioVec_t& Node::GetRadioInterfaceList()
////{
////  return pImpl->GetRadioInterfaceList();
////}
////
////void Node::SetRadioRange(Meters_t range)
////{
////  pImpl->SetRadioRange(range);
////}
////
////Meters_t Node::GetRadioRange()
////{
////  return pImpl->GetRadioRange();
////}

Joules_t Node::getBattery()
{
  return pImpl->getBattery();
}

void Node::setBattery(Joules_t joules)
{
  return pImpl->setBattery(joules);
}

void Node::setComputePower(double pow)
{
  return pImpl->setComputePower(pow);
}

double Node::getComputePower(void)
{
  return pImpl->getComputePower();
}

// Static Methods

Node* Node::GetNode(NodeId_t i)
{
  if (i >= nodes.size()) return nil; // Out of range
  return nodes[i];
}

void Node::Clear()
{
  nodes.clear();
}

#ifdef HAVE_QT

void Node::DefaultPixelSize(Count_t dps)
{
  defaultPixelSize = dps;
}

Count_t Node::DefaultPixelSize()
{
  return defaultPixelSize;
}

void Node::DefaultShape(Shape_t ds)
{
  defaultShape = ds;
}

Node::Shape_t Node::DefaultShape()
{
  return defaultShape;
}

#endif

void Node::DefaultMaxSpeed(double speed)
{
  defaultMaxSpeed = speed;
}

double Node::DefaultMaxSpeed()
{
  return defaultMaxSpeed;
}


// The following is for Proxy Routing alogorithms

void Node::SetProxyRoutingConfig(IPAddr_t ip, Mask mask)
{
  proxyIP = ip;
  proxyMask = mask;
}

bool Node::CanProxyRouteIP(IPAddr_t ip)
{
  return (proxyIP & proxyMask) == (ip & proxyMask);
}

IPAddr_t Node::GetProxyIP()
{
  return proxyIP;
}

Mask Node::GetProxyMask()
{
  return proxyMask;
}

int Node::GetLongestPrefixLength(IPAddr_t ip)
{
  int left = 0, right = proxyMask.NBits(), m;
  Mask_t mask1, mask2;

  if (!HasProxyRoutingConfig()) return 0;

  // I have used a binary search method that works in O(logN) time (max(N)=32)

  do {
    m=(left+right)/2;
    if (m==32) break;

    mask1 = 0xFFFFFFFF << (32-m);
    if ((proxyIP & mask1) != (ip & mask1))
      right = m-1;
    else {
      mask2 = 0xFFFFFFFF << (32- (m+1));
      if ((proxyIP & mask2) == (ip & mask2))
        left = m+1;
      else
        break; // we have found it
    }
  } while (left<=right);
  return m;
}

//void Node::setRouteTable(RouteTable* rtbl)
//{
//  return pImpl->setRouteTable(rtbl);
//}
//
//RouteTable* Node::getRouteTable(void)
//{
//  return pImpl->getRouteTable();
//}
