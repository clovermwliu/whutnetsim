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
//������ǰԪ�ص�����,ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
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
			++pCurrent_Char;               //��ǰ�ַ����ǿո��Ʊ�����з�������
			continue;

		}else if( *pCurrent_Char == ADD_OR_POSITIVE || *pCurrent_Char == SUBTRACT_OR_NEGATIVE || 
			*pCurrent_Char == MULTIPLY || *pCurrent_Char == DIVIDE ||*pCurrent_Char == POWER ||
			*pCurrent_Char == LEFT_BRACKET || *pCurrent_Char == RIGHT_BRACKET ||
			*pCurrent_Char == PARAMETER_SEPERATOR ){

				Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //��������������Ż�","
				++pCurrent_Char ;

		}else if( isalpha( *pCurrent_Char ) ){

			Cur_Element_Species = IDENTIFIER;            //��ǰ����Ӣ����ĸ����ΪӢ����ĸa-z��A-Zʱ�����ط���ֵ�����򷵻���
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
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //�����ַ��������������

		}else{                             

			Cur_Element_Species = NUMBER;   //�������֣�������תΪ��ֵ��ʽ��ŵ�mValue������������ .3������С��
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
