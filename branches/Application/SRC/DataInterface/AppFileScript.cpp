#include "AppFileScript.h"
#include "G_common_defs.h"
#include "node.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "application-tcpserver.h"
#include "application-tcpsend.h"
#include "ApplicationWormUDP.h"
#include "WormScanModel.h"
#include "ipaddr.h"
#include <fstream>

using namespace std;

CAppFileScript::CAppFileScript( const string& appfile ):CFileScript(appfile)
{
	bInitVul = false;


}


CAppFileScript::~CAppFileScript(void)
{
	delete pVulDistribute;


}
int
CAppFileScript::InitWormUDP(int node_id, string appSec, string paraSec, string settingSec)
{
	ApplicationWormUDP worm;

	if (paraSec != "NULL")//构造函数
	{

	}	
	bool start = false;//是否从该结点发起
	string modelSec = "";//模型

	bool bStartRandom = false;//是否随机选择蠕虫开始传播的结点
	int StartRandomNum = 0;//随机选择的数目


	if (settingSec != "NULL")
	{
		list<string> lset_wormudp;
		int set_num = GetKeyNamesBySectionName(settingSec, lset_wormudp);
		if (set_num <= 0)
		{
			SetLastError(ERROR_APPSCRIPT_GET_WORM_SET);
			return -8;

		}
		list<string>::iterator lset_wormudp_it = lset_wormudp.begin();
		while(lset_wormudp_it != lset_wormudp.end())
		{
			string str_tmp = lset_wormudp_it->data();
			if(str_tmp == "model")//模型			
			{
				CGenericConfigItem<string> _nodeSet(*this, settingSec, lset_wormudp_it->data());	
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_PATH);
					return -9;
				}
				modelSec = _nodeSet.MyValue();

			}
			else if(str_tmp == "scanrate")
			{
				CGenericConfigItem<int> _nodeSet(*this, settingSec, lset_wormudp_it->data());
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_SCANRATE);
					return -15;
				}
				int scanrate = _nodeSet.MyValue();

				worm.SetScanspersecond(scanrate);
			}
			else if(str_tmp == "startID")
			{
				CGenericConfigItem<string> _nodeSet(*this, settingSec, lset_wormudp_it->data());		
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
					return -10;
				}
				string WormStartSec = _nodeSet.MyValue();
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
					return -10;
				}

				/*
				//获得是否随机选择开始传播的结点
				CGenericConfigItem<bool> _wormStart(*this, WormStartSec, "startType");
				if (_wormStart.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
				//不存在这一项，故指定开始传播结点
				bStartRandom = false;
				}
				else
				{
				bStartRandom = _wormStart.MyValue();
				if (!bStartRandom)//此时bStartRandom应为true
				{
				SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTTYPE);
				return -30;
				}							
				}

				if (bStartRandom)//随机选择
				{
				//随机开始的个数
				CGenericConfigItem<bool> _wormStart(*this, WormStartSec, "startnum");
				if (_wormStart.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
				//不存在这一项，故指定开始传播结点
				bStartRandom = false;
				}
				StartRandomNum = _wormStart.MyValue();
				}
				*/

				list<string> lstart_wormudp;
				int start_num = GetKeyNamesBySectionName(WormStartSec, lstart_wormudp);
				if (start_num <= 0)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
					return -10;
				}

				int start_id;
				list<string>::iterator lstart_wormudp_it = lstart_wormudp.begin();
				while(lstart_wormudp_it != lstart_wormudp.end())
				{
					CGenericConfigItem<int> _nodeSet(*this, WormStartSec, lstart_wormudp_it->data());												
					if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
					{
						SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
						return -16;
					}
					start_id  = _nodeSet.MyValue();

					if (start_id == node_id)
					{
						start = true;
						break;
					}
					++lstart_wormudp_it;
				}
			}
			else//---增加case参数设置
			{

			}
			++lset_wormudp_it;
		}

	}
	if (modelSec != "")
	{
		CGenericConfigItem<string> _modelType(*this, modelSec, "modelType");												
		if (_modelType.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
		{
			SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE);
			return -27;
		}
		Count_t range = -1;
		Mask m = IPAddr::ToIP("255.255.255.255");
		CGenericConfigItem<unsigned long> _modelRange(*this, modelSec, "scanrange");												
		if (_modelRange.GetLastError() == ERROR_CONFIG_ITEM_SUCCESS)
		{
			range = _modelRange.MyValue();
		}
		else
		{
			CGenericConfigItem<unsigned long> _modelMask(*this, modelSec, "mask");												
			if (_modelMask.GetLastError() == ERROR_CONFIG_ITEM_SUCCESS)
			{
				m = _modelMask.MyValue();
			}
		}

		string model_type = _modelType.MyValue();
		if (model_type == "DynamicVulPrefScan")
		{
			if (!bInitVul)//未初始化脆弱主机分布
			{
				//读脆弱主机分布
				CGenericConfigItem<string> _dPrefVulFile(*this, modelSec, "FileVulDistribute");												
				if (_dPrefVulFile.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE);
					return -28;
				}
				string file_name = _dPrefVulFile.MyValue();
				ifstream in_f(file_name.c_str());
				if (!in_f)
				{
					SetLastError(ERROR_APPSCRIPT_OPEN_DPREF_FILE);
					return -29;
				}
				pVulDistribute = new vector<double>;
				double tmpDW;
				in_f >> tmpDW;
				while(in_f)
				{
					pVulDistribute->push_back(tmpDW);
					in_f >> tmpDW;
				}
				in_f.close();
				in_f.clear();

				bInitVul = true;
			}

			if(range != -1)
			{
				DynamicVulPrefScan model = DynamicVulPrefScan(range, pVulDistribute, *this);
				if (model.GetLastError() != ERROR_MODEL_SUCCESS)
				{
					SetLastError(ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL);
					return -17;
				}
				worm.SetAppModel(model);
			}
			else
			{
				DynamicVulPrefScan model = DynamicVulPrefScan(m, pVulDistribute, *this);
				if (model.GetLastError() != ERROR_MODEL_SUCCESS)
				{
					SetLastError(ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL);
					return -17;
				}
				worm.SetAppModel(model);
			}
		}
		else
		{

		}
	}

	Node * wormnode = Node::GetNode(node_id);
	if (wormnode == NULL)
	{
		SetLastError(ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID);
		return -11;
	}

	ApplicationWormUDP* appworm = 
		(ApplicationWormUDP*)(wormnode->AddApplication(worm));

	if (start) 
	{
		appworm->Infect();
		appworm->Start(0.0);
	}

	return 1;
}

