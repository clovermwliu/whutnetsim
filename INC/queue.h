

// Virtual Base class for all queue types.

#ifndef __queue_h__
#define __queue_h__

#include <iostream>
#include <deque>
#include <list>
#include <vector>

#include "packet.h"
#include "tcp.h"   // TimeSeq_t声明在此头文件中

class Interface;

typedef unsigned int Qrgb; // This is so we don't need any qt includes here

//ForceLoss类用于处理强制丢包的相关信息
class ForcedLoss { // Information for forcing packet losses
public:
  ForcedLoss(Time_t t, Count_t c)
    : time(t), count(c), expiration(INFINITE_TIME) {}
  ForcedLoss(Time_t t, Count_t c, Time_t e)
    : time(t), count(c), expiration(e) {}
public:
  Time_t  time;  // 强制丢包开始时刻
  Count_t count; // 从time时刻开始丢包的数量
  Time_t  expiration; // 强制丢包结束时刻
};

typedef std::list<Packet*>    PktList_t;  // 等待判定的数据包队列
typedef std::list<ForcedLoss> LossList_t; // 被强制丢弃的数据包的队列（类型为ForceLoss）
typedef std::vector<Qrgb>     ColorVec_t; // 每个数据包的颜色

//NDInfo类的定义，此类记录提供non-detailed队列的信息
class NDInfo { 
public:
  NDInfo() : startTx(0), stopTx(0), size(0), updated(false) { }
  NDInfo(Time_t s, Time_t e, Size_t sz)
    : startTx(s), stopTx(e), size(sz), updated(false) { }
public:
  Time_t startTx; // 传输开始时刻  
  Time_t stopTx;  // 传输结束时刻  
  Size_t size;    // 数据包的大小，以字节为单位 
  bool   updated; // 如果队列大小已经更新则updated为True
};

class gui_ctrl;
typedef std::deque<NDInfo> NDDeq_t; // Non-detailed模式的队列缓冲区的类型
typedef std::vector<MyCanvasLine*> ItemVec_t;


//Queue类是队列接口的基类，在数据包要传入链路前将其入队，可以继承该类实现不同的
//队列管理机制
class Queue {
public:
  Queue(bool d = true)
    : totalByteSeconds(0), startTime(0.0),
      lastUpdate(0.0), detailed(d), forcedLosses(nil), timeSizeLog(nil),
      timeSizePackets(false),
#ifndef WIN32
      verbose(false), dropCount(0), totEnq(0), ndDeq(nil), interface(nil),
#else
	  verbose(false), dropCount(0), totEnq(0), ndDeq(nil), interface_(nil),
#endif
      animate(false), qLines(nil)
    { }

  // 析构函数
  virtual ~Queue() { }


  virtual bool Enque(Packet*) = 0;  //将一个数据包入队  

  virtual Packet* Deque() = 0;      //让一个数据包出队，并返回该数据包的指针 
  
  virtual Packet* PeekDeque() = 0; //？？返回指向队首数据包的指针，但是并不删除
 
  virtual void DummyEnque(Packet*);//？？假出队，在链路空闲或者没有必要进行队列操作时被调用
 
  virtual Count_t DequeAllDstMac(MACAddr);//删除队列所有MAC地址为指定MAC地址的数据包，返回删除的数据包个数
   
  virtual Count_t DequeAllDstIP(IPAddr_t);//删除队列所有目标IP为指定目标IP地址的数据包，返回删除的数据包个数

  virtual Packet* DequeOneDstMac(MACAddr);//删除队列一个MAC地址为指定MAC地址的数据包，返回指向删除的数据包的指针
  
  virtual Packet* DequeOneDstIP(IPAddr_t); //删除队列一个目标IP为指定目标IP地址的数据包，返回指向删除的数据包的指针

  virtual Count_t Length() = 0;       //返回当前队列的长度（单位：字节）
  
  virtual Count_t LengthPkts() = 0;   //返回当前队列的长度（单位：数据包的数目）

  virtual Queue* Copy() const = 0;   //复制一个队列，返回指向副本队列的指针 

  virtual void SetLimit(Count_t) = 0; //设置新的队列长度上限值（单位：字节）

  virtual Count_t GetLimit() const = 0;    //获取队列长度上限值（单位：字节） 

  virtual void SetLimitPkts(Count_t) = 0;  //设置新的队列长度上限值（单位：数据包个数） 
  
  virtual Count_t GetLimitPkts() const = 0; //获取队列长度上限值（单位：数据包个数）

  DCount_t Average();             //获取平均的队列长度   

  void     ResetAverage();       //重置平均队列长度，通过启动新的时间间隔来计算新的平均队列长度 

