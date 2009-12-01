// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
#include "ExpressionParse.h"

using namespace WhuTNetSimConfigClass;


int _tmain(int argc, _TCHAR* argv[])
{
	
	map<string,double> v;

	v["a"]=100;
	v["b"]=200;

	string str="a+b*a^2";
	
	CExpressionParse expa(str,v);

	cout<<expa.GetExpValue()<<endl;
	

	getchar();

	
	return 0;


}