int
CAppFileScript::InitTCPServer(int node_id, string appSec, string paraSec, string settingSec)
{
	TCPServer* server;
	if (paraSec != "NULL")
	{
		//---构造函数
		//---到section为paraSec内找type类型应用构造函数的参数

	}
	else
	{
		Node * tcpserverNode = Node::GetNode(node_id);
		if (tcpserverNode == NULL)
		{
			SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID);
			return -12;
		}
		server = (TCPServer*)(tcpserverNode->AddApplication(TCPServer()));
	}

	if (settingSec != "NULL")//不是默认
	{
		list<string> lset_tcpserver;
		int set_num = GetKeyNamesBySectionName(settingSec, lset_tcpserver);
		if (set_num <= 0)
		{
			SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_SET);
			return -18;
		}

		list<string>::iterator lset_tcpserver_it = lset_tcpserver.begin();
		while(lset_tcpserver_it != lset_tcpserver.end())
		{
			string strtmp = lset_tcpserver_it->data();
			if (strtmp == "BindPort")//端口号
			{	
				CGenericConfigItem<int> _nodeSet(*this, settingSec, lset_tcpserver_it->data());
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT);
					return -21;
				}
				int port = _nodeSet.MyValue();
				server->BindAndListen(port); // Bind to port 80			

			}  
			else//---增加case参数设置
			{

			}
			++lset_tcpserver_it;
		}
	}	
	return 1;
}

