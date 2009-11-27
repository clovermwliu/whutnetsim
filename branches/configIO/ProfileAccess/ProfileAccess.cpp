// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "configfile.h"
#include "configitem.h"
#include "configitem_base.h"
#include "FileConfig.h"

using namespace configlib;
using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	CFileConfig f("1.ini.txt");

    int a,b;
	f.LoadFile(a,b);

	string section,key,value;

	CFileConfig::iterator it,first,last;

	
    int i=0;
	string str;

	for (it=f.begin();it!=f.end();++it){

		str=*it;

		cout<<"current item:"<<str<<endl;

		i++;
	}

	cout<<"i="<<i<<std::endl;

	i=0;

    first=f.begin();

	last=f.end();


	for (it=f.end();it!=f.begin();--it)
	{

		str=*it;

		cout<<"current item:"<<str<<endl;

		i++;
	}

	cout<<"i="<<i<<std::endl;

	i=0;

	for (it=f.begin();it!=f.end();it=it.GotoNextSection()){


		str=*it;

		cout<<"current item:"<<str<<endl;

		i++;
	}
	cout<<"i="<<i<<std::endl;


	f.UpdateFile();



	getchar();

	
	return 0;


}

