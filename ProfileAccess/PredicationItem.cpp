#include "StdAfx.h"
#include "PredicationItem.h"


namespace WhuTNetSimConfigClass{


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
	error_code=lExp.GetLastError();
	if (error_code!=ERROR_EXP_SUCCESS){

		err_str="LeftExp:"+lExp.GetLastErrorEx();
		return false;
	
	}else{
		err_str=lExp.GetLastErrorEx();
	}

	double dwr=rExp.GetExpValue();
	error_code=rExp.GetLastError();
	if (error_code!=ERROR_EXP_SUCCESS){

		err_str="RightExp:"+rExp.GetLastErrorEx();
		return false;
	}else{

		err_str=rExp.GetLastErrorEx();
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

		error_code=ERROR_PRED_OPERATORINVAILD;
		err_str="Predication item's operator is invalid";

		return false;

	}

}


}//end