//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:


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


typedef double (* pFunGet)(void); //����һ��Զ�̺���ָ�룬���ຯ�������;�Ϊdouble���βα�Ϊ��


class CExpressionParse
{
public:

	template <typename ValueType> friend class CGenericConfigItem;
	
	CExpressionParse( const std::string&  _expression ,
		              const map< string, double>& _parameter_table,
					  const map< string,void*>& _remote_call_table);
	
	virtual ~CExpressionParse(void);

private:

	CExpressionParse();

public:

	double GetExpValue(); //ȡһ�����ʽ��ֵ����һ���ݹ麯�������


	unsigned long GetFirstError() {return Error_code;}
	string GetFirstErrorEx();
	const string& GetExpStr() {return str_expression;}

	bool SetParamValue(const string& param, double value);
	bool SetRemoteCallAddrs(const string& param, void* p);

	int  GetCommonParamName(vector<string>& param_name);
	int  GetRemoteParamName(vector<string>& param_name);

public:

	void Initial();
	void Initial(const std::string&  _expression ,
                 const map< string, double>& _parameter_table,
				 const map< string,void*>& _remote_call_table);

private:

	void SetFirstError(const unsigned long err) {Error_code=err;}
	void SetErrorStr (const char* p) {str_error_exp.clear();istringstream iss(p);iss >>str_error_exp;}

	void ParseElementThenGotoNext(); //������ǰԪ�ص����ԣ�����Cur_Element_Species��Str_Cur_Identifier��dwCur_Value��ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�

	double GetSubExpValue();
	
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
	map<string,double> parameter_table; //���ز�������������
	map<string,void*> remote_call_addrs_table;//Զ�̸�ֵ������ͨ���������еĺ�����ַ��ȡ����ֵ
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