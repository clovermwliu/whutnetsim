#pragma once
#ifndef __EDGE_H__
#define __EDGE_H__

#include "G_common_defs.h"
class CEdge
{
public:
	~CEdge(void);

private:
	NodeId_t _sou_id; //�ߵ���ʼ����id
	NodeId_t _des_id; //�ߵ�Ŀ�Ľ��id
	double _cost;	  //�ߵĻ���
	double _delay;	  //�ߵ���ʱ

public:
	//�ߵĹ��캯��
	CEdge(NodeId_t sid, NodeId_t did );
	NodeId_t get_sou_id() {return _sou_id;}//���Դ�ڵ�ID
	NodeId_t get_des_id() {return _des_id;}//���Ŀ�Ľڵ�ID
	double get_cost() { return _cost; }		//��ñߵĻ���
	void set_cost(double c) { _cost = c; }	//���ñߵĻ���
	double get_delay() { return _delay; }	//��ñߵ���ʱ
	void set_delay(double d) { _delay = d; }//���ñߵ���ʱ
};
#endif