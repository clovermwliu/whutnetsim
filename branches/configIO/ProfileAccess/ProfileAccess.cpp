// ProfileAccess.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	CFileConfig f("567.ini.txt");

	f.LoadFile();

	CGenericConfigItem<int> item(f,"section5","key5");

	item=567;


	f.UpdateFile(true);

	getchar();

	
	return 0;


}

