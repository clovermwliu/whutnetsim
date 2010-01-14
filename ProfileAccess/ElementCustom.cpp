#include "StdAfx.h"
#include "ElementCustom.h"

namespace WhuTNetSimConfigClass{

CElementCustom::CElementCustom()
:dwDefault(1),Error_code(ERROR_CUSTOM_SUCCESS),err_str("SUCCESS")
{
}

CElementCustom::CElementCustom(map<CExpCondition,CExpressionParse>& t,double d)
:dwDefault(d),custom_table(t),Error_code(ERROR_CUSTOM_SUCCESS),err_str("SUCCESS")
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

		err_str="This object has not been initialized";
		Error_code=ERROR_CUSTOM_NOT_INITAILIZED;
		return dwDefault;
	}


	map<CExpCondition,CExpressionParse>::iterator iter=custom_table.begin();

	while (iter!=custom_table.end()){

		CExpCondition c=iter->first;

		bool buse=c.GetConditionValue();

		if(c.GetFirstError()!=ERROR_CONDITION_SUCCESS){
	
			Error_code=c.GetFirstError();
			err_str="[Condition:"+c.GetConExpStr();
			err_str=err_str+" ERROR]:";
			err_str=err_str+c.GetFirstErrorEx();
		}

		if (buse ){

			CExpressionParse e=iter->second;

			double result=e.GetExpValue();

			if(e.GetFirstError()!=ERROR_EXP_SUCCESS){

				Error_code=e.GetFirstError();
				err_str="[Expression:"+e.GetExpStr();
				err_str=err_str+" ERROR]:";
                err_str=err_str+e.GetFirstErrorEx();
			}

			return result;
		
		}

		++iter;
	}
	
	return dwDefault;
}


}//end