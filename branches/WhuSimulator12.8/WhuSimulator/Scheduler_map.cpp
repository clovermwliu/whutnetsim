#include "StdAfx.h"
#include "Scheduler_map.h"
#include "PacketEvent.h"
#include <assert.h>


#ifdef EventMap
EventMap_t CScheduler_map::eventList;
SimulatorUid_t  CScheduler_map::uid = 0;
CScheduler_map::CScheduler_map(void)
{
	
}

CScheduler_map::~CScheduler_map(void)
{
	
}
size_t	CScheduler_map::EventLength()
{
	return eventList.size();
}
void	CScheduler_map::Clearlist()
{
	for (EventMap_t::iterator i = eventList.begin();i!=eventList.end();i++)
	{
		CEventBase* t = i->second;
		if (t->event == CPacketEvent::QUEUEPROCESS)
		{
			CPacketEvent* pEvent = (CPacketEvent*)t;
			delete pEvent->packet;
		}
		delete t;
	}
	eventList.clear();
}
void CScheduler_map::InsertEvent(CEventBase* p)
/*
���������¼������в���һ���¼�
������[IN] p  �����뵽�¼������е��¼�
���أ���                                                                                       
��ע����
*/
{
	p->uid = uid++;
	eventList.insert(
		EventMap_t::value_type(KeyPair(p->Time(), p->uid), p));
	
	DebugOutputEventList("Insert:");



}
bool CScheduler_map::EventListEmptyT()
/*
�������¼������Ƿ�Ϊ��
��������
���أ��¼�����Ϊ�գ�����true���¼����в�Ϊ�գ�����false
*/
{
	if (eventList.size()==0)
	{
		return true;
	} 
	else
	{ 
		return false;
	}
}
CEventBase* CScheduler_map::GetTopEvent()
/*
����������¼�����ͷ��Ԫ��
��������
���أ����еĵ�һ��Ԫ��      
��ע����ö���ͷԪ�غ�ͬʱ���¼��Ӷ�����ɾ��
*/
{
	DebugOutputEventList("Get:");

	EventMap_t::iterator i = eventList.begin();
	if (i == eventList.end()) return NULL; //����Ϊ��
	CEventBase* p = i->second;
	eventList.erase(i);
	return p;
}
void CScheduler_map::DebugOutputEventList(string debugType)
{
	DEBUG0((GlobalVariable::out_debug_event.open("c:\\debug_event.txt",ofstream::app)));
	DEBUG0((GlobalVariable::out_debug_event <<debugType<<endl));
	for (EventMap_t::iterator i = eventList.begin();i!=eventList.end();i++)
	{
		CEventBase* t = i->second;
		DEBUG0((GlobalVariable::out_debug_event 
			<<t->Time()
			<<" UID"<<t->uid<<" "
			<<t->handler->GetHandleStatus()<<" "));
		switch (t->event)
		{	
		case CPacketEvent::QUEUEPROCESS:
			DEBUG0((GlobalVariable::out_debug_event <<"QUEUEPROCESS "<<endl));
			break;
		case CPacketEvent::SEND: 
			DEBUG0((GlobalVariable::out_debug_event <<"SEND "<<endl));
			break;
		case CPacketEvent::END: 
			DEBUG0((GlobalVariable::out_debug_event <<"END "<<endl));
		default:
			DEBUG0((GlobalVariable::out_debug_event <<" "<<endl));
			break;
		}
	}
	DEBUG0(GlobalVariable::out_debug_event.close());
	DEBUG0(GlobalVariable::out_debug_event.clear());
}
void CScheduler_map::DebugString(CEventBase* p)
{
	DEBUG0((GlobalVariable::out_debug_event.open("c:\\debug_event.txt",ofstream::app)));

	DEBUG0((GlobalVariable::out_debug_event 
							<<p->Time()
							<<" UID"<<p->uid<<" "
							<<p->handler->GetHandleStatus()<<" "));

	DEBUG0(GlobalVariable::out_debug_event.close());
	DEBUG0(GlobalVariable::out_debug_event.clear());
}
#endif //end of #ifdef EventMap

#ifdef EventMapVector
#define MAX_TIME 1000000

