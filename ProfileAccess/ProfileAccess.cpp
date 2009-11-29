// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "configfile.h"
#include "configitem.h"
#include "configitem_base.h"
#include "FileConfig.h"
#include "GenericConfigItem.h"

using namespace configlib;
using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	CFileConfig f("testini.txt");
	f.LoadFile();

	CGenericConfigItem<int> item1(f,"section1","keyT","first set item1",256);

	item1.ChangeRemarkToFile("remark changed");
	
	item1.ChangeRemarkInThisItem("item1++");
	item1++;

	item1.Cancel();
	item1=5;

	CGenericConfigItem<bool> itembool(f,"section1","keybool","BOOL Value",true);

	itembool++;

	CGenericConfigItem<double> itemdw(f,"section1","key1");
	CGenericConfigItem<string> itemstr(f,"section1","key1");
	CGenericConfigItem<bool> itembl(f,"section1","key1");


/*


    int a,b;
	f.LoadFile(a,b);

	
	f.SetValue("sectionC","keyc2","girl","valuec2 changed to girl");
	f.SetValue("sectionA","xiaowang","wangxiaxin","i am guochi ");

	f.CancelConfigLine("sectionA","wangxiaxian");
	f.CancelConfigLine("sectionA","xiaowang");

	f.SetValue("section1","key1","newkey1","r1");
	f.SetValue("section1","key1","newkey2","r2");
	f.SetValue("sectionA","xiaowang","wangxiaxin","");


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

*/
	f.UpdateFile();

	getchar();

	
	return 0;


}

