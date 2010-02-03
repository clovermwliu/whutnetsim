
// Georgia Tech Network Simulator - DropTail Queue class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __droptail_h__
#define __droptail_h__

#include "queue.h"

class Interface;

#define DEFAULT_DROPTAIL_LIMIT 50000

// DropTail类的定义，实现尾丢弃队列或FIFO队列
class DropTail : public Queue {
public:
  DropTail()
      : Queue(true), limit(Queue::DefaultLength()),
        pktLimit(Queue::DefaultLimitPkts()),
        size(0), pktCount(0), lastTime(0) { }
  // 默认的构造函数


  DropTail(Count_t l)
    : Queue(true), limit(l), pktLimit(0),
      size(0), pktCount(0), lastTime(0) { }
  // 创建一个特定缓冲区大小的队列


  DropTail(const DropTail& c)
    : Queue(c), limit(c.limit), pktLimit(c.pktLimit),
      size(c.size), pktCount(c.pktCount), lastTime(c.lastTime) { }
  // 创建一个和某一队列相同的队列


  virtual ~DropTail() { }// 析构函数


  bool Enque(Packet*); // 将一个数据包入队，入队成功返回True，若被丢弃则返回FALSE

  Packet* Deque();     // 让一个数据包出队，并返回该数据包的指针
 
  Packet* PeekDeque(); // 返回指向队首数据包的指针，但是并不删除

  virtual Count_t DequeAllDstMac(MACAddr); // 删除队列所有MAC地址为指定MAC地址的数据包，返回删除的数据包个数

  virtual Count_t DequeAllDstIP(IPAddr_t); // 删除队列所有目标IP为指定目标IP地址的数据包，返回删除的数据包个数

  virtual Packet* DequeOneDstMac(MACAddr);// 删除队列中一个MAC地址为指定MAC地址的数据包，返回指向删除的数据包的指针

  virtual Packet* DequeOneDstIP(IPAddr_t);// 删除队列中一个目标IP为指定目标IP地址的数据包，返回指向删除的数据包的指针

  Count_t Length();    // 返回当前队列的长度（单位：字节）

  Count_t LengthPkts();// 返回当前队列的长度（单位：数据包的数目）

  void    SetInterface(Interface*);// 绑定相关接口

  Queue*  Copy() const;   // 复制一个队列，返回指向副本队列的指针

  virtual void SetLimit(Count_t l);// 设置新的队列长度上限值（单位：字节）

  Count_t GetLimit() const { return limit;} // 获取队列长度上限值（单位：字节）

  virtual void SetLimitPkts(Count_t l) { pktLimit = l;} // 设置新的队列长度上限值（单位：数据包个数）

  virtual Count_t GetLimitPkts() const { return pktLimit;} // 获取队列长度上限值（单位：数据包个数）

  virtual Time_t  QueuingDelay();  // 计算排队延时  

  bool    Check(Size_t, Packet* = nil);// 检查队列缓冲区是否还有空间能放下到达数据包

  Count_t Limit() { return limit;} // 获取队列缓冲区长度上限的最大值（字节）

  Packet* GetPacket(Count_t k);// 获取指向队列中第k个数据包的指针

  void     GetPacketColors(ColorVec_t&);//？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？

private:
  void    UpdateSize();       // 从上次更新时刻调整当前队列长度
public:
  Count_t    limit;           // 队列长度上限（单位：字节）
  Count_t    pktLimit;        // 队列长度上限（单位：数据包数）
  Count_t    size;            // 当前队列长度（单位：字节）
  Count_t    pktCount;        // 当前队列中数据包个数
  Time_t     lastTime;        // 队列长度被计算的时刻
public:
  PktList_t  pkts;            // 队列缓冲区中的数据包列表
};

#endif
