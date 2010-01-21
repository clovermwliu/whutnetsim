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

  virtual NodeImpl* Copy() const = 0; //所有的Nodeimpl都能被复制
  NodeId_t Id() { return pNode->Id(); }//返回结点Id

//IP地址管理
  virtual IPAddr_t GetIPAddr();          // 获取该结点的IP地址
  virtual void     SetIPAddr(IPAddr_t i);// 设置该结点的IP地址
  virtual void     IPAddrs(IPMaskVec_t&);// 返回该结点所有接口的IP地址和掩码信息
  virtual bool     LocalIP(IPAddr_t);    // 判断IPAddr_t是否为本结点IP地址，是则返回true
  virtual bool     IPKnown();            // 判断是否分配了一个非默认的IP地址，是则返回true
  virtual bool     IsReal() = 0;         // 判断是否是real类型结点，是则返回true

//Satellite功能
  virtual bool     FirstBitRx() const { return false;}
  virtual void     FirstBitRx(bool)   { }
  
//交换机功能
  virtual void IsSwitchNode(bool) = 0;//是否将该结点置为交换机
  virtual bool IsSwitchNode() = 0;   //判断该结点是否是一个交换机
  virtual void PacketRX(Packet* p, Interface* iface) = 0;//若交换机结点收到一个数据包，则该函数被执行

//接口
  virtual Interface* AddInterface(Interface*);//添加一个已经设置好的接口
  virtual Interface* AddInterface(const L2Proto&, bool bootstrap=false);//添加新的空接口，该接口分配第二层网络协议
  virtual Interface* AddInterface(const L2Proto&, //为本地结点新增接口，IP地址、掩码指定，MAC地址默认，并分配第二层网络协议
                                  IPAddr_t, Mask_t,
                                  MACAddr = MACAddr::Allocate(),
				                  bool bootstrap = false);
  virtual Interface* AddInterface(const L2Proto&, //为本地结点新增接口，IP地址、掩码、接口类型指定，MAC地址默认，并分配第二层网络协议
	                              const Interface&,
				                  IPAddr_t, 
								  Mask_t, 
				                  MACAddr = MACAddr::Allocate(),
				                  bool bootstrap = false);
  virtual Count_t    InterfaceCount();   //获取与该结点绑定的接口的数目，返回值是接口数目
  virtual const IFVec_t& Interfaces();   //获取与本地结点绑定的所有接口的信息
  virtual Interface* GetIfByLink(Link*); //根据边查找对应接口
  virtual Interface* GetIfByNode(Node*); //根据点查找对应接口
  virtual Interface* GetIfByIP(IPAddr_t);//根据本地接口IP地址查找对应接口
  virtual void       Broadcast(Packet*, Proto_t) = 0;//通过该结点的所有接口广播数据包。

//双向链路：该部分函数功能均为与远程结点建立全双工的链路连接，返回值均为新建链路对应的本地接口
  virtual Interface* AddDuplexLink(Node*);//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为默认
  virtual Interface* AddDuplexLink(Node*, const Linkp2p&);//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型指定
  virtual Interface* AddDuplexLink(Interface*,Interface*);//新建一个连接两个指定接口的双向链路，链路类型默认
  virtual Interface* AddDuplexLink(Interface*,Interface*, const Linkp2p&);//新建一个连接两个指定接口的双向链路，链路类型默认
  virtual Interface* AddDuplexLink(Node*,//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为默认。为本地与远程结点新增的接口分配IP地址和掩码
                                   IPAddr_t, 
								   Mask_t, 
								   IPAddr_t, 
								   Mask_t);
  virtual Interface* AddDuplexLink(Node*,//连接远程结点，本地结点新增一条双向链路和一个接口，链路类型为默认。为本地与远程结点新增的接口分配IP地址和掩码
	                               const Linkp2p&,
                                   IPAddr_t, 
								   Mask_t, 
								   IPAddr_t,
								   Mask_t);
  virtual Link*      GetLink(Node*);    //返回与指定结点联通的链路
  //v1.01
  //virtual Interface* AddRemoteLink(IPAddr_t, Mask_t);//仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。链路的带宽和延迟默认
  //virtual Interface* AddRemoteLink(IPAddr_t, Mask_t, Rate_t, Time_t);//仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。链路的带宽和延迟指定
//单向链路
  virtual Interface* AddSimplexLink(Node*);//连接远程结点，本地结点新增一条单向链路和一个接口，链路类型为默认
  virtual Interface* AddSimplexLink(Node*, const Linkp2p&);//连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定
  virtual Interface* AddSimplexLink(Node*, //连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定，本地接口IP地址和掩码指定
	                                const Linkp2p&,
                                    IPAddr_t, 
									Mask_t);

//队列
  virtual Queue* GetQueue() = 0;        //获取本地结点的队列，仅对单接口的结点有效
  virtual Queue* GetQueue(Node*) = 0;   //获取本地结点通往指定目标结点的队列

//应用
  virtual Application* AddApplication(const Application& a) = 0;//给本地结点添加指定应用

