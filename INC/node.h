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


#ifndef __node_h__
#define __node_h__

#include <vector>
#include <deque>
#include <list>

#include "G_common_defs.h"
#include "l2proto802.3.h"
#include "mask.h"
#include "location.h"
#include "packet-callbacks.h"

class Link;
class Node;
class Interface;
class InterfaceBasic;
////class InterfaceWireless;
////class WirelessLink;
class Routing;
class Protocol;
class ProtocolGraph;
class PortDemux;
class Linkp2p;
class Queue;
class Application;
class NodeImpl;
////class RoutingAODV;
////class AODVRoutingEntry;
////class WormContainment;
class Image;

class  QTWindow;
class  NodeAnimation;
//����Qt����⣬��û��Qt���ã���ʹ��
// Define a callback for custom shapes in animation
typedef MyCanvasPolygonalItem*(*CustomShape_t)(MyCanvas*, Node*,
                                              const MyPoint&,
                                              MyCanvasPolygonalItem*);

// For radio interface list
//class RadioItem
//{
//public:
//  RadioItem() : inf(nil), dist(0) {}
//  RadioItem(InterfaceWireless* i, Meters_t d) : inf(i), dist(d) {}
//
//public:
//  InterfaceWireless* inf;
//  Meters_t           dist;
//};

//typedef std::vector<RadioItem> RadioVec_t;
//typedef std::list<RadioItem> RadioVec_t;

class NodeIfWeight
//��·��Э����ʹ��
{ 
public:
  NodeIfWeight(Node* n, InterfaceBasic* i, Weight_t w)
    : node(n), iface(i), weight(w) { }
public:
  Node*           node;    //�ڽ����
  InterfaceBasic* iface;   //�ӿ�
  Weight_t        weight;  //��·Ȩ��
};


class NodeIf { //����Ŀ�ĵؾ����Ľ��ͽӿڣ�·����ʹ�ã� 
public:
  NodeIf(Node* n, Interface* i)
    : node(n), iface(i) { }
public:
  Node*      node;
  Interface* iface;
};

class IpMask { // IP��ַ������
public:
  IpMask(IPAddr_t i, Mask_t m)
    : ip(i), mask(m) { }
public:
  IPAddr_t ip;
  Mask_t   mask;
};

typedef std::vector<Link*>        LinkVec_t;
typedef std::vector<Node*>        NodeVec_t;
typedef std::deque<NodeIf>        NodeDeque_t;
typedef std::vector<IPAddr_t>     IPAddrVec_t;
typedef std::vector<IpMask>       IPMaskVec_t;
typedef std::vector<Interface*>   IFVec_t;
typedef std::vector<NodeIfWeight> NodeWeightVec_t; 
typedef std::vector<NodeIf>       NodeIfVec_t;     
typedef std::vector<Dist_t>       DistVec_t;
typedef std::vector<Application*> ApplicationVec_t;

const Dist_t   INF  = 0xffffffff;

#include "routing.h"
#include "trace.h"
#include "macaddr.h"

class RoutingNixVector;
class Mobility;
class Packet;
  
/*
Node�ࣺ
�ڷ���������,���еĵ㶼����Node��Ķ�������ʾ�ġ���һ����㱻����ʱ�����Զ���һ��
��֮������·�ɶ���Ĭ��ΪNV·�ɣ��͵�����Э�����Ĭ��Ϊem IPV4��������㴴����
�����ӽӿں���·�������������ӣ��õ������������Ĺ��г�Ա��
*/
class Node {

public:
//�����������е����״
  typedef enum { NONE,   //��
	             CIRCLE, //Բ��
				 SQUARE, //����
				 HEXAGON,//������
				 OCTAGON,//�˱���
                 CUSTOM, //��һ��ص���ʾ
                 IMAGE   //��ʾһ��Qtͼ���ļ�
  } Shape_t;

public:

  Node(SystemId_t = 0);    //����һ��ģ�����˽��

protected:

