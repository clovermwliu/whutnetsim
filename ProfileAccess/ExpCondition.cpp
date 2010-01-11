#include "StdAfx.h"
#include "ExpCondition.h"

namespace WhuTNetSimConfigClass{

CExpCondition::CExpCondition(void)
{
}

CExpCondition::~CExpCondition(void)
{
}


void CExpCondition::ParseElementThenGotoNext()
//分析当前元素的属性,同时使pCurrent_Char指向下一个元素的首字符
{
	if (!pCurrent_Char){
		if (Error_code==ERROR_CONDITION_SUCCESS)
			SetFirstError(ERROR_CONDITION_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (Error_code==ERROR_CONDITION_SUCCESS){
				SetFirstError(ERROR_CONDITION_PRED_FORMAT_INVALID);
				//SetErrorStr(pCurrent_Char);
			}
			++pCurrent_Char;
			break;

		}
		
		if( isspace( *pCurrent_Char)) {           //当前字符是是空格、制表符或换行符，继续
			++pCurrent_Char;               
			continue;

		}else if(*pCurrent_Char ==AND && *(++pCurrent_Char)==AND){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符&&
			++pCurrent_Char ;

		
		}else if(*pCurrent_Char ==OR && *(++pCurrent_Char)==OR){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符||
			++pCurrent_Char ;

		}else if(*pCurrent_Char ==NOT ){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //发现运算符!
			++pCurrent_Char ;

		}else if( isalpha( *pCurrent_Char ) ){

			Cur_Element_Species = PREDICATION_ITEM;          
			Str_Cur_PredicationItem.clear();

			do {
				Str_Cur_PredicationItem += (*pCurrent_Char);
				++pCurrent_Char;

				if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

					if (Error_code==ERROR_CONDITION_SUCCESS){
						SetFirstError(ERROR_CONDITION_PRED_FORMAT_INVALID);
						//SetErrorStr(pCurrent_Char);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //取字母+数字的混合表达式，直到既非字母又非数字的符号结束

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //遇到字符串结束符则结束

		}
		
		break;
	}


}


}//end
