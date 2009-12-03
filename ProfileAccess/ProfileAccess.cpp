// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
#include "ExpressionParse.h"

using namespace WhuTNetSimConfigClass;


void fun(CFileConfig& f)
{

	CGenericConfigItem<CExpressionParse> item(f,"model","formulation2");
	
	double i=(item.MyValue()).GetExpValue();
	
	cout<<"result:"<<i<<"  "<< (item.MyValue()).GetFirstErrorEx()<<endl;

}

int _tmain(int argc, _TCHAR* argv[])
{

    CFileConfig f("1.ini.txt");

	f.LoadFile();


	fun(f);

	getchar();

	
	return 0;


}

