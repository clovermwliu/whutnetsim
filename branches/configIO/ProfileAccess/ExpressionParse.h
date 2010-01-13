
#ifndef EXPRESSIONPARSE_H_
#define EXPRESSIONPARSE_H_

#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <numeric>
#include <cmath>
#include <cctype>
#include "assert.h"

#include "Test.h"

using namespace std;

namespace WhuTNetSimConfigClass{

#define  DEFAULT_VALUE                               1
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


typedef double (* pFunGet)(void); //定义一个远程函数指针，这类函数的类型均为double，形参表为空


class CExpressionParse
{
public:

	template <typename ValueType> friend class CGenericConfigItem;
	
	CExpressionParse();
	
	CExpressionParse( const std::string&  _expression ,
		              const map< string, double>& _parameter_table,
					  const map< string,void*>& _remote_call_table);



	CExpressionParse(const CExpressionParse&);//由于使用pCurrent_Char指针，表达式对象复制需要重定位该指针 

	CExpressionParse& operator=(const CExpressionParse& rhs); 
	
	virtual ~CExpressionParse(void);

public:

	double GetExpValue(); //取一个表达式的值，是一个递归函数的入口


	unsigned long GetFirstError() {return Error_code;}
	string GetFirstErrorEx();
	const string& GetExpStr() {return str_expression;}

	bool SetParamValue(const string& param, double value);
	bool SetRemoteCallAddrs(const string& param, void* p);

	int  GetCommonParamName(vector<string>& param_name);
	int  GetRemoteParamName(vector<string>& param_name);

public:

	void clear();
	void Initial(const std::string&  _expression ,
                 const map< string, double>& _parameter_table,
				 const map< string,void*>& _remote_call_table);

	
private:

	void SetFirstError(const unsigned long err) {Error_code=err;}
	void SetErrorStr (const char* p); 

	void ParseElementThenGotoNext(); //分析当前元素的属性，设置Cur_Element_Species，Str_Cur_Identifier和dwCur_Value，同时使pCurrent_Char指向下一个元素的首字符

	double GetSubExpValue();
	
	double GetExpValueByAddOrMinusExp( const double& left );// 优先级最低的计算函数:计算加减法
	
	double GetExpValueFromSubRight(); //获得+/- 号右侧表达式的值
	
	double GetExpValueByMulOrDivExp( const double& left ); //优先级次低的计算函数:计算乘除法
	
	double GetSingedValueFromSubRight(); //获得表达式的值并设置其极性,作为乘除法的右值
	
	double GetExpValueByPowerExp(); //优先级最高的计算函数:计算幂
	
	double GetElementValue(); // 获得当前表达式元素的值，可以是数字，由()标识的子表达式，参数标识符或子函数
	
	double ParseCurIdentifier(); //解析参数标识符或子函数名
	
	double GetValueFromCurIdentifier( const string& identifier ); //从参数列表parameter_table中获得参数标识符的值

	vector<double> GetParameterValueForSupportFuncs(); //获得子函数需要的各个参数的值
	
	double GetValueFromCurSubFunc(const string& name, const vector< double >& params); //获得子函数返回的值


private:
	map<string,double> parameter_table; //本地参数表，关联容器
	map<string,void*> remote_call_addrs_table;//远程赋值参数表，通过参数表中的函数地址获取参数值
	string str_expression;   //完整的表达式

private://以下成员供解析表达式用

	const char* pCurrent_Char;//取当前解析到的表达式的字符
	//char* pErr_Char;
	
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

	unsigned long Error_code;
	string str_error_exp;

};


}//end of EXPRESSIONPARSE_H_

#endif