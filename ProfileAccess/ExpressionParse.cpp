#include "StdAfx.h"
#include "FileScript.h"
#include "ExpressionParse.h"


//-----------------------------------------------------------------------------------------
// CExpressionParse的实现
//-----------------------------------------------------------------------------------------

CExpressionParse::CExpressionParse()
/*
构造函数
*/
:dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),CErrorHandler()
{

}

	
CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table,
								   const map< string,void*>& _remote_call_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),CErrorHandler(),
  remote_call_addrs_table( _remote_call_table)
/*
构造函数
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}


CExpressionParse::CExpressionParse(const CExpressionParse& rhs)
/*
由于使用pCurrent_Char指针，表达式对象复制需要重定位该指针 
*/
{

	parameter_table=rhs.parameter_table;
	remote_call_addrs_table=rhs.remote_call_addrs_table;
	dwCur_Value=rhs.dwCur_Value;
	Str_Cur_Identifier=rhs.Str_Cur_Identifier;
	Cur_Element_Species=rhs.Cur_Element_Species;
	SetLastError(ERROR_EXP_SUCCESS);	
	SetLastErrorStr("");

	if (!rhs.str_expression.empty()){

		str_expression=rhs.str_expression;
		pCurrent_Char=str_expression.c_str();
		ParseElementThenGotoNext();
	}

}


CExpressionParse& CExpressionParse::operator=(const CExpressionParse& rhs)
/*
由于使用pCurrent_Char指针，表达式对象复制需要重定位该指针 
*/
{
	parameter_table=rhs.parameter_table;
	remote_call_addrs_table=rhs.remote_call_addrs_table;
	dwCur_Value=rhs.dwCur_Value;
	Str_Cur_Identifier=rhs.Str_Cur_Identifier;
	Cur_Element_Species=rhs.Cur_Element_Species;
	SetLastError(ERROR_EXP_SUCCESS);	
	SetLastErrorStr("");
	if (!rhs.str_expression.empty()){

		str_expression=rhs.str_expression;
		pCurrent_Char=str_expression.c_str();
		ParseElementThenGotoNext();
	}
	return *this;
}


void CExpressionParse::clear()
/*
描述：初始化函数
*/
{
	parameter_table.clear();
	remote_call_addrs_table.clear();
	str_expression="";
	pCurrent_Char=NULL;
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");

	//ParseElementThenGotoNext();
}


void CExpressionParse::Initial(const std::string&  _expression ,
			                   const map< string, double>& _parameter_table,
							   const map< string,void*>& _remote_call_table)
/*
描述：初始化函数
*/
{
	parameter_table= _parameter_table;
	remote_call_addrs_table= _remote_call_table;
	str_expression= _expression;
	pCurrent_Char=str_expression.c_str();
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");

	ParseElementThenGotoNext();

}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
描述：分析当前元素的属性，设置Cur_Element_Species，Str_Cur_Identifier和dwCur_Value，同时使pCurrent_Char指向下一个元素的首字符
*/
{
	/*if (!pCurrent_Char){
		if (error_code==ERROR_EXP_SUCCESS)
			SetLastError(ERROR_EXP_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}*/

	assert(pCurrent_Char);

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
				SetErrorStr(pCurrent_Char);
			}
			++pCurrent_Char;
			break;

		}
		
		if( isspace( *pCurrent_Char)) {
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

				if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

					if (GetLastError()==ERROR_EXP_SUCCESS){
						SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
						SetErrorStr(pCurrent_Char);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )|| CFileScript::isValidSymbol(*pCurrent_Char)); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束
			            
		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //遇到字符串结束符则结束
		
		}else{                             
	
			Cur_Element_Species = NUMBER;   //遇到数字，将数字转为数值形式存放到mValue，包括了诸如 .3这样的小数
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				if (GetLastError()==ERROR_EXP_SUCCESS){
					SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
					SetErrorStr(pCurrent_Char);
				}
				++pCurrent_Char;
				break;
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}
		
		break;
	}

}

Error_str CExpressionParse::GetLastErrorEx()
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
#define  ERROR_EXP_OVERFLOW                          0x0000000c

*/
{
	switch (GetLastError())
	{
	case ERROR_EXP_SUCCESS:
		return "SUCCESS";
	case ERROR_EXP_DIVISOR_IS_ZERO:
		return "Divisor is zero. Near by:"+GetLastErrorStr();
	case ERROR_EXP_SIGN_UNKNOWN:
		return "Sign of expression is unknown.  Near by "+GetLastErrorStr();
	case ERROR_EXP_INVAILD_PAPAMETER:
		return "Expression includes invalid parameter.  Near by "+GetLastErrorStr();
	case ERROR_EXP_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by "+GetLastErrorStr();
	case ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS:
		return "Expression includes reserve character.  Near by "+GetLastErrorStr();
	case ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS:
		return "Sub-Functions includes invalid parameters.  Near by "+GetLastErrorStr();
	case ERROR_EXP_USE_NONSUPPORT_FUNCS:
		return "Expression includes  non-support sub-function:"+GetLastErrorStr();
	case ERROR_EXP_CALL_SUBFUNCS_FAIL:
		return "Parameters don't match on sub-functions's request @:"+GetLastErrorStr();
	case ERROR_EXP_NO_EXP:
		return "No expression in this object";
	case ERROR_EXP_MISSING_OPERATOR:
		return "Missing operator.  Near by "+GetLastErrorStr();
	case ERROR_EXP_NUMBER_FORMAT_INVALID:
		return "Expression's format error.  Near by "+GetLastErrorStr();
	case  ERROR_EXP_OVERFLOW:
		return "Overflow!";
	default:
		return "UNKNOWN_ERROR";
	}

}