EventMap_t      CScheduler_map::eventMap;   //�¼�map
//  [7/29/2010 Administrator]
EventLaterVec_t      CScheduler_map::eventVecLater; //��Ž���ִ���¼�
EventVec_t      CScheduler_map::eventVecEarlier;//��Ž���ִ���¼�
SimulatorUid_t  CScheduler_map::uid = 0;       //��һ���¼����õ�ID
size_t CScheduler_map::ptop = 0;
double CScheduler_map::gran = 0.5;

Time_t CScheduler_map::maxMapTime = 0;//eventMap�г�EndEvent����ִ���¼���ʱ��
Time_t CScheduler_map::minEVecTime = MAX_TIME;//eventVecEarlier������ִ���¼���ʱ��

void 
CScheduler_map::UpdateVecLaterToMap()//���£�VecLater�е������¼�ת�浽map��
{
	assert(eventMap.size() == 1 && eventVecEarlier.empty());
	while (ptop < eventVecLater.size() && eventVecLater[ptop]->empty())
	{
		++ptop;
	}
	if (ptop == eventVecLater.size())
		return;

	for (size_t i = 0; i < eventVecLater[ptop]->size(); ++i)
	{
		eventMap.insert(EventMap_t::value_type(KeyPair(eventVecLater[ptop]->at(i)->Time(), eventVecLater[ptop]->at(i)->uid), eventVecLater[ptop]->at(i)));
	}
	eventVecLater[ptop++]->clear();
	DEBUG5(GlobalVariable::out_eventMapSize << "Update:  " <<  GetEventlistSize() << "  " 
		<< eventMap.size() << endl);

	DEBUG4(GlobalVariable::out_eventlistSize<<CSimulator::Now()<<" "<<CScheduler_map::GetEventlistSize()<<endl);
}

Time_t 
CScheduler_map::GetMaxMapTime()//��ȡ��EndEvent��map�е������¼�����ʱ�䣬���ֻ��EndEventһ���¼����򷵻�EndEvent��ʱ��
{	
	EventMap_t::iterator it = eventMap.end();
	if ((--it) == eventMap.begin())//map��ֻ��һ���¼�
	{
		return it->second->Time();
	}
	else
	{
		return (--it)->second->Time();
	}

}
void 
CScheduler_map::MergeVecEarlierToMap()//�鲢��VecEarlier�е������¼�ת�浽map��
{
	DEBUG5(GlobalVariable::out_eventMapSize << "Merge: " << GetEventlistSize() << "  " 
		<< eventMap.size() << "  "
		<< eventVecEarlier.size() << endl);

	for (size_t i = 0; i < eventVecEarlier.size(); ++i)
	{
		eventMap.insert(EventMap_t::value_type(KeyPair(eventVecEarlier[i]->Time(), 
														eventVecEarlier[i]->uid), 
														eventVecEarlier[i]));
	}
	eventVecEarlier.clear();
	minEVecTime = MAX_TIME;
}

double 
CScheduler_map::GetCeil(double num, double g) 
{ 
	return ceil((num/*-0.0000001*/)/g)*g;
}

void
CScheduler_map::EnVecLater(CEventBase* pEv)//��VecLater
{
	assert(pEv->Time() > maxMapTime);
	if (pEv->Time() <= ptop*gran+gran)
	{
		if (ptop >= eventVecLater.size())
		{
			if (pEv->event == CPacketEvent::QUEUEPROCESS)
			{
				CPacketEvent* pEvent = (CPacketEvent*)pEv;
				delete pEvent->packet;
			}
			delete pEv;
		}
		else
		{
			eventVecLater[ptop]->push_back(pEv);
		}
	}
	else
	{
		size_t loc = ptop+GetCeil(pEv->Time()-ptop*gran-gran, gran)/gran;
		if (loc<eventVecLater.size())
		{
			eventVecLater[loc]->push_back(pEv);
		}
		else
		{
			if (pEv->event == CPacketEvent::QUEUEPROCESS)
			{
				CPacketEvent* pEvent = (CPacketEvent*)pEv;
				delete pEvent->packet;
			}
			delete pEv;
		}
	}
}

size_t
CScheduler_map::GeteventVecLaterSize()
{
	size_t s = 0;
	for (size_t i = 0; i < eventVecLater.size(); ++i)
	{
		s += eventVecLater[i]->size();
	}
	return s;
}

void 
CScheduler_map::EnVecEarlier(CEventBase* pEv)//��VecEarlier
{
	assert(pEv->Time() <= maxMapTime);
	eventVecEarlier.push_back(pEv);
	if (pEv->Time() < minEVecTime)
		minEVecTime = pEv->Time();
}