  Node(NodeImpl*, SystemId_t = 0);  //���ڸ�Node��������SatelliteNode�ഴ����㣬�����㲻ͬ�Ľ����������

public:
  virtual ~Node();         //��������
  NodeId_t Id() { return id;}   //���ظý���id �����id�Ƿ�������ÿ������Ψһ��ʶ����һ������Ϊ0

//IP��ַ����
  IPAddr_t GetIPAddr();               //��ȡ�ý���IP��ַ 
  virtual void SetIPAddr(IPAddr_t);   //���øý���IP��ַ
  void IPAddrs(IPMaskVec_t&);         //���ظý�����нӿڵ�IP��ַ��������Ϣ
  bool LocalIP(IPAddr_t);             //�ж�IPAddr_t�Ƿ�Ϊ�����IP��ַ�����򷵻�true
  bool IPKnown();                     //�ж��Ƿ������һ����Ĭ�ϵ�IP��ַ�����򷵻�true

//���������
  bool IsReal();					  //�ж��Ƿ���һ��Real���͵Ľ�㣬���򷵻�true
  bool IsSwitchNode();				  //�жϸý���Ƿ���һ�������������򷵻�true
  void IsSwitchNode(bool);			  //�Ƿ񽫸ý����Ϊ������
  void PacketRX(Packet*, Interface*); //������������յ�һ�����ݰ�����ú�����ִ��
 
//�ӿ�
  Interface* AddInterface(Interface*);  //���һ���Ѿ����úõĽӿ�
  Interface* AddInterface(const L2Proto& = L2Proto802_3(),//����µĿսӿڣ��ýӿڷ���ڶ�������Э��
			               bool bootstrap=false);           
  Interface* AddInterface(const L2Proto&,  //Ϊ���ؽ�������ӿڣ�IP��ַ������ָ����MAC��ַĬ�ϣ�������ڶ�������Э��
                          IPAddr_t,    
						  Mask_t,      
						  MACAddr m = MACAddr::Allocate(),
			              bool bootstrap=false);
  Interface* AddInterface(const L2Proto&,   //Ϊ���ؽ�������ӿڣ�IP��ַ�����롢�ӿ�����ָ����MAC��ַĬ�ϣ�������ڶ�������Э��
	                      const Interface&, 
			              IPAddr_t,         
						  Mask_t,          
			              MACAddr = MACAddr::Allocate(),
			              bool bootstrap = false);
 //Interface* LocalSubnet(IPAddr_t);     //�ж��Ƿ��Ǳ�������
  virtual Count_t    InterfaceCount();   //��ȡ�뱾�ؽ��󶨵Ľӿڵ���Ŀ������ֵ�ǽӿ���Ŀ
  virtual const IFVec_t& Interfaces();   //��ȡ�뱾�ؽ��󶨵����нӿڵ���Ϣ��
  virtual Interface* GetIfByLink(Link*); //���ݱ߲��Ҷ�Ӧ�ӿ�
  virtual Interface* GetIfByNode(Node*); //���ݵ���Ҷ�Ӧ�ӿ�
  virtual Interface* GetIfByIP(IPAddr_t);//���ݱ��ؽӿ�IP��ַ���Ҷ�Ӧ�ӿ�
  
  void    Broadcast(Packet*, Proto_t);   //ͨ���ý������нӿڹ㲥���ݰ���                                       

//˫����·���ò������еĺ�������ֵ��Ϊ���ؽӿڡ���������Զ�̽�㽨��ȫ˫�����ӡ�
  Interface* AddDuplexLink(Node*);  //����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ΪĬ�ϡ�
  Interface* AddDuplexLink(Node*, const Linkp2p&); //����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ָ��
  Interface* AddDuplexLink(Interface*,Interface*);//�½�һ����������ָ���ӿڵ�˫����·����·����Ĭ��
  Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&);//�½�һ����������ָ���ӿڵ�˫����·����·����ָ��
  //Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&, Node*); 
  Interface* AddDuplexLink(Node*,//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ΪĬ�ϡ�Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
                           IPAddr_t, 
						   Mask_t = Mask(32),
                           IPAddr_t = IPADDR_NONE, 
						   Mask_t = Mask(32));
  Interface* AddDuplexLink(Node*,//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����Ϊָ����Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
	                       const Linkp2p&,
                           IPAddr_t, 
						   Mask_t = Mask(32),
                           IPAddr_t = IPADDR_NONE, 
						   Mask_t = Mask(32));
  //v1.01
  //Interface* AddRemoteLink(IPAddr_t, Mask_t);//���ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á���·�Ĵ�����ӳ�Ĭ��
  // Interface* AddRemoteLink(IPAddr_t, Mask_t, Rate_t, Time_t);//���ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á���·�Ĵ�����ӳ�ָ��
  Link*      GetLink(Node*); //������ָ�������ͨ����·
//������·����Զ�̽�㽨���������ӡ�
  Interface* AddSimplexLink(Node*);//����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ΪĬ�ϡ�
  Interface* AddSimplexLink(Node*, const Linkp2p&);//����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ����
  Interface* AddSimplexLink(Node*, //����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ�������ؽӿ�IP��ַ������ָ��
	                        const Linkp2p&, 
							IPAddr_t, 
							Mask_t);

