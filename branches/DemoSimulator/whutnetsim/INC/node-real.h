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

// 无线动画中的画图部件
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

typedef std::map<MACAddr, PortPair> MACPort_t;//定义路由端口表
typedef std::map<Count_t, RoutingEntry> RENixMap_t;//定义cache nixvector路由表
typedef std::vector<RoutingEntry>       RENixVec_t;//定义Nix路由表



class NodeReal : public NodeImpl {
public:
  NodeReal(Node*);
  ~NodeReal();

  NodeImpl* Copy() const;//根据本结点复制NodeImpl
public:
//IP地址管理、接口管理、链路管理、邻接结点管理继承自node-impl

//类型判断，判断是Real或者Ghost
  bool IsReal();//判断结点是否是real类型的，若是返回true，否则返回false

//Satellite类型
  virtual bool IsSatellite() const { return false;}
  virtual void IsSatellite(bool)   {  }

//广播一个数据包
  void Broadcast(Packet*, Proto_t);//通过所有接口广播一个数据包

//队列
  Queue* GetQueue();//获取本地结点的队列，仅对单接口的结点有效
  Queue* GetQueue(Node*);//获取本地结点通往指定远程结点的队列

//应用
  Application* AddApplication(const Application& a);//给本地结点添加指定应用
  ApplicationVec_t* GetApplicationVec(){ return &pappvec; } //查询应用

//路由
  void DefaultRoute(RoutingEntry);//指定发送数据包的默认路由项
  void DefaultRoute(Node*);//指定发送数据包的默认下一跳结点
  void AddRoute(IPAddr_t, Count_t, Interface*, IPAddr_t);//增加一个路由项，目的IP及其掩码、本地接口、下一跳IP地址确定
  RoutingEntry LookupRoute(IPAddr_t);//根据目的IP地址查找本地路由项，若存在则返回
  RoutingEntry LookupRouteNix(Count_t);//查找NIX路由中的路由项
  Routing::RType_t RoutingType();//获取本地结点当前所使用的路由类型
  void       InitializeRoutes(); //本地路由初始化
  void       ReInitializeRoutes(bool);//本地路由更新
  Count_t    RoutingFibSize() const;//统计本地结点路由的大小
  RoutingNixVector* GetNixRouting();//若本地结点存在NixVector类型路由，则返回该路由对象的指针，否则返回空
  Routing*   GetRouting() { return pRouting;} //返回本地结点所使用的路由对象的指针

//AODV路由
  ////AODVRoutingEntry *LookupRouteAODV(IPAddr_t);
  ////void      SetRoutingAODV(void *pRouting);
  ////RoutingAODV * GetRoutingAODV();

//协议族接口
  Protocol* LookupProto(Layer_t, Proto_t);//在本地结点，根据协议层和编号查找对应的协议对象
  void      InsertProto(Layer_t, Proto_t, Protocol*);//在本地结点插入一个协议对象，.协议层和编号指定

//多路分布器接口
  bool      Bind(Proto_t, PortId_t, Protocol*);//为指定端口绑定指定协议
  bool      Bind(Proto_t,
                 PortId_t, 
				 IPAddr_t,
                 PortId_t, 
				 IPAddr_t,
                 Protocol*);//为本地结点指定端口和远程结点指定端口绑定指定协议
  PortId_t  Bind(Proto_t, Protocol*);//绑定指定协议，端口随机
  bool      Unbind(Proto_t, PortId_t, Protocol*);//删除本地结点指定接口绑定的指定协议
  bool      Unbind(Proto_t,
                   PortId_t, 
				   IPAddr_t,
                   PortId_t, 
				   IPAddr_t,
                   Protocol*);//本地结点指定端口和远程结点指定端口绑定指定协议
  Protocol* LookupByPort(Proto_t, PortId_t);//根据本地端口和协议号查找所绑定的协议
  Protocol* LookupByPort(Proto_t,
                         PortId_t, 
						 IPAddr_t,
                         PortId_t, 
						 IPAddr_t);//查找协议，根据协议号、本地结点端口、IP地址、远程结点端口、IP地址
//包跟踪接口
  bool      TracePDU(Protocol*, PDU*, Packet*, char*);//将指定PDU单元内容从指定协议记录到跟踪文件
  void      SetTrace(Trace::TraceStatus);//设置本地结点的跟踪状态

//结点坐标(x, y, z)接口
  void      SetLocation(Meters_t, Meters_t, Meters_t); //直接在xyz坐标系中设置本地结点的坐标
  void      SetLocation(const Location&);//根据location对象设置本地结点在xyz坐标系中的坐标
  void      SetLocationLongLat(const Location&);   // Set x/y/z location
  bool      HasLocation();//检查本地结点是否已分配地址
  Meters_t  LocationX();//返回本地结点的x坐标
  Meters_t  LocationY();//返回本地结点的y坐标
  Meters_t  LocationZ();//返回本地结点的z坐标
  Location  GetLocation();//返回本地结点当前位置
  Location  UpdateLocation();//更新本地结点的位置
  Mobility* AddMobility(const Mobility&);//为本地结点增添一个mobility模型
  Mobility* GetMobility() const;//获取本地结点当前的mobility模型
  bool      IsMobile();//判断本地结点是否含有mobility模型
  bool      IsMoving();//判断本地结点的当前状态是否是正在运行

#ifdef HAVE_QT  
  ////bool      WirelessTx();                          // True if wireless transmit
  ////bool      WirelessRx();                          // True if wireless receive
  ////bool      WirelessCx();                          // True if wireless collid
  ////bool      WirelessRxMe();                        // True if wireless receive
  ////bool      WirelessRxZz();                        // True if wireless receive
  void      UserInformation(void*);                // Specify user info
  void*     UserInformation();                     // Return user info

// QT窗口信息
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
  void Shape(Node::Shape_t);//设置点在显示时的形状
  Node::Shape_t Shape();//返回点在显示时的形状
  CustomShape_t CustomShape();//返回一般图形的回调函数
  void CustomShape(CustomShape_t);//设置一般图形的回调函数
  bool CustomShapeFile(const char*);//载入一个包含一般图形的文件，若成功返回true，否则返回false
  bool CustomShapeImage(const Image&);//载入一个一般图形，若成功返回true，否则返回false
  void Color(const MyColor&);//指定本地结点的颜色
  bool HasColor();//检查本地结点是否已经分配颜色
  MyColor& Color();//返回本地结点的颜色
  NodeAnimation* GetNodeAnimation() const;//返回指向所有结点结点动作信息块的指针
#endif
  
//ICMP相关
  ////bool ICMPEnabled() const;//检查本地结点是否支持ICMP
  ////void DisableICMP();//设定本地结点不支持ICMP


#ifdef MOVED_TO_IMPL
//结点失败管理
  void Down();//设定该结点无效，无法产生或转发数据包
  void Up();//设定该结点已经恢复，重新有效
  bool IsDown();//查看结点是否有效，若无效则返回true，若正常则返回false
#endif