//邻接结点管理：在路由协议中使用
  virtual void Neighbors(NodeWeightVec_t &, bool); //获取所有的邻近结点
  virtual Count_t NeighborCount();       //获取路由表中邻近结点的数目
  virtual void NeighborsByIf(Interface*, IPAddrVec_t&);//获取与本地结点指定接口相连的邻接结点IP地址

//路由
  virtual void DefaultRoute(RoutingEntry) = 0;//指定发送数据包的默认路由项
  virtual void DefaultRoute(Node*) = 0;//指定发送数据包的默认下一跳结点
  virtual void AddRoute( IPAddr_t, Count_t, Interface*, IPAddr_t) = 0;//增加一个路由项，目的IP及其掩码、本地接口、下一跳IP地址确定
  virtual RoutingEntry LookupRoute(IPAddr_t) = 0; //根据目的IP地址查找本地路由项，若存在则返回
  virtual RoutingEntry LookupRouteNix(Count_t) = 0;//根据邻接结点索引查找本地路由项，若存在则返回
  virtual Routing::RType_t RoutingType() = 0;//获取本地结点当前所使用的路由类型
  virtual Interface* LocalRoute(IPAddr_t);//判断某IP地址是否是直接与本地结点某interface相连，若是则返回该interface
  virtual void       InitializeRoutes() = 0;//路由初始化，为路由协议做准备
  virtual void       ReInitializeRoutes(bool) = 0; //重新初始化路由
  virtual Count_t    RoutingFibSize() const = 0;//统计本地结点路由的大小
  virtual Count_t    GetNix(Node*) const;//根据指定结点查找邻接结点的Index，若该结点不是邻接结点，返回MAX_COUNT
  virtual RoutingNixVector* GetNixRouting() = 0;   // Returns Nix rtg pointer
  virtual Routing*   GetRouting() = 0;//返回本地结点所使用的路由对象的指针

//AODV路由
  //////virtual AODVRoutingEntry *LookupRouteAODV(IPAddr_t ip) =0;
  //////virtual void      SetRoutingAODV(void *pRouting)= 0;
  //////virtual RoutingAODV *GetRoutingAODV() = 0;

//协议族接口
  virtual Protocol* LookupProto(Layer_t, Proto_t) = 0;//在本地结点，根据协议层和编号查找对应的协议对象
  virtual void      InsertProto(Layer_t, Proto_t, Protocol*) = 0;//在本地结点插入一个协议对象，.协议层和编号指定

//多路分配器接口
  virtual bool Bind(Proto_t, PortId_t, Protocol*) = 0;//为指定端口绑定指定协议
  virtual bool Bind(Proto_t,
                    PortId_t, 
					IPAddr_t,
                    PortId_t, 
					IPAddr_t,
                    Protocol*) = 0;//为本地结点指定端口和远程结点指定端口绑定指定协议
  virtual PortId_t Bind(Proto_t, Protocol*) = 0;//绑定指定协议，对应本地端口随机
  virtual bool Unbind(Proto_t, PortId_t, Protocol*) = 0;//删除本地结点指定接口绑定的指定协议
  virtual bool Unbind(Proto_t,
                      PortId_t, 
					  IPAddr_t,
                      PortId_t, 
					  IPAddr_t,
                      Protocol*) = 0;//本地结点指定端口和远程结点指定端口绑定指定协议
  virtual Protocol* LookupByPort(Proto_t, PortId_t) = 0;//根据本地端口和协议号查找所绑定的协议
  virtual Protocol* LookupByPort(Proto_t,
                                 PortId_t, IPAddr_t,
                                 PortId_t, IPAddr_t) = 0;  //查找协议，根据协议号、本地结点端口、IP地址、远程结点端口、IP地址

//数据包跟踪接口
  virtual bool      TracePDU(Protocol*, PDU*, Packet*, char*) = 0;//将指定PDU内容从指定协议记录到跟踪文件
  virtual void      SetTrace(Trace::TraceStatus) = 0;//设置本地结点的跟踪状态

//结点坐标(x, y, z)接口
  virtual void      SetLocation(Meters_t, Meters_t, Meters_t) = 0;//直接在xyz坐标系中设置本地结点的坐标
  virtual void      SetLocation(const Location&) = 0; //根据location对象设置本地结点在xyz坐标系中的坐标
  virtual void      SetLocationLongLat(const Location&) = 0;
  virtual bool      HasLocation() = 0;//检查本地结点是否已分配地址
  virtual Meters_t  LocationX() = 0;//返回本地结点的x坐标        
  virtual Meters_t  LocationY() = 0;//返回本地结点的y坐标
  virtual Meters_t  LocationZ() = 0;//返回本地结点的z坐标
  virtual Location  GetLocation() = 0;//返回本地结点当前位置
  virtual Location  UpdateLocation() = 0;//更新本地结点的位置
  virtual Meters_t  Distance(Node*) = 0;//返回本地结点到指定对等结点的距离
  virtual Mobility* AddMobility(const Mobility&) = 0;//为本地结点增添一个mobility模型
  virtual Mobility* GetMobility() const = 0; //获取本地结点当前的mobility模型
  virtual bool      IsMobile() = 0; //判断本地结点是否含有mobility模型
  virtual bool      IsMoving() = 0; //判断本地结点的当前状态是否是正在运行