int
CAppFileScript::InitTCPSend(int node_id, string appSec, string paraSec, string settingSec)
{
	int targetID = 0;
	int port = 80;
	int countnum = 10000;
	//---增加参数
	if (paraSec != "NULL")//用户自定义以上参数
	{
		list<string> lpara_tcpsend;
		int para_num = GetKeyNamesBySectionName(paraSec, lpara_tcpsend);
		if (para_num <= 0)
		{
			SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_PARA);
			return -19;
		}
		list<string>::iterator lpara_tcpsend_it = lpara_tcpsend.begin();
		//读参数
		string str_tmp;
		while(lpara_tcpsend_it != lpara_tcpsend.end())
		{
			str_tmp = lpara_tcpsend_it->data();
			if(str_tmp == "BindPort")//端口号
			{
				CGenericConfigItem<int> _nodeSet(*this, paraSec, str_tmp);
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT);
					return -20;
				}

				port = _nodeSet.MyValue();
			}
			else if(str_tmp == "targetID")
			{
				CGenericConfigItem<int> _nodeSet(*this, paraSec, lpara_tcpsend_it->data());
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_TARGETID);
					return -22;
				}
				targetID = _nodeSet.MyValue();
			}
			else if(str_tmp == "countnum")
			{
				CGenericConfigItem<int> _nodeSet(*this, paraSec, lpara_tcpsend_it->data());
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM);
					return -23;
				}
				countnum = _nodeSet.MyValue();
			}	
			else//---增加case参数设置
			{

			}
			++lpara_tcpsend_it;
		}  						  
	}
	Node * tcpsendNode = Node::GetNode(node_id);
	Node * tcpsendServerNode = Node::GetNode(targetID);
	if (tcpsendNode == NULL || tcpsendServerNode == NULL)
	{
		SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID);
		return -13;
	}
	TCPSend*  client = (TCPSend*)(tcpsendNode->AddApplication(
		TCPSend(tcpsendServerNode->GetIPAddr(), port, Constant(countnum))));

	if (settingSec != "NULL")//不是默认
	{
		list<string> lset_tcpsend;
		int set_num = GetKeyNamesBySectionName(settingSec, lset_tcpsend);
		if (set_num <= 0)
		{
			SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_SET);
			return -24;
		}
		list<string>::iterator lset_tcpsend_it = lset_tcpsend.begin();
		while(lset_tcpsend_it != lset_tcpsend.end())
		{
			string str_tmp = lset_tcpsend_it->data();

			if(str_tmp == "ISstart")
			{
				//if (flag)//为所有结点绑定该应用，不应该有这个配置项
				//{
				//	continue;
				//}
				CGenericConfigItem<bool> _nodeSet(*this, settingSec, lset_tcpsend_it->data());
				if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
				{
					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_ISSTART);
					return -25;
				}	
				bool isStart = _nodeSet.MyValue();
				if (isStart)
				{
					CGenericConfigItem<int> _nodeSet(*this, settingSec, "StartTime");
					if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
					{
						SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME);
						return -26;
					}	
					int startTime = _nodeSet.MyValue();
					client->Start(startTime);
				}						

				break;
			}
			else//---增加参数设置
			{

			}
			++lset_tcpsend_it;


		}
	}	
	return 1;
}

