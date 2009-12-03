#include "StdAfx.h"
#include "ExpressionParse.h"

namespace WhuTNetSimConfigClass{


//-----------------------------------------------------------------------------------------
// CExpressionParse的实现
//-----------------------------------------------------------------------------------------

CExpressionParse::CExpressionParse()
/*
构造函数
*/
{

}

	
CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),Error_code(ERROR_EXP_SUCCESS),str_error_exp("")
/*
构造函数
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}

void CExpressionParse::Initial()
/*
描述：初始化函数
*/
{
	parameter_table.clear();
	str_expression="";
	pCurrent_Char=NULL;
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	Error_code=ERROR_EXP_SUCCESS;
	str_error_exp="";

	ParseElementThenGotoNext();
}


void CExpressionParse::Initial(const std::string&  _expression ,
			                   const map< string, double>& _parameter_table)
/*
描述：初始化函数
*/
{
	parameter_table= _parameter_table;
	str_expression= _expression;
	pCurrent_Char=str_expression.c_str();
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	Error_code=ERROR_EXP_SUCCESS;
	str_error_exp="";
	
	ParseElementThenGotoNext();

}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
描述：分析当前元素的属性，设置Cur_Element_Species，Str_Cur_Identifier和dwCur_Value，同时使pCurrent_Char指向下一个元素的首字符
*/
{
	if (!pCurrent_Char){
		if (Error_code==ERROR_EXP_SUCCESS)
			SetFirstError(ERROR_EXP_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}

	while(true){
		
		if( isspace( *pCurrent_Char) ) {
			++pCurrent_Char;               //当前字符是是空格、制表符或换行符，继续
			continue;
		
		}else if( *pCurrent_Char == ADD_OR_POSITIVE || *pCurrent_Char == SUBTRACT_OR_NEGATIVE || 
			      *pCurrent_Char == MULTIPLY || *pCurrent_Char == DIVIDE ||*pCurrent_Char == POWER ||
				  *pCurrent_Char == LEFT_BRACKET || *pCurrent_Char == RIGHT_BRACKET ||
				  *pCurrent_Char == PARAMETER_SEPERATOR ){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符、括号或","
			++pCurrent_Char ;

		}else if( isalpha( *pCurrent_Char ) ){

			Cur_Element_Species = IDENTIFIER;            //当前遇到英文字母：当为英文字母a-z或A-Z时，返回非零值，否则返回零
			Str_Cur_Identifier.clear();
			
			do {
				Str_Cur_Identifier += (*pCurrent_Char);
				++pCurrent_Char;
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束
			            
		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //遇到字符串结束符则结束
		
		}else{                             
	
			Cur_Element_Species = NUMBER;   //遇到数字，将数字转为数值形式存放到mValue，包括了诸如 .3这样的小数
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				SetFirstError(ERROR_EXP_NUMBER_FORMAT_INVALID);
				SetErrorStr(pCurrent_Char);
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}
		
		break;
	}

}

string CExpressionParse::GetFirstErrorEx()
/*
描述：取表达式计算错误的第一个触发原因，返回其原因文本解释

#define  ERROR_EXP_SUCCESS			                 0x00000000
#define  ERROR_EXP_DIVISOR_IS_ZERO                   0x00000001
#define  ERROR_EXP_SIGN_UNKNOWN                      0x00000002
#define  ERROR_EXP_INVAILD_PAPAMETER                 0x00000003
#define  ERROR_EXP_MISSING_RIGHT_BRACKET             0x00000004
#define  ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS   0x00000005
#define  ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS     0x00000006
#define  ERROR_EXP_USE_NONSUPPORT_FUNCS              0x00000007
#define  ERROR_EXP_CALL_SUBFUNCS_FAIL                0x00000008
#define  ERROR_EXP_NO_EXP                            0x00000009
#define  ERROR_EXP_MISSING_OPERATOR                  0x0000000a
#define  ERROR_EXP_NUMBER_FORMAT_INVALID             0x0000000b

*/
{
	switch (Error_code)
	{
	case ERROR_EXP_SUCCESS:
		return "SUCCESS";
	case ERROR_EXP_DIVISOR_IS_ZERO:
		return "Divisor is zero. Near by:"+str_error_exp;
	case ERROR_EXP_SIGN_UNKNOWN:
		return "Sign of expression is unknown.  Near by:"+str_error_exp;
	case ERROR_EXP_INVAILD_PAPAMETER:
		return "Including invalid parameter.  Near by"+str_error_exp;
	case ERROR_EXP_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by"+str_error_exp;
	case ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS:
		return "Expression includes reserve character.  Near by"+str_error_exp;
	case ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS:
		return "Sub-Functions includes invalid parameters.  Near by"+str_error_exp;
	case ERROR_EXP_USE_NONSUPPORT_FUNCS:
		return "Including non-support sub-functions.  Near by"+str_error_exp;
	case ERROR_EXP_CALL_SUBFUNCS_FAIL:
		return "Parameters dose not match on sub-functions's request @:"+str_error_exp;
	case ERROR_EXP_NO_EXP:
		return "No expression in this object";
	case ERROR_EXP_MISSING_OPERATOR:
		return "Missing operator.  Near by"+str_error_exp;
	case ERROR_EXP_NUMBER_FORMAT_INVALID:
		return "Number format error.  Near by"+str_error_exp;
	default:
		return "UNKNOWN_ERROR";
	}

}



bool CExpressionParse::SetParamValue(const string& param, double value)
/*
描述：更新参数列表的值
*/
{
	map<string,double>::iterator iter=parameter_table.find( param);
	if( iter == parameter_table.end() )//没有这个参数
		return false;

	iter->second=value;

	return true;

}




double CExpressionParse::GetExpValue()
/*
描述：返回一个表达式的值，是一个递归函数的入口
备注：这个入口在以下情况被触发：
      (1) 完整的表达式被第一次解析时；
	  (2) 一个包含在( )内的子表达式时，see to GetElementValue()
	  (3) 一个子函数的参数是一子表达式时，see to GetParameterValueForSupportFuncs()
*/
{

	double result = GetExpValueByAddOrMinusExp( GetExpValueFromSubRight() );
	return result;
}

double CExpressionParse::GetExpValueByAddOrMinusExp( const double& left )
/*
描述：优先级最低的计算函数:计算加减法。传入+/-号的左值，调用GetExpValueFromSubRight()函数获得+/-号的右值，计算和或差
参数：left： +/-号的左值
*/
{
	double result = left;
	if( Cur_Element_Species == ADD_OR_POSITIVE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByAddOrMinusExp( left + GetExpValueFromSubRight() );
	}else if( Cur_Element_Species == SUBTRACT_OR_NEGATIVE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByAddOrMinusExp( left - GetExpValueFromSubRight() );
	}
	return result;
}
double CExpressionParse::GetExpValueFromSubRight()
/*
描述：获得+/- 号右侧表达式的值
*/
{

	if (Error_code==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char);
	}
	return GetExpValueByMulOrDivExp( GetSingedValueFromSubRight() );
}

double CExpressionParse::GetExpValueByMulOrDivExp( const double& left )
/*
描述：优先级次低的计算函数:计算乘除法。传入*,/号的左值，调用GetSingedValueFromSubRight()函数获得*,/号的右值，计算积或商
参数：left： *,/号的左值
备注：*,/的右表达式可能带有+/-号，表明其极性，因此在乘或除运算前因先确定右值的极性
      如果除数为0则返回计算结果为1.#INF
*/
{

	double result = left;
	if( Cur_Element_Species == MULTIPLY ){
		ParseElementThenGotoNext();

		if (Error_code==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char);
		}

		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 

#define DIVZERO 

#ifdef DIVZERO
	
		if (Error_code==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char);
		}
		
		double dwDiv=GetSingedValueFromSubRight();
		if(dwDiv ==0 && Error_code==ERROR_EXP_SUCCESS){
			SetFirstError(ERROR_EXP_DIVISOR_IS_ZERO);
		}
		result = GetExpValueByMulOrDivExp( left / dwDiv );
#else
		result = GetExpValueByMulOrDivExp( left / GetSingedValueFromSubRight() );
#endif
		
	}
	return result;
}
double CExpressionParse::GetSingedValueFromSubRight()
/*
描述：获得表达式的值并设置其极性,作为乘除法的右值
*/
{
	double result;
	if( Cur_Element_Species == ADD_OR_POSITIVE ){
		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();
	}else if( Cur_Element_Species == SUBTRACT_OR_NEGATIVE ){
		ParseElementThenGotoNext(); 
		result = - GetSingedValueFromSubRight();
	}else if (Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==DIVIDE || Cur_Element_Species==POWER){ //这些既非极性符号，又非参数标识符或子函数的合法字符，如果分支转到这里说明原表达式有误，如 5+*3
		
		if (Error_code==ERROR_EXP_SUCCESS){
			SetFirstError(ERROR_EXP_SIGN_UNKNOWN);
		}

		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();	//当作+号处理，同时记录异常值	
	}else{

		if (Error_code==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char);
		result = GetExpValueByPowerExp();
	}
	return result;
}
double CExpressionParse::GetExpValueByPowerExp()
/*
描述：优先级最高的计算函数:计算幂。调用 GetElementValue()获得底数的值，递归调用获得指数的值
备注：指数仍然可能是幂的形式，如a^b^c，递归调用构造堆栈，先计算b^c，在计算a^b^c；
      如果取得底数后下一个运算符并非^，则意味着该底数的1次幂，则递归出口。事实上，对于+,-,*,/运算的任何一个操作数都可以看作幂表达式形式
	  底数可以是数字，由()标识的子表达式，参数标识符或子函数
      
*/
{
	if (Error_code==ERROR_EXP_SUCCESS)
		SetErrorStr(pCurrent_Char);

	double result = GetElementValue();  //取底数
	
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 

		result = pow( result, GetExpValueByPowerExp());
	}
	return result;
}
double CExpressionParse::GetElementValue()
/*
描述：获得当前表达式元素的值，可以是数字，由()标识的子表达式，参数标识符或子函数
*/
{
	double result;
	if( Cur_Element_Species == NUMBER ){
		result = dwCur_Value;
		ParseElementThenGotoNext();	
	}else if( Cur_Element_Species == LEFT_BRACKET ){ //如遇到(则优先计算整个()内表达式的值

		ParseElementThenGotoNext();

		if (Error_code==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char);

		result = GetExpValue();                      //当作一个新的表达式，从头计算
		if (Cur_Element_Species != RIGHT_BRACKET && Error_code==ERROR_EXP_SUCCESS)//计算结束后一定落在右括号上，否则原表达式括号不匹配
			SetFirstError(ERROR_EXP_MISSING_RIGHT_BRACKET);
		ParseElementThenGotoNext();	
	}else{

		result = ParseCurIdentifier();
	}
	return result;
}

