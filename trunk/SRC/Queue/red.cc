


// 从经典的S.Floyd's simulator中提取的有关RED的参数值
#define q_weight 0.002
#define min_threshold 5
#define max_threshold 15
#define queue_size 60
#define max_prob 0.1
#define mean_pkt_size 500 
////////////////////////////////////////////////////////////
#include "red.h"
#include <math.h>
#include "simulator.h"
#include "rng.h"
#include "interface.h"
#include "link.h"

//#define DEBUG_MASK 0x1
#include "G_debug.h"

using namespace std;

REDQueue::REDQueue() : iface(nil)
//默认构造函数
{
  w_q = q_weight;
  mean_pktsize = mean_pkt_size;
  size = 0;
  min_th = min_threshold * mean_pktsize;
  max_th = max_threshold * mean_pktsize;
  SetLimit(queue_size * mean_pktsize);
  max_p = max_prob;
  bandwidth = 800000;
  count = -1;
  q_avg = 0;
  pktspersec = bandwidth / mean_pktsize;  // [(bits/sec) / (bits/pkt)]
  if (Simulator::instance != NULL)
    {
    idle_since = Simulator::instance->Now();
    }
  else
    {
    idle_since = 0.0;                 
    }
}

REDQueue::REDQueue(
   DCount_t in_w_q, Count_t in_min_th, Count_t in_max_th, 
   Count_t in_limit, DCount_t in_max_p, Count_t in_mean_pktsize) : iface(nil)
/*
描述：可以按自己要求为相关参数赋值的构造函数
参数：Arg1 队列的权重
	  Arg2 最小门限值
	  Arg3 最大门限值
	  Arg4 队列长度的上限值
	  Arg5 标记丢包概率的最大值
	  Arg6 数据包的平均大小   
*/
{
  DEBUG(0,(cout << "REDQueue::RedQueue" << endl));
  //由用户设定参数值
  w_q = in_w_q;
  min_th = in_min_th;
  max_th = in_max_th;
  SetLimit(in_limit);
  max_p = in_max_p;
  mean_pktsize = in_mean_pktsize;
  size = 0;
  bandwidth = 800000;                // 此数据从链路获取
  count = -1;
  q_avg = 0;
  if (Simulator::instance != NULL)
    {
    idle_since = Simulator::instance->Now();
    }
  else
    {
    idle_since = 0.0;                  // 调度器未实例化时
    }
  // pktspersec用于队列空闲时更新平均队列长度
  pktspersec = bandwidth / mean_pktsize;  // [(bits/sec) / (bits/pkt)]


}

bool REDQueue::Enque(Packet* p)
/*
描述：使用RED时，如何入队
参数：Arg1 指向数据包的指针
返回值：成功入队返回True，否则返回False
*/
{
  //Time_t now = Simulator::instance->Now();
  DEBUG(0,(cout << "Red::Enque, this " << this
           << " length " << Length() << endl));
  DEBUG(0,(cout << "REDQueue::Enqueue" << endl));
  DEBUG(1,(cout << "w_q: "<< w_q <<"min_th: "<<min_th<<"max_th: "<< max_th
       <<"size: "<<size<<endl));
  DEBUG(1,(cout<<"max_p: "<<max_p<<"count: "<<count<<"q_avg: "<<q_avg<<"limit: "<< GetLimit()<<endl));

  UpdateAverage();
  // 更新平均队列长度
  if (size == 0 ) { // 数据包到达一个空闲的队列时
    DEBUG(1,(cout << "packet finds queue idle" << endl));
    int num_pkts_idle;    // 队列空闲时应到达的数据包数
    Time_t now = Simulator::instance->Now();
    num_pkts_idle = int((now - idle_since) * pktspersec);
    DEBUG(1,(cout << "num_pkts_idle" << num_pkts_idle << endl));
    q_avg = pow((1 - w_q),num_pkts_idle) * q_avg; 
    pkts.push_back(p);    
    size += p->Size();  
    DEBUG(1,(cout << "size: "<<size<< "q_avg: " << q_avg
	     << "now: "<< now << endl));
    return true;
  } 

  pkts.push_back(p);    // 将数据包加入队尾
  size += p->Size();    // 更新队列长度
  q_avg = (1 - w_q) * q_avg + w_q * size;
  // 事实上这里丢弃的总是队尾的数据包!
  // 如果队列已经满了，则丢弃该数据包，返回False
  if (Length() > Limit()) {
    Packet* pkt_to_drop = DropPacket();
    count = 0;          // 丢包后count要重置0
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "queue is full, pkt dropped" << endl));
        dropCount++;
	return false;
      }
    else return true;
  }

  // 以概率p_a标记（丢弃）数据包
  if ((min_th <= q_avg) && (q_avg <= max_th)){
    count++;
    DEBUG(1,(cout << "count: " << count));
    Packet* pkt_to_drop = MarkPacketwProb();
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "pkt marked, min < avg < max" << endl));
        dropCount++;
	return false;
      }
    else
      {
	DEBUG(1,(cout << "pkt not marked, min < avg < max" << endl));
	return true;
      }
  } else if (q_avg >= max_th){
    count++;
    Packet* pkt_to_drop = MarkPacket();
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "pkt marked, max < avg" << endl));
        dropCount++;
	return false;
      }
    else 
      {
	DEBUG(1,(cout << "pkt not marked, max < avg" << endl));
	return true;
      }
  } else {
    DEBUG(1,(cout << "pkt not marked, avg < min" << endl));
    DEBUG(1,(cout << "'count' before initialisation:"<< count << endl));
    count = -1;     // 平均队列长度小于min_th时要重置count
    return true;
  }

}


