#include "StdAfx.h"
#include "ExpCondition.h"

namespace WhuTNetSimConfigClass{

CExpCondition::CExpCondition(void)
{
}

CExpCondition::~CExpCondition(void)
{
}


/*

void CExpCondition::ParseElementThenGotoNext()
//分析当前元素的属性,同时使pCurrent_Char指向下一个元素的首字符
{
	if (!pCurrent_Char){
		if (Error_code==ERROR_EXP_SUCCESS)
			SetFirstError(ERROR_EXP_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (Error_code==ERROR_EXP_SUCCESS){
				SetFirstError(ERROR_EXP_NUMBER_FORMAT_INVALID);
				SetErrorStr(pCurrent_Char);
			}
			++pCurrent_Char;
			break;

		}

		if( isspace( *pCurrent_Char)) {
			++pCurrent_Char;               //当前字符是是空格、制表符或换行符，继续
			continue;

		}else if( *pCurrent_Char == ADD_OR_POSITIVE || *pCurrent_Char == SUBTRACT_OR_NEGATIVE || 
			*pCurrent_Char == MULTIPLY || *pCurrent_Char == DIVIDE ||*pCurrent_Char == POWER ||
			*pCurrent_Char == LEFT_BRACKET || *pCurrent_Char == RIGHT_BRACKET ||
			*pCurrent_Char == PARAMETER_SEPERATOR ){

				Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符、括号或","
				++pCurrent_Char ;

		}else if( isalpha( *pCurrent_Char ) ){

			Cur_Element_Species = IDENTIFIER;            //当前遇到英文字母：当为英文字母a-z或A-Z时，返回非零值，否则返回零
			Str_Cur_Identifier.clear();

			do {
				Str_Cur_Identifier += (*pCurrent_Char);
				++pCurrent_Char;

				if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

					if (Error_code==ERROR_EXP_SUCCESS){
						SetFirstError(ERROR_EXP_NUMBER_FORMAT_INVALID);
						SetErrorStr(pCurrent_Char);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //遇到字符串结束符则结束

		}else{                             

			Cur_Element_Species = NUMBER;   //遇到数字，将数字转为数值形式存放到mValue，包括了诸如 .3这样的小数
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   

			if( ! iss ){

				if (Error_code==ERROR_EXP_SUCCESS){
					SetFirstError(ERROR_EXP_NUMBER_FORMAT_INVALID);
					SetErrorStr(pCurrent_Char);
				}
				++pCurrent_Char;
				break;
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}

		break;
	}


}

*/



}//end
