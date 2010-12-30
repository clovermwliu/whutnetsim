#pragma once
#ifndef __EDGE_H__
#define __EDGE_H__

#include "G_common_defs.h"
class CEdge
{
public:
	~CEdge(void);

private:
	NodeId_t _sou_id; //边的起始结点的id
	NodeId_t _des_id; //边的目的结点id
	double _cost;	  //边的花费
	double _delay;	  //边的延时

public:
	//边的构造函数
	CEdge(NodeId_t sid, NodeId_t did );
	NodeId_t get_sou_id() {return _sou_id;}//获得源节点ID
	NodeId_t get_des_id() {return _des_id;}//获得目的节点ID
	double get_cost() { return _cost; }		//获得边的花费
	void set_cost(double c) { _cost = c; }	//设置边的花费
	double get_delay() { return _delay; }	//获得边的延时
	void set_delay(double d) { _delay = d; }//设置边的延时
};
#endif