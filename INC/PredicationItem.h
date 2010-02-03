#ifndef PREDICATIONITEM_H_
#define PREDICATIONITEM_H_

#include "ExpressionParse.h"
using namespace std;

#define  ERROR_PRED_SUCCESS         ERROR_EXP_SUCCESS
#define  ERROR_PRED_OPERATORINVAILD	0x00030201


class CPredicationItem : public CErrorHandler
{

public:
	
	CPredicationItem();
	
	CPredicationItem(CExpressionParse& l,
		             CExpressionParse& r,
					 string& opstr);

	~CPredicationItem(void);

public:

	bool GetValue();

	Error_str GetLastErrorEx() {return GetLastErrorStr();}

	void SetExp(bool isLeft, CExpressionParse& exp);
	void SetOp(string opstr);
private:

	CExpressionParse lExp;
	CExpressionParse rExp;
	string           op;

	bool RevelantToNode;//该自谓词项是否和结点相关

	Node *node;

public:

	void AttachNode(Node *n);

	bool GetRevelantToNode() { return RevelantToNode; }

	void SetRevelantToNode(bool revelantToNode) { RevelantToNode = revelantToNode; }

};


//end of PREDICATIONITEM_H_
#endif