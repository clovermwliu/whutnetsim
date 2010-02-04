

// Virtual Base class for all queue types.

#ifndef __queue_h__
#define __queue_h__

#include <iostream>
#include <deque>
#include <list>
#include <vector>

#include "packet.h"
#include "tcp.h"   // TimeSeq_t�����ڴ�ͷ�ļ���

class Interface;

typedef unsigned int Qrgb; // This is so we don't need any qt includes here

//ForceLoss�����ڴ���ǿ�ƶ����������Ϣ
class ForcedLoss { // Information for forcing packet losses
public:
  ForcedLoss(Time_t t, Count_t c)
    : time(t), count(c), expiration(INFINITE_TIME) {}
  ForcedLoss(Time_t t, Count_t c, Time_t e)
    : time(t), count(c), expiration(e) {}
public:
  Time_t  time;  // ǿ�ƶ�����ʼʱ��
  Count_t count; // ��timeʱ�̿�ʼ����������
  Time_t  expiration; // ǿ�ƶ�������ʱ��
};

typedef std::list<Packet*>    PktList_t;  // �ȴ��ж������ݰ�����
typedef std::list<ForcedLoss> LossList_t; // ��ǿ�ƶ��������ݰ��Ķ��У�����ΪForceLoss��
typedef std::vector<Qrgb>     ColorVec_t; // ÿ�����ݰ�����ɫ

//NDInfo��Ķ��壬�����¼�ṩnon-detailed���е���Ϣ
class NDInfo { 
public:
  NDInfo() : startTx(0), stopTx(0), size(0), updated(false) { }
  NDInfo(Time_t s, Time_t e, Size_t sz)
    : startTx(s), stopTx(e), size(sz), updated(false) { }
public:
  Time_t startTx; // ���俪ʼʱ��  
  Time_t stopTx;  // �������ʱ��  
  Size_t size;    // ���ݰ��Ĵ�С�����ֽ�Ϊ��λ 
  bool   updated; // ������д�С�Ѿ�������updatedΪTrue
};

class gui_ctrl;
typedef std::deque<NDInfo> NDDeq_t; // Non-detailedģʽ�Ķ��л�����������
typedef std::vector<MyCanvasLine*> ItemVec_t;


//Queue���Ƕ��нӿڵĻ��࣬�����ݰ�Ҫ������·ǰ������ӣ����Լ̳и���ʵ�ֲ�ͬ��
//���й������
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

  // ��������
  virtual ~Queue() { }


  virtual bool Enque(Packet*) = 0;  //��һ�����ݰ����  

  virtual Packet* Deque() = 0;      //��һ�����ݰ����ӣ������ظ����ݰ���ָ�� 
  
  virtual Packet* PeekDeque() = 0; //��������ָ��������ݰ���ָ�룬���ǲ���ɾ��
 
  virtual void DummyEnque(Packet*);//�����ٳ��ӣ�����·���л���û�б�Ҫ���ж��в���ʱ������
 
  virtual Count_t DequeAllDstMac(MACAddr);//ɾ����������MAC��ַΪָ��MAC��ַ�����ݰ�������ɾ�������ݰ�����
   
  virtual Count_t DequeAllDstIP(IPAddr_t);//ɾ����������Ŀ��IPΪָ��Ŀ��IP��ַ�����ݰ�������ɾ�������ݰ�����

  virtual Packet* DequeOneDstMac(MACAddr);//ɾ������һ��MAC��ַΪָ��MAC��ַ�����ݰ�������ָ��ɾ�������ݰ���ָ��
  
  virtual Packet* DequeOneDstIP(IPAddr_t); //ɾ������һ��Ŀ��IPΪָ��Ŀ��IP��ַ�����ݰ�������ָ��ɾ�������ݰ���ָ��

  virtual Count_t Length() = 0;       //���ص�ǰ���еĳ��ȣ���λ���ֽڣ�
  
  virtual Count_t LengthPkts() = 0;   //���ص�ǰ���еĳ��ȣ���λ�����ݰ�����Ŀ��

  virtual Queue* Copy() const = 0;   //����һ�����У�����ָ�򸱱����е�ָ�� 

  virtual void SetLimit(Count_t) = 0; //�����µĶ��г�������ֵ����λ���ֽڣ�

  virtual Count_t GetLimit() const = 0;    //��ȡ���г�������ֵ����λ���ֽڣ� 

  virtual void SetLimitPkts(Count_t) = 0;  //�����µĶ��г�������ֵ����λ�����ݰ������� 
  
  virtual Count_t GetLimitPkts() const = 0; //��ȡ���г�������ֵ����λ�����ݰ�������

  DCount_t Average();             //��ȡƽ���Ķ��г���   

  void     ResetAverage();       //����ƽ�����г��ȣ�ͨ�������µ�ʱ�����������µ�ƽ�����г��� 

  void     UpdateAverage();     //������з����ı䣬����ƽ�����г��ȵ�ֵ     

  virtual bool Check(Size_t, Packet* = nil) = 0;//���Զ��л������Ƿ��пռ��ܷ��¸����ݰ�

  virtual bool Detailed();      //�ж�detailedģʽ�Ƿ�ʹ�ã����򷵻�True�����򷵻�False

  virtual void Detailed(bool);  //�趨�Ƿ�ʹ��detailedģʽ

  virtual Time_t   QueuingDelay();   //�����Ŷ���ʱ 

  virtual DCount_t TotalWorkload(); //�������еĹ���������������λ��byte-secs��  

  virtual void     SetInterface(Interface*);  //����ؽӿ� 

  virtual Count_t  DropCount();     //���ض��������ݰ�������  

  virtual Count_t  EnqueueCount();   //������ӵ����ݰ����� 

  virtual void     CountEnq(Packet* = nil);//��һ����Ӳ������м��� 

  virtual void     ResetStats();      //�������ô����ڴ˶��е�����Ϊ0

  void             AddForcedLoss(Time_t, Count_t = 1, Time_t = INFINITE_TIME);//���һ��ǿ�ƶ�����Ϊ�йص���Ϣ��
    //����ģ������ĳ���ض���ʱ�̶������۲�Э��Ե�ǰ������Ϊ�ķ�Ӧ
   
  bool             CheckForcedLoss(bool = true);//�ж�һ��ǿ�ƶ����Ƿ�Ӧ�ñ�ִ��

  virtual bool CheckSpoofedSource(Packet*); //һЩ���й�����ƻ�ִ�а����ˣ��ж��Ƿ�������ĳ��ԴIP��ƭ

  void    EnableTimeSizeLog(bool = true);//�Ƿ�����־��¼���г�����ʱ��ı仯
 
  void    LogSizePackets(bool = true);//��־�ж��г��ȵĵ�λ��Ϊ���ݰ����������ֽ�
 
  void    PrintTimeSizeLog(std::ostream&,
                           Count_t div = 0,
                           char sep = ' ');//����¼���г�����ʱ��仯����־�����ļ���
 
  void    Animate(bool b = true) { animate = b;}//�趨��������Ƿ���Ҫʹ�ö���

  bool    Animated() { return animate;}//��ѯ�����Ƿ������������ʾ��������
 
  virtual void    DisplayQueue(gui_ctrl*, Node*, Node*);//����չʾ������е�ʹ����

  virtual Packet* GetPacket(Count_t k) = 0;//��ȡָ���k������ָ��
 
  virtual void     GetPacketColors(ColorVec_t&) = 0;//����Ϊ�ȴ��������ݰ���ɫ

  void             Verbose(bool v = true) { verbose = v;}//�趨�Ƿ�ʹ��Verbose����

