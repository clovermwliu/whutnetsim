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

#ifndef __nodereal_h__
#define __nodereal_h__

#include <map>
#include <deque>
#include <string>

#include "G_common_defs.h"
#include "node-impl.h"

#include "handler.h"
#include "routing.h"
#include "trace.h"
#include "macaddr.h"
//#include "routing-aodv.h"
#define DEFAULT_SW_TIMEOUT 10   // default timeout for the switch map  secs

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
class RoutingNixVector;
class Mobility;
class Node;
class Image;
class QTWindow;

#ifdef HAVE_QT
#include "GUI_Defs.h"


class NodeAnimation {
public:
  NodeAnimation() : show(false),
                    pixelSizeX(Node::DefaultPixelSize()),
                    pixelSizeY(Node::DefaultPixelSize()),
                    shape(Node::DefaultShape()),
                    csCallback(nil),
                    color(Qt::black),
                    hasColor(false), placeImage(false), nodePolygon(nil),
                    nodeSprite(nil) {}

public:
  bool          show;         // True if animation display desired
  Count_t       pixelSizeX;   // Size of node in pixels (X)
  Count_t       pixelSizeY;   // Size of node in pixels (Y)
  Node::Shape_t shape;        // Shape of node
  CustomShape_t csCallback;   // Custom Shape Callback
  MyColor        color;        // Color for this node on animation display
  bool          hasColor;     // True if color specified
  bool          placeImage;   // True if image sprite needs to be moved
  MyCanvasPolygonalItem*  nodePolygon;// Polygonal item (if not image)
  MyCanvasSprite*         nodeSprite; // Image item (for IMAGE shape only)
};

// ���߶����еĻ�ͼ����
typedef std::deque<MyCanvasItem*>        CanvasItemDeq_t;

#endif

typedef unsigned int Port_t;

class PortPair {
public:
  PortPair() : port(0), time(0) {}
  PortPair(Port_t p, Time_t t) : port(p), time(t) {}

public:
  Port_t  port;
  Time_t  time;
};

typedef std::map<MACAddr, PortPair> MACPort_t;//����·�ɶ˿ڱ�
typedef std::map<Count_t, RoutingEntry> RENixMap_t;//����cache nixvector·�ɱ�
typedef std::vector<RoutingEntry>       RENixVec_t;//����Nix·�ɱ�



class NodeReal : public NodeImpl {
public:
  NodeReal(Node*);
  ~NodeReal();