  void     UpdateAverage();     //如果队列发生改变，更新平均队列长度的值     

  virtual bool Check(Size_t, Packet* = nil) = 0;//测试队列缓冲区是否还有空间能放下该数据包

  virtual bool Detailed();      //判断detailed模式是否被使用，有则返回True，否则返回False

  virtual void Detailed(bool);  //设定是否使用detailed模式

  virtual Time_t   QueuingDelay();   //计算排队延时 

  virtual DCount_t TotalWorkload(); //技术队列的工作负载总量（单位：byte-secs）  

  virtual void     SetInterface(Interface*);  //绑定相关接口 

  virtual Count_t  DropCount();     //返回丢弃的数据包的总数  

  virtual Count_t  EnqueueCount();   //返回入队的数据包总数 

  virtual void     CountEnq(Packet* = nil);//对一次入队操作进行计数 

  virtual void     ResetStats();      //重置所用从属于此队列的数据为0

  void             AddForcedLoss(Time_t, Count_t = 1, Time_t = INFINITE_TIME);//添加一次强制丢包行为有关的信息。
    //允许模拟器在某个特定的时刻丢包并观察协议对当前丢包行为的反应
   
  bool             CheckForcedLoss(bool = true);//判断一次强制丢包是否应该被执行

  virtual bool CheckSpoofedSource(Packet*); //一些队列管理机制会执行包过滤，判断是否丢弃来自某个源IP欺骗

  void    EnableTimeSizeLog(bool = true);//是否开启日志记录队列长度随时间的变化
 
  void    LogSizePackets(bool = true);//日志中队列长度的单位是为数据包个数或是字节
 
  void    PrintTimeSizeLog(std::ostream&,
                           Count_t div = 0,
                           char sep = ' ');//将记录队列长度随时间变化的日志导入文件中
 
  void    Animate(bool b = true) { animate = b;}//设定这个队列是否需要使用动画

  bool    Animated() { return animate;}//查询动画是否开启，返回真表示动画开启
 
  virtual void    DisplayQueue(gui_ctrl*, Node*, Node*);//动画展示缓冲队列的使用率

  virtual Packet* GetPacket(Count_t k) = 0;//获取指向第k个包的指针
 
  virtual void     GetPacketColors(ColorVec_t&) = 0;//？？为等待处理数据包上色

  void             Verbose(bool v = true) { verbose = v;}//设定是否使用Verbose调试

protected:
  void UpdateTimeSizeLog();//更新记录队列长度随时间变化的日志

public:
  // 管理默认队列
  static void    Default(const Queue& d);//set一个默认的队列

  static Queue&  Default();//返回对默认队列对象的引用

  static void    DefaultLength(Count_t l);//设定默认队列缓冲区的大小（单位：字节）

  static Count_t DefaultLength() { return defaultLength;}//返回默认队列缓冲区的大小（单位：字节）
 
  static void    DefaultLimitPkts(Count_t l);//设定默认队列缓冲区的大小（单位：数据包个数）

  static Count_t DefaultLimitPkts() { return defaultLimitPkts;}//返回默认队列缓冲区的大小（单位：数据包个数）


  

private:
  DCount_t    totalByteSeconds;     // 工作负载
  Time_t      startTime;            // 度量开始时刻
  Time_t      lastUpdate;           // 最近一次更新时刻
  bool        detailed;             // 启用detailed排队则为真True 
  LossList_t* forcedLosses;         // 强制丢包列表
  TimeSeq_t*  timeSizeLog;          // 队列长度随时间变化的日志
  bool        timeSizePackets;      // 队列长度日志用数据包数作为单位，则为真True 
protected:
  bool        verbose;              // 使用verbose调试，则为真
  Count_t     dropCount;            // 丢弃的总的数据包数
  Count_t     totEnq;               // 入队的总数据包数
  NDDeq_t*    ndDeq;                // 指向non-detailed模式下队列信息的指针
public:
#ifndef WIN32
	Interface*  interface;            // 相关接口
#else
	Interface*  interface_;            // 相关接口
#endif
private:
  bool        animate;              // 需要使用队列动画，则为真True
  ItemVec_t*  qLines;               // MyCanvasLines used to animate queue？？？？？？？？？？？？
  // 静态成员
  static Queue*  defaultQueue;      // 默认队列类型
  static Count_t defaultLength;     // 默认的队列长度
  static Count_t defaultLimitPkts;  // 默认的队列长度上限（数据包数）
public:
  static Count_t defaultAnimWidth;  // 动画中默认的队列现实宽度 
  static Count_t globalQueueDrop;   // 因队列缓冲区溢出总的丢包数
};

#endif



