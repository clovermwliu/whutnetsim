

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
��������һ�����ݰ����
������Arg1 ָ�򵽴����ݰ���ָ��
����ֵ����ӳɹ�����True�����������򷵻�FALSE
*/
{
  if (!Detailed()) UpdateSize();
  UpdateAverage();
  totEnq++;
  if (verbose && 0)//verbose����
    {
      cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl;
    }
  bool full = false;
  if (pktLimit)
    { // ���г��ȵ�λΪ���ݰ�����
      full = (pktCount == pktLimit);
    }
  else
    { // ���г��ȵ�λΪ�ֽ�
      full = (size + p->Size()) > Limit();
    }
  
  DEBUG(0,(cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl));
  pktCount++; // ���������ݰ������ļ���
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
      return false; // ������������False
    }
  DBG((Stats::pktsEnqued++));
  size += p->Size(); // ���¶��г���ֵ
  if (Detailed())
    {
      pkts.push_back(p);// �����detailedģʽ����������ݰ������л�����
    }
  else 
    { // Non-Detailedģʽ��
#ifndef WIN32
      if (!interface) return false;      //������������������������������������ Can't handle w/o interface����������������������
#else
		if (!interface_) return false;      // Can't handle w/o interface
#endif
      if (!ndDeq) ndDeq = new NDDeq_t(); // ����non-detailedģʽ�����ݰ��Ŀռ䣨deque������
      Time_t startTx = Simulator::Now(); // �����ݰ���ʼ�����ʱ��
      if (!ndDeq->empty())
        { // ���˫�˶����зǿգ�������ݰ�����ʼ����ʱ��Ϊ���������һ����������ϵ�ʱ��
          NDInfo& nd = ndDeq->back();   
          startTx = nd.stopTx;           
        }
#ifndef WIN32
      Link* link = interface->GetLink();
#else
	  Link* link = interface_->GetLink();
#endif
      if (!link) return false;           // ��·������ڣ����򷵻�False
      Time_t txTime =  ((Rate_t)p->Size() * 8) / link->Bandwidth();
      NDInfo  newND(startTx, startTx + txTime, p->Size());
      ndDeq->push_back(newND);           // �������ݰ�����Ϣ��ӵ�����
    }
  DEBUG(1,(cout << " newsize " << size << endl));
  return true;                           // ����δ�������ݰ��ɹ����
}

Packet* DropTail::Deque()
/*
��������һ�����ݰ�����
������Arg1 ָ�򵽴����ݰ���ָ��
����ֵ���ɹ������򷵻�ָ������ݰ���ָ�룬��Ϊnon-detailed���л����Ϊ�գ��򷵻�0
��ע��non-detailed���������Deque
*/
{ 
  if (!Detailed()) return nil;            //��detailed���У�����0
  UpdateAverage();
  if (Length() == 0) return nil;         // ����Ϊ�գ�����0
  DBG((Stats::pktsDequed++));
  Packet* p = pkts.front();              // ��ȡ�������ݰ���ָ��
  pkts.pop_front();                      // �������ݰ�����
  size -= p->Size();                     // ���¶��г���
  pktCount--;                            // ���¶��������ݰ���
  return p;                              // ����ָ��������ݰ���ָ��
}

Packet* DropTail::PeekDeque()
//����ָ��������ݰ���ָ�룬���ǲ���ɾ��
/*
������������������ݰ�����
������
����ֵ�����в����򷵻�ָ��������ݰ���ָ�룬�����򷵻�0
*/
{
  if (Length() == 0) return nil;        
  return pkts.front(); 
}

Count_t DropTail::DequeAllDstMac(MACAddr m)
/*
������ɾ����������MAC��ַΪָ��MAC��ַ�����ݰ�
������Arg1 ָ����MAC��ֵַ
����ֵ������ɾ�������ݰ�����
*/
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
  //�Ӷ��׿�ʼ���ң�������MAC��ַΪm�����ݰ�ɾ��
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
������ɾ����������MAC��ַΪָ��IP��ַ�����ݰ�
������Arg1 ָ����IP��ֵַ
����ֵ������ɾ�������ݰ�����
*/
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
  //�Ӷ��׿�ʼ���ң�������IP��ַΪip�����ݰ�ɾ��
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
������ɾ��������һ��MAC��ַΪָ��MAC��ַ�����ݰ�
������Arg1 ָ����MAC��ֵַ
����ֵ������ָ��ɾ�������ݰ���ָ�룬δ�ҵ��򷵻�0
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
������ɾ��������һ��IP��ַΪָ��IP��ַ�����ݰ�
������Arg1 ָ����IP��ֵַ
����ֵ������ָ��ɾ�������ݰ���ָ�룬δ�ҵ��򷵻�0
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
// ���ص�ǰ���еĳ��ȣ���λ���ֽڣ�
{
  if (!Detailed())UpdateSize();
  return (Count_t)size;
}

