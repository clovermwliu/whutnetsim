#pragma once
#ifndef EXPRESSIONPARSE_H_
#define EXPRESSIONPARSE_H_

#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <numeric>
#include <cmath>
#include <assert.h>

using namespace std;

namespace WhuTNetSimConfigClass{


class CSupportFunctionsLib
{
public:
	double GetValue( const string, const vector< double >& );
	
	double GetValue( const char* name, const ::std::vector< double >& params )
	{
		if( stricmp( name, "return1" ) == 0 )
		{
			if( params.size() != 0 )
				throw 1;
			return 1;
		}
		else if( stricmp( name, "negative" ) == 0 )
		{
			if( params.size() != 1 )
				throw 1;
			return - params[ 0 ];
		}
		else if( stricmp( name, "sum" ) == 0 )
		{
			return accumulate( params.begin(), params.end(), 0 );
		}
	}
};



class CExpressionParse
{
public:
	CExpressionParse( const std::string&  _expression ,
		              const map< string, double>& _parameter_table);
	
	virtual ~CExpressionParse(void);

public:

	double GetExpValue(); //取一个表达式的值，是一个递归函数的入口

private:

	void ParseElementThenGotoNext(); //分析当前元素的属性，设置Cur_Element_Species，Str_Cur_Identifier和dwCur_Value，同时使pCurrent_Char指向下一个元素的首字符

	double GetExpValueByAddOrMinusExp( const double& left );// 优先级最低的计算函数:计算加减法
	
	double GetExpValueFromSubRight(); //获得+/- 号右侧表达式的值
	
	double GetExpValueByMulOrDivExp( const double& left ); //优先级次低的计算函数:计算乘除法
	
	double GetSingedValueFromSubRight(); //获得表达式的值并设置其极性,作为乘除法的右值
	
	double GetExpValueByPowerExp(); //优先级最高的计算函数:计算幂
	
	double GetElementValue(); // 获得当前表达式元素的值，可以是数字，由()标识的子表达式，参数标识符或子函数
	
	double ParseCurIdentifier(); //解析参数标识符或子函数名
	
	double GetValueFromCurIdentifier( const string& identifier ); //从参数列表parameter_table中获得参数标识符的值
	
	vector<double> GetParameterValueForSupportFuncs(); //获得子函数需要的各个参数的值
		
private:
	const map<string,double>& parameter_table; //参数表，关联容器
	const string& str_expression;   //完整的表达式
	CSupportFunctionsLib funcs; //这个支持函数库的对象作为CExpressionParse的成员，似有不妥

private://以下成员供解析表达式用

	const char* pCurrent_Char;//取当前解析到的表达式的字符
	
	string Str_Cur_Identifier;//如果在解析表达式过程中遇到元素为参数标识符或函数名，则放在这里

	double dwCur_Value;//如果在解析表达式过程中遇到数字，则放在这里

	enum ElementSpecies
	{
		ADD_OR_POSITIVE = '+',
		SUBTRACT_OR_NEGATIVE = '-',
		MULTIPLY = '*',
		DIVIDE = '/',
		POWER = '^',
		LEFT_BRACKET = '(',
		RIGHT_BRACKET = ')',
		PARAMETER_SEPERATOR = ',',
		IDENTIFIER = 257,
		NUMBER = 258,
		FINISHED = 259,
		BEGININI = 260
	};
	
	ElementSpecies Cur_Element_Species; //记录当前解析到的表达式元素的种类


};


}//end of EXPRESSIONPARSE_H_

#endif