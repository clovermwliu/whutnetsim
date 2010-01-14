#ifndef PREDICATIONITEM_H_
#define PREDICATIONITEM_H_

#include "ExpressionParse.h"
using namespace std;

namespace WhuTNetSimConfigClass{

#define  ERROR_PRED_SUCCESS         ERROR_EXP_SUCCESS
#define  ERROR_PRED_OPERATORINVAILD	0x00030201


class CPredicationItem
{

public:
	
	CPredicationItem();
	
	CPredicationItem(CExpressionParse& l,
		             CExpressionParse& r,
					 string& opstr);

	~CPredicationItem(void);

public:

	bool GetValue();

	void SetExp(bool isLeft, CExpressionParse& exp);
	void SetOp(string opstr);

	unsigned long GetFirstError() {return Error_code;}
	string GetFirstErrorEx(){return Error_Str;}

private:

	unsigned long Error_code;
	string Error_Str;

private:

	CExpressionParse lExp;
	CExpressionParse rExp;
	string           op;

};


}//end of PREDICATIONITEM_H_
#endif