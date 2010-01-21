

//#define DEBUG_MASK 0x08
#include "G_debug.h"
#include "droptail.h"
#include "interface.h"
#include "link.h"

#ifdef HAVE_QT
#include "GUI_Defs.h"
#endif

using namespace std;

bool DropTail::Enque(Packet* p)
/*
描述：将一个数据包入队
参数：Arg1 指向到达数据包的指针
返回值：入队成功返回True，若被丢弃则返回FALSE
*/
{
  if (!Detailed()) UpdateSize();
  UpdateAverage();
  totEnq++;
  if (verbose && 0)//verbose调试
    {
      cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl;
    }
  bool full = false;
  if (pktLimit)
    { // 队列长度单位为数据包个数
      full = (pktCount == pktLimit);
    }
  else
    { // 队列长度单位为字节
      full = (size + p->Size()) > Limit();
    }
  
  DEBUG(0,(cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl));
  pktCount++; // 队列中数据包个数的计数
  if (CheckForcedLoss() || full)
    {
      DEBUG(2,(cout << "Dropping pkt, size " << size
               << " pSize " << p->Size()
               << " limit " << Limit()
               << " time " << Simulator::Now()
               << endl));
      dropCount++;
      pktCount--;
      globalQueueDrop++;
      Stats::pktsDropped++;
      if (verbose)
        {
          cout << "Dropping pkt, size " << size
               << " pSize " << p->Size()
               << " limit " << Limit()
               << " time " << Simulator::Now()
               << endl;
        }
      return false; // 队列已满返回False
    }
  DBG((Stats::pktsEnqued++));
  size += p->Size(); // 更新队列长度值
  if (Detailed())
    {
      pkts.push_back(p);// 如果是detailed模式，保存该数据包到队列缓冲区
    }
  else 
    { // Non-Detailed模式下
#ifndef WIN32
      if (!interface) return false;      //？？？？？？？？？？？？？？？？？？ Can't handle w/o interface？？？？？？？？？？？
#else
		if (!interface_) return false;      // Can't handle w/o interface
#endif
      if (!ndDeq) ndDeq = new NDDeq_t(); // 分配non-detailed模式存数据包的空间（deque容器）
      Time_t startTx = Simulator::Now(); // 此数据包开始传输的时刻
      if (!ndDeq->empty())
        { // 如果双端队列中非空，则此数据包的起始传送时刻为队列中最后一个包传输完毕的时刻
          NDInfo& nd = ndDeq->back();   
          startTx = nd.stopTx;           
        }
#ifndef WIN32
      Link* link = interface->GetLink();
#else
	  Link* link = interface_->GetLink();
#endif
      if (!link) return false;           // 链路必须存在，否则返回False
      Time_t txTime =  ((Rate_t)p->Size() * 8) / link->Bandwidth();
      NDInfo  newND(startTx, startTx + txTime, p->Size());
      ndDeq->push_back(newND);           // 将新数据包的信息添加到队列
    }
  DEBUG(1,(cout << " newsize " << size << endl));
  return true;                           // 队列未满，数据包成功入队
}

Packet* DropTail::Deque()
/*
描述：将一个数据包出队
参数：Arg1 指向到达数据包的指针
返回值：成功出队则返回指向该数据包的指针，若为non-detailed队列或队列为空，则返回0
备注：non-detailed永不会调用Deque
*/
{ 
  if (!Detailed()) return nil;            //非detailed队列，返回0
  UpdateAverage();
  if (Length() == 0) return nil;         // 队列为空，返回0
  DBG((Stats::pktsDequed++));
  Packet* p = pkts.front();              // 获取队首数据包的指针
  pkts.pop_front();                      // 队首数据包出队
  size -= p->Size();                     // 更新队列长度
  pktCount--;                            // 更新队列中数据包数
  return p;                              // 返回指向出队数据包的指针
}

Packet* DropTail::PeekDeque()
//返回指向队首数据包的指针，但是并不删除
/*
描述：不是真的让数据包出队
参数：
返回值：队列不空则返回指向队首数据包的指针，否则则返回0
*/
{
  if (Length() == 0) return nil;        
  return pkts.front(); 
}

Count_t DropTail::DequeAllDstMac(MACAddr m)
/*
描述：删除队列所有MAC地址为指定MAC地址的数据包
参数：Arg1 指定的MAC地址值
返回值：返回删除的数据包个数
*/
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
  //从队首开始查找，凡遇到MAC地址为m的数据包删除
    {
      PktList_t::iterator next = i;
      next++;
      Packet* p = *i;
      if (p->GetDstMac() == m)
        {
          count++;
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          delete p;
        }
      i = next;
    }
  return count;
}

Count_t DropTail::DequeAllDstIP(IPAddr_t ip)
/*
描述：删除队列所有MAC地址为指定IP地址的数据包
参数：Arg1 指定的IP地址值
返回值：返回删除的数据包个数
*/
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
  //从队首开始查找，凡遇到IP地址为ip的数据包删除
    {
      PktList_t::iterator next = i;
      next++;
      Packet* p = *i;
      if (p->GetDstIP() == ip)
        {
          count++;
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          delete p;
        }
      i = next;
    }
  return count;
}

