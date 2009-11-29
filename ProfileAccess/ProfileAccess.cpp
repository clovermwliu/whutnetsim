// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	CFileConfig f("testini.txt");

	f.LoadFile();

	CGenericConfigItem<int> item1(f,"section1","key1","remark on this item",256);

	string str=item1.GetSection();//str="section1"
	str=item1.GetKey();           //str="key1"
	str=item1.GetRemark();        //str="remark on this item"
	int i=item1.MyValue();        //i=256


	f.UpdateFile(false);

	getchar();

	
	return 0;


}

