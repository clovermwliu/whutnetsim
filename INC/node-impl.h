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

#ifndef __nodeimpl_h__
#define __nodeimpl_h__

#include "G_common_defs.h"
#include "macaddr.h"
#include "node.h"

class Link;
class Node;
class Interface;
class Routing;
class Protocol;
class ProtocolGraph;
class PortDemux;
class Linkp2p;
class Queue;
class Application;
////class RoutingAODV;
////class AODVRoutingEntry;
class Image;
////class WirelessLink;

class NodeImpl {
public:
  NodeImpl(Node*);
  virtual ~NodeImpl();

  virtual NodeImpl* Copy() const = 0; //���е�Nodeimpl���ܱ�����
  NodeId_t Id() { return pNode->Id(); }//���ؽ��Id

//IP��ַ����
  virtual IPAddr_t GetIPAddr();          // ��ȡ�ý���IP��ַ
  virtual void     SetIPAddr(IPAddr_t i);// ���øý���IP��ַ
  virtual void     IPAddrs(IPMaskVec_t&);// ���ظý�����нӿڵ�IP��ַ��������Ϣ
  virtual bool     LocalIP(IPAddr_t);    // �ж�IPAddr_t�Ƿ�Ϊ�����IP��ַ�����򷵻�true
  virtual bool     IPKnown();            // �ж��Ƿ������һ����Ĭ�ϵ�IP��ַ�����򷵻�true
  virtual bool     IsReal() = 0;         // �ж��Ƿ���real���ͽ�㣬���򷵻�true

//Satellite����
  virtual bool     FirstBitRx() const { return false;}
  virtual void     FirstBitRx(bool)   { }
  
//����������
  virtual void IsSwitchNode(bool) = 0;//�Ƿ񽫸ý����Ϊ������
  virtual bool IsSwitchNode() = 0;   //�жϸý���Ƿ���һ��������
  virtual void PacketRX(Packet* p, Interface* iface) = 0;//������������յ�һ�����ݰ�����ú�����ִ��

//�ӿ�
  virtual Interface* AddInterface(Interface*);//���һ���Ѿ����úõĽӿ�
  virtual Interface* AddInterface(const L2Proto&, bool bootstrap=false);//����µĿսӿڣ��ýӿڷ���ڶ�������Э��
  virtual Interface* AddInterface(const L2Proto&, //Ϊ���ؽ�������ӿڣ�IP��ַ������ָ����MAC��ַĬ�ϣ�������ڶ�������Э��
                                  IPAddr_t, Mask_t,
                                  MACAddr = MACAddr::Allocate(),
				                  bool bootstrap = false);
  virtual Interface* AddInterface(const L2Proto&, //Ϊ���ؽ�������ӿڣ�IP��ַ�����롢�ӿ�����ָ����MAC��ַĬ�ϣ�������ڶ�������Э��
	                              const Interface&,
				                  IPAddr_t, 
								  Mask_t, 
				                  MACAddr = MACAddr::Allocate(),
				                  bool bootstrap = false);
  virtual Count_t    InterfaceCount();   //��ȡ��ý��󶨵Ľӿڵ���Ŀ������ֵ�ǽӿ���Ŀ
  virtual const IFVec_t& Interfaces();   //��ȡ�뱾�ؽ��󶨵����нӿڵ���Ϣ
  virtual Interface* GetIfByLink(Link*); //���ݱ߲��Ҷ�Ӧ�ӿ�
  virtual Interface* GetIfByNode(Node*); //���ݵ���Ҷ�Ӧ�ӿ�
  virtual Interface* GetIfByIP(IPAddr_t);//���ݱ��ؽӿ�IP��ַ���Ҷ�Ӧ�ӿ�
  virtual void       Broadcast(Packet*, Proto_t) = 0;//ͨ���ý������нӿڹ㲥���ݰ���

//˫����·���ò��ֺ������ܾ�Ϊ��Զ�̽�㽨��ȫ˫������·���ӣ�����ֵ��Ϊ�½���·��Ӧ�ı��ؽӿ�
  virtual Interface* AddDuplexLink(Node*);//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ΪĬ��
  virtual Interface* AddDuplexLink(Node*, const Linkp2p&);//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ָ��
  virtual Interface* AddDuplexLink(Interface*,Interface*);//�½�һ����������ָ���ӿڵ�˫����·����·����Ĭ��
  virtual Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&);//�½�һ����������ָ���ӿڵ�˫����·����·����Ĭ��
  virtual Interface* AddDuplexLink(Node*,//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ΪĬ�ϡ�Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
                                   IPAddr_t, 
								   Mask_t, 
								   IPAddr_t, 
								   Mask_t);
  virtual Interface* AddDuplexLink(Node*,//����Զ�̽�㣬���ؽ������һ��˫����·��һ���ӿڣ���·����ΪĬ�ϡ�Ϊ������Զ�̽�������Ľӿڷ���IP��ַ������
	                               const Linkp2p&,
                                   IPAddr_t, 
								   Mask_t, 
								   IPAddr_t,
								   Mask_t);
  virtual Link*      GetLink(Node*);    //������ָ�������ͨ����·
  //v1.01
  //virtual Interface* AddRemoteLink(IPAddr_t, Mask_t);//���ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á���·�Ĵ�����ӳ�Ĭ��
  //virtual Interface* AddRemoteLink(IPAddr_t, Mask_t, Rate_t, Time_t);//���ڷֲ�ʽģ������ʹ�ã�����һ����ͨ���ؽ������������˫����·������һ��ģ�������ʹ�á���·�Ĵ�����ӳ�ָ��
//������·
  virtual Interface* AddSimplexLink(Node*);//����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ΪĬ��
  virtual Interface* AddSimplexLink(Node*, const Linkp2p&);//����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ��
  virtual Interface* AddSimplexLink(Node*, //����Զ�̽�㣬���ؽ������һ��������·��һ���ӿڣ���·����ָ�������ؽӿ�IP��ַ������ָ��
	                                const Linkp2p&,
                                    IPAddr_t, 
									Mask_t);

//����
  virtual Queue* GetQueue() = 0;        //��ȡ���ؽ��Ķ��У����Ե��ӿڵĽ����Ч
  virtual Queue* GetQueue(Node*) = 0;   //��ȡ���ؽ��ͨ��ָ��Ŀ����Ķ���

//Ӧ��
  virtual Application* AddApplication(const Application& a) = 0;//�����ؽ�����ָ��Ӧ��

//�ڽӽ�������·��Э����ʹ��
  virtual void Neighbors(NodeWeightVec_t &, bool); //��ȡ���е��ڽ����
  virtual Count_t NeighborCount();       //��ȡ·�ɱ����ڽ�������Ŀ
  virtual void NeighborsByIf(Interface*, IPAddrVec_t&);//��ȡ�뱾�ؽ��ָ���ӿ��������ڽӽ��IP��ַ

//·��
  virtual void DefaultRoute(RoutingEntry) = 0;//ָ���������ݰ���Ĭ��·����
  virtual void DefaultRoute(Node*) = 0;//ָ���������ݰ���Ĭ����һ�����
  virtual void AddRoute( IPAddr_t, Count_t, Interface*, IPAddr_t) = 0;//����һ��·���Ŀ��IP�������롢���ؽӿڡ���һ��IP��ַȷ��
  virtual RoutingEntry LookupRoute(IPAddr_t) = 0; //����Ŀ��IP��ַ���ұ���·����������򷵻�
  virtual RoutingEntry LookupRouteNix(Count_t) = 0;//�����ڽӽ���������ұ���·����������򷵻�
  virtual Routing::RType_t RoutingType() = 0;//��ȡ���ؽ�㵱ǰ��ʹ�õ�·������
  virtual Interface* LocalRoute(IPAddr_t);//�ж�ĳIP��ַ�Ƿ���ֱ���뱾�ؽ��ĳinterface�����������򷵻ظ�interface
  virtual void       InitializeRoutes() = 0;//·�ɳ�ʼ����Ϊ·��Э����׼��
  virtual void       ReInitializeRoutes(bool) = 0; //���³�ʼ��·��
  virtual Count_t    RoutingFibSize() const = 0;//ͳ�Ʊ��ؽ��·�ɵĴ�С
  virtual Count_t    GetNix(Node*) const;//����ָ���������ڽӽ���Index�����ý�㲻���ڽӽ�㣬����MAX_COUNT
  virtual RoutingNixVector* GetNixRouting() = 0;   // Returns Nix rtg pointer
  virtual Routing*   GetRouting() = 0;//���ر��ؽ����ʹ�õ�·�ɶ����ָ��

//AODV·��
  //////virtual AODVRoutingEntry *LookupRouteAODV(IPAddr_t ip) =0;
  //////virtual void      SetRoutingAODV(void *pRouting)= 0;
  //////virtual RoutingAODV *GetRoutingAODV() = 0;

//Э����ӿ�
  virtual Protocol* LookupProto(Layer_t, Proto_t) = 0;//�ڱ��ؽ�㣬����Э���ͱ�Ų��Ҷ�Ӧ��Э�����
  virtual void      InsertProto(Layer_t, Proto_t, Protocol*) = 0;//�ڱ��ؽ�����һ��Э�����.Э���ͱ��ָ��

//��·�������ӿ�
  virtual bool Bind(Proto_t, PortId_t, Protocol*) = 0;//Ϊָ���˿ڰ�ָ��Э��
  virtual bool Bind(Proto_t,
                    PortId_t, 
					IPAddr_t,
                    PortId_t, 
					IPAddr_t,
                    Protocol*) = 0;//Ϊ���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  virtual PortId_t Bind(Proto_t, Protocol*) = 0;//��ָ��Э�飬��Ӧ���ض˿����
  virtual bool Unbind(Proto_t, PortId_t, Protocol*) = 0;//ɾ�����ؽ��ָ���ӿڰ󶨵�ָ��Э��
  virtual bool Unbind(Proto_t,
                      PortId_t, 
					  IPAddr_t,
                      PortId_t, 
					  IPAddr_t,
                      Protocol*) = 0;//���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  virtual Protocol* LookupByPort(Proto_t, PortId_t) = 0;//���ݱ��ض˿ں�Э��Ų������󶨵�Э��
  virtual Protocol* LookupByPort(Proto_t,
                                 PortId_t, IPAddr_t,
                                 PortId_t, IPAddr_t) = 0;  //����Э�飬����Э��š����ؽ��˿ڡ�IP��ַ��Զ�̽��˿ڡ�IP��ַ

//���ݰ����ٽӿ�
  virtual bool      TracePDU(Protocol*, PDU*, Packet*, char*) = 0;//��ָ��PDU���ݴ�ָ��Э���¼�������ļ�
  virtual void      SetTrace(Trace::TraceStatus) = 0;//���ñ��ؽ��ĸ���״̬

//�������(x, y, z)�ӿ�
  virtual void      SetLocation(Meters_t, Meters_t, Meters_t) = 0;//ֱ����xyz����ϵ�����ñ��ؽ�������
  virtual void      SetLocation(const Location&) = 0; //����location�������ñ��ؽ����xyz����ϵ�е�����
  virtual void      SetLocationLongLat(const Location&) = 0;
  virtual bool      HasLocation() = 0;//��鱾�ؽ���Ƿ��ѷ����ַ
  virtual Meters_t  LocationX() = 0;//���ر��ؽ���x����        
  virtual Meters_t  LocationY() = 0;//���ر��ؽ���y����
  virtual Meters_t  LocationZ() = 0;//���ر��ؽ���z����
  virtual Location  GetLocation() = 0;//���ر��ؽ�㵱ǰλ��
  virtual Location  UpdateLocation() = 0;//���±��ؽ���λ��
  virtual Meters_t  Distance(Node*) = 0;//���ر��ؽ�㵽ָ���ԵȽ��ľ���
  virtual Mobility* AddMobility(const Mobility&) = 0;//Ϊ���ؽ������һ��mobilityģ��
  virtual Mobility* GetMobility() const = 0; //��ȡ���ؽ�㵱ǰ��mobilityģ��
  virtual bool      IsMobile() = 0; //�жϱ��ؽ���Ƿ���mobilityģ��
  virtual bool      IsMoving() = 0; //�жϱ��ؽ��ĵ�ǰ״̬�Ƿ�����������
#ifdef HAVE_QT //v1.01
  virtual void      UserInformation(void*) = 0;//�洢�û�����������κ�������Ϣ 
  virtual void*     UserInformation() = 0;//����֮ǰָ�����û���Ϣָ�룬��û�з��ؿ�
#endif
  ////virtual bool      WirelessTx() = 0; 
  ////virtual bool      WirelessRx() = 0; 
  ////virtual bool      WirelessCx() = 0;     
  ////virtual bool      WirelessRxMe() = 0;  
  ////virtual bool      WirelessRxZz() = 0;
  ////void UseWormContainment(bool);
  ////bool UseWormContainment() { return usewormcontainment; }
  ////virtual void SetWormContainment(WormContainment*) = 0;
  ////virtual void UseARP(bool) = 0;
  ////virtual WormContainment* GetWormContainment() = 0;

