// ProfileAccess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "FileConfig.h"
#include "FileScript.h"
#include "GenericConfigItem.h"
#include "ExpressionParse.h"
#include "PredicationItem.h"
#include "ExpCondition.h"
#include "ElementCustom.h"
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

		if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

			cout<<"parameter "<<v[i]<<" does not exist"<<endl;

		}

		(form.MyValue()).SetParamValue(v[i],item);

	}

    v.clear();

	//读取远程赋值参数，从配置缓存赋值
	int count_j= (form.MyValue()).GetRemoteParamName(v);

	for (int j=0;j<count_j;++j){

		CGenericConfigItem<string> item(f,"model",v[j]);

		if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

			cout<<"parameter "<<v[j]<<" does not exist"<<endl;

		}
		
		if (item.MyValue() == "cur_time"){

			(form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue1);

		}else{

			(form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue2);
		}
		
		
	}
			
	double result=(form.MyValue()).GetExpValue();
	
	cout<<"result="<<result<<"  "<< (form.MyValue()).GetFirstErrorEx()<<endl;

}


void fun2(CFileScript& f)
{


	CExpressionParse e;
	if(!f.InitExpressionBySectionName("std_expression",e)) return;

	double result=e.GetExpValue();

	double result2=e.GetExpValue();

	cout<<"result="<<result<<"  "<< e.GetFirstErrorEx()<<endl;

	cout<<"result2="<<result2<<"  "<< e.GetFirstErrorEx()<<endl;

}

void fun3(CFileScript& f)
{

    CPredicationItem pred;
    if(!f. InitPredicationItemBySectionName("std_predication_item",pred)) return;

	bool result=pred.GetValue();

	bool result2=pred.GetValue();

	cout<<"result="<<result<<"  "<< pred.GetFirstErrorEx()<<endl;

	cout<<"result2="<<result2<<"  "<< pred.GetFirstErrorEx()<<endl;

}


void fun4(CFileScript& f)
{

	CExpCondition cond; 
	
	if(!f. InitConditionBySectionName("std_Condition",cond)){

		cout<<f.GetLastError()<<":"<<f.GetLastErrorEx()<<"\n"<<endl;
		//return;
	}

	bool result=cond.GetConditionValue();

	bool result2=cond.GetConditionValue();

	cout<<"result="<<result<<"  "<< cond.GetFirstErrorEx()<<endl;

	cout<<"result2="<<result2<<"  "<<cond.GetFirstErrorEx()<<endl;

}

void fun5(CFileScript& f)
{

	CElementCustom ce2;
	CElementCustom ce=ce2;
	double result0=ce.GetValue();
	cout<<"result0="<<result0<<"  "<< ce.GetFirstErrorEx()<<endl;


	
	CExpCondition cond2; 

	CExpCondition cond=cond2;

	bool result=cond.GetConditionValue();
	cout<<"result="<<result<<"  "<< cond.GetFirstErrorEx()<<endl;

    CPredicationItem pred;
    bool result2=pred.GetValue();
    cout<<"result2="<<result2<<"  "<< pred.GetFirstErrorEx()<<endl;

	CExpressionParse e;
	double result3=e.GetExpValue();
    cout<<"result3="<<result3<<"  "<< e.GetFirstErrorEx()<<endl;

}

void fun6(CFileScript& f)
{
	CElementCustom ce;

	if(!f. InitCustomElementBySectionName("std_custom_element",ce)){

		cout<<f.GetLastError()<<":"<<f.GetLastErrorEx()<<"\n"<<endl;
		//return;
	}

	double result=ce.GetValue();

	double result2=ce.GetValue();

	cout<<"result="<<result<<"  "<< ce.GetFirstErrorEx()<<endl;

	cout<<"result2="<<result2<<"  "<< ce.GetFirstErrorEx()<<endl;

}

int _tmain(int argc, _TCHAR* argv[])
{

	CFileScript f("1.ini.txt");

	if (!f.LoadFile()){

		cout<<f.GetLastError()<<":"<<f.GetLastErrorEx()<<"\n"<<endl;
	
	}else{
		
		fun6(f);

	}

	getchar();

	
	return 0;


}

