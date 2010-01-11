#ifndef  __CInet3_H__ 
#define  __CInet3_H__ 

#pragma once
#include "PlatTopoBase.h"
typedef struct _node_type{
	int nid; /* node id */
	int degree; /* node degree */
	int free_degree; /* unconnected node degree */
	int x, y; /* x, y coordinate */
	struct _node_type **nnp;
} node_type;

class CInet3: public CPlatTopoBase
{
public:
	CInet3(float _frac,int   _seed,int _node_num,
		   IPAddr_t i = IPADDR_NONE,     
		   SystemId_t id = 0,const Linkp2p& link = Linkp2p::Default());
	~CInet3(void);

    bool GenerateTopo();

	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
		 const Location& BoundBoxRightUpper,
		 BoxType  type = DEFAULT);
private:
	bool ConstructorHelper();
	void AllocateForNode();
	void generate_degrees(node_type *, int, int, int);
	void connect_nodes(node_type *, int, int);
private:
	float frac;//��ʼΪ0
	int   seed;  //��ʼΪ0
    int   node_num;//�ڵ����Ŀ������Ѿ��ڸ������ж���,�̳й�ϵ����֮��ȥ��
};
#endif