void 
CScheduler_map::InsertEndEvent(CEndEvent* pEndEv)//����end�¼���map��
{
	pEndEv->uid = uid++;
	eventMap.insert(
		EventMap_t::value_type(KeyPair(pEndEv->Time(), pEndEv->uid), pEndEv));

}

void 
CScheduler_map::InsertEvent(CEventBase* pEv)
/*
���������¼������в���һ���¼�
������[IN] p  �����뵽�¼������е��¼�
���أ���                                                                                       
��ע����
*/
{
	DebugOutputEventList("Insert:");

	pEv->uid = uid++;
	Time_t t = pEv->Time() - maxMapTime;
	if (t > 0)
	{
		EnVecLater(pEv);
	}
	else if (t < 0)
	{
		EnVecEarlier(pEv);
	}
	else//enMap
	{
		eventMap.insert(EventMap_t::value_type(KeyPair(pEv->Time(),pEv->uid),pEv));		
	}
}

CEventBase* 
CScheduler_map::GetTopEvent()
/*
����������¼�����ͷ��Ԫ��
��������
���أ����еĵ�һ��Ԫ��      
��ע����ö���ͷԪ�غ�ͬʱ���¼��Ӷ�����ɾ��
*/
{
	if (eventMap.size() == 1)
	{
		assert(eventVecEarlier.empty());
		UpdateVecLaterToMap();//����
	}
	else
	{
		if (eventMap.begin()->second->Time() >= minEVecTime)
		{
			MergeVecEarlierToMap();
		}
	}
	maxMapTime = GetMaxMapTime();
	EventMap_t::iterator it = eventMap.begin();
	CEventBase* pEv = it->second;
	eventMap.erase(it);	

	return pEv;
}
void 
CScheduler_map::SetVecLaterSize(double vecLaterTime, int p)
{
	switch(p)
	{
	case -3: gran = 0.125;
		break;
	case -2: gran = 0.25;
		break;
	case -1: gran = 0.5;
	    break;
	case 0: gran = 1;
	    break;
	case 1: gran = 2;
		break;
	case 2: gran = 4;
		break;
	case 3: gran = 8;
		break;
	default: gran = 0.5;
	    break;
	}
	size_t c =  ceil(vecLaterTime/gran);
	
	for (size_t i = 0; i < c; ++i)
	{
		eventVecLater.push_back(new EventVec_t);
	}
}

void	
CScheduler_map::Clearlist()
{
	//eventMap		
	for (EventMap_t::iterator it = eventMap.begin(); it != eventMap.end(); ++it)
	{
		CEventBase* pEv = it->second;
		if (pEv->event == CPacketEvent::QUEUEPROCESS)
		{
			CPacketEvent* pEvent = (CPacketEvent*)pEv;
			delete pEvent->packet;
		}
		delete pEv;
	}
	eventMap.clear();	
	//eventVecEarlier
	for (size_t i = 0; i < eventVecEarlier.size(); ++i)
	{
		CEventBase* pEv = eventVecEarlier[i];
		if (pEv->event == CPacketEvent::QUEUEPROCESS)
		{
			CPacketEvent* pEvent = (CPacketEvent*)pEv;
			delete pEvent->packet;
		}
		delete pEv;
	}
	eventVecEarlier.clear();
	//eventVecLater
	for (size_t i = 0; i < eventVecLater.size(); ++i)
	{
		for (size_t j = 0; j < eventVecLater[i]->size(); ++j)
		{
			CEventBase* pEv = eventVecLater[i]->at(j);
			if (pEv->event == CPacketEvent::QUEUEPROCESS)
			{
				CPacketEvent* pEvent = (CPacketEvent*)pEv;
				delete pEvent->packet;
			}
			delete pEv;
		}
		eventVecLater[i]->clear();		
		delete eventVecLater[i];
	}
	eventVecLater.clear();


	uid = 0;
	ptop = 0;
	gran = 0.5;
	maxMapTime = 0;//eventMap�г�EndEvent����ִ���¼���ʱ��
	minEVecTime = MAX_TIME;//eventVecEarlier������ִ���¼���ʱ��
}