double CExpressionParse::ParseCurIdentifier()
/*
描述：解析参数标识符或子函数名
*/
{
	if(Cur_Element_Species !=IDENTIFIER && Error_code==ERROR_EXP_SUCCESS ){
		SetFirstError(ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS); 
		SetErrorStr(pCurrent_Char);
	}
	string str = Str_Cur_Identifier;
	ParseElementThenGotoNext();	
	return GetValueFromCurIdentifier( str);
}


double CExpressionParse::GetValueFromCurIdentifier( const string& identifier )
/*
描述：从参数列表parameter_table中获得参数标识符的值
备注：如果一段字符后紧跟(，则表明该字符串不是参数标识符，而是子函数名，比如遇到"sin("，说明sin是一个子函数
*/
{
	double result;
	if( Cur_Element_Species == LEFT_BRACKET )               //如果一个表达式取完发现后面紧跟一个（则说明这是一个子函数
	{
		ParseElementThenGotoNext();	
		vector<double> param = GetParameterValueForSupportFuncs();   
       
		//GUOCHI
		result = GetValueFromCurSubFunc( identifier,param );        //调用支持函数库处理这个子函数
	
	}else{                                                   //没有紧跟（则说明这是一个参数标识符
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    //去参数列表里去参数值
			
			if (Error_code==ERROR_EXP_SUCCESS)
				SetFirstError(ERROR_EXP_INVAILD_PAPAMETER);
			result=DEFAULT_VALUE;                                              //说明参数列表里找不到该参数，返回默认值
		
		}else{
			result = parameter_table.find( identifier )->second;
		}
		
	}
	return result;
}


