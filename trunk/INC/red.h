


#ifndef __red_h__
#define __red_h__

#include <list>
#include "droptail.h"

//REDQueue类的定义，该类派生于DropTail类，用来实现随机早期丢弃（RED）的队列管理策略
class REDQueue : public DropTail {

  public:
//REDQueue类的构造和析构

    REDQueue();//默认的构造函数
    
    REDQueue( DCount_t in_w_q, Count_t in_min_th, Count_t in_max_th, 
	     Count_t in_limit, DCount_t in_max_p, Count_t in_mean_pktsize);//可以设定相关参数的构造函数

    virtual ~REDQueue() { }// 析构函数

//数据包的调度和管理
    bool Enque(Packet*);  // 将一个数据包入队，成功则返回True，若被丢弃则返回False
   
    Packet* Deque();      // 将一个数据包出队，并返回指向该数据包的指针
 
    Packet* MarkPacketwProb();// 按概率标记一个数据包是否要被丢弃（min_th < q_avg < max_th时使用）

    Packet* MarkPacket(); // 标记一个数据包，并返回指向该数据包的指针（q_avg > max_th时调用）

    Packet* DropPacket(); // 从队列中丢弃数据包（需要无条件丢弃时，如队列溢出）

//
    Count_t Length() { return size;} // 返回瞬时队列长度（字节）

    Queue*  Copy() const;    // 复制一个队列，返回指向副本的指针        

    void    SetInterface(Interface*);// 将队列绑定到指定接口

    Rate_t  Bandwidth();        // 获取相关链路的带宽（bits/sec）

  private:
    int count;            // 自上次标记数据包后，未标记数据包的个数
    Count_t mean_pktsize; // 数据包的平均大小（位）
    Count_t size;         // 当前队列长度（字节）
    DCount_t q_avg;       // 平均队列长度   
    Count_t min_th;       // 最小门限
    Count_t max_th;       // 最大门限
    DCount_t w_q;         // 瞬时队列的权重（用于计算平均队列长度）
    DCount_t max_p;       // 标记丢包概率的最大值
    Time_t idle_since;    // 队列空闲开始的时刻
    PktList_t pkts;       // 队列缓冲区中的数据包链表
    Interface* iface;     // 相关的接口
    Count_t bandwidth;    // 链路带宽（bit/s）
    DCount_t pktspersec;  // 链路处理数据包的速度（packet/s）

};
#endif



    
