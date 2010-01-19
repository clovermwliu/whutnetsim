#include "StdAfx.h"
#include "PredicationItem.h"



CPredicationItem::CPredicationItem()
:op("=="),CErrorHandler()
{

}

CPredicationItem::CPredicationItem(CExpressionParse& l,
								   CExpressionParse& r,
     							   string& opstr)
:CErrorHandler()
{
	lExp=l;
	rExp=r;
	op=opstr;
}

CPredicationItem::~CPredicationItem(void)

{
}


void CPredicationItem::SetExp(bool isLeft, CExpressionParse& exp)
/*

*/

{
	if(isLeft){

		lExp=exp;

	}else{

		rExp=exp;
	}
}

void CPredicationItem::SetOp(string opstr)
/*

*/
{
	op=opstr;
}

bool CPredicationItem::GetValue()
/*

*/
{
	double dwl=lExp.GetExpValue();
	SetLastError(lExp.GetLastError());
	if (GetLastError()!=ERROR_EXP_SUCCESS){

		SetLastErrorStr("LeftExp:"+lExp.GetLastErrorEx());
		return false;
	
	}else{
		SetLastErrorStr(lExp.GetLastErrorEx());
	}

	double dwr=rExp.GetExpValue();
	SetLastError(rExp.GetLastError());
	if (GetLastError()!=ERROR_EXP_SUCCESS){

		SetLastErrorStr("RightExp:"+rExp.GetLastErrorEx());
		return false;
	}else{

			SetLastErrorStr(rExp.GetLastErrorEx());
	}

	if (op==">"){

		return (dwl > dwr);
	
	}else if (op=="<"){

		return (dwl < dwr);
	
	}else if (op==">="){

		return (dwl >= dwr);
	
	}else if (op=="<="){

		return (dwl <= dwr);
	
	}else if (op=="=="){

		return (dwl == dwr);
	
	}else if (op=="!="){

		return (dwl != dwr);
	
	}else{

		
		SetLastError(ERROR_PRED_OPERATORINVAILD);
		SetLastErrorStr("Predication item's operator is invalid");

		return false;

	}

}


//end