//QT������Ϣ
#ifdef HAVE_QT  //v1.01
  virtual void           Show(bool) = 0; //���ݲ���ȷ���Ƿ�Ҫ��ʾ����
  virtual bool           Show() = 0;//������ʾ�Ƿ�Ҫ������
  virtual MyCanvasItem*   Display(QTWindow*) = 0;//ȷ������ʾ����״����С�ȣ���ʾλ��Ϊ�㵱ǰ��λ��
  virtual MyCanvasItem*   Display(const MyPoint&, QTWindow*) = 0;//ȷ������ʾ����״����С�ȣ���ʾλ�ò������ṩ��λ��
  ////virtual void           WirelessTxColor(const MyColor&) = 0;
  ////virtual const MyColor&  WirelessTxColor() = 0;
  ////virtual bool           PushWirelessTx(MyCanvasItem*) = 0;
  ////virtual MyCanvasItem*   PopWirelessTx() = 0;
  virtual void           PixelSize(Count_t) = 0;//���õ�����ʾʱ�Ĵ�С
  virtual Count_t        PixelSizeX() = 0;//���ص�����ʾʱ�Ŀ��
  virtual Count_t        PixelSizeY() = 0;//���ص�����ʾʱ�ĸ߶�
  virtual void           Shape(Node::Shape_t) = 0;//���õ�����ʾʱ����״
  virtual Node::Shape_t  Shape() = 0;//���ص�����ʾʱ����״
  virtual CustomShape_t  CustomShape();//����һ��ͼ�εĻص�����
  virtual void           CustomShape(CustomShape_t);//����һ��ͼ�εĻص�����
  virtual bool           CustomShapeFile(const char*);//����һ������һ��ͼ�ε��ļ������ɹ�����true�����򷵻�false
  virtual bool           CustomShapeImage(const Image&);//����һ��һ��ͼ�Σ����ɹ�����true�����򷵻�false
  virtual void           Color(const MyColor&) = 0;//ָ�����ؽ�����ɫ
  virtual bool           HasColor() = 0;//��鱾�ؽ���Ƿ��Ѿ�������ɫ
  virtual MyColor&        Color() = 0;//���ر��ؽ�����ɫ
  virtual NodeAnimation* GetNodeAnimation() const = 0;//����ָ�����н���㶯����Ϣ���ָ��
#endif
  
//ICMP�����Ϣ
  ////virtual bool   ICMPEnabled() const;//��鱾�ؽ���Ƿ�֧��ICMP
  ////virtual void   DisableICMP();//�趨���ؽ�㲻֧��ICMP

//��������ʧ����Ϣ
  virtual void   Down();//�趨�ý����Ч���޷�������ת�����ݰ�
  virtual void   Up();//�趨�ý���Ѿ��ָ���������Ч
  virtual bool   IsDown();//�鿴����Ƿ���Ч������Ч�򷵻�true���������򷵻�false

//���ߴ�����Ϣ
  ////virtual void      BuildRadioInterfaceList(WirelessLink*) = 0;
  ////virtual const RadioVec_t& GetRadioInterfaceList() = 0;
  ////virtual void      SetRadioRange(Meters_t ) =0;
  ////virtual Meters_t  GetRadioRange(void ) =0;

//�ص�����
  virtual void      AddCallback(Layer_t, Proto_t,
                                PacketCallbacks::Type_t,
                                Interface*,
                                PacketCallbacks::Function_t) = 0;//�����������һ���ص��������ص������Ȿ�����ջ��������Э�����������ݰ�
  virtual void      AddCallbackHead(Layer_t, Proto_t,
                                    PacketCallbacks::Type_t,
                                    Interface*,
                                    PacketCallbacks::Function_t) = 0;//�����ؽ�����һ���ص��������ú����ص����ȼ����
  virtual void      DeleteCallback(Layer_t, Proto_t, 
                                   PacketCallbacks::Type_t,
                                   Interface*) = 0;//�ӱ��ؽ��ɾ��һ���ص�����
  virtual bool      CallCallbacks(Layer_t, Proto_t, 
                                  PacketCallbacks::Type_t, Packet*,
                                  Interface*) = 0;//���ñ��ؽ��Ļص�����

  virtual void      setBattery (Joules_t) = 0;//���ñ��ؽ��������
  virtual Joules_t  getBattery (void) = 0;//��ȡ���ؽ��������
  virtual void      setComputePower (double) = 0;//���ñ��ؽ��CPU����
  virtual double    getComputePower (void) = 0;//��ȡ���ؽ��CPU����
  //virtual void        setRouteTable (RouteTable*) = 0;//���ñ��ؽ���·�ɱ�
  //virtual RouteTable* getRouteTable (void) = 0;//���ر��ؽ���·�ɱ�

  
protected:
  Node*    pNode;// ָ����Ӧ���ӿڶ���
  bool     usewormcontainment;//�ж��������Ƿ������湦��
//real��ghost֮������ݳ�Ա
  IPAddr_t ipAddr;//���ؽ���IP��ַ
  IFVec_t  interfaces;//���еĽӿ�
  Count_t  neighborCount;//�ڽӽӿ�����
  bool     down;//�жϽ���Ƿ���Ч
};

#endif