protected:
  void UpdateTimeSizeLog();//���¼�¼���г�����ʱ��仯����־

public:
  // ����Ĭ�϶���
  static void    Default(const Queue& d);//setһ��Ĭ�ϵĶ���

  static Queue&  Default();//���ض�Ĭ�϶��ж��������

  static void    DefaultLength(Count_t l);//�趨Ĭ�϶��л������Ĵ�С����λ���ֽڣ�

  static Count_t DefaultLength() { return defaultLength;}//����Ĭ�϶��л������Ĵ�С����λ���ֽڣ�
 
  static void    DefaultLimitPkts(Count_t l);//�趨Ĭ�϶��л������Ĵ�С����λ�����ݰ�������

  static Count_t DefaultLimitPkts() { return defaultLimitPkts;}//����Ĭ�϶��л������Ĵ�С����λ�����ݰ�������


  

private:
  DCount_t    totalByteSeconds;     // ��������
  Time_t      startTime;            // ������ʼʱ��
  Time_t      lastUpdate;           // ���һ�θ���ʱ��
  bool        detailed;             // ����detailed�Ŷ���Ϊ��True 
  LossList_t* forcedLosses;         // ǿ�ƶ����б�
  TimeSeq_t*  timeSizeLog;          // ���г�����ʱ��仯����־
  bool        timeSizePackets;      // ���г�����־�����ݰ�����Ϊ��λ����Ϊ��True 
protected:
  bool        verbose;              // ʹ��verbose���ԣ���Ϊ��
  Count_t     dropCount;            // �������ܵ����ݰ���
  Count_t     totEnq;               // ��ӵ������ݰ���
  NDDeq_t*    ndDeq;                // ָ��non-detailedģʽ�¶�����Ϣ��ָ��
public:
#ifndef WIN32
	Interface*  interface;            // ��ؽӿ�
#else
	Interface*  interface_;            // ��ؽӿ�
#endif
private:
  bool        animate;              // ��Ҫʹ�ö��ж�������Ϊ��True
  ItemVec_t*  qLines;               // MyCanvasLines used to animate queue������������������������
  // ��̬��Ա
  static Queue*  defaultQueue;      // Ĭ�϶�������
  static Count_t defaultLength;     // Ĭ�ϵĶ��г���
  static Count_t defaultLimitPkts;  // Ĭ�ϵĶ��г������ޣ����ݰ�����
public:
  static Count_t defaultAnimWidth;  // ������Ĭ�ϵĶ�����ʵ��� 
  static Count_t globalQueueDrop;   // ����л���������ܵĶ�����
};

#endif



