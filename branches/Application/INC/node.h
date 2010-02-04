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
//调用Qt的类库，若没有Qt配置，则不使用
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
//在路由协议中使用
{ 
public:
  NodeIfWeight(Node* n, InterfaceBasic* i, Weight_t w)
    : node(n), iface(i), weight(w) { }
public:
  Node*           node;    //邻近结点
  InterfaceBasic* iface;   //接口
  Weight_t        weight;  //链路权重
};


class NodeIf { //到达目的地经过的结点和接口（路由中使用） 
public:
  NodeIf(Node* n, Interface* i)
    : node(n), iface(i) { }
public:
  Node*      node;
  Interface* iface;
};

class IpMask { // IP地址和掩码
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
Node类：
在仿真拓扑中,所有的点都是用Node类的对象来表示的。当一个结点被创建时，它自动有一个
与之相连的路由对象（默认为NV路由）和第三层协议对象（默认为em IPV4）。当结点创建后，
会增加接口和线路来描述结点的连接，用到了以下描述的共有成员。
*/
class Node {

public:
//定义在运行中点的形状
  typedef enum { NONE,   //空
	             CIRCLE, //圆形
				 SQUARE, //方形
				 HEXAGON,//六边形
				 OCTAGON,//八边形
                 CUSTOM, //由一般回调显示
                 IMAGE   //显示一个Qt图形文件
  } Shape_t;

public:

  Node(SystemId_t = 0);    //创建一个模拟拓扑结点

protected:

  Node(NodeImpl*, SystemId_t = 0);  //用于给Node的子类如SatelliteNode类创建结点，以满足不同的结点类型需求

public:
  virtual ~Node();         //析构函数
  NodeId_t Id() { return id;}   //返回该结点的id ，结点id是仿真器中每个结点的唯一标识，第一个点标记为0

//IP地址管理
  IPAddr_t GetIPAddr();               //获取该结点的IP地址 
  virtual void SetIPAddr(IPAddr_t);   //设置该结点的IP地址
  void IPAddrs(IPMaskVec_t&);         //返回该结点所有接口的IP地址和掩码信息
  bool LocalIP(IPAddr_t);             //判断IPAddr_t是否为本结点IP地址，是则返回true
  bool IPKnown();                     //判断是否分配了一个非默认的IP地址，是则返回true

//交换机相关
  bool IsReal();					  //判断是否是一个Real类型的结点，是则返回true
  bool IsSwitchNode();				  //判断该结点是否是一个交换机，是则返回true
  void IsSwitchNode(bool);			  //是否将该结点置为交换机
  void PacketRX(Packet*, Interface*); //若交换机结点收到一个数据包，则该函数被执行
 
//接口
  Interface* AddInterface(Interface*);  //添加一个已经设置好的接口
  Interface* AddInterface(const L2Proto& = L2Proto802_3(),//添加新的空接口，该接口分配第二层网络协议
			               bool bootstrap=false);           
  Interface* AddInterface(const L2Proto&,  //为本地结点新增接口，IP地址、掩码指定，MAC地址默认，并分配第二层网络协议
                          IPAddr_t,    
						  Mask_t,      
						  MACAddr m = MACAddr::Allocate(),
			              bool bootstrap=false);
  Interface* AddInterface(const L2Proto&,   //为本地结点新增接口，IP地址、掩码、接口类型指定，MAC地址默认，并分配第二层网络协议
	                      const Interface&, 
			              IPAddr_t,         
						  Mask_t,          
			              MACAddr = MACAddr::Allocate(),
			              bool bootstrap = false);
 //Interface* LocalSubnet(IPAddr_t);     //判断是否是本地子网
  virtual Count_t    InterfaceCount();   //获取与本地结点绑定的接口的数目，返回值是接口数目
  virtual const IFVec_t& Interfaces();   //获取与本地结点绑定的所有接口的信息。
  virtual Interface* GetIfByLink(Link*); //根据边查找对应接口
  virtual Interface* GetIfByNode(Node*); //根据点查找对应接口
  virtual Interface* GetIfByIP(IPAddr_t);//根据本地接口IP地址查找对应接口
  
