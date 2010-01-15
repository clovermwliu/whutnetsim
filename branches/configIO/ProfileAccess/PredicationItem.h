#ifndef PREDICATIONITEM_H_
#define PREDICATIONITEM_H_

#include "ExpressionParse.h"
using namespace std;

namespace WhuTNetSimConfigClass{

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

	Error_str GetLastErrorEx() {return err_str;}

	void SetExp(bool isLeft, CExpressionParse& exp);
	void SetOp(string opstr);
private:

	CExpressionParse lExp;
	CExpressionParse rExp;
	string           op;

};


}//end of PREDICATIONITEM_H_
#endif