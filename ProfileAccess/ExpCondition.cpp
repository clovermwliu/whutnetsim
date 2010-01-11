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
//������ǰԪ�ص�����,ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
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
		
		if( isspace( *pCurrent_Char)) {           //��ǰ�ַ����ǿո��Ʊ�����з�������
			++pCurrent_Char;               
			continue;

		}else if(*pCurrent_Char ==AND && *(++pCurrent_Char)==AND){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������&&
			++pCurrent_Char ;

		
		}else if(*pCurrent_Char ==OR && *(++pCurrent_Char)==OR){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������||
			++pCurrent_Char ;

		}else if(*pCurrent_Char ==NOT ){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������!
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
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //�����ַ��������������

		}
		
		break;
	}


}


}//end
