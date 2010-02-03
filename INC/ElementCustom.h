#ifndef ELEMENTCUSTOM_H_
#define ELEMENTCUSTOM_H_


#include <vector>

#include "ExpressionParse.h"
#include "ExpCondition.h"
#include "ErrorHandler.h"
#include "node.h"

using namespace std;

#define ERROR_CUSTOM_SUCCESS          ERROR_CONDITION_SUCCESS
#define ERROR_CUSTOM_NOT_INITAILIZED  0x00030301
#define ERROR_CON_UNMATCH_EXP  0x00030302


class CElementCustom : public CErrorHandler
{
public:

	CElementCustom(void);
	//CElementCustom(map<CExpCondition,CExpressionParse>& t,
 //                  double d);
	CElementCustom(vector<CExpCondition>& t_con,
		           vector<CExpressionParse>& t_exp,
				   double d);

	~CElementCustom(void);

public:

	double GetValue();

	void initail(vector<CExpCondition>& t_con,
		         vector<CExpressionParse>& t_exp,
				 double d);

	virtual Error_str GetLastErrorEx() {return GetLastErrorStr();}


private:
	double dwDefault;

	//map<CExpCondition, CExpressionParse> custom_table;
	vector<CExpressionParse> custom_table_exp;
	vector<CExpCondition> custom_table_con;

	bool RevelantToNode;//该自定义部件是否和结点相关

	Node *node;

public:
	bool GetRevelantToNode() { return RevelantToNode; }

	void AttachNode(Node *n);

	void SetRevelantToNode(bool revelantToNode) { RevelantToNode = revelantToNode; }

};



#endif //end of  ELEMENTCUSTOM_H_