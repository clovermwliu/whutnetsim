#include "TopoFile.h"
#include "Waxman.h"
#include "PFP.h"

CTopoFile::CTopoFile(const string _file)
/*
��������ȡ�����ļ������½����ǵ�������
������[IN]_file �������ļ���·��
����ֵ����
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
���������������ļ�
��������
����ֵ����
*/
{
	file = new CFileConfig(configurePath);

	file->LoadFile();
}
bool CTopoFile::ReadAllConnectInfo(CHiberTopoBase* Hiber)
/*
��������ȡ�����ļ��е�������Ϣ�����Բ�����˵Ĳ�ͬƽ�����˽�������
������[OUT]Hiber ����ȡ��������Ϣ�������Ĳ����������
����ֵ���Ƿ�����ɹ�
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
��������ȡһ��������Ϣ���Բ�����˵Ĳ�ͬƽ�����˽�������
������[IN] connectType�� ��ȡ��������Ϣ��Section����
	  [OUT]Hiber      �� ��ȡ��������Ϣ�������Ĳ����������
���أ��Ƿ�����ɹ�
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
��������ȡһ��ƽ�����˵���Ϣ�����½�����
������[IN]_lay     ����ȡ���ǵڼ�������
      [OUT]TopoVec �������½�������
����ֵ���Ƿ��½��ɹ�
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
��������ȡ������˵�ƽ����������
��������
����ֵ��������˵�ƽ����������
*/
{
	WhuTNetSimConfigClass::CGenericConfigItem<int> item1(*file,"HiberTopo","CplatNum");
	int CplatNum=item1.MyValue();

	return CplatNum;
}
bool CTopoFile::ReadOnceWaxmanInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo)
/*
��������ȡһ��Waxman���˵���Ϣ���½�һ��Waxman����
������[IN]PlatType     ��Waxman�����������Section
	  [OUT]newPlatTopo ���½�Waxman���˵�ָ��
����ֵ���Ƿ�����ɹ�
*/
{
	//WhuTNetSimConfigClass::CGenericConfigItem<string> item5(*file,inputType,Plati);
	//string PlatType=item5.MyValue();

	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//û���õ�v�����ú��˷�
	//���PlatType��ĸ���
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
��������ȡһ��PFP���˵���Ϣ���½�һ��PFP����
������[IN]PlatType     ��PFP�����������Section
	  [OUT]newPlatTopo ���½�PFP���˵�ָ��
����ֵ���Ƿ�����ɹ�
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//û���õ�v�����ú��˷�
	//���PlatType��ĸ���
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
��������ȡһ��Star���˵���Ϣ���½�һ��Star����
������[IN]PlatType     ��Star�����������Section
	  [OUT]newPlatTopo ���½�Star���˵�ָ��
����ֵ���Ƿ�����ɹ�
*/
{
	list<string> v;
	int paraNum=file->GetKeyNamesBySectionName(PlatType,v);//û���õ�v�����ú��˷�
	//���PlatType��ĸ���
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