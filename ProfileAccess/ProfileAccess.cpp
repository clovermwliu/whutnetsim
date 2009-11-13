// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "configfile.h"
#include "configitem.h"

using namespace configlib;


int _tmain(int argc, _TCHAR* argv[])
{
	
	configfile f("c:\\1.ini.txt");

	f.read();

	getchar();

	
	return 0;

	


}

