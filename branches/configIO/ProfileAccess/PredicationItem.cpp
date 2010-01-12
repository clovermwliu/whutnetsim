#include "StdAfx.h"
#include "PredicationItem.h"


namespace WhuTNetSimConfigClass{


CPredicationItem::CPredicationItem()
:Error_code(ERROR_EXP_SUCCESS)
{

}

CPredicationItem::CPredicationItem(CExpressionParse& l,
								   CExpressionParse& r,
     							   string& opstr)
:Error_code(ERROR_EXP_SUCCESS),Error_Str("")
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
	Error_code=lExp.GetFirstError();
	Error_Str=lExp.GetFirstErrorEx();

	if (Error_code!=ERROR_EXP_SUCCESS){

		return false;
	}

	double dwr=rExp.GetExpValue();
	Error_code=rExp.GetFirstError();
	Error_Str=rExp.GetFirstErrorEx();

	if (Error_code!=ERROR_EXP_SUCCESS){

		return false;
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

		Error_code=ERROR_PRED_OPERATORINVAILD;
		Error_Str="Predication is invalid";

		return false;

	}

}


}//end