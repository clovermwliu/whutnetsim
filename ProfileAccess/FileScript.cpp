#include "StdAfx.h"
#include "FileScript.h"

CFileScript::CFileScript(const string& strFilePath)
:CFileConfig(strFilePath)
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
#define  ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND       0x00020008
#define  ERROR_SCRIPT_CUNSTOM_ELEMENT_INVALID         0x00020009
*/
{
	switch (GetLastError())
	{
	case ERROR_SCRIPT_SUCCESS:
		return "Parse script successfully";
	case ERROR_SCRIPT_INITIAL_EXP_FAIL:
		return "Parse expression fail:"+GetLastErrorStr();
	case ERROR_SCRIPT_EXP_NOT_FOUND:
		return "Expression is not found:"+GetLastErrorStr();
	case ERROR_SCRIPT_INITIAL_PRED_FAIL:
		return "Parse predication item fail:"+GetLastErrorStr();
	case ERROR_SCRIPT_PRED_NOT_FOUND:
		return "Predication item is not found:"+GetLastErrorStr();
	case ERROR_SCRIPT_INITIAL_COND_FAIL:
		return "Parse condition expression fail:"+GetLastErrorStr();
	case ERROR_SCRIPT_COND_NOT_FOUND:
		return "Condition expression is not found:"+GetLastErrorStr();
	case ERROR_SCRIPT_CONDEXP_INVALID:
		return "Condition expression may not be right:"+GetLastErrorStr();
	case ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND:
		return "Custom element is not found:"+GetLastErrorStr();
	case ERROR_SCRIPT_CUNSTOM_ELEMENT_INVALID:
		return "Custom element is invalid:"+GetLastErrorStr();

	default:
		return CFileConfig::GetLastErrorEx();
	}

}


bool
CFileScript::InitExpressionBySectionName(const string& exp_section,
					         			 CExpressionParse& exp,
                                         const string& expform)
