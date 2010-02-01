
// Georgia Tech Network Simulator - DropTail Queue class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __droptail_h__
#define __droptail_h__

#include "queue.h"

class Interface;

#define DEFAULT_DROPTAIL_LIMIT 50000

// DropTail��Ķ��壬ʵ��β�������л�FIFO����
class DropTail : public Queue {
public:
  DropTail()
      : Queue(true), limit(Queue::DefaultLength()),
        pktLimit(Queue::DefaultLimitPkts()),
        size(0), pktCount(0), lastTime(0) { }
  // Ĭ�ϵĹ��캯��


  DropTail(Count_t l)
    : Queue(true), limit(l), pktLimit(0),
      size(0), pktCount(0), lastTime(0) { }
  // ����һ���ض���������С�Ķ���


  DropTail(const DropTail& c)
    : Queue(c), limit(c.limit), pktLimit(c.pktLimit),
      size(c.size), pktCount(c.pktCount), lastTime(c.lastTime) { }
  // ����һ����ĳһ������ͬ�Ķ���


  virtual ~DropTail() { }// ��������


  bool Enque(Packet*); // ��һ�����ݰ���ӣ���ӳɹ�����True�����������򷵻�FALSE

  Packet* Deque();     // ��һ�����ݰ����ӣ������ظ����ݰ���ָ��
 
  Packet* PeekDeque(); // ����ָ��������ݰ���ָ�룬���ǲ���ɾ��

  virtual Count_t DequeAllDstMac(MACAddr); // ɾ����������MAC��ַΪָ��MAC��ַ�����ݰ�������ɾ�������ݰ�����

  virtual Count_t DequeAllDstIP(IPAddr_t); // ɾ����������Ŀ��IPΪָ��Ŀ��IP��ַ�����ݰ�������ɾ�������ݰ�����

  virtual Packet* DequeOneDstMac(MACAddr);// ɾ��������һ��MAC��ַΪָ��MAC��ַ�����ݰ�������ָ��ɾ�������ݰ���ָ��

  virtual Packet* DequeOneDstIP(IPAddr_t);// ɾ��������һ��Ŀ��IPΪָ��Ŀ��IP��ַ�����ݰ�������ָ��ɾ�������ݰ���ָ��

  Count_t Length();    // ���ص�ǰ���еĳ��ȣ���λ���ֽڣ�

  Count_t LengthPkts();// ���ص�ǰ���еĳ��ȣ���λ�����ݰ�����Ŀ��

  void    SetInterface(Interface*);// ����ؽӿ�

  Queue*  Copy() const;   // ����һ�����У�����ָ�򸱱����е�ָ��

  virtual void SetLimit(Count_t l);// �����µĶ��г�������ֵ����λ���ֽڣ�

  Count_t GetLimit() const { return limit;} // ��ȡ���г�������ֵ����λ���ֽڣ�

  virtual void SetLimitPkts(Count_t l) { pktLimit = l;} // �����µĶ��г�������ֵ����λ�����ݰ�������

  virtual Count_t GetLimitPkts() const { return pktLimit;} // ��ȡ���г�������ֵ����λ�����ݰ�������

  virtual Time_t  QueuingDelay();  // �����Ŷ���ʱ  

  bool    Check(Size_t, Packet* = nil);// �����л������Ƿ��пռ��ܷ��µ������ݰ�

  Count_t Limit() { return limit;} // ��ȡ���л������������޵����ֵ���ֽڣ�

  Packet* GetPacket(Count_t k);// ��ȡָ������е�k�����ݰ���ָ��

  void     GetPacketColors(ColorVec_t&);//����������������������������������������������������������������

private:
  void    UpdateSize();       // ���ϴθ���ʱ�̵�����ǰ���г���
public:
  Count_t    limit;           // ���г������ޣ���λ���ֽڣ�
  Count_t    pktLimit;        // ���г������ޣ���λ�����ݰ�����
  Count_t    size;            // ��ǰ���г��ȣ���λ���ֽڣ�
  Count_t    pktCount;        // ��ǰ���������ݰ�����
  Time_t     lastTime;        // ���г��ȱ������ʱ��
public:
  PktList_t  pkts;            // ���л������е����ݰ��б�
};

#endif
