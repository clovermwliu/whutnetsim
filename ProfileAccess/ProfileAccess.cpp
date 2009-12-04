// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "GenericConfigItem.h"
#include "ExpressionParse.h"
#include "Test.h"

using namespace WhuTNetSimConfigClass;


void fun(CFileConfig& f)
{
	
	CGenericConfigItem<CExpressionParse> form(f,"model","formulation2");

	vector<string> v;
    
	//读取普通参数，从配置缓存赋值
	int count_i= (form.MyValue()).GetCommonParamName(v);

	for (int i=0;i<count_i;++i){

		CGenericConfigItem<double> item(f,"model",v[i]);

		(form.MyValue()).SetParamValue(v[i],item);

	}

    v.clear();

	//读取远程赋值参数，从配置缓存赋值
	int count_j= (form.MyValue()).GetRemoteParamName(v);

	for (int j=0;j<count_j;++j){

		CGenericConfigItem<string> item(f,"model",v[j]);
		
		if (item.MyValue() == "cur_time"){

			(form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue1);

		}else{

			(form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue2);
		}
		
		
	}
			
	double result=(form.MyValue()).GetExpValue();
	
	cout<<"result="<<result<<"  "<< (form.MyValue()).GetFirstErrorEx()<<endl;

}

int _tmain(int argc, _TCHAR* argv[])
{

    CFileConfig f("1.ini.txt");

	f.LoadFile();


	fun(f);

	getchar();

	
	return 0;


}

