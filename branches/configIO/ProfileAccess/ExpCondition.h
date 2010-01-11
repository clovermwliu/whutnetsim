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


class CExpCondition
{
public:
	CExpCondition(void);
	
	CExpCondition(string condstr,
		          map<string,CPredicationItem>& t);

	CExpCondition(const CExpCondition&);
	CExpCondition& operator=(const CExpCondition& rhs); 

	~CExpCondition(void);

public:

	double GetConditionValue(); //ȡ�������ʽ��ֵ����һ���ݹ麯�������


private:

	void SetFirstError(const unsigned long err) {Error_code=err;}

	void ParseElementThenGotoNext(); //������ǰԪ�ص�����,ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�

	double GetSubConditionValue();//���������е��������ʽ




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

	bool dwCur_Value;//��¼��ǰν�����ֵ

	unsigned long Error_code;
	string str_error_exp;


private:
	string str_conditon;
	map<string,CPredicationItem> pred_table;



};





}//end of EXPCONDITION_H_


#endif
