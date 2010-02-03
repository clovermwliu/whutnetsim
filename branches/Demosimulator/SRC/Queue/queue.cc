
// Virtual Base class for all queue types.
// Provides some common functionality

#include "queue.h"
#include "droptail.h"
#include "simulator.h"
#include "link.h"
#include "interface.h"

#ifdef HAVE_QT
#include "qtwindow.h"
#include "GUI_Defs.h"
#endif

using namespace std;

// ��̬��Ա����
Queue*  Queue::defaultQueue  = nil;
Count_t Queue::defaultLength = DEFAULT_DROPTAIL_LIMIT;
Count_t Queue::defaultLimitPkts = 0;  
Count_t Queue::defaultAnimWidth = 10; 
Count_t Queue::globalQueueDrop = 0;   

void Queue::DummyEnque(Packet*)
// �������ఴ����и���
{ 
}

Count_t Queue::DequeAllDstMac(MACAddr)
// �������ఴ����и���
{ 
  return 0;
}

Count_t Queue::DequeAllDstIP(IPAddr_t)
// �������ఴ����и���
{ 
  return 0;
}

Packet* Queue::DequeOneDstMac(MACAddr)
// �������ఴ����и���
{ 
  return nil;
}

Packet* Queue::DequeOneDstIP(IPAddr_t)
// �������ఴ����и���
{ 
  return nil;
}

DCount_t Queue::Average()
// ����ƽ�����г���
{ 
  Time_t now = Simulator::Now();
  if (now == startTime) return 0.0;    // �������Ϊ0
  return totalByteSeconds / (now - startTime);
}

void     Queue::ResetAverage()
// ʹ���µ�ʱ����������ƽ������
{ 
  startTime = Simulator::Now();
  lastUpdate = startTime;
  totalByteSeconds = 0;
}

void     Queue::UpdateAverage()
// ���з����仯����ƽ������ֵ
{ 
  Time_t now = Simulator::Now();
  totalByteSeconds += (now - lastUpdate) * Length();
  lastUpdate = now;
  // ��������˶��г��ȵ���־�����������־
  UpdateTimeSizeLog();
}

bool     Queue::Detailed()
// �ж�detailedģʽ�Ƿ�ʹ�ã����򷵻�True�����򷵻�False     
{
  return detailed; 
}

void     Queue::Detailed(bool d)
// �趨�Ƿ�ʹ��detailedģʽ
{
  detailed = d;
}

Time_t   Queue::QueuingDelay()
// �����Ŷ���ʱ
{
#ifndef WIN32
  if (!interface) return 0;  // û����صĽӿ���������
  Rate_t linkBw = interface->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // ��գ������ݰ�ͨ������������ʱ��
#else
  if (!interface_) return 0;  // û�а󶨽ӿ���������
  Rate_t linkBw = interface_->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // ��գ������ݰ�ͨ������������ʱ��

#endif
}

DCount_t Queue::TotalWorkload()
// �����ܵĹ�������
{
  return totalByteSeconds;
}

void Queue::SetInterface(Interface* i)
// �趨��صĽӿ�
{ 
#ifndef WIN32
  interface = i;
#else
  interface_ = i;
#endif
}

Count_t Queue::DropCount()
//���ض��������ݰ�������
{
  return dropCount;
}

Count_t Queue::EnqueueCount()
//������ӵ����ݰ�����
{
  return totEnq;
}

void    Queue::CountEnq(Packet* p)
//��һ����Ӳ������м���������·��æʱ�ɽӿڵ���
{ 
  totEnq++;
}

void    Queue::ResetStats()
//�������ô����ڴ˶��е�����Ϊ0
{
  dropCount = 0;
  totEnq = 0;
}

void    Queue::AddForcedLoss(Time_t t, Count_t c, Time_t e)
/*
���������һ��ǿ�ƶ�����Ϊ�йص���Ϣ
������Arg1 ������ʱ��
      Arg2 ���ʱ����������������Ŀ
      Arg3 ����������ʱ��
*/
{
  if (!forcedLosses) forcedLosses = new LossList_t();
  LossList_t::iterator i;
  for (i = forcedLosses->begin(); i != forcedLosses->end(); ++i)
    {
      if (t < i->time) break; // Insert before here
    }
  forcedLosses->insert(i, ForcedLoss(t, c, e));
}

bool    Queue::CheckForcedLoss(bool remove)
//�ж�һ��ǿ�ƶ����Ƿ�Ӧ�ñ�ִ��
{
  if (!forcedLosses) return false; // û�ж����б�
  if (forcedLosses->empty()) return false;  // �����б�Ϊ��
  LossList_t::iterator i;
  Time_t now = Simulator::Now();
  bool r = false;

  for (i = forcedLosses->begin(); i != forcedLosses->end(); )
    {
      if (now < i->time) return r; // Done checking
      if (now < i->expiration) r = true;
      if (0) cout << "Forced loss at " << now << " count " << i->count << endl;
      if (remove) i->count--;
      if (i->count == 0 || now >= i->expiration)
        { // Done with this one, remove it
          LossList_t::iterator j = i;
          ++i;
          if (remove) forcedLosses->erase(j);
        }
      if (r) return r; // No need to keep checking
    }
  return r; // code later
}

bool   Queue::CheckSpoofedSource(Packet*)
//һЩ���й�����ƻ�ִ�а����ˣ��ж��Ƿ�������ĳ��ԴIP��ƭ��һ����в�����
{
  return false; 
}


void   Queue::EnableTimeSizeLog(bool b)
//�Ƿ�����־��¼���г�����ʱ��ı仯
{
  if (b)
    { // bΪTrue������
      if (!timeSizeLog) timeSizeLog = new TimeSeq_t();
    }
  else
    { // ����ر���־����ɾ�����д��ڵļ�¼
      delete timeSizeLog;
      timeSizeLog = nil;
    }
}