Packet* DropTail::DequeOneDstMac(MACAddr m)
/*
描述：删除队列中一个MAC地址为指定MAC地址的数据包
参数：Arg1 指定的MAC地址值
返回值：返回指向删除的数据包的指针，未找到则返回0
*/
{
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      if (p->GetDstMac() == m)
        {
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          return p;
        }
    }
  return nil;
}

Packet* DropTail::DequeOneDstIP(IPAddr_t ip)
/*
描述：删除队列中一个IP地址为指定IP地址的数据包
参数：Arg1 指定的IP地址值
返回值：返回指向删除的数据包的指针，未找到则返回0
*/
{
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      if (p->GetDstIP() == ip)
        {
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          return p;
        }
    }
  return nil;
}

Count_t DropTail::Length()
// 返回当前队列的长度（单位：字节）
{
  if (!Detailed())UpdateSize();
  return (Count_t)size;
}

Count_t DropTail::LengthPkts()
// 返回当前队列的长度（单位：数据包个数）
{
  if (!Detailed())UpdateSize();
  return pktCount;
}

void   DropTail::SetInterface(Interface* i)
/*
描述：将队列绑定到相应的接口上
参数：Arg1 相应的接口
*/
{
#ifndef WIN32
  interface = i;
#else
  interface_ = i;
#endif
}

Queue* DropTail::Copy() const
/*
描述：复制队列
参数：
返回值：指向队列副本的指针
*/
{ 
  return new DropTail(*this);
}

void DropTail::SetLimit(Count_t l)
/*
描述：设定队列长度值的上限
参数：Arg1 队列长度的上限值（字节）
返回值：无
*/
{
  limit = l;
}

Time_t   DropTail::QueuingDelay()
/*
描述：计算排队延时
参数：无
返回值：排队延时，若队列不存在或为空时返回0
*/
{
  if (Detailed()) return Queue::QueuingDelay();
  UpdateSize();
  // non-detailed模式时，使用ndDeq列表
  Time_t now = Simulator::Now();
  // 找到队列中最后一个数据包传输完的时刻
  if (!ndDeq) return 0;  
  if (ndDeq->empty()) return 0; 
  NDInfo& nd = ndDeq->back();   
  return (nd.stopTx - now);     
}


bool DropTail::Check(Size_t s, Packet*)
/*
描述：检查队列缓冲区是否还有空间能放下到达数据包
参数：Arg1 数据包的大小（字节）
	    Arg2 指向该数据包的指针
返回值：若存在足够的空间放该数据包则返回True，否则返回False
*/
{ 
  if(!Detailed())UpdateSize();
  if (Length() == 0) return true; // 如果队列为空，则总是可以放下的
  if (pktLimit)
    { // 使用数据包个数作为队列长度限制时
      return pktCount < pktLimit;
    }
  // 使用字节数作为队列长度限制时
  return ((Length() + s) < Limit());
}

Packet* DropTail::GetPacket(Count_t k)
/*
描述：获取指向队列中第k个数据包的指针
参数：Arg1 序数k
返回值：指向队列中第k个数据包的指针，如果队列中数据包不足k个，则返回0
*/
{
  if (pkts.empty()) return nil;

  PktList_t::iterator i = pkts.begin();

  for (Count_t j = 0; j < k; ++j)
    {
      ++i;
      if (i == pkts.end()) return nil; // 不存在第k个数据包
    }
  return *i;
}

void    DropTail::GetPacketColors(ColorVec_t& cv)
{ // Build a list of packet colors from head to tail？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
#ifdef HAVE_QT
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      MyColor c;
      if (p->IsColored())
        c = MyColor(p->R(), p->G(), p->B());
      else
        c = Qt::blue;
      cv.push_back(c.rgb());
    }
#endif
}

// 私有方法
void DropTail::UpdateSize()
/*
描述：自上次更新后，由于又有数据包被传送，需要更新队列长度
参数：无
返回值：无
*/
{ 
  if (!ndDeq) return; // 如果不存在non-detailed的队列，直接返回
  Time_t now = Simulator::Now();
  while(!ndDeq->empty())
    {
      NDInfo& nd = ndDeq->front();
      if (nd.startTx > now) return;      // 传输时刻未到，将来再启动
      if (nd.stopTx  > now)
        { //此时该数据包正在传输，将队列长度更新，但此包不出队，并返回
		
          if (!nd.updated)
            {
              size -= nd.size;
              pktCount--;
              nd.updated = true;
            }
          return;
        }
      //nd代表的数据包处理完了，将其出队，并考察其后一数据包如何处理
      if (!nd.updated)  
        {
          size -= nd.size;
          pktCount--;
        }
      ndDeq->pop_front(); 
    }
}



