// ProfileAccess.cpp : 定义控制台应用程序的入口点。
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

	configitem<int> item(f,"section4","key4","param_me",66);

	if(item.parse("section4","key4")){
		item.set(128);
	}

	f.write();




	getchar();

	
	return 0;

	


}

