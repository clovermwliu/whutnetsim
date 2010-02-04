


// �Ӿ����S.Floyd's simulator����ȡ���й�RED�Ĳ���ֵ
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
//Ĭ�Ϲ��캯��
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
���������԰��Լ�Ҫ��Ϊ��ز�����ֵ�Ĺ��캯��
������Arg1 ���е�Ȩ��
	  Arg2 ��С����ֵ
	  Arg3 �������ֵ
	  Arg4 ���г��ȵ�����ֵ
	  Arg5 ��Ƕ������ʵ����ֵ
	  Arg6 ���ݰ���ƽ����С   
*/
{
  DEBUG(0,(cout << "REDQueue::RedQueue" << endl));
  //���û��趨����ֵ
  w_q = in_w_q;
  min_th = in_min_th;
  max_th = in_max_th;
  SetLimit(in_limit);
  max_p = in_max_p;
  mean_pktsize = in_mean_pktsize;
  size = 0;
  bandwidth = 800000;                // �����ݴ���·��ȡ
  count = -1;
  q_avg = 0;
  if (Simulator::instance != NULL)
    {
    idle_since = Simulator::instance->Now();
    }
  else
    {
    idle_since = 0.0;                  // ������δʵ����ʱ
    }
  // pktspersec���ڶ��п���ʱ����ƽ�����г���
  pktspersec = bandwidth / mean_pktsize;  // [(bits/sec) / (bits/pkt)]


}

bool REDQueue::Enque(Packet* p)
/*
������ʹ��REDʱ��������
������Arg1 ָ�����ݰ���ָ��
����ֵ���ɹ���ӷ���True�����򷵻�False
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
  // ����ƽ�����г���
  if (size == 0 ) { // ���ݰ�����һ�����еĶ���ʱ
    DEBUG(1,(cout << "packet finds queue idle" << endl));
    int num_pkts_idle;    // ���п���ʱӦ��������ݰ���
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

  pkts.push_back(p);    // �����ݰ������β
  size += p->Size();    // ���¶��г���
  q_avg = (1 - w_q) * q_avg + w_q * size;
  // ��ʵ�����ﶪ�������Ƕ�β�����ݰ�!
  // ��������Ѿ����ˣ����������ݰ�������False
  if (Length() > Limit()) {
    Packet* pkt_to_drop = DropPacket();
    count = 0;          // ������countҪ����0
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "queue is full, pkt dropped" << endl));
        dropCount++;
	return false;
      }
    else return true;
  }

  // �Ը���p_a��ǣ����������ݰ�
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
    count = -1;     // ƽ�����г���С��min_thʱҪ����count
    return true;
  }

}


Packet* REDQueue::MarkPacketwProb()
/*
������ʵ����p_a �ĸ��ʶ�����������ݰ�
������
����ֵ�����������ݰ��򷵻�ָ������ݰ���ָ�룬���򷵻�null
��ע������ͨ���޸��������ʵ�ֲ�ͬ�Ķ��й�����
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
����������ƽ�����г��ȴ���max_th��������������ݰ�
������
����ֵ��ָ�򱻶������ݰ���ָ��
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
���������ڶ��г���������������������ݰ�
������
����ֵ��ָ�򱻶������ݰ���ָ��
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
���������ƶ���
������
����ֵ��ָ����и�����ָ��
*/
{
  return new REDQueue(*this);
}

Packet* REDQueue::Deque()
/*
������ ���ݰ�����
������
����ֵ��ָ��������ݰ���ָ��
*/
{
  DEBUG(0,(cout << "Red::Deque, this " << this
           << " length " << Length() << endl));
  DEBUG(0,(cout << "REDQueue::Deque" << endl));
  Time_t now = Simulator::instance->Now();
  UpdateAverage();
  if (Length() == 0){
    idle_since = now;                // �������Ϊ�գ������趨idle_since
    return NULL;                     // û�б����
  }
  Packet* p = pkts.front();              // ��ȡ�������ݰ�
  pkts.pop_front();                      // ɾ���ö������ݰ�
  size -= p->Size();                     // �������г���ֵ
  return p;                              // ����ָ��ɾ���ݰ���ָ��
}

void REDQueue::SetInterface(Interface* i)
/*
�����������а󶨵���Ӧ�Ľӿ���
������Arg1 ��Ӧ�Ľӿ�
*/
{ 
  iface = i;
}

Rate_t REDQueue::Bandwidth()
/*
��������ȡ��·����
������
����ֵ����·������֪�򷵻���ֵ֪�����򷵻�0.0
*/
{
  if (iface)//�ӿ���֪
    { 
      Link* l = iface->GetLink();
      if (l)//�ӿ��Ѻ���Ӧ����·����
        { 
          return l->Bandwidth();
        }
    }
  return 0.0; // δ֪�򷵻�0
}
