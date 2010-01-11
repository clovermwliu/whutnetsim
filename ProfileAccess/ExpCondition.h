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

	double GetConditionValue(); //取条件表达式真值，是一个递归函数的入口


private:

	void SetFirstError(const unsigned long err) {Error_code=err;}

	void ParseElementThenGotoNext(); //分析当前元素的属性,同时使pCurrent_Char指向下一个元素的首字符

	double GetSubConditionValue();//处理括号中的条件表达式




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
	ElementSpecies Cur_Element_Species; //记录当前解析到的表达式元素的种类


	const char* pCurrent_Char;//取当前解析到的表达式的字符

	string Str_Cur_PredicationItem;//当前解析到的谓词项

	bool dwCur_Value;//记录当前谓词项的值

	unsigned long Error_code;
	string str_error_exp;


private:
	string str_conditon;
	map<string,CPredicationItem> pred_table;



};





}//end of EXPCONDITION_H_


#endif