vector<double> CExpressionParse::GetParameterValueForSupportFuncs()
/*
描述：获得子函数需要的各个参数的值
备注：子函数的每个参数都可以被看作是一个子表达式，逐步计算各个子表达式的值，放入这个vector容器
*/
{	vector<double> result;
	while( true)
	{
		if( Cur_Element_Species == RIGHT_BRACKET ){  //遇到右括号表明子函数参数列表结束。若循环第一次及转入此分支，则表明该子函数不带参数

			ParseElementThenGotoNext();	
			break;
		}

		if( Cur_Element_Species == PARAMETER_SEPERATOR ){

			ParseElementThenGotoNext();
 			if(Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==DIVIDE || Cur_Element_Species ==POWER ||
		       Cur_Element_Species==RIGHT_BRACKET || Cur_Element_Species==PARAMETER_SEPERATOR || Cur_Element_Species==FINISHED){
					if (Error_code==ERROR_EXP_SUCCESS)
						SetFirstError(ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS );
			}
		}
		result.push_back( GetExpValue() );//每个参数都可以被看作是一个子表达式
	}
	return result;
}


double  CExpressionParse::GetValueFromCurSubFunc(const string& name, const vector< double >& params)
/*

*/
{
	if (name== "sum" || name== "SUM"){

		return accumulate( params.begin(), params.end(), (double)0 );
	
	}else if (name=="exp" || name=="EXP"){

		if (params.size()>1 || params.size()==0){
			if(Error_code==ERROR_EXP_SUCCESS){

				SetFirstError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		return exp(params[0]);
	}

	if(Error_code==ERROR_EXP_SUCCESS)
		SetFirstError(ERROR_EXP_USE_NONSUPPORT_FUNCS);

	return DEFAULT_VALUE; //默认值返回
}



}//end of ExpressionParse.cpp