int CAppFileScript::ReadAppbyInifile()    //读取应用脚本文件
/*
返回值：1 SUCCESS
-1 ERROR_APPSCRIPT_NODES_EMPTY
-2 ERROR_APPSCRIPT_GET_NODES
-3 ERROR_APPSCRIPT_GET_NODES_SIZE
-4 ERROR_APPSCRIPT_GET_NODES_SECTION
-5 ERROR_APPSCRIPT_GET_APP_TYPE
-6 ERROR_APPSCRIPT_GET_APP_PARA
-7 ERROR_APPSCRIPT_GET_APP_SET
-8 ERROR_APPSCRIPT_GET_WORM_SET
-9 ERROR_APPSCRIPT_GET_WORM_MODEL_PATH
-10 ERROR_APPSCRIPT_GET_WORM_STARTID
-11 ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID
-12 ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID
-13 ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID
-14 ERROR_APPSCRIPT_GET_NODE_ID
-15 ERROR_APPSCRIPT_GET_WORM_SCANRATE
-16 ERROR_APPSCRIPT_WORM_MODEL_PARA
-17 ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL
-18 ERROR_APPSCRIPT_GET_TCPSERVER_SET
-19 ERROR_APPSCRIPT_GET_TCPSEND_PARA
-20 ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT
-21 ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT
-22 ERROR_APPSCRIPT_GET_TCPSEND_TARGETID
-23 ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM
-24 ERROR_APPSCRIPT_GET_TCPSEND_SET
-25 ERROR_APPSCRIPT_GET_TCPSEND_ISSTART
-26 ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME
-27 ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE
-28 ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE
-29 ERROR_APPSCRIPT_OPEN_DPREF_FILE
-30 ERROR_APPSCRIPT_GET_WORM_STARTTYPE
*/
{
	list<string> l_node;
	int num = GetKeyNamesBySectionName("AppNodes", l_node);//需要绑定应用的结点数
	if (num <= 0)
	{
		SetLastError(ERROR_APPSCRIPT_NODES_EMPTY);
		return -1;
	}
	list<string>::iterator l_node_it = l_node.begin();

	//判断是否同一应用到绑定所有结点
	bool flag = false;
	if (num == 1)
	{
		CGenericConfigItem<string> _node(*this, "AppNodes", l_node_it->data());
		if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
		{
			SetLastError(ERROR_APPSCRIPT_GET_NODES);
			return -2;
		}
		string str_id = _node.MyValue();

		if (str_id =="all")
		{
			flag = true;
			num = (Node::nodes).size();//当前拓扑结点个数
			if (num <= 0)
			{
				SetLastError(ERROR_APPSCRIPT_GET_NODES_SIZE);
				return -3;
			}
		}
	}
	//绑定应用到相应结点
	string appSec;
	int node_id;
	string app_type;

	int tmp_num = num;

	for (int i = 0; i < num; ++i)
	{
		if (flag)
		{
			appSec = "all";
			node_id = Node::nodes.at((--tmp_num))->Id();
		}
		else
		{
			CGenericConfigItem<string> _node(*this, "AppNodes", l_node_it->data());
			if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
			{
				SetLastError(ERROR_APPSCRIPT_GET_NODES_SECTION);
				return -4;
			}
			appSec = _node.MyValue();

			CGenericConfigItem<int> _nodeApp(*this, appSec, "node_id");
			node_id = _nodeApp.MyValue();
			if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
			{
				SetLastError(ERROR_APPSCRIPT_GET_NODE_ID);
				return -14;
			}
			++l_node_it;

		}
		CGenericConfigItem<string> _appType(*this, appSec, "type");
		string type = _appType.MyValue();
		if (_appType.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
		{
			SetLastError(ERROR_APPSCRIPT_GET_APP_TYPE);
			return -5;
		}

		CGenericConfigItem<string> _appPara(*this, appSec, "para");
		if (_appPara.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
		{
			SetLastError(ERROR_APPSCRIPT_GET_APP_PARA);
			return -6;
		}
		string paraSec = _appPara.MyValue();


		CGenericConfigItem<string> _appSet(*this, appSec, "setting");
		if (_appSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
		{
			SetLastError(ERROR_APPSCRIPT_GET_APP_SET);
			return -7;
		}
		string settingSec = _appSet.MyValue();

		if (type == "WormUDP")
		{
			InitWormUDP(node_id, appSec, paraSec, settingSec);

		}
		else if(type == "TCPServer")//应该有默认值
		{
			InitTCPServer(node_id, appSec, paraSec, settingSec);				  
		}

		else if(type == "TCPSend")
		{
			InitTCPSend(node_id, appSec, paraSec, settingSec);

		}
		else//----增加应用
		{
		}	
	}

	return 1;
}

//
//int CAppFileScript::ReadAppbyInifile()    //读取应用脚本文件
///*
//返回值：1 SUCCESS
//-1 ERROR_APPSCRIPT_NODES_EMPTY
//-2 ERROR_APPSCRIPT_GET_NODES
//-3 ERROR_APPSCRIPT_GET_NODES_SIZE
//-4 ERROR_APPSCRIPT_GET_NODES_SECTION
//-5 ERROR_APPSCRIPT_GET_APP_TYPE
//-6 ERROR_APPSCRIPT_GET_APP_PARA
//-7 ERROR_APPSCRIPT_GET_APP_SET
//-8 ERROR_APPSCRIPT_GET_WORM_SET
//-9 ERROR_APPSCRIPT_GET_WORM_MODEL_PATH
//-10 ERROR_APPSCRIPT_GET_WORM_STARTID
//-11 ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID
//-12 ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID
//-13 ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID
//-14 ERROR_APPSCRIPT_GET_NODE_ID
//-15 ERROR_APPSCRIPT_GET_WORM_SCANRATE
//-16 ERROR_APPSCRIPT_WORM_MODEL_PARA
//-17 ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL
//-18 ERROR_APPSCRIPT_GET_TCPSERVER_SET
//-19 ERROR_APPSCRIPT_GET_TCPSEND_PARA
//-20 ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT
//-21 ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT
//-22 ERROR_APPSCRIPT_GET_TCPSEND_TARGETID
//-23 ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM
//-24 ERROR_APPSCRIPT_GET_TCPSEND_SET
//-25 ERROR_APPSCRIPT_GET_TCPSEND_ISSTART
//-26 ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME
//-27 ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE
//-28 ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE
//-29 ERROR_APPSCRIPT_OPEN_DPREF_FILE
//-30 ERROR_APPSCRIPT_GET_WORM_STARTTYPE
//*/
//{
//	list<string> l_node;
//	int num = GetKeyNamesBySectionName("AppNodes", l_node);//需要绑定应用的结点数
//	if (num <= 0)
//	{
//		SetLastError(ERROR_APPSCRIPT_NODES_EMPTY);
//		return -1;
//	}
//	list<string>::iterator l_node_it = l_node.begin();
//
//	//判断是否同一应用到绑定所有结点
//	bool flag = false;
//	if (num == 1)
//	{
//		CGenericConfigItem<string> _node(*this, "AppNodes", l_node_it->data());
//		if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//		{
//			SetLastError(ERROR_APPSCRIPT_GET_NODES);
//			return -2;
//		}
//		string str_id = _node.MyValue();
//
//		if (str_id =="all")
//		{
//			flag = true;
//			num = (Node::nodes).size();//当前拓扑结点个数
//			if (num <= 0)
//			{
//				SetLastError(ERROR_APPSCRIPT_GET_NODES_SIZE);
//				return -3;
//			}
//		}
//	}
//	//绑定应用到相应结点
//	string appSec;
//	int node_id;
//	string app_type;
//
//	int tmp_num = num;
//	////worm
//	//vector<int> _worm_startID;
//
//	for (int i = 0; i < num; ++i)
//	{
//		if (flag)
//		{
//			appSec = "all";
//			node_id = Node::nodes.at((--tmp_num))->Id();
//		}
//		else
//		{
//			CGenericConfigItem<string> _node(*this, "AppNodes", l_node_it->data());
//			if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//			{
//				SetLastError(ERROR_APPSCRIPT_GET_NODES_SECTION);
//				return -4;
//			}
//			appSec = _node.MyValue();
//
//			CGenericConfigItem<int> _nodeApp(*this, appSec, "node_id");
//			node_id = _nodeApp.MyValue();
//			if (_node.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//			{
//				SetLastError(ERROR_APPSCRIPT_GET_NODE_ID);
//				return -14;
//			}
//			++l_node_it;
//
//		}
//		CGenericConfigItem<string> _appType(*this, appSec, "type");
//		string type = _appType.MyValue();
//		if (_appType.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//		{
//			SetLastError(ERROR_APPSCRIPT_GET_APP_TYPE);
//			return -5;
//		}
//
//		CGenericConfigItem<string> _appPara(*this, appSec, "para");
//		if (_appPara.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//		{
//			SetLastError(ERROR_APPSCRIPT_GET_APP_PARA);
//			return -6;
//		}
//		string paraSec = _appPara.MyValue();
//
//
//		CGenericConfigItem<string> _appSet(*this, appSec, "setting");
//		if (_appSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//		{
//			SetLastError(ERROR_APPSCRIPT_GET_APP_SET);
//			return -7;
//		}
//		string settingSec = _appSet.MyValue();
//
//		if (type == "WormUDP")
//		{
//			ApplicationWormUDP worm;
//
//			if (paraSec != "NULL")//构造函数
//			{
//
//			}	
//			bool start = false;//是否从该结点发起
//			string modelSec = "";//模型
//			Count_t range = -1;
//			Mask m = IPAddr::ToIP("255.255.255.255");
//			//vector<int> _wormStartId;
//			bool bStartRandom = false;//是否随机选择蠕虫开始传播的结点
//			int StartRandomNum = 0;//随机选择的数目
//			//bool bInitStartID = false;
//
//			if (settingSec != "NULL")
//			{
//				list<string> lset_wormudp;
//				int set_num = GetKeyNamesBySectionName(settingSec, lset_wormudp);
//				if (set_num <= 0)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_WORM_SET);
//					return -8;
//
//				}
//				list<string>::iterator lset_wormudp_it = lset_wormudp.begin();
//				while(lset_wormudp_it != lset_wormudp.end())
//				{
//					string str_tmp = lset_wormudp_it->data();
//					if(str_tmp == "model")//模型			
//					{
//						CGenericConfigItem<string> _nodeSet(*this, settingSec, lset_wormudp_it->data());	
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_PATH);
//							return -9;
//						}
//						modelSec = _nodeSet.MyValue();
//
//					}
//					else if(str_tmp == "scanrate")
//					{
//						CGenericConfigItem<int> _nodeSet(*this, settingSec, lset_wormudp_it->data());
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_SCANRATE);
//							return -15;
//						}
//						int scanrate = _nodeSet.MyValue();
//
//						worm.SetScanspersecond(scanrate);
//					}
//					else if(str_tmp == "startID")
//					{
//						CGenericConfigItem<string> _nodeSet(*this, settingSec, lset_wormudp_it->data());		
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
//							return -10;
//						}
//						string WormStartSec = _nodeSet.MyValue();
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
//							return -10;
//						}
//
//						/*
//						//获得是否随机选择开始传播的结点
//						CGenericConfigItem<bool> _wormStart(*this, WormStartSec, "startType");
//						if (_wormStart.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//						//不存在这一项，故指定开始传播结点
//						bStartRandom = false;
//						}
//						else
//						{
//						bStartRandom = _wormStart.MyValue();
//						if (!bStartRandom)//此时bStartRandom应为true
//						{
//						SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTTYPE);
//						return -30;
//						}							
//						}
//
//						if (bStartRandom)//随机选择
//						{
//						//随机开始的个数
//						CGenericConfigItem<bool> _wormStart(*this, WormStartSec, "startnum");
//						if (_wormStart.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//						//不存在这一项，故指定开始传播结点
//						bStartRandom = false;
//						}
//						StartRandomNum = _wormStart.MyValue();
//						}
//						*/
//
//						list<string> lstart_wormudp;
//						int start_num = GetKeyNamesBySectionName(WormStartSec, lstart_wormudp);
//						if (start_num <= 0)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
//							return -10;
//						}
//
//						int start_id;
//						list<string>::iterator lstart_wormudp_it = lstart_wormudp.begin();
//						while(lstart_wormudp_it != lstart_wormudp.end())
//						{
//							CGenericConfigItem<int> _nodeSet(*this, WormStartSec, lstart_wormudp_it->data());												
//							if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//							{
//								SetLastError(ERROR_APPSCRIPT_GET_WORM_STARTID);
//								return -16;
//							}
//							start_id  = _nodeSet.MyValue();
//							//_worm_startID.push_back(start_id);
//
//							if (start_id == node_id)
//							{
//								start = true;
//								break;
//							}
//							++lstart_wormudp_it;
//						}
//						//bInitStartID = true;
//					}
//					else//---增加case参数设置
//					{
//
//					}
//					++lset_wormudp_it;
//				}
//
//			}
//			if (modelSec != "")
//			{
//				CGenericConfigItem<string> _modelType(*this, modelSec, "modelType");												
//				if (_modelType.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE);
//					return -27;
//				}
//				string model_type = _modelType.MyValue();
//				if (model_type == "DynamicVulPrefScan")
//				{
//					if (range == -1 && m.NBits() == -1)
//					{
//						SetLastError(ERROR_APPSCRIPT_WORM_MODEL_PARA);
//						return -16;
//
//					}
//					if (!bInitVul)//未初始化脆弱主机分布
//					{
//						//读脆弱主机分布
//						CGenericConfigItem<string> _dPrefVulFile(*this, modelSec, "FileVulDistribute");												
//						if (_dPrefVulFile.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE);
//							return -28;
//						}
//						string file_name = _dPrefVulFile.MyValue();
//						ifstream in_f(file_name.c_str());
//						if (!in_f)
//						{
//							SetLastError(ERROR_APPSCRIPT_OPEN_DPREF_FILE);
//							return -29;
//						}
//						pVulDistribute = new vector<double>;
//						double tmpDW;
//						in_f >> tmpDW;
//						while(in_f)
//						{
//							pVulDistribute->push_back(tmpDW);
//							in_f >> tmpDW;
//						}
//						in_f.close();
//						in_f.clear();
//
//						bInitVul = true;
//					}
//
//					if(range != -1)
//					{
//						DynamicVulPrefScan model = DynamicVulPrefScan(range, pVulDistribute, *this);
//						if (model.GetLastError() != ERROR_MODEL_SUCCESS)
//						{
//							SetLastError(ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL);
//							return -17;
//						}
//						worm.SetAppModel(model);
//					}
//					else
//					{
//						DynamicVulPrefScan model = DynamicVulPrefScan(m, pVulDistribute, *this);
//						if (model.GetLastError() != ERROR_MODEL_SUCCESS)
//						{
//							SetLastError(ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL);
//							return -17;
//						}
//						worm.SetAppModel(model);
//					}
//				}
//				else
//				{
//
//				}
//			}
//
//			Node * wormnode = Node::GetNode(node_id);
//			if (wormnode == NULL)
//			{
//				SetLastError(ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID);
//				return -11;
//			}
//
//			ApplicationWormUDP* appworm = 
//				(ApplicationWormUDP*)(wormnode->AddApplication(worm));
//
//			if (start) 
//			{
//				appworm->Infect();
//				appworm->Start(0.0);
//			}
//		}
//		else if(type == "TCPServer")//应该有默认值
//		{
//			TCPServer* server;
//			if (paraSec != "NULL")
//			{
//				//---构造函数
//				//---到section为paraSec内找type类型应用构造函数的参数
//
//			}
//			else
//			{
//				Node * tcpserverNode = Node::GetNode(node_id);
//				if (tcpserverNode == NULL)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID);
//					return -12;
//				}
//				server = (TCPServer*)(tcpserverNode->AddApplication(TCPServer()));
//			}
//
//			if (settingSec != "NULL")//不是默认
//			{
//				list<string> lset_tcpserver;
//				int set_num = GetKeyNamesBySectionName(settingSec, lset_tcpserver);
//				if (set_num <= 0)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_SET);
//					return -18;
//				}
//
//				list<string>::iterator lset_tcpserver_it = lset_tcpserver.begin();
//				while(lset_tcpserver_it != lset_tcpserver.end())
//				{
//					string strtmp = lset_tcpserver_it->data();
//					if (strtmp == "BindPort")//端口号
//					{	
//						CGenericConfigItem<int> _nodeSet(*this, settingSec, lset_tcpserver_it->data());
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT);
//							return -21;
//						}
//						int port = _nodeSet.MyValue();
//						server->BindAndListen(port); // Bind to port 80			
//
//					}  
//					else//---增加case参数设置
//					{
//
//					}
//					++lset_tcpserver_it;
//				}
//			}					  
//		}
//
//		else if(type == "TCPSend")
//		{
//			int targetID = 0;
//			int port = 80;
//			int countnum = 10000;
//			//---增加参数
//			if (paraSec != "NULL")//用户自定义以上参数
//			{
//				list<string> lpara_tcpsend;
//				int para_num = GetKeyNamesBySectionName(paraSec, lpara_tcpsend);
//				if (para_num <= 0)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_PARA);
//					return -19;
//				}
//				list<string>::iterator lpara_tcpsend_it = lpara_tcpsend.begin();
//				//读参数
//				string str_tmp;
//				while(lpara_tcpsend_it != lpara_tcpsend.end())
//				{
//					str_tmp = lpara_tcpsend_it->data();
//					if(str_tmp == "BindPort")//端口号
//					{
//						CGenericConfigItem<int> _nodeSet(*this, paraSec, str_tmp);
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT);
//							return -20;
//						}
//
//						port = _nodeSet.MyValue();
//					}
//					else if(str_tmp == "targetID")
//					{
//						CGenericConfigItem<int> _nodeSet(*this, paraSec, lpara_tcpsend_it->data());
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_TARGETID);
//							return -22;
//						}
//						targetID = _nodeSet.MyValue();
//					}
//					else if(str_tmp == "countnum")
//					{
//						CGenericConfigItem<int> _nodeSet(*this, paraSec, lpara_tcpsend_it->data());
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM);
//							return -23;
//						}
//						countnum = _nodeSet.MyValue();
//					}	
//					else//---增加case参数设置
//					{
//
//					}
//					++lpara_tcpsend_it;
//				}  						  
//			}
//			Node * tcpsendNode = Node::GetNode(node_id);
//			Node * tcpsendServerNode = Node::GetNode(targetID);
//			if (tcpsendNode == NULL || tcpsendServerNode == NULL)
//			{
//				SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID);
//				return -13;
//			}
//			TCPSend*  client = (TCPSend*)(tcpsendNode->AddApplication(
//				TCPSend(tcpsendServerNode->GetIPAddr(), port, Constant(countnum))));
//
//			if (settingSec != "NULL")//不是默认
//			{
//				list<string> lset_tcpsend;
//				int set_num = GetKeyNamesBySectionName(settingSec, lset_tcpsend);
//				if (set_num <= 0)
//				{
//					SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_SET);
//					return -24;
//				}
//				list<string>::iterator lset_tcpsend_it = lset_tcpsend.begin();
//				while(lset_tcpsend_it != lset_tcpsend.end())
//				{
//					string str_tmp = lset_tcpsend_it->data();
//
//					if(str_tmp == "ISstart")
//					{
//						if (flag)//为所有结点绑定该应用，不应该有这个配置项
//						{
//							continue;
//						}
//						CGenericConfigItem<bool> _nodeSet(*this, settingSec, lset_tcpsend_it->data());
//						if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//						{
//							SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_ISSTART);
//							return -25;
//						}	
//						bool isStart = _nodeSet.MyValue();
//						if (isStart)
//						{
//							CGenericConfigItem<int> _nodeSet(*this, settingSec, "StartTime");
//							if (_nodeSet.GetLastError() == ERROR_CONFIG_ITEM_NOT_EXIST)
//							{
//								SetLastError(ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME);
//								return -26;
//							}	
//							int startTime = _nodeSet.MyValue();
//							client->Start(startTime);
//						}						
//
//						break;
//					}
//					else//---增加参数设置
//					{
//
//					}
//					++lset_tcpsend_it;
//
//
//				}
//			}			  
//
//		}
//		else//----增加应用
//		{
//		}	
//	}
//	////处理worm
//	//for (int i = 0; i < _worm_startID.size(); ++i)
//	//{
//	//	int id = _worm_startID[i];
//	//	ApplicationWormUDP *worm = (ApplicationWormUDP *)((Node::GetNode(id)->GetApplication())->at(i));
//	//	worm->Infect();
//	//	worm->Start(0.0);
//
//	//}
//	return 1;
//}




