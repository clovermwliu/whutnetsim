#pragma once

#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__


#include "G_common_defs.h"
#include "Handler.h"
#include "ErrorHandler.h"
#include "GlobalVariable.h"


//��û�д�����ٰ��Ĳ���
class CHandler;
class Packet;
#define NO_EVENT 0xffffffff

//�¼��Ļ���
class CEventBase : public CErrorHandler
{
public:
	Error_str GetLastErrorEx(){return "hello";}
	CEventBase() : event(NO_EVENT), uid(++uid_all), time(0), handler(0)
	{GlobalVariable::eventsAllocated++;}//�¼����๹�캯��

	CEventBase(Event_t ev) : event(ev), uid(++uid_all), time(0), handler(0)
	{GlobalVariable::eventsAllocated++;}//�¼����๹�캯��

	CEventBase(Event_t ev, CHandler* h) : event(ev), uid(++uid_all), time(0), handler(h)
	{GlobalVariable::eventsAllocated++;}//�¼����๹�캯��

	CEventBase(CHandler* h) : event(0), uid(++uid_all), time(0), handler(h)
	{GlobalVariable::eventsAllocated++;}//�¼����๹�캯��

	CEventBase(Event_t p,CHandler* h,Time_t t);
	
	virtual ~CEventBase(void);
	virtual Packet* GetPacket(){return NULL;}

	inline Time_t         Time() const { return time;} //�����¼�ִ�е�ʱ��
	inline void           Time(Time_t t){  time = t; } //�޸��¼�ִ�е�ʱ��


	// handler [8/3/2010 Administrator]
	virtual Packet_t GetPacketNumROnce(){return 0;}
private:
	Time_t         time;	 //�¼�ִ�е�ʱ��
public:
	CHandler*       handler; //�¼������ָ��	
	Event_t        event;//�¼�����
	SimulatorUid_t uid;

	static SimulatorUid_t uid_all;	 //�¼���ID
	
};
#endif