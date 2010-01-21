#include "ElementCustom.h"

CElementCustom::CElementCustom()
:dwDefault(1),CErrorHandler()
{
}

CElementCustom::CElementCustom(map<CExpCondition,CExpressionParse>& t,double d)
:dwDefault(d),custom_table(t),CErrorHandler()
{
}

CElementCustom::~CElementCustom(void)
{
}

double CElementCustom::GetValue()
/*

*/
{

	if (custom_table.empty()){

		SetLastErrorStr("This object has not been initialized");
		SetLastError(ERROR_CUSTOM_NOT_INITAILIZED);
		return dwDefault;
	}


	map<CExpCondition,CExpressionParse>::iterator iter=custom_table.begin();

	while (iter!=custom_table.end()){

		CExpCondition c=iter->first;

		bool buse=c.GetConditionValue();

		if(c.GetLastError()!=ERROR_CONDITION_SUCCESS){
	
			SetLastError(c.GetLastError());
			Error_str e_str="[Condition:"+c.GetConExpStr();		
			e_str=e_str+" ERROR]:";
			e_str=e_str+c.GetLastErrorEx();
			SetLastErrorStr(e_str);
		}

		if (buse ){

			CExpressionParse e=iter->second;

			double result=e.GetExpValue();

			if(e.GetLastError()!=ERROR_EXP_SUCCESS){

				
				SetLastError(e.GetLastError());
				Error_str e_str="[Expression:"+e.GetExpStr();	
				e_str=e_str+" ERROR]:";
				e_str=e_str+e.GetLastErrorEx();
				SetLastErrorStr(e_str);
			}

			return result;
		
		}

		++iter;
	}
	
	return dwDefault;
}


//end