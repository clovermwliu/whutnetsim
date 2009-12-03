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


class CExpressionParse
{
public:

	template <typename ValueType> friend class CGenericConfigItem;
	
	CExpressionParse( const std::string&  _expression ,
		              const map< string, double>& _parameter_table);
	
	virtual ~CExpressionParse(void);

private:

	CExpressionParse();

public:

	double GetExpValue(); //ȡһ�����ʽ��ֵ����һ���ݹ麯�������

	unsigned long GetFirstError() {return Error_code;}
	string GetFirstErrorEx();
	const string& GetExpStr() {return str_expression;}

	bool SetParamValue(const string& param, double value);

public:

	void Initial();
	void Initial(const std::string&  _expression ,
                 const map< string, double>& _parameter_table);

private:

	void SetFirstError(const unsigned long err) {Error_code=err;}
	void SetErrorStr (const char* p) {str_error_exp.clear();istringstream iss(p);iss >>str_error_exp;}

	void ParseElementThenGotoNext(); //������ǰԪ�ص����ԣ�����Cur_Element_Species��Str_Cur_Identifier��dwCur_Value��ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�

	double GetExpValueByAddOrMinusExp( const double& left );// ���ȼ���͵ļ��㺯��:����Ӽ���
	
	double GetExpValueFromSubRight(); //���+/- ���Ҳ���ʽ��ֵ
	
	double GetExpValueByMulOrDivExp( const double& left ); //���ȼ��ε͵ļ��㺯��:����˳���
	
	double GetSingedValueFromSubRight(); //��ñ��ʽ��ֵ�������伫��,��Ϊ�˳�������ֵ
	
	double GetExpValueByPowerExp(); //���ȼ���ߵļ��㺯��:������
	
	double GetElementValue(); // ��õ�ǰ���ʽԪ�ص�ֵ�����������֣���()��ʶ���ӱ��ʽ��������ʶ�����Ӻ���
	
	double ParseCurIdentifier(); //����������ʶ�����Ӻ�����
	
	double GetValueFromCurIdentifier( const string& identifier ); //�Ӳ����б�parameter_table�л�ò�����ʶ����ֵ

	vector<double> GetParameterValueForSupportFuncs(); //����Ӻ�����Ҫ�ĸ���������ֵ
	
	double GetValueFromCurSubFunc(const string& name, const vector< double >& params); //����Ӻ������ص�ֵ


private:
	map<string,double> parameter_table; //��������������
	string str_expression;   //�����ı��ʽ

private://���³�Ա���������ʽ��

	const char* pCurrent_Char;//ȡ��ǰ�������ı��ʽ���ַ�
	//char* pErr_Char;
	
	string Str_Cur_Identifier;//����ڽ������ʽ����������Ԫ��Ϊ������ʶ�������������������

	double dwCur_Value;//����ڽ������ʽ�������������֣����������

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
	
	ElementSpecies Cur_Element_Species; //��¼��ǰ�������ı��ʽԪ�ص�����

	unsigned long Error_code;
	string str_error_exp;


};


}//end of EXPRESSIONPARSE_H_

#endif