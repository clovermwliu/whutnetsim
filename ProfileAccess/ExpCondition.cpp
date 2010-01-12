#include "StdAfx.h"
#include "ExpCondition.h"

namespace WhuTNetSimConfigClass{

CExpCondition::CExpCondition(void)
:Cur_Element_Species(BEGININI),Error_code(ERROR_CONDITION_SUCCESS),
 Str_Cur_PredicationItem(""),bCur_Value(false),str_error_exp(""),err_str("")
{
}


CExpCondition::CExpCondition(string condstr,
			                 const map<string,CPredicationItem>& t)

:str_conditon(condstr),pred_table(t),Cur_Element_Species(BEGININI),Error_code(ERROR_CONDITION_SUCCESS),
 pCurrent_Char(str_conditon.c_str()),Str_Cur_PredicationItem(""),bCur_Value(false),str_error_exp(""),err_str("")
{
	ParseElementThenGotoNext();
}

CExpCondition::~CExpCondition(void)
{
}


void CExpCondition::Initial(string condstr,
			                const map<string,CPredicationItem>& t)
/*
*/
{
	str_conditon=condstr;
	pred_table=t;
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
}


CExpCondition::CExpCondition(const CExpCondition& c)
/*

*/
{
	str_conditon=c.str_conditon;
	pred_table=c.pred_table;
	Cur_Element_Species=c.Cur_Element_Species;
	Error_code=c.Error_code;
	Str_Cur_PredicationItem=c.Str_Cur_PredicationItem;
	bCur_Value=c.bCur_Value;
	str_error_exp=c.str_error_exp;
	err_str=c.err_str;
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
}

CExpCondition& CExpCondition::operator=(const CExpCondition& rhs)
/*
由于使用pCurrent_Char指针，表达式对象复制需要重定位该指针 
*/
{
	str_conditon=rhs.str_conditon;
	pred_table=rhs.pred_table;
	Cur_Element_Species=rhs.Cur_Element_Species;
	Error_code=rhs.Error_code;
	Str_Cur_PredicationItem=rhs.Str_Cur_PredicationItem;
	bCur_Value=rhs.bCur_Value;
	str_error_exp=rhs.str_error_exp;
	err_str=rhs.err_str;
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
	return *this;
}

void CExpCondition::ParseElementThenGotoNext()
//分析当前元素的属性,同时使pCurrent_Char指向下一个元素的首字符
{
	if (!pCurrent_Char){
		if (Error_code==ERROR_CONDITION_SUCCESS)
			SetFirstError(ERROR_CONDITION_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (Error_code==ERROR_CONDITION_SUCCESS){
				SetFirstError(ERROR_CONDITION_PRED_FORMAT_INVALID);
				//SetErrorStr(pCurrent_Char);
			}
			++pCurrent_Char;
			break;

		}
		
		if( isspace( *pCurrent_Char)) {           //当前字符是是空格、制表符或换行符，继续
			++pCurrent_Char;               
			continue;

		}else if(*pCurrent_Char ==AND && *(++pCurrent_Char)==AND){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符&&
			++pCurrent_Char ;

		
		}else if(*pCurrent_Char ==OR && *(++pCurrent_Char)==OR){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符||
			++pCurrent_Char ;

		}else if(*pCurrent_Char ==NOT ){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符!
			++pCurrent_Char ;

		}else if (*pCurrent_Char ==LEFT_BRACKET || *pCurrent_Char ==RIGHT_BRACKET){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符!
			++pCurrent_Char ;


		}else if( isalpha( *pCurrent_Char ) || isdigit(  *pCurrent_Char )){

			Cur_Element_Species = PREDICATION_ITEM;          
			Str_Cur_PredicationItem.clear();

			do {
				Str_Cur_PredicationItem += (*pCurrent_Char);
				++pCurrent_Char;

				if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

					if (Error_code==ERROR_CONDITION_SUCCESS){
						SetFirstError(ERROR_CONDITION_PRED_FORMAT_INVALID);
						SetErrorStr(pCurrent_Char);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //遇到字符串结束符则结束

		}else{

			if (Error_code==ERROR_CONDITION_SUCCESS){
				SetFirstError(ERROR_CONDITION_OPERATOR_INVALID);
				SetErrorStr(pCurrent_Char-1);
			}
			++pCurrent_Char;
		}
		
		break;
	}


}

string  CExpCondition::GetFirstErrorEx()
/*
描述：取表达式计算错误的第一个触发原因，返回其原因文本解释

#define  ERROR_CONDITION_SUCCESS			          0x00001000
#define  ERROR_CONDITION_NO_EXP                       0x00001001
#define  ERROR_CONDITION_MISSING_OPERATOR             0x00001002
#define  ERROR_CONDITION_OPERATOR_INVALID             0x00001003
#define  ERROR_CONDITION_PRED_FORMAT_INVALID          0x00001004
#define  ERROR_CONDITION_MISSING_RIGHT_BRACKET        0x00001005
#define  ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST       0x00001006

*/
{
	switch (Error_code)
	{
	case ERROR_CONDITION_SUCCESS:
		return "SUCCESS";
	case ERROR_CONDITION_NO_EXP:
		return "No expression in this object";
	case ERROR_EXP_MISSING_OPERATOR:
		return "Missing operator.  Near by "+str_error_exp;
	case ERROR_CONDITION_PRED_FORMAT_INVALID:
		return "PRED's format error.  Near by "+str_error_exp;
	case  ERROR_CONDITION_OPERATOR_INVALID:
		return "Operator invalid. Near by "+str_error_exp;
	case ERROR_CONDITION_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by "+str_error_exp;
	case ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST:
		return "Condition Expression includes invalid predication identify.  Near by "+str_error_exp;
	default:
		return err_str;
	}

}


bool CExpCondition::GetConditionValue()

//取条件表达式真值，是一个递归函数的入口
{
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
	Error_code=ERROR_CONDITION_SUCCESS;
	str_error_exp.clear();
	err_str.clear();

	bool bresult = GetCondExpValueByOR( GetCondExpValueFromSubRight() );

	if (Cur_Element_Species != FINISHED && Error_code==ERROR_CONDITION_SUCCESS){

		SetFirstError(ERROR_CONDITION_MISSING_OPERATOR);
		SetErrorStr(pCurrent_Char-1);
	}

	return bresult;

}

bool CExpCondition::GetSubCondExpValue()
/*
*/
{
	if (Error_code==ERROR_CONDITION_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
	bool bresult = GetCondExpValueByOR( GetCondExpValueFromSubRight() );
	return bresult;

}


bool  CExpCondition::GetCondExpValueByOR( const bool& left )
// 优先级最低的计算函数:计算或运算
{
	bool result = left;
	if( Cur_Element_Species == OR ){
		ParseElementThenGotoNext(); 
		result = GetCondExpValueByOR( left || GetCondExpValueFromSubRight() );
	}
	return result;
}

bool  CExpCondition::GetCondExpValueFromSubRight()
//获得||号右侧表达式的值
{
	if (Error_code==ERROR_CONDITION_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
	return GetCondExpValueByAND( GetCondExpValueByNOT() );

}

bool  CExpCondition::GetCondExpValueByAND( const bool& left )
//优先级次低的计算函数:计算与运算
{
	bool result = left;
	if( Cur_Element_Species == AND ){
		ParseElementThenGotoNext(); 
		result = GetCondExpValueByOR( left && GetCondExpValueFromSubRight() );
	}
	return result;

}

bool  CExpCondition::GetCondExpValueByNOT()
//获得表达式的值,计算非运算
{
	bool result;
	if( Cur_Element_Species == NOT ){
		ParseElementThenGotoNext(); 
		result = !GetCondExpValueByNOT();
	}else if (Cur_Element_Species ==AND || Cur_Element_Species ==OR ){ //这些既取反运算符号，又非谓词标识符，如果分支转到这里说明原表达式有误，如 5&&||3

		if (Error_code==ERROR_CONDITION_SUCCESS){
			SetFirstError(ERROR_CONDITION_OPERATOR_INVALID);
		}

		ParseElementThenGotoNext(); 
		result = GetCondExpValueByNOT();	//继续处理，同时记录异常值	
	}else{

		if (Error_code==ERROR_CONDITION_SUCCESS)
			SetErrorStr(pCurrent_Char-1);
		result = GetElementValue();
	}
	return result;
}

bool  CExpCondition::GetElementValue()
/*
描述：获得当前表达式元素的值，可以由()标识的子表达式或谓词标识符
*/
{
	bool result;
	if( Cur_Element_Species == PREDICATION_ITEM ){
		result = ParseCurPredItem();
	}else if( Cur_Element_Species == LEFT_BRACKET ){ //如遇到(则优先计算整个()内表达式的值

		ParseElementThenGotoNext();

		//当作一个新的表达式，从头计算
		result=GetSubCondExpValue();

		if (Cur_Element_Species != RIGHT_BRACKET && Error_code==ERROR_CONDITION_SUCCESS)//计算结束后一定落在右括号上，否则原表达式括号不匹配
			SetFirstError(ERROR_EXP_MISSING_RIGHT_BRACKET);
		ParseElementThenGotoNext();	

	}
	return result;
}

bool  CExpCondition::ParseCurPredItem()
/*

*/
{
	bool result;
	string PredIdentify = Str_Cur_PredicationItem;
	ParseElementThenGotoNext();	

	if( pred_table.find( PredIdentify) == pred_table.end() ) {    

		if (Error_code==ERROR_CONDITION_SUCCESS)
			SetFirstError(ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST);

		result=false;   //说明找不到该参数，返回默认值

	}else{ 

		CPredicationItem tmp=pred_table.find( PredIdentify)->second;
		
		result = tmp.GetValue();  //去参数列表里取参数值

		if (Error_code==ERROR_CONDITION_SUCCESS && tmp.GetFirstError()!=ERROR_EXP_SUCCESS){

			SetFirstError(tmp.GetFirstError());
			err_str=tmp.GetFirstErrorEx();
		}
	}

    return result;

}



}//end
