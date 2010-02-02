 #pragma once
#ifndef __WS_H__
#define __WS_H__

#include "node.h"
#include "PlatTopoBase.h"
//#include "link.h"
#define ERROR_WS_LINK  0x00051001    //ws�������Ӵ���
#define ERROR_WSID_OUT_OF_NODECOUNT_FAIL  0x00051002
class Link;
class CWS : public CPlatTopoBase
{
public:
	CWS(Count_t _nodeCount,
		Count_t _lineForOneNode,
		double _p,
		IPAddr_t i= IPADDR_NONE,
		const Linkp2p& = Linkp2p::Default(),
		SystemId_t = 0);

	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location&, const Location&, BoxType type = DEFAULT);
	
public:
	~CWS(void);

private:
	 //��Ҫ�������ļ���
	int lineForOneNode;     //һ��������ӵıߵ���Ŀ,K��ֵ
	double p;               //��������

private:
	void AddNodes();
	bool AddLines(const Linkp2p& link,
			      IPAddr_t leafIP);
	bool ConstructorHelper(const Linkp2p&,IPAddr_t);
};
#endif
