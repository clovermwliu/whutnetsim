
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

#include "ErrorHandler.h"
#include "node.h"

using namespace std;

#define  DEFAULT_VALUE                               1
#define  ERROR_EXP_SUCCESS			                 ERROR_NO_ERROR
#define  ERROR_EXP_DIVISOR_IS_ZERO                   0x00030101
#define  ERROR_EXP_SIGN_UNKNOWN                      0x00030102
#define  ERROR_EXP_INVAILD_PAPAMETER                 0x00030103
#define  ERROR_EXP_MISSING_RIGHT_BRACKET             0x00030104
#define  ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS   0x00030105
#define  ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS     0x00030106
#define  ERROR_EXP_USE_NONSUPPORT_FUNCS              0x00030107
#define  ERROR_EXP_CALL_SUBFUNCS_FAIL                0x00030108
#define  ERROR_EXP_NO_EXP                            0x00030109
#define  ERROR_EXP_MISSING_OPERATOR                  0x0003010a
#define  ERROR_EXP_NUMBER_FORMAT_INVALID             0x0003010b
#define  ERROR_EXP_OVERFLOW                          0x0003010c

typedef double (* pFunGet)(void); //����һ��Զ�̺���ָ�룬���ຯ�������;�Ϊdouble���βα�Ϊ��


class CExpressionParse : public CErrorHandler
{
public:

	template <typename ValueType> friend class CGenericConfigItem;
	
	CExpressionParse();
	
	CExpressionParse( const std::string&  _expression ,
		              const map< string, double>& _parameter_table,
					  const map< string,void*>& _remote_call_table,
					  bool revelantToNode);



	CExpressionParse(const CExpressionParse&);//����ʹ��pCurrent_Charָ�룬���ʽ��������Ҫ�ض�λ��ָ�� 

	CExpressionParse& operator=(const CExpressionParse& rhs); 
	
	virtual ~CExpressionParse(void);

public:

	double GetExpValue(); //ȡһ�����ʽ��ֵ����һ���ݹ麯�������


	virtual Error_str GetLastErrorEx();

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

	void SetErrorStr (const char* p); 

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

	bool RevelantToNode;//���Ա��ʽ�Ƿ�ͽ�����

	Node *node;

public:
	void AttachNode(Node *n) { node = n;}

	bool GetRevelantToNode() { return RevelantToNode; }

	void SetRevelantToNode(bool revelantToNode) { RevelantToNode = revelantToNode; }

};


//end of EXPRESSIONPARSE_H_

#endif