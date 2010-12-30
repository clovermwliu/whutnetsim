#pragma once


#ifndef __SCHEDULER_MAP_H__
#define __SCHEDULER_MAP_H__


#include <map>
#include "G_common_defs.h"
#include "EventBase.h"
#include "ErrorHandler.h"
#include "Simulator.h"
#include <vector>
#include "EndEvent.h"
#include <math.h>

//Ϊ�˱�֤���ظ��ԵĽ���������¼���UID����������
class KeyPair {
public:
	KeyPair(Time_t t, SimulatorUid_t u) : time(t), uid(u) { }
public:
	Time_t         time; //�¼���ִ��ʱ��
	SimulatorUid_t uid;  //�¼���ID��
};

// �¼����й���Ķ���
typedef std::pair<Time_t,SimulatorUid_t> KeyPair_t; //map�ṹ��key
class key_less
{
public:
	key_less() { }
	bool operator()(const KeyPair& l, const KeyPair& r) const {
		return (l.time < r.time ||
			l.time == r.time && l.uid < r.uid);
	}
};


#ifdef EventMap
// ����map���ͱ����¼�
typedef std::map<KeyPair, CEventBase*, key_less> EventMap_t;

class CScheduler_map : public CErrorHandler
{
public:
	CScheduler_map(void);
	~CScheduler_map(void); 
	static void InsertEvent(CEventBase*);//�����¼�
	static size_t GetEventlistSize(){return eventList.size();}
	static size_t EventLength();//�����¼�
	static CEventBase* GetTopEvent();//�ԴӶ�����ɾ��
	static bool EventListEmptyT();  //�ж϶����Ƿ�Ϊ��
	static void  ResetUid(){ uid = 0;}       //��һ���¼����õ�ID
private:
	static void   DebugOutputEventList(string debugType);
	static void   DebugString(CEventBase* p);

public:
	static void	  Clearlist();
	
private:
	static EventMap_t      eventList;   //�¼�����
	static SimulatorUid_t  uid;       //��һ���¼����õ�ID

public:

	Count_t         totevs;      //�����Ź����¼�����
	Count_t         totrm;       //�Ӷ������Ƴ������¼�����
	Count_t         totevp;      //���й����¼�����
	Count_t         totevc;      //ȡ�������¼�����
	Count_t         evlSize;     //�¼������е��¼�����
	Count_t         maxSize;     //�������¼��������¼����������ֵ
};
#endif//end of #ifdef EventMap

#ifdef EventMapVector 
typedef std::map<KeyPair, CEventBase*, key_less> EventMap_t;
typedef std::vector<CEventBase* > EventVec_t;
//  [7/29/2010 Administrator]
typedef std::vector< vector<CEventBase* >* > EventLaterVec_t; 


#define CHECK_TIME
class CScheduler_map : public CErrorHandler
{
public:
	CScheduler_map(void) {}
	~CScheduler_map(void) {}

	static void InsertEvent(CEventBase* pEv);//�����¼�
	static void InsertEndEvent(CEndEvent* pEndEv);//����end�¼���map��
	static size_t GetEventlistSize();
	static size_t GetMapSize();
	static CEventBase* GetTopEvent();//�ԴӶ�����ɾ��
	static bool EventListEmptyT(); //�ж϶����Ƿ�Ϊ��
	static void  ResetUid(){ uid = 0;}       //��һ���¼����õ�ID

	static void	  Clearlist();
	// B [7/29/2010 Administrator]
	static  void SetVecLaterSize(double vecLaterTime, int p = 3);

private:
	static void   DebugOutputEventList(string debugType);
	static void   DebugString(CEventBase* p);
	static size_t GeteventVecLaterSize();

private:
	static EventMap_t      eventMap;   //�¼�map
	static EventLaterVec_t      eventVecLater; //��Ž���ִ���¼�
	static EventVec_t      eventVecEarlier;//��Ž���ִ���¼�
	static SimulatorUid_t  uid;       //��һ���¼����õ�ID

	static size_t ptop;

	static double gran;
	static Time_t maxMapTime;////eventMap�г�EndEvent����ִ���¼���ʱ��
	static Time_t minEVecTime;//eventVecEarlier������ִ���¼���ʱ��

private:
	// B�޸� [7/29/2010 Administrator]�����¼�ʱ����vectorlater
	static void UpdateVecLaterToMap();//���£�VecLater�е������¼�ת�浽map��
	static void MergeVecEarlierToMap(); //�鲢��VecEarlier�е������¼�ת�浽map��

	// B�޸� [7/29/2010 Administrator]�����¼�ʱ����vectorlater
	static void EnVecLater(CEventBase* pEv);//��VecLater
	static void EnVecEarlier(CEventBase* pEv); //��VecEarlier

	static Time_t GetMaxMapTime(); //��ȡ��EndEvent��map�е������¼�����ʱ�䣬���ֻ��EndEventһ���¼����򷵻�EndEvent��ʱ��

	static double GetCeil(double num, double g = 1);
};

#endif//end of #ifdef EventMapVector
#endif//end of #ifndef __SCHEDULER_MAP_H__