//���У����ڴ洢���ݰ�
  Queue* GetQueue();//��ȡ���ؽ��Ķ��У����Ե��ӿڵĽ����Ч��
  Queue* GetQueue(Node*); //��ȡ���ؽ��ͨ��ָ��Զ�̽��Ķ���

//Ӧ��
  Application* AddApplication(const Application& a);//�����ؽ�����ָ��Ӧ��
  ApplicationVec_t* GetApplication();

//�ڽӽ�������һЩ·��Э����ʹ��
  virtual void Neighbors(NodeWeightVec_t &, bool = false); // ��ȡ���е��ڽ����
  void AddNeighbor(Node*, Weight_t); //�ú���Ϊ�գ�Ghost���͵Ľ�㱣���ڽ�����б� 
  Count_t NeighborCount();//��ȡ·�ɱ����ڽ�������Ŀ 
  void NeighborsByIf(Interface*, IPAddrVec_t&);//��ȡ�뱾�ؽ��ָ���ӿ��������ڽӽ��IP��ַ

//·��
  void DefaultRoute(RoutingEntry);//ָ���������ݰ���Ĭ��·����
  void DefaultRoute(Node*);//ָ���������ݰ���Ĭ����һ�����
  void AddRoute(IPAddr_t, Count_t, Interface*, IPAddr_t);//����һ��·���Ŀ��IP�������롢���ؽӿڡ���һ��IP��ַȷ��
  RoutingEntry LookupRoute(IPAddr_t); //����Ŀ��IP��ַ���ұ���·����������򷵻�
  RoutingEntry LookupRouteNix(Count_t);//�����ڽӽ���������ұ���·����������򷵻�
  Routing::RType_t RoutingType();//��ȡ���ؽ�㵱ǰ��ʹ�õ�·������
  Interface* LocalRoute(IPAddr_t); //�ж�ĳIP��ַ�Ƿ���ֱ���뱾�ؽ��ĳ�ӿ������������򷵻ظýӿ�
  void InitializeRoutes();//·�ɳ�ʼ����Ϊ·��Э����׼��
  void ReInitializeRoutes(bool);//���³�ʼ��·��
  Count_t RoutingFibSize() const;//ͳ�Ʊ��ؽ��·�ɱ�fib���Ĵ�С
  Count_t GetNix(Node*) const;//����ָ���������ڽӽ�������
  RoutingNixVector* GetNixRouting();//�����ؽ�����NixVector����·�ɣ��򷵻ظ�·�ɶ����ָ�룬���򷵻ؿ�
  Routing* GetRouting();//���ر��ؽ����ʹ�õ�·�ɶ����ָ��
  void DumpNV(); // �������ã���NixVector���

//AODV·��
  ////AODVRoutingEntry *LookupRouteAODV(IPAddr_t);
  ////void      SetRoutingAODV(void *pRouting);
  ////RoutingAODV* GetRoutingAODV();

//Э����ӿ�
  Protocol* LookupProto(Layer_t, Proto_t);//�ڱ��ؽ�㣬����Э���ͱ�Ų��Ҷ�Ӧ��Э�����
  void InsertProto(Layer_t, Proto_t, Protocol*); //�ڱ��ؽ�����һ��Э�����.Э���ͱ��ָ��  
  bool Bind(Proto_t, PortId_t, Protocol*);//Ϊָ���˿ڰ�ָ��Э��
  bool Bind(Proto_t,    
            PortId_t, 
			IPAddr_t,
            PortId_t, 
			IPAddr_t,
            Protocol*);//Ϊ���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  PortId_t  Bind(Proto_t, Protocol*);//��ָ��Э�飬�˿����
  bool Unbind(Proto_t, PortId_t, Protocol*);//ɾ�����ؽ��ָ���ӿڰ󶨵�ָ��Э��
  bool Unbind(Proto_t,
              PortId_t, 
			  IPAddr_t,
              PortId_t, 
			  IPAddr_t,
              Protocol*);//ɾ�����ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  Protocol* LookupByPort(Proto_t, PortId_t);//���ݱ��ض˿ں�Э��Ų������󶨵�Э��
  Protocol* LookupByPort(Proto_t,
                         PortId_t, 
						 IPAddr_t,
                         PortId_t, 
						 IPAddr_t);//����Э�飬����Э��š����ؽ��˿ڡ�IP��ַ��Զ�̽��˿ڡ�IP��ַ
  ////bool ICMPEnabled() const;//��鱾�ؽ���Ƿ�֧��ICMP
  ////void DisableICMP();//�趨���ؽ�㲻֧��ICMP

