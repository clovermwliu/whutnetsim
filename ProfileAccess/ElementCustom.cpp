#include "StdAfx.h"
#include "ElementCustom.h"

namespace WhuTNetSimConfigClass{

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
	
			error_code=c.GetLastError();
			err_str="[Condition:"+c.GetConExpStr();
			err_str=err_str+" ERROR]:";
			err_str=err_str+c.GetLastErrorEx();
		}

		if (buse ){

			CExpressionParse e=iter->second;

			double result=e.GetExpValue();

			if(e.GetLastError()!=ERROR_EXP_SUCCESS){

				error_code=e.GetLastError();
				err_str="[Expression:"+e.GetExpStr();
				err_str=err_str+" ERROR]:";
                err_str=err_str+e.GetLastErrorEx();
			}

			return result;
		
		}

		++iter;
	}
	
	return dwDefault;
}


}//end