//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:

//更改人：李玉
//更改时间：2010-3-17
#include "TopoFileScript.h"
#include "Waxman.h"
#include "PFP.h"
#include "Simple.h"
#include "StdTS.h"
#include "SelectSetIp.h"
//#define DEGBUG
#ifndef DEGBUG
#include <time.h>
#endif



CTopoFileScript::CTopoFileScript(const string& _file)
:CFileConfig(_file)
/*
描述：读取配置文件，来新建我们的拓扑类
参数：[IN]_file ：配置文件的路径
返回值：无
*/
{
}

CTopoFileScript::~CTopoFileScript(void)
{
}

bool CTopoFileScript::ReadAllConnectInfo(CHiberTopoBase* Hiber)
/*
描述：读取配置文件中的连接信息，来对层次拓扑的不同平面拓扑进行连接
参数：[OUT]Hiber ：读取的连接信息所操作的层次拓扑名称
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=GetKeyNamesBySectionName("Connect",v);
	for (list<string>::iterator iter = v.begin();iter!=v.end();iter++)
	{
		CGenericConfigItem<string> item1(*this,"Connect",*iter);
		string connectType=item1.MyValue();

		if (!ReadOneConnectInfo(Hiber,connectType))
		{
			return false;
		}
	}
	return true;
}
bool CTopoFileScript::ReadOneConnectInfo(CHiberTopoBase* Hiber,string connectType)
/*
描述：读取一个连接信息，对层次拓扑的不同平面拓扑进行连接
参数：[IN] connectType： 读取的连接信息的Section名称
[OUT]Hiber      ： 读取的连接信息所操作的层次拓扑名称
返回：是否操作成功
*/
{
	list<string> v;
	int paraNum=GetKeyNamesBySectionName(connectType,v);
	vector<Count_t> connect;
	for (list<string>::iterator iter = v.begin();iter!=v.end();iter++)
	{
		CGenericConfigItem<Count_t> item1(*this,connectType,*iter);
		Count_t connectPara=item1.MyValue();
		connect.push_back(connectPara);
	}
	if (connect.size()==6)
	{
		Hiber->ConnectDomain(connect[0],connect[1],connect[2],
			                 connect[3],connect[4],connect[5]);
		return true;
	}
	else 
	{
		SetLastError(ERROR_CONNECT_PARA_ERROR);
		string tmp="Connect para error!";
		SetLastErrorStr(tmp);
		return false;
	}
}
bool CTopoFileScript::ReadOneLayerInfo(int _lay,vector<CPlatTopoBase*>&  TopoVec,const string& section)
/*
描述：读取一层平面拓扑的信息，并新建拓扑
参数：[IN]_lay     ：读取的是第几层拓扑
[OUT]TopoVec ：保存新建的拓扑
返回值：是否新建成功
*/
{
	string lay;
	switch(_lay)
	{
	case 1:lay="L1";break;
	case 2:lay="L2";break;
	case 3:lay="L3";break;
	default:lay="L0";
	}
	CGenericConfigItem<string> item2(*this,section,lay);
	string inputType=item2.MyValue();

	if (inputType=="")
	{
		SetLastError(ERROR_HIBERTOPO_LAY_NO_SECTION);
		string tmp="HiberTopo "+lay+" read NULL";
		tmp=tmp+" Plat style read NULL";
		SetLastErrorStr(tmp);
		return false;
	}
	CGenericConfigItem<int> item3(*this,inputType,"Groups");
	int Groups=item3.MyValue();

	for (int i=0;i<Groups;i++)
	{
		string help;
		stringstream ss;
		ss<<i + 1;
		ss>>help;
		string Plati = "Plat"+help;

		string PlatiNum = Plati + "Num";
		CGenericConfigItem<int> item4(*this,inputType,PlatiNum);
		int Plat1Num=item4.MyValue();

		CGenericConfigItem<string> item5(*this,inputType,Plati);
		string PlatType=item5.MyValue();

		string Type = PlatType.substr(0,2);

		CPlatTopoBase* newTopo;
		if (Type=="Wa")
		{
			for (int newPlat = 0;newPlat<Plat1Num;newPlat++)
			{
				if (ReadOnceWaxmanInfo(PlatType,newTopo))
				{
					TopoVec.push_back(newTopo);
				}	
			}
		}
		else if (Type=="PF")
		{
			for (int newPlat = 0;newPlat<Plat1Num;newPlat++)
			{
				if (ReadOncePFPInfo(PlatType,newTopo))
				{
					TopoVec.push_back(newTopo);
				}
			}
		}
		else if (Type=="St")
		{
			for (int newPlat = 0;newPlat<Plat1Num;newPlat++)
			{
				if (ReadOnceStarInfo(PlatType,newTopo))
				{
					TopoVec.push_back(newTopo);
				}
			}
		}
	}
	return true;
}
int CTopoFileScript::ReadTopoNum()
/*
描述：读取层次拓扑的平面拓扑数量
参数：无
返回值：层次拓扑的平面拓扑数量
*/
{
	CGenericConfigItem<int> item1(*this,"HiberTopo","CplatNum");
	int CplatNum=item1.MyValue();

	return CplatNum;
}
bool CTopoFileScript::ReadOnceWaxmanInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
描述：读取一个Waxman拓扑的信息，新建一个Waxman拓扑
参数：[IN]PlatType     ：Waxman的输入参数的Section
[OUT]newPlatTopo ：新建Waxman拓扑的指针
返回值：是否操作成功
*/
{
	//WhuTNetSimConfigClass::CGenericConfigItem<string> item5(*file,inputType,Plati);
	//string PlatType=item5.MyValue();

	list<string> v;
	int paraNum=GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
	//获得PlatType域的个数
	//CWaxman(Count_t count,double _alpha,double _beta,
	//       IPAddr_t i = IPADDR_NONE,     
	//    	SystemId_t id = 0,const Linkp2p& link = Linkp2p::Default());
	switch(paraNum)
	{
	case 0:
	case 1:
	case 2:
		return false;
		break;
	case 3:
		{
			CGenericConfigItem<Count_t> item2(*this,PlatType,"count");
			Count_t count = item2.MyValue();

			CGenericConfigItem<double> item3(*this,PlatType,"alpha");
			double _alpha = item3.MyValue();

			CGenericConfigItem<double> item4(*this,PlatType,"beta");
			double _beta = item4.MyValue();

			newPlatTopo = new CWaxman(count,_alpha,_beta);
			newPlatTopo->GenerateTopo();
			break;
		}
		
		//case 4:break;
	default:
		SetLastError(ERROR_NO_WRITE_YET);
		string tmp="Not write yet";
		SetLastErrorStr(tmp);
		return false;

	}
	return true;
}
bool CTopoFileScript::ReadOncePFPInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
描述：读取一个PFP拓扑的信息，新建一个PFP拓扑
参数：[IN]PlatType     ：PFP的输入参数的Section
[OUT]newPlatTopo ：新建PFP拓扑的指针
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
	//获得PlatType域的个数
	//CPFP( Count_t count,  
	//	double  _sigma,
	//	double  _p,double _q,
	//	IPAddr_t i = IPADDR_NONE,     
	//	SystemId_t id = 0,
	//	const Linkp2p& link = Linkp2p::Default());
	switch(paraNum)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return false;
		break;
	case 4:
		{
			CGenericConfigItem<int> item1(*this,PlatType,"count");
			int count=item1.MyValue();

			CGenericConfigItem<Count_t> item2(*this,PlatType,"alpha");
			Count_t alpha = item2.MyValue();

			CGenericConfigItem<double> item3(*this,PlatType,"p");
			double p = item3.MyValue();

			CGenericConfigItem<double> item4(*this,PlatType,"q");
			double q = item4.MyValue();

			newPlatTopo = new CPFP(count,alpha,p,q);
			newPlatTopo->GenerateTopo();
			break;
		}
		

		//case 5:break;
	default:
		SetLastError(ERROR_NO_WRITE_YET);
		string tmp="Not write yet";
		SetLastErrorStr(tmp);
		return false;

	}
	return true;
}