Count_t DropTail::LengthPkts()
// ���ص�ǰ���еĳ��ȣ���λ�����ݰ�������
{
  if (!Detailed())UpdateSize();
  return pktCount;
}

void   DropTail::SetInterface(Interface* i)
/*
�����������а󶨵���Ӧ�Ľӿ���
������Arg1 ��Ӧ�Ľӿ�
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
���������ƶ���
������
����ֵ��ָ����и�����ָ��
*/
{ 
  return new DropTail(*this);
}

void DropTail::SetLimit(Count_t l)
/*
�������趨���г���ֵ������
������Arg1 ���г��ȵ�����ֵ���ֽڣ�
����ֵ����
*/
{
  limit = l;
}

Time_t   DropTail::QueuingDelay()
/*
�����������Ŷ���ʱ
��������
����ֵ���Ŷ���ʱ�������в����ڻ�Ϊ��ʱ����0
*/
{
  if (Detailed()) return Queue::QueuingDelay();
  UpdateSize();
  // non-detailedģʽʱ��ʹ��ndDeq�б�
  Time_t now = Simulator::Now();
  // �ҵ����������һ�����ݰ��������ʱ��
  if (!ndDeq) return 0;  
  if (ndDeq->empty()) return 0; 
  NDInfo& nd = ndDeq->back();   
  return (nd.stopTx - now);     
}


bool DropTail::Check(Size_t s, Packet*)
/*
�����������л������Ƿ��пռ��ܷ��µ������ݰ�
������Arg1 ���ݰ��Ĵ�С���ֽڣ�
	    Arg2 ָ������ݰ���ָ��
����ֵ���������㹻�Ŀռ�Ÿ����ݰ��򷵻�True�����򷵻�False
*/
{ 
  if(!Detailed())UpdateSize();
  if (Length() == 0) return true; // �������Ϊ�գ������ǿ��Է��µ�
  if (pktLimit)
    { // ʹ�����ݰ�������Ϊ���г�������ʱ
      return pktCount < pktLimit;
    }
  // ʹ���ֽ�����Ϊ���г�������ʱ
  return ((Length() + s) < Limit());
}

Packet* DropTail::GetPacket(Count_t k)
/*
��������ȡָ������е�k�����ݰ���ָ��
������Arg1 ����k
����ֵ��ָ������е�k�����ݰ���ָ�룬������������ݰ�����k�����򷵻�0
*/
{
  if (pkts.empty()) return nil;

  PktList_t::iterator i = pkts.begin();

  for (Count_t j = 0; j < k; ++j)
    {
      ++i;
      if (i == pkts.end()) return nil; // �����ڵ�k�����ݰ�
    }
  return *i;
}

void    DropTail::GetPacketColors(ColorVec_t& cv)
{ // Build a list of packet colors from head to tail����������������������������������������������������������������������������
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

// ˽�з���
void DropTail::UpdateSize()
/*
���������ϴθ��º������������ݰ������ͣ���Ҫ���¶��г���
��������
����ֵ����
*/
{ 
  if (!ndDeq) return; // ���������non-detailed�Ķ��У�ֱ�ӷ���
  Time_t now = Simulator::Now();
  while(!ndDeq->empty())
    {
      NDInfo& nd = ndDeq->front();
      if (nd.startTx > now) return;      // ����ʱ��δ��������������
      if (nd.stopTx  > now)
        { //��ʱ�����ݰ����ڴ��䣬�����г��ȸ��£����˰������ӣ�������
		
          if (!nd.updated)
            {
              size -= nd.size;
              pktCount--;
              nd.updated = true;
            }
          return;
        }
      //nd��������ݰ��������ˣ�������ӣ����������һ���ݰ���δ���
      if (!nd.updated)  
        {
          size -= nd.size;
          pktCount--;
        }
      ndDeq->pop_front(); 
    }
}



