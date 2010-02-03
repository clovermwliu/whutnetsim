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

	bool GetConditionValue(); //ȡ�������ʽ��ֵ����һ���ݹ麯�������

	virtual Error_str GetLastErrorEx();

	const string& GetConExpStr() {return str_conditon;}

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
	ElementSpecies Cur_Element_Species; //��¼��ǰ�������ı��ʽԪ�ص�����


	const char* pCurrent_Char;//ȡ��ǰ�������ı��ʽ���ַ�

	string Str_Cur_PredicationItem;//��ǰ��������ν����

	string str_error_exp;//str_conditon���Ӵ����ӿ��ܳ���ĵط���ʼ����SetErrorStr()ά��

	int id;//��������ID��

	bool RevelantToNode;//����ν�����Ƿ�ͽ�����

	Node *node;

public:

	void AttachNode(Node *n);

	bool GetRevelantToNode() const { return RevelantToNode; }

	void SetRevelantToNode(bool revelantToNode) { RevelantToNode = revelantToNode; }


};



//end of EXPCONDITION_H_


#endif