  void    Broadcast(Packet*, Proto_t);   //通过该结点的所有接口广播数据包。                                       

//双向链路：该部分所有的函数返回值均为本地接口。作用是与远程结点建立全双工连接。
  Interface* AddDuplexLink(Node*);  //连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为默认。
  Interface* AddDuplexLink(Node*, const Linkp2p&); //连接远程结点，本地结点新增一条双向链路和一个接口，链路类型指定
  Interface* AddDuplexLink(Interface*,Interface*);//新建一个连接两个指定接口的双向链路，链路类型默认
  Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&);//新建一个连接两个指定接口的双向链路，链路类型指定
  //Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&, Node*); 
  Interface* AddDuplexLink(Node*,//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为默认。为本地与远程结点新增的接口分配IP地址和掩码
                           IPAddr_t, 
						   Mask_t = Mask(32),
                           IPAddr_t = IPADDR_NONE, 
						   Mask_t = Mask(32));
  Interface* AddDuplexLink(Node*,//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为指定。为本地与远程结点新增的接口分配IP地址和掩码
	                       const Linkp2p&,
                           IPAddr_t, 
						   Mask_t = Mask(32),
                           IPAddr_t = IPADDR_NONE, 
						   Mask_t = Mask(32));
  //v1.01
  //Interface* AddRemoteLink(IPAddr_t, Mask_t);//仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。链路的带宽和延迟默认
  // Interface* AddRemoteLink(IPAddr_t, Mask_t, Rate_t, Time_t);//仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。链路的带宽和延迟指定
  Link*      GetLink(Node*); //返回与指定结点联通的链路
//单向链路：与远程结点建立单向连接。
  Interface* AddSimplexLink(Node*);//连接远程结点，本地结点新增一条单向链路和一个接口，链路类型为默认。
  Interface* AddSimplexLink(Node*, const Linkp2p&);//连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定。
  Interface* AddSimplexLink(Node*, //连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定，本地接口IP地址和掩码指定
	                        const Linkp2p&, 
							IPAddr_t, 
							Mask_t);

//队列：用于存储数据包
  Queue* GetQueue();//获取本地结点的队列，仅对单接口的结点有效。
  Queue* GetQueue(Node*); //获取本地结点通往指定远程结点的队列

//应用
  Application* AddApplication(const Application& a);//给本地结点添加指定应用
  ApplicationVec_t* GetApplication();

//邻接结点管理：在一些路由协议中使用
  virtual void Neighbors(NodeWeightVec_t &, bool = false); // 获取所有的邻近结点
  void AddNeighbor(Node*, Weight_t); //该函数为空，Ghost类型的结点保存邻近结点列表 
  Count_t NeighborCount();//获取路由表中邻近结点的数目 
  void NeighborsByIf(Interface*, IPAddrVec_t&);//获取与本地结点指定接口相连的邻接结点IP地址

//路由
  void DefaultRoute(RoutingEntry);//指定发送数据包的默认路由项
  void DefaultRoute(Node*);//指定发送数据包的默认下一跳结点
  void AddRoute(IPAddr_t, Count_t, Interface*, IPAddr_t);//增加一个路由项，目的IP及其掩码、本地接口、下一跳IP地址确定
  RoutingEntry LookupRoute(IPAddr_t); //根据目的IP地址查找本地路由项，若存在则返回
  RoutingEntry LookupRouteNix(Count_t);//根据邻接结点索引查找本地路由项，若存在则返回
  Routing::RType_t RoutingType();//获取本地结点当前所使用的路由类型
  Interface* LocalRoute(IPAddr_t); //判断某IP地址是否是直接与本地结点某接口相连，若是则返回该接口
  void InitializeRoutes();//路由初始化，为路由协议做准备
  void ReInitializeRoutes(bool);//重新初始化路由
  Count_t RoutingFibSize() const;//统计本地结点路由表（fib）的大小
  Count_t GetNix(Node*) const;//根据指定结点查找邻接结点的索引
  RoutingNixVector* GetNixRouting();//若本地结点存在NixVector类型路由，则返回该路由对象的指针，否则返回空
  Routing* GetRouting();//返回本地结点所使用的路由对象的指针
  void DumpNV(); // 调试所用，与NixVector相关

//AODV路由
  ////AODVRoutingEntry *LookupRouteAODV(IPAddr_t);
  ////void      SetRoutingAODV(void *pRouting);
  ////RoutingAODV* GetRoutingAODV();

//协议族接口
  Protocol* LookupProto(Layer_t, Proto_t);//在本地结点，根据协议层和编号查找对应的协议对象
  void InsertProto(Layer_t, Proto_t, Protocol*); //在本地结点插入一个协议对象，.协议层和编号指定  
  bool Bind(Proto_t, PortId_t, Protocol*);//为指定端口绑定指定协议
  bool Bind(Proto_t,    
            PortId_t, 
			IPAddr_t,
            PortId_t, 
			IPAddr_t,
            Protocol*);//为本地结点指定端口和远程结点指定端口绑定指定协议
  PortId_t  Bind(Proto_t, Protocol*);//绑定指定协议，端口随机
  bool Unbind(Proto_t, PortId_t, Protocol*);//删除本地结点指定接口绑定的指定协议
  bool Unbind(Proto_t,
              PortId_t, 
			  IPAddr_t,
              PortId_t, 
			  IPAddr_t,
              Protocol*);//删除本地结点指定端口和远程结点指定端口绑定指定协议
  Protocol* LookupByPort(Proto_t, PortId_t);//根据本地端口和协议号查找所绑定的协议
  Protocol* LookupByPort(Proto_t,
                         PortId_t, 
						 IPAddr_t,
                         PortId_t, 
						 IPAddr_t);//查找协议，根据协议号、本地结点端口、IP地址、远程结点端口、IP地址
  ////bool ICMPEnabled() const;//检查本地结点是否支持ICMP
  ////void DisableICMP();//设定本地结点不支持ICMP

//数据包跟踪接口
  bool TracePDU(Protocol*, PDU*, Packet* = nil, char* = nil);//将指定PDU单元内容从指定协议记录到跟踪文件
  void SetTrace(Trace::TraceStatus);//设置本地结点的跟踪状态

//结点坐标(x, y, z)接口
  void SetLocation(Meters_t, Meters_t, Meters_t=0);//直接在xyz坐标系中设置本地结点的坐标
  void SetLocation(const Location&);//根据location对象设置本地结点在xyz坐标系中的坐标
  void SetLocationLongLat(const Location&); 
  bool HasLocation(); //检查本地结点是否已分配地址
  Meters_t  LocationX();//返回本地结点的x坐标
  Meters_t  LocationY();//返回本地结点的y坐标
  Meters_t  LocationZ();//返回本地结点的z坐标
  Location  GetLocation();//返回本地结点当前位置
  Location  UpdateLocation();//更新本地结点的位置
  Meters_t Distance(Node*);//返回本地结点到指定对等结点的距离