void 
CScheduler_map::DebugOutputEventList(string debugType)
{
	DEBUG0((GlobalVariable::out_debug_event.open("c:\\debug_event.txt",ofstream::app)));
	DEBUG0((GlobalVariable::out_debug_event <<debugType<<endl));

	DEBUG0((GlobalVariable::out_debug_event << "eventMap:"<<endl));
	for (EventMap_t::iterator i = eventMap.begin();i!=eventMap.end();i++)
	{
		CEventBase* t = i->second;
		DEBUG0((GlobalVariable::out_debug_event 
			<<t->Time()
			<<" UID"<<t->uid<<" "
			<<t->handler->GetHandleStatus()<<" "));
		switch (t->event)
		{	
		case CPacketEvent::QUEUEPROCESS:
			DEBUG0((GlobalVariable::out_debug_event <<"QUEUEPROCESS "<<endl));
			break;
		case CPacketEvent::SEND: 
			DEBUG0((GlobalVariable::out_debug_event <<"SEND "<<endl));
			break;
		case CPacketEvent::END: 
			DEBUG0((GlobalVariable::out_debug_event <<"END "<<endl));
		default:
			DEBUG0((GlobalVariable::out_debug_event <<" "<<endl));
			break;
		}
	}

	DEBUG0((GlobalVariable::out_debug_event << "eventVecLater:"<<endl));
	//eventVecLater
	for (size_t i = 0; i < eventVecLater.size(); ++i)
	{
		
		for (size_t j = 0;j<eventVecLater[i]->size();j++)
		{
		////  [7/29/2010 Administrator]
				CEventBase* t = eventVecLater.at(i)->at(j);
				DEBUG0((GlobalVariable::out_debug_event 
					<<t->Time()
					<<" UID"<<t->uid<<" "
					<<t->handler->GetHandleStatus()<<" "));
				switch (t->event)
				{	
				case CPacketEvent::QUEUEPROCESS:
					DEBUG0((GlobalVariable::out_debug_event <<"QUEUEPROCESS "<<endl));
					break;
				case CPacketEvent::SEND: 
					DEBUG0((GlobalVariable::out_debug_event <<"SEND "<<endl));
					break;
				case CPacketEvent::END: 
					DEBUG0((GlobalVariable::out_debug_event <<"END "<<endl));
				default:
					DEBUG0((GlobalVariable::out_debug_event <<" "<<endl));
					break;
				}
		}
		
	}

	DEBUG0((GlobalVariable::out_debug_event << "eventVecEarlier:"<<endl));
	//eventVecEarlier
	for (size_t i = 0; i < eventVecEarlier.size(); ++i)
	{
		CEventBase* t = eventVecEarlier[i];
		DEBUG0((GlobalVariable::out_debug_event 
			<<t->Time()
			<<" UID"<<t->uid<<" "
			<<t->handler->GetHandleStatus()<<" "));
		switch (t->event)
		{	
		case CPacketEvent::QUEUEPROCESS:
			DEBUG0((GlobalVariable::out_debug_event <<"QUEUEPROCESS "<<endl));
			break;
		case CPacketEvent::SEND: 
			DEBUG0((GlobalVariable::out_debug_event <<"SEND "<<endl));
			break;
		case CPacketEvent::END: 
			DEBUG0((GlobalVariable::out_debug_event <<"END "<<endl));
		default:
			DEBUG0((GlobalVariable::out_debug_event <<" "<<endl));
			break;
		}
	}


	DEBUG0(GlobalVariable::out_debug_event.close());
	DEBUG0(GlobalVariable::out_debug_event.clear());
}

void 
CScheduler_map::DebugString(CEventBase* p)
{
	DEBUG0((GlobalVariable::out_debug_event.open("c:\\debug_event.txt",ofstream::app)));

	DEBUG0((GlobalVariable::out_debug_event 
		<<p->Time()
		<<" UID"<<p->uid<<" "
		<<p->handler->GetHandleStatus()<<" "));

	DEBUG0(GlobalVariable::out_debug_event.close());
	DEBUG0(GlobalVariable::out_debug_event.clear());
}

bool 
CScheduler_map::EventListEmptyT() //�ж϶����Ƿ�Ϊ��
{ 
	return (eventMap.empty() ? true: false);
} 

size_t 
CScheduler_map::GetEventlistSize()
{ 
	return eventMap.size() + eventVecEarlier.size() + eventVecLater.size();
}
size_t 
CScheduler_map::GetMapSize()
{ 
	return eventMap.size();
}

#endif//end of #ifdef EventMapVector