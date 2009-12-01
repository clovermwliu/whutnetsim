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

	double GetExpValue(); //ȡһ�����ʽ��ֵ����һ���ݹ麯�������

private:

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
		
private:
	const map<string,double>& parameter_table; //��������������
	const string& str_expression;   //�����ı��ʽ
	CSupportFunctionsLib funcs; //���֧�ֺ�����Ķ�����ΪCExpressionParse�ĳ�Ա�����в���

private://���³�Ա���������ʽ��

	const char* pCurrent_Char;//ȡ��ǰ�������ı��ʽ���ַ�
	
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


};


}//end of EXPRESSIONPARSE_H_

#endif