  Meters_t Distance(Node*);//返回本地结点到指定对等结点的距离
 
  ////void      BuildRadioInterfaceList(WirelessLink*);
  ////const RadioVec_t& GetRadioInterfaceList();
  ////void      SetRadioRange(Meters_t range) { radioRange = range; }
  ////Meters_t  GetRadioRange() { return radioRange; }

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

  void      setBattery(Joules_t joules) { battery = joules;}//设置本地结点电池容量
  Joules_t  getBattery(void) { return battery; }//获取本地结点电池容量
  void      setComputePower(double pow) { computePower = pow; }//设置本地结点CPU功耗
  double    getComputePower(void) { return computePower; }//获取本地结点CPU功耗

  //void        setRouteTable(RouteTable* rtable_) { rtable = rtable_;}//设置本地结点的路由表
  //RouteTable* getRouteTable(void) { return rtable;}//返回本地结点的路由表
//交换机功能
  void        SetTimeout(Time_t);//设置仿真器时间
  int         FindSRCPort(Interface*);//在本地接口中查找是否与目标接口相同的项，若存在则返回与之对应的端口号，否则返回空
  void        PacketRX(Packet*, Interface*);//处理数据包
  MACPort_t*  GetPortMap(){ return &portmap;}//获取端口表
  Time_t      GetTimeout(){ return timeout;}//获取仿真器时间
  bool        IsSwitchNode() { return isswitch; }//判断本地结点是否有交换机功能，若有返回true，否则返回false
  void        IsSwitchNode(bool);//根据参数设置结点是否为交换机

//worm
  ////void SetWormContainment(WormContainment*);
  ////WormContainment* GetWormContainment() { return wormcontainment; } 
  ////void UseARP(bool);

private:
  bool CheckTrace(Protocol*, PDU*);// 检查跟踪是否能进行
#ifdef HAVE_QT
  bool CustomShapeHelper(MyPixmap&);//处理一般图形
#endif
//数据成员
private:
  Routing*   pRouting;    //本地结点路由对象的指针
  RENixVec_t nixLookup;   //nix路由缓冲区

//AODV路由
  ////RoutingAODV*  pRoutingAODV;

  Meters_t x;             // x轴坐标
  Meters_t y;             // y轴坐标
  Meters_t z;             // z轴坐标
  bool     hasLocation;   // True表示坐标已经设定
#ifndef COMPACT
  ProtocolGraph* graph;   //私有协议族
#endif
  Trace::TraceStatus traceStatus;//本地结点的跟踪状态
  Mobility* mobility;     //当前mobility模型
public:
  void*       userInfo;           // User信息

  PortDemux*       demux;           //多路分布器
  Meters_t         radioRange;      //属于接口，不属于结点
  Joules_t         battery;         //假定结点需要电池
  double           computePower;    //CPU损耗
  ////RadioVec_t       radioInterfaces;
  ////RadioVec_t       radioInf2;      
  ////Time_t           radioMarginTime;
  PacketCallbacks  callbacks;//本地结点的数据包回调

#ifdef HAVE_QT  
  CanvasItemDeq_t  canvasItems;     //无线
  NodeAnimation*   animInfo;  //结点动作
  MyColor           wirelessColor;   //无线动画颜色
#endif
  
  bool             icmpEnabled;     //True代表本地结点支持ICMP

  //RouteTable*      rtable;          //路由表对象
  
private:
//交换机成员
  MACPort_t        portmap;//端口表
  Time_t           timeout;//仿真器时间
  bool             isswitch;//true代表该结点为交换机，false则代表该结点不是交换机
  //WormContainment* wormcontainment;//v1.01 

public:
//用于调试
  static Count_t count;    
  static Count_t count1;   
  static Count_t count2; 
  static Count_t count3;  
  static Count_t count4;  
};

//#include "interface.h"

#endif