#ifdef HAVE_QT //v1.01
  virtual void      UserInformation(void*) = 0;//存储用户仿真过程中任何有用信息 
  virtual void*     UserInformation() = 0;//返回之前指定的用户信息指针，若没有返回空
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

//QT窗口信息
#ifdef HAVE_QT  //v1.01
  virtual void           Show(bool) = 0; //根据参数确定是否要显示窗口
  virtual bool           Show() = 0;//返回显示是否要被激活
  virtual MyCanvasItem*   Display(QTWindow*) = 0;//确定点显示的形状、大小等，显示位置为点当前的位置
  virtual MyCanvasItem*   Display(const MyPoint&, QTWindow*) = 0;//确定点显示的形状、大小等，显示位置参数中提供的位置
  ////virtual void           WirelessTxColor(const MyColor&) = 0;
  ////virtual const MyColor&  WirelessTxColor() = 0;
  ////virtual bool           PushWirelessTx(MyCanvasItem*) = 0;
  ////virtual MyCanvasItem*   PopWirelessTx() = 0;
  virtual void           PixelSize(Count_t) = 0;//设置点在显示时的大小
  virtual Count_t        PixelSizeX() = 0;//返回点在显示时的宽度
  virtual Count_t        PixelSizeY() = 0;//返回点在显示时的高度
  virtual void           Shape(Node::Shape_t) = 0;//设置点在显示时的形状
  virtual Node::Shape_t  Shape() = 0;//返回点在显示时的形状
  virtual CustomShape_t  CustomShape();//返回一般图形的回调函数
  virtual void           CustomShape(CustomShape_t);//设置一般图形的回调函数
  virtual bool           CustomShapeFile(const char*);//载入一个包含一般图形的文件，若成功返回true，否则返回false
  virtual bool           CustomShapeImage(const Image&);//载入一个一般图形，若成功返回true，否则返回false
  virtual void           Color(const MyColor&) = 0;//指定本地结点的颜色
  virtual bool           HasColor() = 0;//检查本地结点是否已经分配颜色
  virtual MyColor&        Color() = 0;//返回本地结点的颜色
  virtual NodeAnimation* GetNodeAnimation() const = 0;//返回指向所有结点结点动作信息块的指针
#endif
  
//ICMP相关信息
  ////virtual bool   ICMPEnabled() const;//检查本地结点是否支持ICMP
  ////virtual void   DisableICMP();//设定本地结点不支持ICMP

//管理运行失败信息
  virtual void   Down();//设定该结点无效，无法产生或转发数据包
  virtual void   Up();//设定该结点已经恢复，重新有效
  virtual bool   IsDown();//查看结点是否有效，若无效则返回true，若正常则返回false

//无线传输信息
  ////virtual void      BuildRadioInterfaceList(WirelessLink*) = 0;
  ////virtual const RadioVec_t& GetRadioInterfaceList() = 0;
  ////virtual void      SetRadioRange(Meters_t ) =0;
  ////virtual Meters_t  GetRadioRange(void ) =0;

//回调函数
  virtual void      AddCallback(Layer_t, Proto_t,
                                PacketCallbacks::Type_t,
                                Interface*,
                                PacketCallbacks::Function_t) = 0;//给本结点增加一个回调函数。回调允许检测本结点接收或传输的任意协议层的任意数据包
  virtual void      AddCallbackHead(Layer_t, Proto_t,
                                    PacketCallbacks::Type_t,
                                    Interface*,
                                    PacketCallbacks::Function_t) = 0;//给本地结点田间一个回调函数，该函数回调优先级最高
  virtual void      DeleteCallback(Layer_t, Proto_t, 
                                   PacketCallbacks::Type_t,
                                   Interface*) = 0;//从本地结点删除一个回调函数
  virtual bool      CallCallbacks(Layer_t, Proto_t, 
                                  PacketCallbacks::Type_t, Packet*,
                                  Interface*) = 0;//调用本地结点的回调函数

  virtual void      setBattery (Joules_t) = 0;//设置本地结点电池容量
  virtual Joules_t  getBattery (void) = 0;//获取本地结点电池容量
  virtual void      setComputePower (double) = 0;//设置本地结点CPU功耗
  virtual double    getComputePower (void) = 0;//获取本地结点CPU功耗
  //virtual void        setRouteTable (RouteTable*) = 0;//设置本地结点的路由表
  //virtual RouteTable* getRouteTable (void) = 0;//返回本地结点的路由表

  
protected:
  Node*    pNode;// 指向相应结点接口对象
  bool     usewormcontainment;//判断这个结点是否包含蠕虫功能
//real和ghost之间的数据成员
  IPAddr_t ipAddr;//本地结点的IP地址
  IFVec_t  interfaces;//所有的接口
  Count_t  neighborCount;//邻接接口总数
  bool     down;//判断结点是否无效
};

#endif
