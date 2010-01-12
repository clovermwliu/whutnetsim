#ifndef EXPCONDITION_H_
#define EXPCONDITION_H_


#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <numeric>
#include <cmath>
#include <cctype>


#include "PredicationItem.h"

using namespace std;

namespace WhuTNetSimConfigClass{


#define  ERROR_CONDITION_SUCCESS			          0x00001000
#define  ERROR_CONDITION_NO_EXP                       0x00001001
#define  ERROR_CONDITION_MISSING_OPERATOR             0x00001002
#define  ERROR_CONDITION_OPERATOR_INVALID             0x00001003
#define  ERROR_CONDITION_PRED_FORMAT_INVALID          0x00001004
#define  ERROR_CONDITION_MISSING_RIGHT_BRACKET        0x00001005
#define  ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST       0x00001006


class CExpCondition
{
public:
	CExpCondition(void);
	
	CExpCondition(string condstr,
		          const map<string,CPredicationItem>& t);

	CExpCondition(const CExpCondition&);
	CExpCondition& operator=(const CExpCondition& rhs); 

	~CExpCondition(void);

	void Initial(string condstr,
		         const map<string,CPredicationItem>& t);

public:

	bool GetConditionValue(); //ȡ�������ʽ��ֵ����һ���ݹ麯�������

	string GetFirstErrorEx();
	unsigned long GetFirstError() {return Error_code;}

private:

	bool GetSubCondExpValue();//���������е��������ʽ

	bool GetCondExpValueByOR( const bool& left );// ���ȼ���͵ļ��㺯��:���������

	bool GetCondExpValueFromSubRight(); //���||���Ҳ���ʽ��ֵ

	bool GetCondExpValueByAND( const bool& left ); //���ȼ��ε͵ļ��㺯��:����������

	bool GetCondExpValueByNOT(); //��ñ��ʽ��ֵ,���������

	bool GetElementValue(); // ��õ�ǰ���ʽԪ�ص�ֵ��������()��ʶ���ӱ��ʽ��ν�ʱ�ʶ��

	bool  ParseCurPredItem(); //����ν�ʱ�ʶ����Ӧ��ν�������ֵ

	void ParseElementThenGotoNext(); //������ǰԪ�ص�����,ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�


private:

	void SetFirstError(const unsigned long err) {Error_code=err;}
	void SetErrorStr (const char* p) {str_error_exp.clear();istringstream iss(p);iss >>str_error_exp;}


private:
	string str_conditon;
	map<string,CPredicationItem> pred_table;


private:

	enum ElementSpecies
	{
		OR = '|',
		AND = '&',
		NOT = '!',
		LEFT_BRACKET = '(',
		RIGHT_BRACKET = ')',
		PREDICATION_ITEM = 257,
		FINISHED = 259,
		BEGININI = 260
	};
	ElementSpecies Cur_Element_Species; //��¼��ǰ�������ı��ʽԪ�ص�����


	const char* pCurrent_Char;//ȡ��ǰ�������ı��ʽ���ַ�

	string Str_Cur_PredicationItem;//��ǰ��������ν����

	bool bCur_Value;//��¼��ǰν�����ֵ

	unsigned long Error_code;
	string str_error_exp;//str_conditon���Ӵ����ӿ��ܳ���ĵط���ʼ����SetErrorStr()ά��
	string err_str;//���ڽ���CPredication�з��صĴ�����Ϣ


};



}//end of EXPCONDITION_H_


#endif