  Mobility* AddMobility(const Mobility&);//为本地结点增添一个mobility模型
  Mobility* GetMobility();//获取本地结点当前的mobility模型
  bool IsMobile();//判断本地结点是否含有mobility模型
  bool IsMoving();//判断本地结点的当前状态是否是正在运行
  NodeAnimation* GetNodeAnimation() const;//返回指向所有结点结点动作信息块的指针


//无线
  ////void BuildRadioInterfaceList(WirelessLink*);
  ////const RadioVec_t& GetRadioInterfaceList();
  ////void      SetRadioRange(Meters_t range);
  ////Meters_t  GetRadioRange(void);

//回调函数
  void AddCallback(Layer_t, 
	               Proto_t,
                   PacketCallbacks::Type_t,
                   Interface*, 
				   PacketCallbacks::Function_t);//给本结点增加一个回调函数。回调允许检测本结点接收或传输的任意协议层的任意数据包
  void AddCallbackHead(Layer_t, 
	                   Proto_t,
                       PacketCallbacks::Type_t,
                       Interface*, 
					   PacketCallbacks::Function_t);//给本地结点田间一个回调函数，该函数回调优先级最高
  void DeleteCallback(Layer_t, 
	                  Proto_t, 
					  PacketCallbacks::Type_t,
                      Interface*);//从本地结点删除一个回调函数
  bool CallCallbacks(Layer_t, 
	                 Proto_t,
                     PacketCallbacks::Type_t, 
					 Packet*, 
					 Interface* = nil);//调用本地结点的回调函数
  void UserInformation(void*);//存储用户仿真过程中任何有用信息
  void* UserInformation();//返回之前指定的用户信息指针，若没有返回空

 
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

//QT窗口信息
  void Show(bool); //根据参数确定是否要显示窗口
  bool Show();//返回显示是否要被激活
  MyCanvasItem* Display(QTWindow*);//确定点显示的形状、大小等，显示位置为点当前的位置
  MyCanvasItem* Display(const MyPoint&, QTWindow*);//确定点显示的形状、大小等，显示位置参数中提供的位置
  ////void WirelessTxColor(const MyColor&);
  ////const MyColor& WirelessTxColor();
  ////bool PushWirelessTx(MyCanvasItem*);
  ////MyCanvasItem* PopWirelessTx();
  void PixelSize(Count_t);//设置点在显示时的大小
  Count_t PixelSizeX();//返回点在显示时的宽度
  Count_t PixelSizeY();//返回点在显示时的高度
  void Shape(Shape_t);//设置点在显示时的形状
  Shape_t Shape();//返回点在显示时的形状
  CustomShape_t CustomShape();//返回一般图形的回调函数
  void CustomShape(CustomShape_t);//设置一般图形的回调函数
  bool CustomShapeFile(const char*);//载入一个包含一般图形的文件，若成功返回true，否则返回false
  bool CustomShapeImage(const Image&);//载入一个一般图形，若成功返回true，否则返回false
  void Color(const MyColor&);//指定本地结点的颜色
  bool HasColor();//检查本地结点是否已经分配颜色
  MyColor& Color();//返回本地结点的颜色


//结点失败管理
  void Down();//设定该结点无效，无法产生或转发数据包
  void Up();//设定该结点已经恢复，重新有效
  bool IsDown();//查看结点是否有效，若无效则返回true，若正常则返回false

//数据成员
private:
  NodeId_t id; // 本地结点id
public:
  NodeImpl* pImpl; //ghost或者 real型结点

//静态成员
public:
  static NodeId_t nextId;// 邻接有效结点的id
  static NodeVec_t nodes;// 所有有效点的集合
  static double defaultMaxSpeed;//结点的默认最大速率(无线有关)
#ifdef HAVE_QT
  static Count_t defaultPixelSize; //结点显示的默认像素大小
  static Shape_t defaultShape;     //结点显示的默认形状
#endif
public:
  static void SetNextNodeId(NodeId_t i) { nextId = i;}//设置下一结点的id
  static const NodeVec_t& GetNodes() { return nodes;}//返回包含所有结点的vector
  static Node* GetNode(NodeId_t);//返回指定结点id对应的结点的指针，若不存在则返回空
  static void Clear();//清空所有结点

#ifdef HAVE_QT  
  static void DefaultPixelSize(Count_t);//设置有效结点显示时的默认像素大小
  static Count_t DefaultPixelSize();//返回有效结点显示时的默认像素大小
  static void DefaultShape(Shape_t);//设置结点显示时的默认形状
  static Shape_t DefaultShape();//返回结点显示时的默认形状

#endif
  static void   DefaultMaxSpeed(double speed);//设置点默认的最大传输速率
  static double DefaultMaxSpeed();//返回点默认的最大速率
  
