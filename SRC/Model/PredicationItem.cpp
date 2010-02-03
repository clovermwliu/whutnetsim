
#include "PredicationItem.h"



CPredicationItem::CPredicationItem()
:op("=="),CErrorHandler(), RevelantToNode(false), node(NULL) 
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
	//是否与结点状态有关
	node = NULL;
	if (lExp.GetRevelantToNode() || rExp.GetRevelantToNode())
		RevelantToNode = true;
}

CPredicationItem::~CPredicationItem(void)
{
}

void
CPredicationItem::AttachNode(Node *n)
{
	node = n;
	if (lExp.GetRevelantToNode())
	{
		lExp.AttachNode(n);
	}
	if (rExp.GetRevelantToNode())
	{
		rExp.AttachNode(n);
	}
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