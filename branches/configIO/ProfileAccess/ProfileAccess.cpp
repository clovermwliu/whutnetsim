// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
#include "ExpressionParse.h"

using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{

    CFileConfig f("1.ini.txt");

	f.LoadFile();
   
	CGenericConfigItem<CExpressionParse> item(f,"model","formulation");

	(item.MyValue()).SetParamValue("a",1);

	(item.MyValue()).SetParamValue("b",2);

	(item.MyValue()).SetParamValue("c",3);

	(item.MyValue()).SetParamValue("d",4);


	double i=(item.MyValue()).GetExpValue();

	cout<<"result is:"<<i<<"	"<<(item.MyValue()).GetFirstErrorEx()<<endl;


	getchar();

	
	return 0;


}

