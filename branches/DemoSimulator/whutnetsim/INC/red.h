


#ifndef __red_h__
#define __red_h__

#include <list>
#include "droptail.h"

//REDQueue��Ķ��壬����������DropTail�࣬����ʵ��������ڶ�����RED���Ķ��й������
class REDQueue : public DropTail {

  public:
//REDQueue��Ĺ��������

    REDQueue();//Ĭ�ϵĹ��캯��
    
    REDQueue( DCount_t in_w_q, Count_t in_min_th, Count_t in_max_th, 
	     Count_t in_limit, DCount_t in_max_p, Count_t in_mean_pktsize);//�����趨��ز����Ĺ��캯��

    virtual ~REDQueue() { }// ��������

//���ݰ��ĵ��Ⱥ͹���
    bool Enque(Packet*);  // ��һ�����ݰ���ӣ��ɹ��򷵻�True�����������򷵻�False
   
    Packet* Deque();      // ��һ�����ݰ����ӣ�������ָ������ݰ���ָ��
 
    Packet* MarkPacketwProb();// �����ʱ��һ�����ݰ��Ƿ�Ҫ��������min_th < q_avg < max_thʱʹ�ã�

    Packet* MarkPacket(); // ���һ�����ݰ���������ָ������ݰ���ָ�루q_avg > max_thʱ���ã�

    Packet* DropPacket(); // �Ӷ����ж������ݰ�����Ҫ����������ʱ������������

//
    Count_t Length() { return size;} // ����˲ʱ���г��ȣ��ֽڣ�

    Queue*  Copy() const;    // ����һ�����У�����ָ�򸱱���ָ��        

    void    SetInterface(Interface*);// �����а󶨵�ָ���ӿ�

    Rate_t  Bandwidth();        // ��ȡ�����·�Ĵ���bits/sec��

  private:
    int count;            // ���ϴα�����ݰ���δ������ݰ��ĸ���
    Count_t mean_pktsize; // ���ݰ���ƽ����С��λ��
    Count_t size;         // ��ǰ���г��ȣ��ֽڣ�
    DCount_t q_avg;       // ƽ�����г���   
    Count_t min_th;       // ��С����
    Count_t max_th;       // �������
    DCount_t w_q;         // ˲ʱ���е�Ȩ�أ����ڼ���ƽ�����г��ȣ�
    DCount_t max_p;       // ��Ƕ������ʵ����ֵ
    Time_t idle_since;    // ���п��п�ʼ��ʱ��
    PktList_t pkts;       // ���л������е����ݰ�����
    Interface* iface;     // ��صĽӿ�
    Count_t bandwidth;    // ��·����bit/s��
    DCount_t pktspersec;  // ��·�������ݰ����ٶȣ�packet/s��

};
#endif



    
