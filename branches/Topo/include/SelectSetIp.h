#pragma once
#include "G_common_defs.h"
#include "node.h"
#include "PlatTopoBase.h"
#include "HiberTopoBase.h"

#ifndef __SELECTSETIP_H__ 
#define __SELECTSETIP_H__ 


class CHostTopo
{
public:
	CHostTopo(Node* _route):route(_route),hostNum(0){}
	CHostTopo(Count_t hnum):route(0)
	{
		hostNum = 0;
		AddHosts(hnum);
	}
	bool SetRoute(Node* _route){route = _route;return true;}
	~CHostTopo(void){hosts.clear();ips.clear();}
	bool  AddHosts(Count_t hnum);
 
	void  SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper
								/*BoxType  type = DEFAULT*/);
	bool  AddHost();        //添加一个主机
	Node* GetHosti(Count_t i)
	{
		if (i < hosts.size())
		{
			return hosts[i];
		}
		else 
		{
			return NULL;
		}
	}

private:	
	IPAddr_t routeIp;
	Count_t  moveBit;
	Node* route;
	Count_t hostNum;
	vector<Node*> hosts;

	map<IPAddr_t,int> ips;
};
class CSelectSetIp : public CHiberTopoBase
{
public:
	CSelectSetIp(NodeId_t IpBits);
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
    bool GenerateTopo();
	bool AddHostBySelectSetIP(IPAddr_t ip);
	void GetHostLay(vector<CHostTopo*>& _hostLay){_hostLay = hostLay;}

	bool AutoSetRouteAllDomainRoute();
    bool AutoSetDefaultRoute();
	void AutoSetTopoIP();
	void ClearIpHosts(void);
	//Node* GetNode(NodeId_t i);
public:
	~CSelectSetIp(void);
private:
	NodeId_t IpBits;

	vector<CHostTopo*> hostLay;//主机层
	//NodeId_t first;
};
#endif