int CAppFileScript::WriteAppbyInifile()     //新取应用脚本文件
{


	return GetLastError();
}

Error_str CAppFileScript::GetLastErrorEx()
{
	switch ( GetLastError() )
	{
	case ERROR_APPSCRIPT_NODES_EMPTY:
		return "There is no node needed to attach app";
	case ERROR_APPSCRIPT_GET_NODES:
		return "Error when get nodes needed to attach app";
	case ERROR_APPSCRIPT_GET_NODES_SIZE:
		return "Error when get nodes' size, please check if topology is loaded";
	case ERROR_APPSCRIPT_GET_NODES_SECTION:
		return "Error when get node's section";
	case ERROR_APPSCRIPT_GET_APP_TYPE:
		return "Error when get app's type";
	case ERROR_APPSCRIPT_GET_APP_PARA:
		return "Error when get app's para";
	case ERROR_APPSCRIPT_GET_APP_SET:
		return "Error when get app's setting";
	case ERROR_APPSCRIPT_GET_WORM_SET:
		return "Error when get worm's setting";
	case ERROR_APPSCRIPT_GET_WORM_MODEL_PATH:
		return "Error when get worm's model path";
	case ERROR_APPSCRIPT_GET_WORM_STARTID:
		return "Error when get worm's start id";
	case ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID:
		return "Error when get node of worm by id";
	case ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID:
		return "Error when get node of tcpserver by id";
	case ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID:
		return "Error when get node of tcpsend by id";
	case ERROR_APPSCRIPT_GET_NODE_ID:
		return "Error when get node id in section";
	case ERROR_APPSCRIPT_GET_WORM_SCANRATE:
		return "Error when get worm's sacnrate";
	case ERROR_APPSCRIPT_WORM_MODEL_PARA:
		return "Error when init model:para error";
	case ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL:
		return "Error when construct worm's model"; 
	case ERROR_APPSCRIPT_GET_TCPSERVER_SET:
		return "Error when get tcpserver's setting";
	case ERROR_APPSCRIPT_GET_TCPSEND_PARA:
		return "Error when get tcpsend's para";
	case ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT:
		return "Error when get tcpsend's para:bindport";
	case ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT:
		return "Error when get tcpserver's para:bindport";
	case ERROR_APPSCRIPT_GET_TCPSEND_TARGETID:
		return "Error when get tcpsend's para:targetID";
	case ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM:
		return "Error when get tcpsend's para:countnum";
	case ERROR_APPSCRIPT_GET_TCPSEND_SET:
		return "Error when get tcpsend'set";
	case ERROR_APPSCRIPT_GET_TCPSEND_ISSTART:
		return "Error when get tcpsend's isstart";
	case ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME:
		return "Error when get tcpsend's starttime";
	case ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE:
		return "Error when get worm's model type";
	case ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE:
		return "Error when get file_name of dynamicPrefModel";
	case ERROR_APPSCRIPT_OPEN_DPREF_FILE:
		return "Error when open file for dynamicPrefModel";
	case ERROR_APPSCRIPT_GET_WORM_STARTTYPE:
		return "Error when get worm's startType";

	default:
		return "UNKNOWN";


	}


}