  NodeImpl* Copy() const;//���ݱ���㸴��NodeImpl
public:
//IP��ַ�����ӿڹ�����·�����ڽӽ�����̳���node-impl

//�����жϣ��ж���Real����Ghost
  bool IsReal();//�жϽ���Ƿ���real���͵ģ����Ƿ���true�����򷵻�false

//Satellite����
  virtual bool IsSatellite() const { return false;}
  virtual void IsSatellite(bool)   {  }

//�㲥һ�����ݰ�
  void Broadcast(Packet*, Proto_t);//ͨ�����нӿڹ㲥һ�����ݰ�

//����
  Queue* GetQueue();//��ȡ���ؽ��Ķ��У����Ե��ӿڵĽ����Ч
  Queue* GetQueue(Node*);//��ȡ���ؽ��ͨ��ָ��Զ�̽��Ķ���

//Ӧ��
  Application* AddApplication(const Application& a);//�����ؽ�����ָ��Ӧ��
  ApplicationVec_t* GetApplicationVec(){ return &pappvec; } //��ѯӦ��

//·��
  void DefaultRoute(RoutingEntry);//ָ���������ݰ���Ĭ��·����
  void DefaultRoute(Node*);//ָ���������ݰ���Ĭ����һ�����
  void AddRoute(IPAddr_t, Count_t, Interface*, IPAddr_t);//����һ��·���Ŀ��IP�������롢���ؽӿڡ���һ��IP��ַȷ��
  RoutingEntry LookupRoute(IPAddr_t);//����Ŀ��IP��ַ���ұ���·����������򷵻�
  RoutingEntry LookupRouteNix(Count_t);//����NIX·���е�·����
  Routing::RType_t RoutingType();//��ȡ���ؽ�㵱ǰ��ʹ�õ�·������
  void       InitializeRoutes(); //����·�ɳ�ʼ��
  void       ReInitializeRoutes(bool);//����·�ɸ���
  Count_t    RoutingFibSize() const;//ͳ�Ʊ��ؽ��·�ɵĴ�С
  RoutingNixVector* GetNixRouting();//�����ؽ�����NixVector����·�ɣ��򷵻ظ�·�ɶ����ָ�룬���򷵻ؿ�
  Routing*   GetRouting() { return pRouting;} //���ر��ؽ����ʹ�õ�·�ɶ����ָ��

//AODV·��
  ////AODVRoutingEntry *LookupRouteAODV(IPAddr_t);
  ////void      SetRoutingAODV(void *pRouting);
  ////RoutingAODV * GetRoutingAODV();

//Э����ӿ�
  Protocol* LookupProto(Layer_t, Proto_t);//�ڱ��ؽ�㣬����Э���ͱ�Ų��Ҷ�Ӧ��Э�����
  void      InsertProto(Layer_t, Proto_t, Protocol*);//�ڱ��ؽ�����һ��Э�����.Э���ͱ��ָ��

//��·�ֲ����ӿ�
  bool      Bind(Proto_t, PortId_t, Protocol*);//Ϊָ���˿ڰ�ָ��Э��
  bool      Bind(Proto_t,
                 PortId_t, 
				 IPAddr_t,
                 PortId_t, 
				 IPAddr_t,
                 Protocol*);//Ϊ���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  PortId_t  Bind(Proto_t, Protocol*);//��ָ��Э�飬�˿����
  bool      Unbind(Proto_t, PortId_t, Protocol*);//ɾ�����ؽ��ָ���ӿڰ󶨵�ָ��Э��
  bool      Unbind(Proto_t,
                   PortId_t, 
				   IPAddr_t,
                   PortId_t, 
				   IPAddr_t,
                   Protocol*);//���ؽ��ָ���˿ں�Զ�̽��ָ���˿ڰ�ָ��Э��
  Protocol* LookupByPort(Proto_t, PortId_t);//���ݱ��ض˿ں�Э��Ų������󶨵�Э��
  Protocol* LookupByPort(Proto_t,
                         PortId_t, 
						 IPAddr_t,
                         PortId_t, 
						 IPAddr_t);//����Э�飬����Э��š����ؽ��˿ڡ�IP��ַ��Զ�̽��˿ڡ�IP��ַ
//�����ٽӿ�
  bool      TracePDU(Protocol*, PDU*, Packet*, char*);//��ָ��PDU��Ԫ���ݴ�ָ��Э���¼�������ļ�
  void      SetTrace(Trace::TraceStatus);//���ñ��ؽ��ĸ���״̬

//�������(x, y, z)�ӿ�
  void      SetLocation(Meters_t, Meters_t, Meters_t); //ֱ����xyz����ϵ�����ñ��ؽ�������
  void      SetLocation(const Location&);//����location�������ñ��ؽ����xyz����ϵ�е�����
  void      SetLocationLongLat(const Location&);   // Set x/y/z location
  bool      HasLocation();//��鱾�ؽ���Ƿ��ѷ����ַ
  Meters_t  LocationX();//���ر��ؽ���x����
  Meters_t  LocationY();//���ر��ؽ���y����
  Meters_t  LocationZ();//���ر��ؽ���z����
  Location  GetLocation();//���ر��ؽ�㵱ǰλ��
  Location  UpdateLocation();//���±��ؽ���λ��
  Mobility* AddMobility(const Mobility&);//Ϊ���ؽ������һ��mobilityģ��
  Mobility* GetMobility() const;//��ȡ���ؽ�㵱ǰ��mobilityģ��
  bool      IsMobile();//�жϱ��ؽ���Ƿ���mobilityģ��
  bool      IsMoving();//�жϱ��ؽ��ĵ�ǰ״̬�Ƿ�����������

#ifdef HAVE_QT  
  ////bool      WirelessTx();                          // True if wireless transmit
  ////bool      WirelessRx();                          // True if wireless receive
  ////bool      WirelessCx();                          // True if wireless collid
  ////bool      WirelessRxMe();                        // True if wireless receive
  ////bool      WirelessRxZz();                        // True if wireless receive
  void      UserInformation(void*);                // Specify user info
  void*     UserInformation();                     // Return user info

// QT������Ϣ
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
  void Shape(Node::Shape_t);//���õ�����ʾʱ����״
  Node::Shape_t Shape();//���ص�����ʾʱ����״
  CustomShape_t CustomShape();//����һ��ͼ�εĻص�����
  void CustomShape(CustomShape_t);//����һ��ͼ�εĻص�����
  bool CustomShapeFile(const char*);//����һ������һ��ͼ�ε��ļ������ɹ�����true�����򷵻�false
  bool CustomShapeImage(const Image&);//����һ��һ��ͼ�Σ����ɹ�����true�����򷵻�false
  void Color(const MyColor&);//ָ�����ؽ�����ɫ
  bool HasColor();//��鱾�ؽ���Ƿ��Ѿ�������ɫ
  MyColor& Color();//���ر��ؽ�����ɫ
  NodeAnimation* GetNodeAnimation() const;//����ָ�����н���㶯����Ϣ���ָ��
#endif
  
//ICMP���
  ////bool ICMPEnabled() const;//��鱾�ؽ���Ƿ�֧��ICMP
  ////void DisableICMP();//�趨���ؽ�㲻֧��ICMP


#ifdef MOVED_TO_IMPL
//���ʧ�ܹ���
  void Down();//�趨�ý����Ч���޷�������ת�����ݰ�
  void Up();//�趨�ý���Ѿ��ָ���������Ч
  bool IsDown();//�鿴����Ƿ���Ч������Ч�򷵻�true���������򷵻�false
#endif

