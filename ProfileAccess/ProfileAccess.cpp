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


	f.LoadFile();

	CFileConfig::iterator it=f.begin();

	
	f.UpdateFile();



	getchar();

	
	return 0;

	


}