bool CTopoFileScript::ReadOnceStarInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
描述：读取一个Star拓扑的信息，新建一个Star拓扑
参数：[IN]PlatType     ：Star的输入参数的Section
[OUT]newPlatTopo ：新建Star拓扑的指针
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
	//获得PlatType域的个数
	//Star1(Count_t, IPAddr_t = IPADDR_NONE, SystemId_t = 0);// Number of nodes
	switch(paraNum)
	{
	case 0:
		return false;
		break;
	case 1:
		{
			CGenericConfigItem<int> item1(*this,PlatType,"count");
			int count=item1.MyValue();

			newPlatTopo = new Star1(count);
			newPlatTopo->GenerateTopo();
			break;
		}
	default:
		SetLastError(ERROR_NO_WRITE_YET);
		string tmp="Not write yet";
		SetLastErrorStr(tmp);
		return false;
	}
	return true;
}
bool CTopoFileScript::ReadOnceSimpleInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
{
	list<string> v;
	int paraNum=GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
	//获得PlatType域的个数
	//Star1(Count_t, IPAddr_t = IPADDR_NONE, SystemId_t = 0);// Number of nodes

	switch(paraNum)
	{
	case 0:
		newPlatTopo = new CSimple();
		newPlatTopo->GenerateTopo();
		break;
	case 1:
		{
			CGenericConfigItem<string> item1(*this,PlatType,"ip");
			string ips=item1.MyValue();
			IPAddr_t i = IPAddr(ips);

			newPlatTopo = new CSimple(i);
			newPlatTopo->GenerateTopo();
			break;
		}
		
	default:
		SetLastError(ERROR_NO_WRITE_YET);
		string tmp="Not write yet";
		SetLastErrorStr(tmp);
		return false;
	}
	return true;
}
bool CTopoFileScript::CreatePlatTopo(CPlatTopoBase*& newTopo)
{
	
	CGenericConfigItem<string> total(*this,"Total","style");
	string style = total.MyValue();

	if (style=="")
	{
		SetLastError(ERROR_PLATTOPO_NO_SECTION);
		string tmp="Total";
		tmp=tmp+" Plat style read NULL";
		SetLastErrorStr(tmp);
		return false;
	}
	
	string Type = style.substr(0,2);
	if (Type =="Wa")                   //Waxman
	{
		ReadOnceWaxmanInfo(style,newTopo);
		return true;
	}
	else if (Type=="PF")              //PFP
	{
		ReadOncePFPInfo(style,newTopo);
		return true;
	}
	else if (Type=="St")             //Star
	{
		ReadOnceStarInfo(style,newTopo);
		return true;
	}
	else  if (Type=="Si")           //Simple
	{
		ReadOnceSimpleInfo(style,newTopo);
		return true;
	}
	else if (Type=="Se")
	{
	}
	return true;
}
bool CTopoFileScript::CreateHiberTopo(CHiberTopoBase*& newTopo)
{
	CGenericConfigItem<string> total(*this,"Total","style");
	string style = total.MyValue();
	
	if (style=="")
	{
		SetLastError(ERROR_HIBERTOPO_NO_SECTION);
		string tmp="Total";
		tmp=tmp+" Hiber style read NULL";
		SetLastErrorStr(tmp);
		return false;
	}

	string Type = style.substr(0,2);
	if (Type =="TS")                   //Waxman
	{
		ReadOnceTSInfo(style,newTopo);
		return true;
	}	
	else if (Type =="SE")
	{
		ReadOnceSelectSetIPInfo(style,newTopo);
	}
	return true;
}
bool CTopoFileScript::ReadOnceSelectSetIPInfo(const string& PlatType,CHiberTopoBase*& newHiberTopo)
{


	CGenericConfigItem<int> item1(*this,PlatType,"N");
	int NMax=item1.MyValue();

	CGenericConfigItem<int> item2(*this,PlatType,"NetId");
	int NetIdbit=item2.MyValue();

	newHiberTopo = new CSelectSetIp(NetIdbit); 
	newHiberTopo->GenerateTopo();

#ifndef DEGBUG
	clock_t start,end,alltime;
	start = clock();
#endif
	newHiberTopo->AutoSetRouteAllDomainRoute();
#ifndef DEGBUG
	end = clock();
	alltime = (end - start)/CLOCKS_PER_SEC;
#endif

	newHiberTopo->AutoSetTopoIP();

	CGenericConfigItem<string> item3(*this,PlatType,"FileVulDistribute");
	string FileVulDistribute=item3.MyValue();

	std::ifstream  readVul(FileVulDistribute.c_str());
	if (!readVul)
	{
		std::cerr<<"unable to open input file!";
		return false;
	}
	Count_t oneHostLan;
	double oneHostp;
	double allP = 0;
	CHostTopo* newHost;
	vector<CHostTopo*> hostLay;
	newHiberTopo->GetHostLay(hostLay);

	size_t addHostNum = 0;
	while (readVul>>oneHostp)
	{
		allP += oneHostp;
		if (allP>1)
		{
			return false;
		}
		oneHostLan = oneHostp*NMax;
		if (oneHostLan == 0)
		{
			addHostNum++;
			continue;
		}
		if (addHostNum<hostLay.size())
		{
			#ifndef DEGBUG
				//clock_t start,end,alltime;
				start = clock();
			#endif
			hostLay[addHostNum++]->AddHosts(oneHostLan);
			#ifndef DEGBUG
				end = clock();
				alltime = (end - start)/CLOCKS_PER_SEC;
			#endif
		}
		else 
		{
			return false;
		}
	}   
}
bool CTopoFileScript::ReadOnceTSInfo(const string& PlatType,CHiberTopoBase*& newPlatTopo)
{
	newPlatTopo = new CStdTS();
	ReadOneLayerInfo(1,newPlatTopo->GetTransit(),PlatType);
	ReadOneLayerInfo(2,newPlatTopo->GetStub(),PlatType);
	ReadOneLayerInfo(3,newPlatTopo->GetLan(),PlatType);
	ReadAllConnectInfo(newPlatTopo);
	return true;
}
void CTopoFileScript::OutputTopo(string outputTopoFile)
/*
描述：将拓扑输出        
参数：[IN] outputTopoFile  ：保存拓扑的文件路径
返回值：空                                                                                       
备注：
*/
{
	ofstream coutTopo(outputTopoFile.c_str());

	NodeVec_t& nodes = Node::nodes;
	Node* outNode;

	
	coutTopo<<nodes.size()<<endl<<endl;                //输出节点的总个数nodeCount

	for (size_t i = 0;i<nodes.size();i++)
	{
		outNode = nodes[i];
		coutTopo<<outNode->Id()<<endl;                  //id
		coutTopo<<outNode->GetProxyIP()<<endl;          //proxyIP
		coutTopo<<outNode->GetProxyMask().NBits()<<endl;//proxyMask

		coutTopo<<outNode->GetDomainRoute()<<endl;      //domainRoute
		NodeWeightVec_t nwv;
		outNode->Neighbors(nwv);

		coutTopo<<nwv.size()<<" ";                     //邻居节点的个数
		NodeWeightVec_t::iterator iter;
		for (iter=nwv.begin();iter!=nwv.end();iter++)
		{
			coutTopo<<iter->node->Id()<<" ";           //neighboursId
		}
		coutTopo<<endl;

		coutTopo<<outNode->GetIPAddr()<<endl;          //ipAddr
		coutTopo<<outNode->GetLocation().X()<<endl;    //x
		coutTopo<<outNode->GetLocation().Y()<<endl;    //y
		coutTopo<<outNode->GetLocation().Z()<<endl;    //z

		if (outNode->IsSwitchNode())
		{
			coutTopo<<1<<endl;                         //isswitch
		}
		else coutTopo<<0<<endl;

		if (outNode->IsRouteNode())
		{
			coutTopo<<1<<endl;                         //isroute
		}
		else coutTopo<<0<<endl;

		if (outNode->IsHostNode())
		{
			coutTopo<<1<<endl;                         //ishost
		}
		else coutTopo<<0<<endl;





		coutTopo<<endl;
	}
}
void CTopoFileScript::InputTopo(string outputTopoFile)
{
	ifstream cinTopo(outputTopoFile.c_str());

	NodeVec_t& nodes = Node::nodes;
	Node* outNode;

	size_t nodeCount;
	cinTopo>>nodeCount;                                  //输出节点的总个数nodeCount
	for (size_t i = 0;i<nodeCount;i++)
	{
		Node* newNode = new Node();
	}

	NodeId_t id;
	IPAddr_t proxyIP;
	Mask_t proxyMask;
	bool domainRoute;
	size_t  neighCount;
	NodeId_t     neighboursId;   
	IPAddr_t ipAddr;
	Meters_t x;
	Meters_t y;
	Meters_t z;
	bool isswitch;
	bool isroute;
	bool ishost;

	while (cinTopo.peek()!=EOF)
	{
		cinTopo>>id>>proxyIP>>proxyMask>>domainRoute;
		cinTopo>>neighCount;
		for (size_t i = 0;i<neighCount;i++)
		{
			cinTopo>>neighboursId;                       //与邻居节点相连
			nodes[id]->AddDuplexLink(nodes[neighboursId]);
		}
		
		cinTopo>>ipAddr>>x>>y>>z>>isswitch>>isroute>>ishost;
		Mask mask(proxyMask);
		nodes[id]->SetProxyRoutingConfig(proxyIP,mask);
		if (domainRoute)
		{
			nodes[id]->SetDomainRoute(true);
		}
		else nodes[id]->SetDomainRoute(false);
		
		nodes[id]->SetIPAddr(ipAddr);

		nodes[id]->SetLocation(x,y,z);

		if (isswitch)
		{
			nodes[id]->IsSwitchNode(true);
		}
		else nodes[id]->IsSwitchNode(false);

		if (isroute)
		{
			nodes[id]->IsRouteNode(true);
		}
		else nodes[id]->IsRouteNode(false);

		if (ishost)
		{
			nodes[id]->IsHostNode(true);
		}
		else nodes[id]->IsHostNode(false);
	}
}