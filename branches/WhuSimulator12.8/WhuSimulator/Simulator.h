#pragma once

#ifndef __SIMULAROR_H__
#define __SIMULAROR_H__

#include <map>
#include "EventBase.h"
#include "ErrorHandler.h"
class CMonitor;

class CSimulator : public CErrorHandler
{
public:
	Error_str GetLastErrorEx(){return "hello";}
	CSimulator(void);
	~CSimulator(void);
	//void InitEventList();//��ʼ���¼�����
	void Run();//����
	//Time_t lastT;

	static void SetEndSimulator(bool t){endSimulator = t;}

	static Time_t Now(){return T;}//���ط����¼���ʱ��
	static Time_t T;//��ǰ����ʱ��

	static bool endSimulator;//��������
private:
	CMonitor* monitor;
};
#endif