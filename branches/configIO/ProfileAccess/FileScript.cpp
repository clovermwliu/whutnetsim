#include "StdAfx.h"
#include "FileScript.h"

namespace WhuTNetSimConfigClass{

CFileScript::CFileScript(const string& strFilePath)
:dwErr_code(ERROR_SCRIPT_SUCCESS),err_str(""),CFileConfig(strFilePath)
{
}

CFileScript::~CFileScript(void)
{
}


string CFileScript::GetLastErrorEx()
/*
#define  ERROR_SCRIPT_SUCCESS              0x00020000
#define  ERROR_SCRIPT_INITIAL_EXP_FAIL     0x00020001
#define  ERROR_SCRIPT_EXP_NOT_FOUND        0x00020002
#define  ERROR_SCRIPT_INITIAL_PRED_FAIL    0x00020003
#define  ERROR_SCRIPT_PRED_NOT_FOUND       0x00020004
#define  ERROR_SCRIPT_INITIAL_COND_FAIL    0x00020005
#define  ERROR_SCRIPT_COND_NOT_FOUND       0x00020006
#define  ERROR_SCRIPT_CONDEXP_INVALID      0x00020007
*/
{
	switch (dwErr_code)
	{
	case ERROR_SCRIPT_SUCCESS:
		return "Parse script successfully";
	case ERROR_SCRIPT_INITIAL_EXP_FAIL:
		return "Parse expression fail:"+err_str;
	case ERROR_SCRIPT_EXP_NOT_FOUND:
		return "Expression is not found:"+err_str;
	case ERROR_SCRIPT_INITIAL_PRED_FAIL:
		return "Parse predication item fail:"+err_str;
	case ERROR_SCRIPT_PRED_NOT_FOUND:
		return "Predication item is not found:"+err_str;
	case ERROR_SCRIPT_INITIAL_COND_FAIL:
		return "Parse condition expression fail:"+err_str;
	case ERROR_SCRIPT_COND_NOT_FOUND:
		return "Condition expression is not found:"+err_str;
	case ERROR_SCRIPT_CONDEXP_INVALID:
		return "Condition expression may not be right:"+err_str;

	default:
		return "Unknown";
	}

}


bool
CFileScript::InitExpressionBySectionName(const string& expsection,
					         			 CExpressionParse& exp,
                                         const string& expform)
/*
描述：根据一个表达式节的内容，初始化一个表达式
参数：expsection 表达式节名
      expform 表达式公式所在键名，默认为"formulation"
*/
{
 CGenericConfigItem<CExpressionParse> form(*this,expsection,expform);

 if (form.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

	 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
	 string tmp=expsection;
	 tmp=tmp+":";
	 tmp=tmp+expform;
	 SetLastErrorStr(tmp);
	 return false;
 }

 vector<string> v;

 //读取普通参数，从配置缓存赋值
 int count_i= (form.MyValue()).GetCommonParamName(v);

 for (int i=0;i<count_i;++i){

	 CGenericConfigItem<double> item(*this,expsection,v[i]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 //cout<<"parameter "<<v[i]<<" does not exist"<<endl;

		 SetLastError(ERROR_SCRIPT_EXP_NOT_FOUND);
		 string tmp=expsection;
		 tmp=tmp+":[Parameter]";
		 tmp=tmp+v[i];
		 SetLastErrorStr(tmp);
     	 return false;
	 }

	 if (!(form.MyValue()).SetParamValue(v[i],item)){

		 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
		 SetLastErrorStr(expsection);
	 }


 }

 v.clear();

 //读取远程赋值参数，从配置缓存赋值
 int count_j= (form.MyValue()).GetRemoteParamName(v);

 for (int j=0;j<count_j;++j){

	 CGenericConfigItem<string> item(*this,expsection,v[j]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 //cout<<"parameter "<<v[j]<<" does not exist"<<endl;

		 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
		 string tmp=expsection;
		 tmp=tmp+":[Remote call Parameter]";
		 tmp=tmp+v[j];
		 SetLastErrorStr(tmp);
		 return false;

	 }

	 //给远程调用函数挂钩

	 if (item.MyValue() == "cur_time"){

		 (form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue1);

	 }else{

		 (form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue2);
	 }
 }

 exp=form.MyValue();
 return true;

}


bool
CFileScript::InitPredicationItemBySectionName(const string& pred_item_section,
									          CPredicationItem& pred,
											  const string& keyleft,
											  const string& keyright,
											  const string& keyop)
/*

*/
{
	CGenericConfigItem<string> l(*this,pred_item_section,keyleft);

	if (l.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_PRED_NOT_FOUND);
		string tmp=pred_item_section;
		tmp=tmp+":";
		tmp=tmp+keyleft;
		SetLastErrorStr(tmp);
		return false;
	}

	CGenericConfigItem<string> r(*this,pred_item_section,keyright);

	if (r.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_PRED_NOT_FOUND);
		string tmp=pred_item_section;
		tmp=tmp+":";
		tmp=tmp+keyright;
		SetLastErrorStr(tmp);
		return false;
	}

	CGenericConfigItem<string> op(*this,pred_item_section, keyop);

	if (op.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_PRED_NOT_FOUND);
		string tmp=pred_item_section;
		tmp=tmp+":";
		tmp=tmp+keyop;
		SetLastErrorStr(tmp);
		return false;
	}

	 CExpressionParse exp_tmp;
	
	if (!InitExpressionBySectionName(l.MyValue(),exp_tmp)){

		return false;
	}

	pred.SetExp(true,exp_tmp);

	exp_tmp.clear();

	if (!InitExpressionBySectionName(r.MyValue(),exp_tmp)){

		return false;
	}

	pred.SetExp(false,exp_tmp);

	pred.SetOp(op.MyValue());

	return true;

}


bool 
CFileScript::InitConditionBySectionName(const string& condsection,
						         		CExpCondition& cond,
								        const string& condkey)
/*
描述：根据脚本节名设置一个CExpCondition的对象
参数：[out]cond 待设置的CExpCondition对象
*/
{
	CGenericConfigItem<string> stritem(*this,condsection,condkey);
	if (stritem.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_COND_NOT_FOUND);
		string tmp=condsection;
		tmp=tmp+":";
		tmp=tmp+condkey;
		SetLastErrorStr(tmp);
		return false;
	}
    
	map<string,CPredicationItem> table;
	string str_pred_identify;
	string str=stritem.MyValue();
	string::iterator iter=str.begin();

	bool result=true;

	while(iter!=str.end()){

		if( isspace( *iter)) { //当前字符是是空格、制表符或换行符，继续
			++iter;               
			continue;

		}else if(*iter == '&' && *(iter+1)=='&'){

			++iter ;
			++iter ;

		}else if(*iter == '|' && *(iter+1)=='|'){

			++iter ;
			++iter ;

		}else if(*iter =='!' ){

			++iter ;

		}else if (*iter =='(' || *iter ==')'){

			++iter;		
		
		}else if( isalpha( *iter) || isdigit(  *iter)){

			str_pred_identify.clear();

			do {
				str_pred_identify += (*iter);
				++iter;
			} while(iter!=str.end() && (isalpha( *iter ) || isdigit(  *iter ))); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束
			
			//开始构造

			CPredicationItem preditem;

			result=InitPredicationItemBySectionName(str_pred_identify,preditem) && result;

			table.insert(pair<string, CPredicationItem>(str_pred_identify,preditem));


		}else{

			result=false;
			SetLastError(ERROR_SCRIPT_CONDEXP_INVALID);
			SetLastErrorStr(condsection);
			++iter;
		}

	}

	cond.Initial(str,table);
	return result;

}



}//end