void CExpressionParse::SetErrorStr (const char* p)
/*

*/
{
	SetLastErrorStr("");
	const char* q=p;

	int i=0;

	while ((q-i)!=str_expression.c_str() && (isalpha( *(q-i)) || isdigit( *(q-i) ))){

		i++;
	}

	string s(q-i);
	SetLastErrorStr(s);
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

bool CExpressionParse::SetRemoteCallAddrs(const string& param, void* p)
/*
描述：更新远程调用函数的地址
*/
{
	map<string,void*>::iterator iter=remote_call_addrs_table.find( param);
	if( iter == remote_call_addrs_table.end() )//没有这个参数
		return false;

	iter->second=p;

	return true;

}

int   CExpressionParse::GetCommonParamName(vector<string>& param_name)
/*
描述：获得当前表达式对象的普通参数个数及参数名
参数：[OUT] param_name带出初始化好的参数名
*/
{
	map<string,double>::iterator iter=parameter_table.begin();

	while (iter!=parameter_table.end()){

		param_name.push_back(iter->first);
		++iter;
		
	}

	return static_cast<int> (parameter_table.size());


}

int  CExpressionParse:: GetRemoteParamName(vector<string>& param_name)
/*
描述：获得当前表达式对象的远程赋值参数个数及参数名
参数：[OUT] param_name带出初始化好的远程赋值参数名
*/
{

	map<string,void*>::iterator iter=remote_call_addrs_table.begin();

	while (iter!=remote_call_addrs_table.end()){

		param_name.push_back(iter->first);
		++iter;

	}

	return static_cast<int> (remote_call_addrs_table.size());


}



double CExpressionParse::GetExpValue()
/*
描述：返回一个表达式的值，是一个递归函数的入口
备注：这个入口在以下情况被触发：
      (1) 完整的表达式被第一次解析时；
*/
{

	if (str_expression.empty()){

		SetLastError(ERROR_EXP_NO_EXP);
		return DEFAULT_VALUE;
	}
		
	pCurrent_Char=str_expression.c_str();
	ParseElementThenGotoNext();
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");
	
	double result = GetExpValueByAddOrMinusExp( GetExpValueFromSubRight() );

	if (Cur_Element_Species != FINISHED && GetLastError()==ERROR_EXP_SUCCESS){

		SetLastError(ERROR_EXP_MISSING_OPERATOR);
		SetErrorStr(pCurrent_Char-1);
	}

	return result;
}

double CExpressionParse::GetSubExpValue()
/*
描述：返回一个表达式的值，是一个递归函数的入口
备注：这个入口在以下情况被触发：
     (1) 一个包含在( )内的子表达式时，see to GetElementValue()
     (2) 一个子函数的参数是一子表达式时，see to GetParameterValueForSupportFuncs()
*/

{
	if (GetLastError()==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
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

	if (GetLastError()==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
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

		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char-1);
		}

		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 

#define DIVZERO 

#ifdef DIVZERO
	
		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char-1);
		}
		
		double dwDiv=GetSingedValueFromSubRight();
		if(dwDiv ==0 && GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_DIVISOR_IS_ZERO);
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
		
		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_SIGN_UNKNOWN);
		}

		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();	//当作+号处理，同时记录异常值	
	}else{

		if (GetLastError()==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char-1);
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
	if (GetLastError()==ERROR_EXP_SUCCESS)
		SetErrorStr(pCurrent_Char-1);

	double result = GetElementValue();  //取底数
	
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 

		result = pow( result, GetSingedValueFromSubRight());
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

		//result = GetExpValue();                      //当作一个新的表达式，从头计算
		result=GetSubExpValue();

		if (Cur_Element_Species != RIGHT_BRACKET && GetLastError()==ERROR_EXP_SUCCESS)//计算结束后一定落在右括号上，否则原表达式括号不匹配
			SetLastError(ERROR_EXP_MISSING_RIGHT_BRACKET);
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
	if(Cur_Element_Species !=IDENTIFIER && GetLastError()==ERROR_EXP_SUCCESS ){
		SetLastError(ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS); 
		SetErrorStr(pCurrent_Char-1);
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
       
		
		result = GetValueFromCurSubFunc( identifier,param );        //调用支持函数库处理这个子函数
	
	}else{                                                   //没有紧跟（则说明这是一个参数标识符
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    
			
			if (remote_call_addrs_table.find(identifier) == remote_call_addrs_table.end()){  

				if (GetLastError()==ERROR_EXP_SUCCESS)
					SetLastError(ERROR_EXP_INVAILD_PAPAMETER);

				result=DEFAULT_VALUE;   //说明参数列表里和程函数调用表均找不到该参数，返回默认值
 			
			}else{

				pFunGet pRemoteFuncs =static_cast<pFunGet>(remote_call_addrs_table.find(identifier)->second);  //去远程函数调用表调用远程函数获得参数值
     			result=pRemoteFuncs();

			}

		}else{ 
			result = parameter_table.find( identifier )->second;                  //去参数列表里取参数值
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
					
				   if (GetLastError()==ERROR_EXP_SUCCESS)
						SetLastError(ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS );
				   
				   continue;
					
			}
		}

		if (Cur_Element_Species == FINISHED){  //函数缺少右括号

			if (GetLastError()==ERROR_EXP_SUCCESS)
				SetLastError(ERROR_EXP_MISSING_RIGHT_BRACKET );

			break;
		}
		//result.push_back( GetExpValue() );//每个参数都可以被看作是一个子表达式
		result.push_back(GetSubExpValue());
	}
	return result;
}


