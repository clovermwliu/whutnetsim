#ifndef EXPCONDITION_H_
#define EXPCONDITION_H_
#pragma once


#include <map>
#include <cctype>


#include "PredicationItem.h"
#include "ErrorHandler.h"

using namespace std;

#define  ERROR_CONDITION_SUCCESS			          ERROR_PRED_SUCCESS
#define  ERROR_CONDITION_NO_EXP                       0x00030201
#define  ERROR_CONDITION_MISSING_OPERATOR             0x00030202
#define  ERROR_CONDITION_OPERATOR_INVALID             0x00030203
#define  ERROR_CONDITION_PRED_FORMAT_INVALID          0x00030204
#define  ERROR_CONDITION_MISSING_RIGHT_BRACKET        0x00030205
#define  ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST       0x00030206
#define  ERROR_CONDITION_MISSING_PREDITEM             0x00030207



class CExpCondition :public CErrorHandler
{
public:
	CExpCondition(void);
	
	CExpCondition(string condstr,
		          const map<string,CPredicationItem>& t);

	CExpCondition(const CExpCondition&);
	CExpCondition& operator=(const CExpCondition& rhs); 

	friend bool operator<(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id <rhs.id ;} 
	friend bool operator>(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id >rhs.id ;} 
	friend bool operator<=(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id <=rhs.id ;} 
	friend bool operator>=(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id >=rhs.id ;} 
	friend bool operator==(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id ==rhs.id ;} 
	friend bool operator!=(const CExpCondition& lhs,const CExpCondition& rhs) {return lhs.id !=rhs.id ;} 


	~CExpCondition(void);

	void Initial(string condstr,
		         const map<string,CPredicationItem>& t,
				 int idnum=0);

public:

	bool GetConditionValue(); //取条件表达式真值，是一个递归函数的入口

	virtual Error_str GetLastErrorEx();

	const string& GetConExpStr() {return str_conditon;}

private:

	bool GetSubCondExpValue();//处理括号中的条件表达式

	bool GetCondExpValueByOR( const bool& left );// 优先级最低的计算函数:计算或运算

	bool GetCondExpValueFromSubRight(); //获得||号右侧表达式的值

	bool GetCondExpValueByAND( const bool& left ); //优先级次低的计算函数:计算与运算

	bool GetCondExpValueByNOT(); //获得表达式的值,计算非运算

	bool GetElementValue(); // 获得当前表达式元素的值，可以由()标识的子表达式或谓词标识符

	bool  ParseCurPredItem(); //解析谓词标识符对应的谓词项的真值

	void ParseElementThenGotoNext(); //分析当前元素的属性,同时使pCurrent_Char指向下一个元素的首字符


private:

	void SetErrorStr (const char* p);

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
	ElementSpecies Cur_Element_Species; //记录当前解析到的表达式元素的种类


	const char* pCurrent_Char;//取当前解析到的表达式的字符

	string Str_Cur_PredicationItem;//当前解析到的谓词项

	string str_error_exp;//str_conditon的子串，从可能出错的地方开始，由SetErrorStr()维护

	int id;//该条件的ID号

	bool RevelantToNode;//该自谓词项是否和结点相关

	Node *node;

public:

	void AttachNode(Node *n);

	bool GetRevelantToNode() const { return RevelantToNode; }

	void SetRevelantToNode(bool revelantToNode) { RevelantToNode = revelantToNode; }


};



//end of EXPCONDITION_H_


#endif
