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
	item2.Cancel();

	item2++;

	item2.Cancel();

	CGenericConfigItem<int> item4(f,"section2","key3","remark on this item",11111111);

	item2++;

	item2.Cancel();



	CGenericConfigItem<bool> item3(f,"section1","key3","remark on this item",true);

	for (CFileConfig::iterator it=f.begin();it!=f.end();it=it.GotoNextSection())
	{
		cout<<*it<<endl;
	}


    CFileConfig::iterator it2=f.begin();
	for (int i=0;i<f.GetSectionNum();i++)
	{
		cout<<*it2<<endl;
		it2=it2.GotoNextSection();
	}


	f.UpdateFile(true);

	getchar();

	
	return 0;


}