void   Queue::LogSizePackets(bool b)
//��־�ж��г��ȵĵ�λ��Ϊ���ݰ�������True�������ֽڣ�False��
{
  timeSizePackets = b;
}

void   Queue::PrintTimeSizeLog(ostream& os, Count_t div, char sep)
/*
����������¼���г�����ʱ��仯����־�����ļ���
������Arg1 �����
	  Arg2 ����ʱ��ÿ�����Ҫ���Ը���
	  Arg3 ʱ������кż�ķָ���
*/
{
  if (!timeSizeLog) return; //�����ݿ���
  for (TimeSeq_t::size_type i = 0; i < timeSizeLog->size(); ++i)
    {
      TimeSeq& ts = (*timeSizeLog)[i];
      Count_t v = ts.seq;
      if (div) v /= div;
      os << ts.time << sep << v << endl;
    }
}

void   Queue::DisplayQueue(QTWindow* qtw, Node* s, Node* d)
//����չʾ������е�ʹ����
{
#ifdef HAVE_QT
  if (!animate) return;
  MyPoint sp = qtw->NodeLocation(s); // Get location of source
  MyPoint dp = qtw->NodeLocation(d); // Get location of destination
  // Compute slope of line connecting src/dst
  double dx = dp.x() - sp.x();
  double dy = dp.y() - sp.y();
  // Compute the angle of a line from src to dst
  double theta = atan2(dy, dx);
  double sinTheta = sin(theta);
  double cosTheta = cos(theta);
  double sinThetaM2 = sin(theta + M_PI_2);
  double cosThetaM2 = cos(theta + M_PI_2);

  // Allocate the qLines vector if not already
  if (!qLines) qLines = new ItemVec_t();

#ifdef OLD_WAY
  Count_t k = 0;

  while(true)
    {
      // Display this one
      Packet* p = GetPacket(k);
      if (!p) break; // No more
      double lx = sp.x() + k * cosThetaM2; // Left x
      double ly = sp.y() + k * sinThetaM2; // Left y
      double rx = sp.x() + Queue::defaultAnimWidth * cosTheta + k * cosThetaM2;
      double ry = sp.y() + Queue::defaultAnimWidth * sinTheta + k * sinThetaM2;
      MyCanvasLine* line = nil;
      if (k < qLines->size())
        {
          line = (*qLines)[k];
        }
      else
        {
          line = new MyCanvasLine(qtw->Canvas());
          qLines->push_back(line);
        }
      if (p->IsColored())
        {
          line->setPen(MyColor(p->R(), p->G(), p->B()));
        }
      else
        {
          line->setPen(Qt::blue);
        }
      line->setPoints((int)lx, (int)ly, (int)rx, (int)ry);
      line->show();
      ++k;
    }
#else
  ColorVec_t colors;
  GetPacketColors(colors);

  ColorVec_t::size_type i = 0;
  ItemVec_t::size_type k = 0;
  for (; i < colors.size(); ++i)
    {
      MyCanvasLine* line = nil;
      if (k < qLines->size())
        {
          line = (*qLines)[k];
        }
      else
        {
          line = new MyCanvasLine(qtw->Canvas());
          qLines->push_back(line);
          double lx = sp.x() + k * cosThetaM2; // Left x
          double ly = sp.y() + k * sinThetaM2; // Left y
          double rx = sp.x() + Queue::defaultAnimWidth * cosTheta +
              k * cosThetaM2;
          double ry = sp.y() + Queue::defaultAnimWidth * sinTheta +
              k * sinThetaM2;
          line->setPoints((int)lx, (int)ly, (int)rx, (int)ry);
        }
      line->show();
      line->setPen(MyColor(colors[i]));
      ++k;
    }
#endif

  while(k < qLines->size())
    { // Hide any excess
      (*qLines)[k]->hide();
      ++k;
    }
#endif
}

//����������Ա
void   Queue::UpdateTimeSizeLog()
//���������������¼�¼���г�����ʱ��仯����־
{
  // Note that we use the TimeSeq log from TCP for this, which is
  // a slight hack.  The TimeSeq object uses a type Seq_t for
  // the value being logged, but we need a Count_t value.  Luckily
  // these are both just unsigned long's so we are ok.
  if (!timeSizeLog) return;
  Count_t l = Length();
  if (timeSizePackets) l = LengthPkts();
  if (!timeSizeLog->empty())
    { // See if current timestamp is same as prior,
      // if so, overwrite last entry
      TimeSeq& last = timeSizeLog->back();
      if (last.time == Simulator::Now())
        { // Same, just overwrite
          last.seq = l;
          return;
        }
    }
  timeSizeLog->push_back(TimeSeq(Simulator::Now(), l));
}





// ��̬����
void   Queue::Default(const Queue& d)
//setһ��Ĭ�ϵĶ���
{
  if (defaultQueue) delete defaultQueue; // ɾ�����ڵ�Ĭ�϶���
  defaultQueue = d.Copy();
}

Queue& Queue::Default()
//���ض�Ĭ�϶��ж��������
{
  if (!defaultQueue) defaultQueue = new DropTail();
  return *defaultQueue;
}

void   Queue::DefaultLength(Count_t l)
//�趨Ĭ�϶��л������Ĵ�С����λ���ֽڣ�
{
  defaultLength = l;
  Default().SetLimit(l);
}

void   Queue::DefaultLimitPkts(Count_t l)
//�趨Ĭ�϶��л������Ĵ�С����λ�����ݰ�������
{
  defaultLimitPkts = l;
  Default().SetLimitPkts(l);
}