/*
描述：根据一个表达式节的内容，初始化一个表达式
参数：expsection 表达式节名
      expform 表达式公式所在键名，默认为"formulation"
*/
{
 CGenericConfigItem<CExpressionParse> form(*this,exp_section,expform);

 if (form.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

	 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
	 string tmp=exp_section;
	 tmp=tmp+":";
	 tmp=tmp+expform;
	 SetLastErrorStr(tmp);
	 return false;
 }

 vector<string> v;

 //读取普通参数，从配置缓存赋值
 int count_i= (form.MyValue()).GetCommonParamName(v);

 for (int i=0;i<count_i;++i){

	 CGenericConfigItem<double> item(*this,exp_section,v[i]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 //cout<<"parameter "<<v[i]<<" does not exist"<<endl;

		 SetLastError(ERROR_SCRIPT_EXP_NOT_FOUND);
		 string tmp=exp_section;
		 tmp=tmp+":[Parameter]";
		 tmp=tmp+v[i];
		 SetLastErrorStr(tmp);
     	 return false;
	 }

	 if (!(form.MyValue()).SetParamValue(v[i],item)){

		 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
		 SetLastErrorStr(exp_section);
	 }


 }

 v.clear();

 //读取远程赋值参数，从配置缓存赋值
 int count_j= (form.MyValue()).GetRemoteParamName(v);

 for (int j=0;j<count_j;++j){

	 CGenericConfigItem<string> item(*this,exp_section,v[j]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 //cout<<"parameter "<<v[j]<<" does not exist"<<endl;

		 SetLastError(ERROR_SCRIPT_INITIAL_EXP_FAIL);
		 string tmp=exp_section;
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
CFileScript::InitConditionBySectionName(const string& cond_section,
						         		CExpCondition& cond,
										int id,
								        const string& condkey)
/*
描述：根据脚本节名设置一个CExpCondition的对象
参数：[out]cond 待设置的CExpCondition对象
*/
{
	CGenericConfigItem<string> stritem(*this,cond_section,condkey);
	if (stritem.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_COND_NOT_FOUND);
		string tmp=cond_section;
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

		}else if(*iter == '&' && iter!=str.end()-1 && *(iter+1)=='&'){

			++iter ;
			++iter ;

		}else if(*iter == '|' && iter!=str.end()-1 && *(iter+1)=='|'){

			++iter ;
			++iter ;

		}else if(*iter =='!' ){

			++iter ;

		}else if (*iter =='(' || *iter ==')'){

			++iter;		
		
		}else if( isalpha( *iter) || isdigit(  *iter)||isValidSymbol(*iter)){

			str_pred_identify.clear();

			do {
				str_pred_identify += (*iter);
				++iter;
			} while(iter!=str.end() && (isalpha( *iter ) || isdigit(  *iter )||isValidSymbol(*iter))); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束
			
			//开始构造

			CPredicationItem preditem;

			result=InitPredicationItemBySectionName(str_pred_identify,preditem) && result;

			table.insert(pair<string, CPredicationItem>(str_pred_identify,preditem));


		}else{

			result=false;
			SetLastError(ERROR_SCRIPT_CONDEXP_INVALID);
			SetLastErrorStr(cond_section);
			++iter;
		}

	}

	cond.Initial(str,table,id);
	return result;

}


bool 
CFileScript::InitCustomElementBySectionName(const string& cust_ele_section,
			      						    CElementCustom& cust_ele,
									        const string& keyoption,
									        const string& keydefault)
/*

*/
{
	CGenericConfigItem<int> opnum(*this,cust_ele_section,keyoption);

	if (opnum.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND);
		string tmp=cust_ele_section;
		tmp=tmp+":";
		tmp=tmp+keyoption;
		SetLastErrorStr(tmp);
		return false;
	}

	CGenericConfigItem<double> default_value(*this,cust_ele_section,keydefault);

	if (default_value.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		SetLastError(ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND);
		string tmp=cust_ele_section;
		tmp=tmp+":";
		tmp=tmp+keydefault;
		SetLastErrorStr(tmp);
		return false;
	}

	int i=1;
	map<CExpCondition,CExpressionParse> t;
	bool result=true;


	while (i<=opnum.MyValue()){

	
		stringstream stream_value(std::stringstream::out);
		stream_value << i;
		string keytmp=stream_value.str();
		keytmp=keyoption+keytmp;


		CGenericConfigItem<string> stritem(*this,cust_ele_section,keytmp);

		if (stritem.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

			SetLastError(ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND);
			string tmp=cust_ele_section;
			tmp=tmp+":";
			tmp=tmp+keytmp;
			SetLastErrorStr(tmp);
			return false;
		}

		string str=stritem.MyValue();
		string tmpif, tmpthen;
		bool bIF;

		string::iterator iter=str.begin();
		while (iter!=str.end()){

			if( isspace( *iter)) { //当前字符是是空格、制表符或换行符，继续
				++iter;               
			}else if(*iter == 'I' && iter!=str.end()-1 && *(iter+1)=='F'){
				++iter ;
				++iter ;
				bIF=true;
			}else if (*iter == 'T' && iter<str.end()-3 && *(iter+1)=='H' && *(iter+2)=='E' && *(iter+3)=='N'){
				++iter ;
				++iter ;
				++iter ;
				++iter ;
				bIF=false;
			}else if (*iter =='{' || *iter =='}'){
				++iter;
			}else{
				
				if (bIF){

					do {
						tmpif += (*iter);
						++iter;
					} while(iter!=str.end() && *iter!='}'); 
			
				}else{

					do {
						tmpthen += (*iter);
						++iter;
					} while(iter!=str.end() && *iter!='}'); 

				}
			}
		}

		if (tmpif.empty()||tmpthen.empty()){

			SetLastError(ERROR_SCRIPT_CUNSTOM_ELEMENT_INVALID);
			string tmp=cust_ele_section;
			tmp=tmp+":";
			tmp=tmp+keytmp;
			SetLastErrorStr(tmp);
			return false;
		}

		//开始构造

		CExpCondition c;
		CExpressionParse e;

		result=InitConditionBySectionName(tmpif,c,i) && result;
		result=InitExpressionBySectionName(tmpthen,e) && result;

		t.insert(pair<CExpCondition, CExpressionParse>(c,e));


		//
		i++;
	}

	cust_ele.initail(t,default_value.MyValue());
	
	return result;

}



bool CFileScript::isValidSymbol(const char c)
/*
描述：脚本允许的符号
#define CHAR_UNDERLINE        '_'
#define CHAR_INVERTED_COMMA   '\''
#define CHAR_INVERTED_COMMA2  '\"'
*/
{
	switch (c){

		case CHAR_UNDERLINE: return true;
		case CHAR_INVERTED_COMMA: return true;
		case CHAR_INVERTED_COMMA2: return true;

		default:return false;
		
	}

}



//end
