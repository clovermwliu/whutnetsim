#ifndef ELEMENTCUSTOM_H_
#define ELEMENTCUSTOM_H_

#include <map>

#include "ExpressionParse.h"
#include "ExpCondition.h"

using namespace std;

namespace WhuTNetSimConfigClass{

#define ERROR_CUSTOM_SUCCESS          ERROR_CONDITION_SUCCESS
#define ERROR_CUSTOM_NOT_INITAILIZED  0x00030301


class CElementCustom
{
public:

	CElementCustom(void);
	CElementCustom(map<CExpCondition,CExpressionParse>& t,
                   double d);

	~CElementCustom(void);

public:

	double GetValue();

	void initail(map<CExpCondition,CExpressionParse>& t,double d) {custom_table.clear();dwDefault=d;custom_table=t;}

	string GetFirstErrorEx() {return err_str;}
	unsigned long GetFirstError() {return Error_code;}

private:

	double dwDefault;

	map<CExpCondition,CExpressionParse> custom_table;

	unsigned long Error_code;
	string err_str;

};


}//end of 


#endif