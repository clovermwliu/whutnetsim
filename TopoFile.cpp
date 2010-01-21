#include "TopoFile.h"
#include "Waxman.h"
#include "PFP.h"

CTopoFile::CTopoFile(const string _file)
/*
描述：读取配置文件，来新建我们的拓扑类
参数：[IN]_file ：配置文件的路径
返回值：无
*/
{
	configurePath = _file;
	InitFile();
}

CTopoFile::~CTopoFile(void)
{
}
void CTopoFile::InitFile()
/*
描述：加载配置文件
参数：无
返回值：无
*/
{
	file = new CFileConfig(configurePath);

	file->LoadFile();
}
bool CTopoFile::ReadAllConnectInfo(CHiberTopoBase* Hiber)
/*
描述：读取配置文件中的连接信息，来对层次拓扑的不同平面拓扑进行连接
参数：[OUT]Hiber ：读取的连接信息所操作的层次拓扑名称
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName("Connect",v);
	for (list<string>::iterator iter = v.begin();iter!=v.end();iter++)
	{
		WhuTNetSimConfigClass::CGenericConfigItem<string> item1(*file,"Connect",*iter);
		string connectType=item1.MyValue();

		if (!ReadOneConnectInfo(Hiber,connectType))
		{
			return false;
		}
	}
	return true;
}
bool CTopoFile::ReadOneConnectInfo(CHiberTopoBase* Hiber,string connectType)
/*
描述：读取一个连接信息，对层次拓扑的不同平面拓扑进行连接
参数：[IN] connectType： 读取的连接信息的Section名称
	  [OUT]Hiber      ： 读取的连接信息所操作的层次拓扑名称
返回：是否操作成功
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(connectType,v);
	vector<Count_t> connect;
	for (list<string>::iterator iter = v.begin();iter!=v.end();iter++)
	{
		WhuTNetSimConfigClass::CGenericConfigItem<Count_t> item1(*file,connectType,*iter);
		Count_t connectPara=item1.MyValue();
        connect.push_back(connectPara);
	}
	if (connect.size()==6)
	{
		Hiber->ConnectDomain(connect[0],connect[1],connect[2],
							 connect[3],connect[4],connect[5]);
		return true;
	}
	else {return false;}
}
bool CTopoFile::ReadOneLayerInfo(int _lay,vector<CPlatTopoBase*>&  TopoVec)
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
	WhuTNetSimConfigClass::CGenericConfigItem<string> item2(*file,"HiberTopo",lay);
	string inputType=item2.MyValue();

	WhuTNetSimConfigClass::CGenericConfigItem<int> item3(*file,inputType,"Groups");
	int Groups=item3.MyValue();

	for (int i=0;i<Groups;i++)
	{
		string help;
		stringstream ss;
		ss<<i + 1;
		ss>>help;
		string Plati = "Plat"+help;

		string PlatiNum = Plati + "Num";
		WhuTNetSimConfigClass::CGenericConfigItem<int> item4(*file,inputType,PlatiNum);
		int Plat1Num=item4.MyValue();
		
		WhuTNetSimConfigClass::CGenericConfigItem<string> item5(*file,inputType,Plati);
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
int CTopoFile::ReadTopoNum()
/*
描述：读取层次拓扑的平面拓扑数量
参数：无
返回值：层次拓扑的平面拓扑数量
*/
{
	WhuTNetSimConfigClass::CGenericConfigItem<int> item1(*file,"HiberTopo","CplatNum");
	int CplatNum=item1.MyValue();

	return CplatNum;
}
bool CTopoFile::ReadOnceWaxmanInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
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
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
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
		WhuTNetSimConfigClass::CGenericConfigItem<Count_t> item2(*file,PlatType,"count");
		Count_t count = item2.MyValue();

		WhuTNetSimConfigClass::CGenericConfigItem<double> item3(*file,PlatType,"alpha");
		double _alpha = item3.MyValue();

		WhuTNetSimConfigClass::CGenericConfigItem<double> item4(*file,PlatType,"beta");
		double _beta = item4.MyValue();
        
		newPlatTopo = new CWaxman(count,_alpha,_beta);
		newPlatTopo->GenerateTopo();
		break;

	//case 4:break;
	//default:break;

	}
	return true;
}
bool CTopoFile::ReadOncePFPInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
描述：读取一个PFP拓扑的信息，新建一个PFP拓扑
参数：[IN]PlatType     ：PFP的输入参数的Section
	  [OUT]newPlatTopo ：新建PFP拓扑的指针
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
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
		WhuTNetSimConfigClass::CGenericConfigItem<int> item1(*file,PlatType,"count");
		int count=item1.MyValue();

		WhuTNetSimConfigClass::CGenericConfigItem<Count_t> item2(*file,PlatType,"alpha");
		Count_t alpha = item2.MyValue();

		WhuTNetSimConfigClass::CGenericConfigItem<double> item3(*file,PlatType,"p");
		double p = item3.MyValue();

		WhuTNetSimConfigClass::CGenericConfigItem<double> item4(*file,PlatType,"q");
		double q = item4.MyValue();

		newPlatTopo = new CPFP(count,alpha,p,q);
		newPlatTopo->GenerateTopo();
		break;

	//case 5:break;
	//default:break;

	}
	return true;
}

bool CTopoFile::ReadOnceStarInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
描述：读取一个Star拓扑的信息，新建一个Star拓扑
参数：[IN]PlatType     ：Star的输入参数的Section
	  [OUT]newPlatTopo ：新建Star拓扑的指针
返回值：是否操作成功
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//没有用到v，觉得很浪费
	//获得PlatType域的个数
    //Star1(Count_t, IPAddr_t = IPADDR_NONE, SystemId_t = 0);// Number of nodes
	switch(paraNum)
	{
	case 0:
		return false;
		break;
	case 1:
		WhuTNetSimConfigClass::CGenericConfigItem<int> item1(*file,PlatType,"count");
		int count=item1.MyValue();

		newPlatTopo = new Star1(count);
		newPlatTopo->GenerateTopo();
		break;

		//case 2:break;
		//default:break;
	}

	return true;
}