double  CExpressionParse::GetValueFromCurSubFunc(const string& name, const vector< double >& params)
/*

*/
{	
	if (name== "sum" || name== "SUM"){

		return accumulate( params.begin(), params.end(), (double)0 );
	
	}else if(name == "avg" || name == "AVG" || name == "mean" || name == "MEAN"){

		
		if (params.size()!=0){

			return accumulate( params.begin(), params.end(), (double)0 )/static_cast<double>(params.size());
		
		}else{

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;

		}
		
	}else if(name == "min"|| name=="MIN"){

		
		if (params.size()==0){

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;
		}
		
		vector<double>::const_iterator it=params.begin();
		double result=params[0];

		while (it!=params.end()){

			if (result>(*it)) result=*it;
			++it;
		}
        
		return  result;
	
	}else if(name == "max"|| name=="MAX"){


		if (params.size()==0){

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;
		}

		vector<double>::const_iterator it=params.begin();
		double result=params[0];

		while (it!=params.end()){

			if (result<(*it)) result=*it;
			++it;
		}

		return  result;
	
	}else if (name == "factorial" || name == "FACTORIAL" || name == "fa"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return DEFAULT_VALUE;
		
		double dwresult= DEFAULT_VALUE;
		double i= floor(params[0]);

		while(  i> 1 ){
			dwresult *= i;
			i -= 1;
		}

		return dwresult;

	}else if (name == "sqrt" || name == "SQRT"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return exp((double)DEFAULT_VALUE);

		return sqrt(params[0]);

	}else if (name=="exp" || name=="EXP"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return exp((double)DEFAULT_VALUE);
		
		return exp(params[0]);
	
	}else if (name == "logXY" || name == "LOGXY"){

		if (params.size()!=2 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==1 )	return log(params[0]);
		
		if (params.size()==0 )	return DEFAULT_VALUE;
	
		return log(params[0])/log(params[1]);

	}else if (name == "ln" || name == "log"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;

		return log(params[0]);

	}else if (name == "lg" || name == "log10"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return log10(params[0]);

	}else if (name == "sin" || name == "SIN"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return sin(params[0]);

	}else if (name == "asin" || name == "ASIN"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return asin(params[0]);

	}else if (name == "cos" || name == "COS"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return cos(params[0]);

	}else if (name == "acos" || name == "ACOS"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return acos(params[0]);

	}else if (name == "tan" || name == "tg" || name == "TAN" || name=="TG"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return tan(params[0]);

	}else if (name == "atan" || name == "atg" || name == "ATAN" || name=="ATG"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return atan(params[0]);

	}else if (name == "pi" || name == "PI"){
		
		if (params.size()!=0 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		return (double)3.1415926535897932384626433832795; 

	}else if (name == "abs" || name == "ABS"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return abs(params[0]);
	
	}else if (name == "floor" || name == "FLOOR")	{

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return floor(params[0]);

	}else if (name == "ceil" || name == "CEIL")	{

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return ceil(params[0]);

	}else if (name == "mod" || name == "MOD")	{

		if (params.size()!=2){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==1)  return params[0];
		if (params.size()==0)  return DEFAULT_VALUE;

		return fmod(params[0],params[1]);

	}else if (name == "sinh" || name == "SINH"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return sinh(params[0]);

	}else if (name == "cosh" || name == "COSH"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return cosh(params[0]);

	}else if (name == "tanh" || name == "tgh" || name == "TANH" || name=="TGH"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return tanh(params[0]);

	}else{

		if(GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_USE_NONSUPPORT_FUNCS);
			SetLastErrorStr(name);
		}

		return DEFAULT_VALUE; //默认值返回

	}

	
}



//end of ExpressionParse.cpp