//���ݰ����ٽӿ�
  bool TracePDU(Protocol*, PDU*, Packet* = nil, char* = nil);//��ָ��PDU��Ԫ���ݴ�ָ��Э���¼�������ļ�
  void SetTrace(Trace::TraceStatus);//���ñ��ؽ��ĸ���״̬

//�������(x, y, z)�ӿ�
  void SetLocation(Meters_t, Meters_t, Meters_t=0);//ֱ����xyz����ϵ�����ñ��ؽ�������
  void SetLocation(const Location&);//����location�������ñ��ؽ����xyz����ϵ�е�����
  void SetLocationLongLat(const Location&); 
  bool HasLocation(); //��鱾�ؽ���Ƿ��ѷ����ַ
  Meters_t  LocationX();//���ر��ؽ���x����
  Meters_t  LocationY();//���ر��ؽ���y����
  Meters_t  LocationZ();//���ر��ؽ���z����
  Location  GetLocation();//���ر��ؽ�㵱ǰλ��
  Location  UpdateLocation();//���±��ؽ���λ��
  Meters_t Distance(Node*);//���ر��ؽ�㵽ָ���ԵȽ��ľ���

  Mobility* AddMobility(const Mobility&);//Ϊ���ؽ������һ��mobilityģ��
  Mobility* GetMobility();//��ȡ���ؽ�㵱ǰ��mobilityģ��
  bool IsMobile();//�жϱ��ؽ���Ƿ���mobilityģ��
  bool IsMoving();//�жϱ��ؽ��ĵ�ǰ״̬�Ƿ�����������
  NodeAnimation* GetNodeAnimation() const;//����ָ�����н���㶯����Ϣ���ָ��


//����
  ////void BuildRadioInterfaceList(WirelessLink*);
  ////const RadioVec_t& GetRadioInterfaceList();
  ////void      SetRadioRange(Meters_t range);
  ////Meters_t  GetRadioRange(void);

//�ص�����
  void AddCallback(Layer_t, 
	               Proto_t,
                   PacketCallbacks::Type_t,
                   Interface*, 
				   PacketCallbacks::Function_t);//�����������һ���ص��������ص������Ȿ�����ջ��������Э�����������ݰ�
  void AddCallbackHead(Layer_t, 
	                   Proto_t,
                       PacketCallbacks::Type_t,
                       Interface*, 
					   PacketCallbacks::Function_t);//�����ؽ�����һ���ص��������ú����ص����ȼ����
  void DeleteCallback(Layer_t, 
	                  Proto_t, 
					  PacketCallbacks::Type_t,
                      Interface*);//�ӱ��ؽ��ɾ��һ���ص�����
  bool CallCallbacks(Layer_t, 
	                 Proto_t,
                     PacketCallbacks::Type_t, 
					 Packet*, 
					 Interface* = nil);//���ñ��ؽ��Ļص�����
  void UserInformation(void*);//�洢�û�����������κ�������Ϣ
  void* UserInformation();//����֮ǰָ�����û���Ϣָ�룬��û�з��ؿ�

 
  ////bool FirstBitRx();
  ////void FirstBitRx(bool);
  ////void UseWormContainment(bool b);
  ////bool UseWormContainment();
  ////void SetWormContainment(WormContainment*);
  ////WormContainment* GetWormContainment();
  ////void UseARP(bool);
  ////bool WirelessTx();
  ////bool WirelessRx();
  ////bool WirelessCx();
  ////bool WirelessRxMe();
  ////bool WirelessRxZz();