Packet* REDQueue::MarkPacketwProb()
/*
描述：实现以p_a 的概率丢弃到达的数据包
参数：
返回值：丢弃了数据包则返回指向该数据包的指针，否则返回null
备注：可以通过修改这个函数实现不同的队列管理方法
*/
{
  DEBUG(0,(cout << "REDQueue::MarkPacketwProb" << endl));
  DCount_t p_b;        

  DCount_t p_a;        
  p_b = (max_p * (q_avg - min_th) / (max_th - min_th));
  p_a = p_b / (1 - count * p_b); 
  DEBUG(1,(cout << "mark_prob: " << p_a <<endl));
  Uniform U_rng = Uniform();          
  if (U_rng.Value() <= p_a){
    count = 0;          
    Packet* pkt = pkts.back();  
    pkts.pop_back();          
    size -= pkt->Size();   
    return pkt;
  }
  return NULL;
}


Packet* REDQueue::MarkPacket()
/*
描述：由于平均队列长度大于max_th，丢弃到达的数据包
参数：
返回值：指向被丢弃数据包的指针
*/
{
  DEBUG(0,(cout << "REDQueue::MarkPacket" << endl));
  count = 0;          
  Packet* pkt = pkts.back(); 
  pkts.pop_back();          
  size -= pkt->Size();      
  return pkt;
}

// Drops the arriving packet because queue is full
// Different queings may be implemented by modifying this function
Packet* REDQueue::DropPacket()
/*
描述：由于队列长度已满，丢弃到达的数据包
参数：
返回值：指向被丢弃数据包的指针
*/
{
  DEBUG(0,(cout << "REDQueue::DropPacket" << endl));
  count = 0;         
  Packet* pkt = pkts.back(); 
  pkts.pop_back();          
  size -= pkt->Size();      
  return pkt;
}

Queue*  REDQueue::Copy() const
/*
描述：复制队列
参数：
返回值：指向队列副本的指针
*/
{
  return new REDQueue(*this);
}

Packet* REDQueue::Deque()
/*
描述： 数据包出队
参数：
返回值：指向出队数据包的指针
*/
{
  DEBUG(0,(cout << "Red::Deque, this " << this
           << " length " << Length() << endl));
  DEBUG(0,(cout << "REDQueue::Deque" << endl));
  Time_t now = Simulator::instance->Now();
  UpdateAverage();
  if (Length() == 0){
    idle_since = now;                // 如果队列为空，重新设定idle_since
    return NULL;                     // 没有报入队
  }
  Packet* p = pkts.front();              // 获取队首数据包
  pkts.pop_front();                      // 删除该队首数据包
  size -= p->Size();                     // 调整队列长度值
  return p;                              // 返回指向被删数据包的指针
}

void REDQueue::SetInterface(Interface* i)
/*
描述：将队列绑定到相应的接口上
参数：Arg1 相应的接口
*/
{ 
  iface = i;
}

Rate_t REDQueue::Bandwidth()
/*
描述：获取链路带宽
参数：
返回值：链路带宽已知则返回已知值，否则返回0.0
*/
{
  if (iface)//接口已知
    { 
      Link* l = iface->GetLink();
      if (l)//接口已和相应的链路关联
        { 
          return l->Bandwidth();
        }
    }
  return 0.0; // 未知则返回0
}