  Meters_t Distance(Node*);//���ر��ؽ�㵽ָ���ԵȽ��ľ���
 
  ////void      BuildRadioInterfaceList(WirelessLink*);
  ////const RadioVec_t& GetRadioInterfaceList();
  ////void      SetRadioRange(Meters_t range) { radioRange = range; }
  ////Meters_t  GetRadioRange() { return radioRange; }

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

  void      setBattery(Joules_t joules) { battery = joules;}//���ñ��ؽ��������
  Joules_t  getBattery(void) { return battery; }//��ȡ���ؽ��������
  void      setComputePower(double pow) { computePower = pow; }//���ñ��ؽ��CPU����
  double    getComputePower(void) { return computePower; }//��ȡ���ؽ��CPU����

  //void        setRouteTable(RouteTable* rtable_) { rtable = rtable_;}//���ñ��ؽ���·�ɱ�
  //RouteTable* getRouteTable(void) { return rtable;}//���ر��ؽ���·�ɱ�
//����������
  void        SetTimeout(Time_t);//���÷�����ʱ��
  int         FindSRCPort(Interface*);//�ڱ��ؽӿ��в����Ƿ���Ŀ��ӿ���ͬ����������򷵻���֮��Ӧ�Ķ˿ںţ����򷵻ؿ�
  void        PacketRX(Packet*, Interface*);//�������ݰ�
  MACPort_t*  GetPortMap(){ return &portmap;}//��ȡ�˿ڱ�
  Time_t      GetTimeout(){ return timeout;}//��ȡ������ʱ��
  bool        IsSwitchNode() { return isswitch; }//�жϱ��ؽ���Ƿ��н��������ܣ����з���true�����򷵻�false
  void        IsSwitchNode(bool);//���ݲ������ý���Ƿ�Ϊ������

//worm
  ////void SetWormContainment(WormContainment*);
  ////WormContainment* GetWormContainment() { return wormcontainment; } 
  ////void UseARP(bool);

private:
  bool CheckTrace(Protocol*, PDU*);// �������Ƿ��ܽ���
#ifdef HAVE_QT
  bool CustomShapeHelper(MyPixmap&);//����һ��ͼ��
#endif
//���ݳ�Ա
private:
  Routing*   pRouting;    //���ؽ��·�ɶ����ָ��
  RENixVec_t nixLookup;   //nix·�ɻ�����

//AODV·��
  ////RoutingAODV*  pRoutingAODV;

  Meters_t x;             // x������
  Meters_t y;             // y������
  Meters_t z;             // z������
  bool     hasLocation;   // True��ʾ�����Ѿ��趨
#ifndef COMPACT
  ProtocolGraph* graph;   //˽��Э����
#endif
  Trace::TraceStatus traceStatus;//���ؽ��ĸ���״̬
  Mobility* mobility;     //��ǰmobilityģ��
public:
  void*       userInfo;           // User��Ϣ

  PortDemux*       demux;           //��·�ֲ���
  Meters_t         radioRange;      //���ڽӿڣ������ڽ��
  Joules_t         battery;         //�ٶ������Ҫ���
  double           computePower;    //CPU���
  ////RadioVec_t       radioInterfaces;
  ////RadioVec_t       radioInf2;      
  ////Time_t           radioMarginTime;
  PacketCallbacks  callbacks;//���ؽ������ݰ��ص�

#ifdef HAVE_QT  
  CanvasItemDeq_t  canvasItems;     //����
  NodeAnimation*   animInfo;  //��㶯��
  MyColor           wirelessColor;   //���߶�����ɫ
#endif
  
  bool             icmpEnabled;     //True�����ؽ��֧��ICMP

  //RouteTable*      rtable;          //·�ɱ����
  
private:
//��������Ա
  MACPort_t        portmap;//�˿ڱ�
  Time_t           timeout;//������ʱ��
  bool             isswitch;//true����ý��Ϊ��������false�����ý�㲻�ǽ�����
  //WormContainment* wormcontainment;//v1.01 

public:
//���ڵ���
  static Count_t count;    
  static Count_t count1;   
  static Count_t count2; 
  static Count_t count3;  
  static Count_t count4;  
};

//#include "interface.h"

#endif