//QT������Ϣ
  void Show(bool); //���ݲ���ȷ���Ƿ�Ҫ��ʾ����
  bool Show();//������ʾ�Ƿ�Ҫ������
  MyCanvasItem* Display(QTWindow*);//ȷ������ʾ����״����С�ȣ���ʾλ��Ϊ�㵱ǰ��λ��
  MyCanvasItem* Display(const MyPoint&, QTWindow*);//ȷ������ʾ����״����С�ȣ���ʾλ�ò������ṩ��λ��
  ////void WirelessTxColor(const MyColor&);
  ////const MyColor& WirelessTxColor();
  ////bool PushWirelessTx(MyCanvasItem*);
  ////MyCanvasItem* PopWirelessTx();
  void PixelSize(Count_t);//���õ�����ʾʱ�Ĵ�С
  Count_t PixelSizeX();//���ص�����ʾʱ�Ŀ��
  Count_t PixelSizeY();//���ص�����ʾʱ�ĸ߶�
  void Shape(Shape_t);//���õ�����ʾʱ����״
  Shape_t Shape();//���ص�����ʾʱ����״
  CustomShape_t CustomShape();//����һ��ͼ�εĻص�����
  void CustomShape(CustomShape_t);//����һ��ͼ�εĻص�����
  bool CustomShapeFile(const char*);//����һ������һ��ͼ�ε��ļ������ɹ�����true�����򷵻�false
  bool CustomShapeImage(const Image&);//����һ��һ��ͼ�Σ����ɹ�����true�����򷵻�false
  void Color(const MyColor&);//ָ�����ؽ�����ɫ
  bool HasColor();//��鱾�ؽ���Ƿ��Ѿ�������ɫ
  MyColor& Color();//���ر��ؽ�����ɫ


//���ʧ�ܹ���
  void Down();//�趨�ý����Ч���޷�������ת�����ݰ�
  void Up();//�趨�ý���Ѿ��ָ���������Ч
  bool IsDown();//�鿴����Ƿ���Ч������Ч�򷵻�true���������򷵻�false

//���ݳ�Ա
private:
  NodeId_t id; // ���ؽ��id
public:
  NodeImpl* pImpl; //ghost���� real�ͽ��

//��̬��Ա
public:
  static NodeId_t nextId;// �ڽ���Ч����id
  static NodeVec_t nodes;// ������Ч��ļ���
  static double defaultMaxSpeed;//����Ĭ���������(�����й�)
#ifdef HAVE_QT
  static Count_t defaultPixelSize; //�����ʾ��Ĭ�����ش�С
  static Shape_t defaultShape;     //�����ʾ��Ĭ����״
#endif
public:
  static void SetNextNodeId(NodeId_t i) { nextId = i;}//������һ����id
  static const NodeVec_t& GetNodes() { return nodes;}//���ذ������н���vector
  static Node* GetNode(NodeId_t);//����ָ�����id��Ӧ�Ľ���ָ�룬���������򷵻ؿ�
  static void Clear();//������н��

#ifdef HAVE_QT  
  static void DefaultPixelSize(Count_t);//������Ч�����ʾʱ��Ĭ�����ش�С
  static Count_t DefaultPixelSize();//������Ч�����ʾʱ��Ĭ�����ش�С
  static void DefaultShape(Shape_t);//���ý����ʾʱ��Ĭ����״
  static Shape_t DefaultShape();//���ؽ����ʾʱ��Ĭ����״

#endif
  static void   DefaultMaxSpeed(double speed);//���õ�Ĭ�ϵ����������
  static double DefaultMaxSpeed();//���ص�Ĭ�ϵ��������
  
  Joules_t getBattery (void);//��ȡ���ؽ��������
  void     setBattery (Joules_t);//���ñ��ؽ��������
  double   getComputePower(void);//��ȡ���ؽ��CPU����
  void     setComputePower(double);//���ñ��ؽ��CPU����

public:
  bool GetDomainRoute()                 {return domainRoute;}
  void SetDomainRoute(bool _domainroute){domainRoute=_domainroute;}

//����·���������
private:
  IPAddr_t    proxyIP;//����·��IP
  Mask        proxyMask;//����·������
  bool        domainRoute;//�߽�·��

public:
  void        SetProxyRoutingConfig(IPAddr_t ip, Mask mask);//���ô���·��IP������
  bool        HasProxyRoutingConfig() {return proxyIP!=IPADDR_NONE;};//�жϱ��ؽ���Ƿ�Ϊ·�ɽ�㣬���򷵻�true������false
  bool        CanProxyRouteIP(IPAddr_t ip);//�ж�Ŀ��ip�뱾�ؽ���Ƿ���ͬһ���Σ����򷵻�true������false
  IPAddr_t    GetProxyIP();//���ش���·��IP
  Mask        GetProxyMask();//���ش���·������
  int         GetLongestPrefixLength(IPAddr_t ip);//�����ҪʹĿ��IP�뱾�ؽ����ͬһ���Σ��������õ���������ǰ׺���ȣ������䷵��

//v1.01 
//public:
//
//  RouteTable* getRouteTable(void);//���ر��ؽ���·�ɱ�
//  void        setRouteTable(RouteTable*);//���ñ��ؽ��·�ɱ�
  
};


#endif
