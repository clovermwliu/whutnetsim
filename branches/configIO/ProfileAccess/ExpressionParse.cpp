#include "StdAfx.h"
#include "ExpressionParse.h"

namespace WhuTNetSimConfigClass{

CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI)
/*
构造函数
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
描述：分析当前元素的属性，设置Cur_Element_Species，Str_Cur_Identifier和dwCur_Value，同时使pCurrent_Char指向下一个元素的首字符
*/
{
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
	
			Cur_Element_Species = NUMBER;   //遇到数字，将数字转为数值形式存放到mValue
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				//throw eval_exception( "error in number format" );
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}
		
		break;
	}

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

	return GetExpValueByMulOrDivExp( GetSingedValueFromSubRight() );
}

double CExpressionParse::GetExpValueByMulOrDivExp( const double& left )
/*
描述：优先级次低的计算函数:计算乘除法。传入*,/号的左值，调用GetSingedValueFromSubRight()函数获得*,/号的右值，计算积或商
参数：left： *,/号的左值
备注：*,/的右表达式可能带有+/-号，表明其极性，因此在乘或除运算前因先确定右值的极性
*/
{

	double result = left;
	if( Cur_Element_Species == MULTIPLY ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByMulOrDivExp( left / GetSingedValueFromSubRight() );
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
	}else if (Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==MULTIPLY || Cur_Element_Species==POWER){ //这些既非极性符号，又非参数标识符或子函数的合法字符，如果分支转到这里说明原表达式有误，如 5+*3
		assert(0);		
	}else{
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
	double result = GetElementValue();  //取底数
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 
		result = pow( result, static_cast<int>( GetExpValueByPowerExp() ) );
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
		result = GetExpValue();                      //当作一个新的表达式，从头计算
		assert(Cur_Element_Species == RIGHT_BRACKET ); //计算结束后一定落在右括号上，否则原表达式括号不匹配
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
	assert(Cur_Element_Species ==IDENTIFIER );
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
//		result = funcs.GetValue( identifier,param );        //调用支持函数库对象f处理这个子函数
	
	}else{                                                   //没有紧跟（则说明这是一个参数标识符
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    //去参数列表里去参数值
			
			//GUOCHI 
			result=1;                                                          //说明参数列表里找不到该参数，返回默认值
		
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
            
			//GUOCHI
			assert(Cur_Element_Species!=MULTIPLY && Cur_Element_Species!=DIVIDE && Cur_Element_Species!=POWER &&
				   Cur_Element_Species!=RIGHT_BRACKET && Cur_Element_Species!=PARAMETER_SEPERATOR && Cur_Element_Species!=FINISHED);
		}
		result.push_back( GetExpValue() );//每个参数都可以被看作是一个子表达式
	}
	return result;
}


}//end of ExpressionParse.cpp