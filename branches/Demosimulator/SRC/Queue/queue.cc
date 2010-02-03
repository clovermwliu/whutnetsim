
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

// 静态成员变量
Queue*  Queue::defaultQueue  = nil;
Count_t Queue::defaultLength = DEFAULT_DROPTAIL_LIMIT;
Count_t Queue::defaultLimitPkts = 0;  
Count_t Queue::defaultAnimWidth = 10; 
Count_t Queue::globalQueueDrop = 0;   

void Queue::DummyEnque(Packet*)
// 由派生类按需进行覆盖
{ 
}

Count_t Queue::DequeAllDstMac(MACAddr)
// 由派生类按需进行覆盖
{ 
  return 0;
}

Count_t Queue::DequeAllDstIP(IPAddr_t)
// 由派生类按需进行覆盖
{ 
  return 0;
}

Packet* Queue::DequeOneDstMac(MACAddr)
// 由派生类按需进行覆盖
{ 
  return nil;
}

Packet* Queue::DequeOneDstIP(IPAddr_t)
// 由派生类按需进行覆盖
{ 
  return nil;
}

DCount_t Queue::Average()
// 计算平均队列长度
{ 
  Time_t now = Simulator::Now();
  if (now == startTime) return 0.0;    // 避免除数为0
  return totalByteSeconds / (now - startTime);
}

void     Queue::ResetAverage()
// 使用新的时间间隔来计算平均队列
{ 
  startTime = Simulator::Now();
  lastUpdate = startTime;
  totalByteSeconds = 0;
}

void     Queue::UpdateAverage()
// 队列发生变化更新平均队列值
{ 
  Time_t now = Simulator::Now();
  totalByteSeconds += (now - lastUpdate) * Length();
  lastUpdate = now;
  // 如果启用了队列长度的日志，则还需更新日志
  UpdateTimeSizeLog();
}

bool     Queue::Detailed()
// 判断detailed模式是否被使用，有则返回True，否则返回False     
{
  return detailed; 
}

void     Queue::Detailed(bool d)
// 设定是否使用detailed模式
{
  detailed = d;
}

Time_t   Queue::QueuingDelay()
// 计算排队延时
{
#ifndef WIN32
  if (!interface) return 0;  // 没有相关的接口则不做处理
  Rate_t linkBw = interface->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // 清空（让数据包通过）队列所需时间
#else
  if (!interface_) return 0;  // 没有绑定接口则不做处理
  Rate_t linkBw = interface_->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // 清空（让数据包通过）队列所需时间

#endif
}

DCount_t Queue::TotalWorkload()
// 计算总的工作负载
{
  return totalByteSeconds;
}

void Queue::SetInterface(Interface* i)
// 设定相关的接口
{ 
#ifndef WIN32
  interface = i;
#else
  interface_ = i;
#endif
}

Count_t Queue::DropCount()
//返回丢弃的数据包的总数
{
  return dropCount;
}

Count_t Queue::EnqueueCount()
//返回入队的数据包总数
{
  return totEnq;
}

void    Queue::CountEnq(Packet* p)
//对一次入队操作进行计数，当链路不忙时由接口调用
{ 
  totEnq++;
}

void    Queue::ResetStats()
//重置所用从属于此队列的数据为0
{
  dropCount = 0;
  totEnq = 0;
}

void    Queue::AddForcedLoss(Time_t t, Count_t c, Time_t e)
/*
描述：添加一次强制丢包行为有关的信息
参数：Arg1 丢包的时刻
      Arg2 这段时间内连续丢包的数目
      Arg3 丢包结束的时刻
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
//判断一次强制丢包是否应该被执行
{
  if (!forcedLosses) return false; // 没有丢包列表
  if (forcedLosses->empty()) return false;  // 丢包列表为空
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
//一些队列管理机制会执行包过滤，判断是否丢弃来自某个源IP欺骗，一般队列不过滤
{
  return false; 
}


void   Queue::EnableTimeSizeLog(bool b)
//是否开启日志记录队列长度随时间的变化
{
  if (b)
    { // b为True，开启
      if (!timeSizeLog) timeSizeLog = new TimeSeq_t();
    }
  else
    { // 否则关闭日志，并删除所有存在的记录
      delete timeSizeLog;
      timeSizeLog = nil;
    }
}

void   Queue::LogSizePackets(bool b)
//日志中队列长度的单位是为数据包个数（True）或是字节（False）
{
  timeSizePackets = b;
}

void   Queue::PrintTimeSizeLog(ostream& os, Count_t div, char sep)
/*
描述：将记录队列长度随时间变化的日志导入文件中
参数：Arg1 输出流
	  Arg2 非零时，每个序号要除以该数
	  Arg3 时间和序列号间的分隔符
*/
{
  if (!timeSizeLog) return; //无数据可用
  for (TimeSeq_t::size_type i = 0; i < timeSizeLog->size(); ++i)
    {
      TimeSeq& ts = (*timeSizeLog)[i];
      Count_t v = ts.seq;
      if (div) v /= div;
      os << ts.time << sep << v << endl;
    }
}

void   Queue::DisplayQueue(QTWindow* qtw, Node* s, Node* d)
//动画展示缓冲队列的使用率
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

//保护方法成员
void   Queue::UpdateTimeSizeLog()
//由派生类用来更新记录队列长度随时间变化的日志
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





// 静态方法
void   Queue::Default(const Queue& d)
//set一个默认的队列
{
  if (defaultQueue) delete defaultQueue; // 删除存在的默认队列
  defaultQueue = d.Copy();
}

Queue& Queue::Default()
//返回对默认队列对象的引用
{
  if (!defaultQueue) defaultQueue = new DropTail();
  return *defaultQueue;
}

void   Queue::DefaultLength(Count_t l)
//设定默认队列缓冲区的大小（单位：字节）
{
  defaultLength = l;
  Default().SetLimit(l);
}

void   Queue::DefaultLimitPkts(Count_t l)
//设定默认队列缓冲区的大小（单位：数据包个数）
{
  defaultLimitPkts = l;
  Default().SetLimitPkts(l);
}