  Joules_t getBattery (void);//获取本地结点电池容量
  void     setBattery (Joules_t);//设置本地结点电池容量
  double   getComputePower(void);//获取本地结点CPU功耗
  void     setComputePower(double);//设置本地结点CPU功耗

public:
  bool GetDomainRoute()                 {return domainRoute;}
  void SetDomainRoute(bool _domainroute){domainRoute=_domainroute;}

//代理路由相关内容
private:
  IPAddr_t    proxyIP;//代理路由IP
  Mask        proxyMask;//代理路由掩码
  bool        domainRoute;//边界路由

public:
  void        SetProxyRoutingConfig(IPAddr_t ip, Mask mask);//设置代理路由IP和掩码
  bool        HasProxyRoutingConfig() {return proxyIP!=IPADDR_NONE;};//判断本地结点是否为路由结点，是则返回true，否则false
  bool        CanProxyRouteIP(IPAddr_t ip);//判断目标ip与本地结点是否在同一网段，在则返回true，否则false
  IPAddr_t    GetProxyIP();//返回代理路由IP
  Mask        GetProxyMask();//返回代理路由掩码
  int         GetLongestPrefixLength(IPAddr_t ip);//求出若要使目标IP与本地结点在同一网段，所需设置的掩码的最大前缀长度，并将其返回

//v1.01 
//public:
//
//  RouteTable* getRouteTable(void);//返回本地结点的路由表
//  void        setRouteTable(RouteTable*);//设置本地结点路由表
  
};


#endif
