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
	
	CFileConfig f("2.ini.txt");


	f.LoadFile();

	string section,key,value;

	CFileConfig::iterator it,first,last;

	
    int i=0;
	string str;

	first=f.begin();
	
	str=first.GetCurSection();

	str=*first;



	for (it=f.begin();it!=f.end();++it)
	{

		section=it.GetCurSection();
		key=it.GetCurKey();
		value=it.GetCurValue();

		str=*it;

		i++;
	}

	f.UpdateFile();



	getchar();

	
	return 0;


}

