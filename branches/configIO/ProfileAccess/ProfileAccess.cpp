// ProfileAccess.cpp : �������̨Ӧ�ó������ڵ㡣
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
   
	/*CGenericConfigItem<CExpressionParse> item(f,"model","formulation");

	(item.MyValue()).SetParamValue("a",1);

	(item.MyValue()).SetParamValue("b",2);

	(item.MyValue()).SetParamValue("c",3);

	(item.MyValue()).SetParamValue("d",4);


	double i=(item.MyValue()).GetExpValue();

	cout<<"result is:"<<i<<"	"<<(item.MyValue()).GetFirstErrorEx()<<endl;*/

	CGenericConfigItem<CExpressionParse> item2(f,"model","formulation2");


	double i=(item2.MyValue()).GetExpValue();

	cout<<"result is:"<<i<<"	"<<(item2.MyValue()).GetFirstErrorEx()<<endl;


	getchar();

	
	return 0;


}

