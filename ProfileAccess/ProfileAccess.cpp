// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	CFileConfig f("testini.txt");

	//f.LoadFile();

	CGenericConfigItem<string> item1(f,"section1","key1","remark on this item","This is a dog");
	
	CGenericConfigItem<int> item2(f,"section2","key2","remark on this item",234);

	CGenericConfigItem<bool> item3(f,"section1","key3","remark on this item",true);

	CGenericConfigItem<double> item4(f,"sectionX","keyX");

	CGenericConfigItem<double> item5(f,"section1","key3");

	item4++;

	f.UpdateFile(true);

	getchar();

	
	return 0;


}

