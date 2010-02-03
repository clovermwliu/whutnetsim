
#include "ElementCustom.h"

CElementCustom::CElementCustom()
:dwDefault(1),CErrorHandler(), RevelantToNode(false), node(NULL)
{
}

CElementCustom::CElementCustom(vector<CExpCondition>& t_con,
							   vector<CExpressionParse>& t_exp,
							   double d)
:dwDefault(d),custom_table_con(t_con),custom_table_exp(t_exp), CErrorHandler()
{
}

CElementCustom::~CElementCustom(void)
{
}

void
CElementCustom::AttachNode(Node *n)
{
	node = n;
	//±Ì¥Ô Ω
	if (custom_table_con.size() != custom_table_exp.size())
	{

		SetLastErrorStr("The number of conditions and the number of expressions do not match!");
		SetLastError(ERROR_CON_UNMATCH_EXP);
	}
	for (int i = 0; i < (int)custom_table_con.size(); ++i)
	{
		if (custom_table_con[i].GetRevelantToNode())
			custom_table_con[i].AttachNode(n);

		if (custom_table_exp[i].GetRevelantToNode())
			custom_table_exp[i].AttachNode(n);
	}
}


void 
CElementCustom::initail(vector<CExpCondition>& t_con,
			            vector<CExpressionParse>& t_exp,
						double d)
{ 
	custom_table_con.clear();
	custom_table_exp.clear(); 
	dwDefault = d;
	custom_table_con = t_con;
	custom_table_exp = t_exp;
}

double CElementCustom::GetValue()
/*

*/
{

	if (custom_table_con.empty())
	{

		SetLastErrorStr("This object has not been initialized");
		SetLastError(ERROR_CUSTOM_NOT_INITAILIZED);
		return dwDefault;
	}
	if (custom_table_con.size() != custom_table_exp.size())
	{

		SetLastErrorStr("The number of conditions and the number of expressions do not match!");
		SetLastError(ERROR_CON_UNMATCH_EXP);
		return dwDefault;
	}

	for (int i = 0; i < (int)custom_table_con.size(); ++i)
	{
		CExpCondition c = custom_table_con[i];

		bool buse=c.GetConditionValue();

		if(c.GetLastError()!=ERROR_CONDITION_SUCCESS)
		{

			SetLastError(c.GetLastError());
			Error_str e_str="[Condition:"+c.GetConExpStr();		
			e_str=e_str+" ERROR]:";
			e_str=e_str+c.GetLastErrorEx();
			SetLastErrorStr(e_str);
		}

		if (buse )
		{

			CExpressionParse e = custom_table_exp[i];

			double result=e.GetExpValue();

			if(e.GetLastError()!=ERROR_EXP_SUCCESS)
			{


				SetLastError(e.GetLastError());
				Error_str e_str="[Expression:"+e.GetExpStr();	
				e_str=e_str+" ERROR]:";
				e_str=e_str+e.GetLastErrorEx();
				SetLastErrorStr(e_str);
			}

			return result;
		}
	}
	return dwDefault;
}


//end