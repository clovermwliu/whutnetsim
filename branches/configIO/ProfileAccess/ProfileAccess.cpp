// ProfileAccess.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "configfile.h"
#include "configitem.h"
#include "configitem_base.h"

using namespace configlib;


int _tmain(int argc, _TCHAR* argv[])
{
	
	configfile f("1.ini.txt");

	f.read();

	configitem<int> item(f,"section4","keyxxx","param_me",66);

//	item++;

	
	f.write();




	getchar();

	
	return 0;

	


}

