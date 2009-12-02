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

	double i=(item.MyValue()).GetExpValue();


/*
	
	map<string,double> v;

	v["a"]=100;
	v["b"]=200;

	CExpressionParse expa("a+2*a+exp(a)-(2+a+(2b-1200)+3+*a+1)-a^2",v);

	double i=expa.GetExpValue();

	cout<<i<<expa.GetFirstErrorEx()<<endl;
*/

	getchar();

	
	return 0;


}

