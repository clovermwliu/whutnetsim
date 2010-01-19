#include "StdAfx.h"
#include "FileScript.h"
#include "ExpCondition.h"


CExpCondition::CExpCondition(void)
:Cur_Element_Species(BEGININI),Str_Cur_PredicationItem(""),str_error_exp(""),str_conditon(""),id(0),CErrorHandler()
{
}


CExpCondition::CExpCondition(string condstr,
			                 const map<string,CPredicationItem>& t)

:str_conditon(condstr),pred_table(t),Cur_Element_Species(BEGININI),
 pCurrent_Char(str_conditon.c_str()),Str_Cur_PredicationItem(""),str_error_exp(""),id(0),CErrorHandler()
{
	ParseElementThenGotoNext();
}

CExpCondition::~CExpCondition(void)
{
}


void CExpCondition::Initial(string condstr,
			                const map<string,CPredicationItem>& t,
							int idnum)
/*
*/
{
	str_conditon=condstr;
	pred_table=t;
	id=idnum;
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
}


CExpCondition::CExpCondition(const CExpCondition& c)
/*

*/
{
	pred_table=c.pred_table;
	Cur_Element_Species=c.Cur_Element_Species;
	SetLastError(ERROR_CONDITION_SUCCESS);	
	SetLastErrorStr("");

	Str_Cur_PredicationItem=c.Str_Cur_PredicationItem;
	str_error_exp=c.str_error_exp;
	id=c.id;
	if (!c.str_conditon.empty()){

		str_conditon=c.str_conditon;
		pCurrent_Char=str_conditon.c_str();
		ParseElementThenGotoNext();
	}

}

CExpCondition& CExpCondition::operator=(const CExpCondition& rhs)
/*
����ʹ��pCurrent_Charָ�룬���ʽ��������Ҫ�ض�λ��ָ�� 
*/
{
	
	pred_table=rhs.pred_table;
	Cur_Element_Species=rhs.Cur_Element_Species;
	SetLastError(ERROR_CONDITION_SUCCESS);	
	SetLastErrorStr("");
	Str_Cur_PredicationItem=rhs.Str_Cur_PredicationItem;
	str_error_exp=rhs.str_error_exp;
	str_error_exp=rhs.str_error_exp;
	id=rhs.id;

	if (!rhs.str_conditon.empty()){

		str_conditon=rhs.str_conditon;
		pCurrent_Char=str_conditon.c_str();
		ParseElementThenGotoNext();
	}
	return *this;
}

void CExpCondition::ParseElementThenGotoNext()
//������ǰԪ�ص�����,ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
{
	//if (!pCurrent_Char){
	//	if (error_code==ERROR_CONDITION_SUCCESS)
	//		SetFirstError(ERROR_CONDITION_NO_EXP);
	//	Cur_Element_Species = FINISHED;
	//	return;
	//}

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (GetLastError()==ERROR_CONDITION_SUCCESS){
				SetLastError(ERROR_CONDITION_PRED_FORMAT_INVALID);
				//SetErrorStr(pCurrent_Char);
			}
			++pCurrent_Char;
			break;

		}
		
		if( isspace( *pCurrent_Char)) {           //��ǰ�ַ����ǿո��Ʊ�����з�������
			++pCurrent_Char;               
			continue;

		}else if(*pCurrent_Char ==AND && *(pCurrent_Char+1)==AND){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������&&
			++pCurrent_Char ;
			++pCurrent_Char ;
		
		}else if(*pCurrent_Char ==OR && *(pCurrent_Char+1)==OR){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������||
			++pCurrent_Char ;
			++pCurrent_Char ;

		}else if(*pCurrent_Char ==NOT ){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  //���������!
			++pCurrent_Char ;

		}else if (*pCurrent_Char ==LEFT_BRACKET || *pCurrent_Char ==RIGHT_BRACKET){

			Cur_Element_Species = ( ElementSpecies )*pCurrent_Char;  
			++pCurrent_Char ;


		}else if( isalpha( *pCurrent_Char ) || isdigit(  *pCurrent_Char )||CFileScript::isValidSymbol(*pCurrent_Char)){

			Cur_Element_Species = PREDICATION_ITEM;          
			Str_Cur_PredicationItem.clear();

			do {
				Str_Cur_PredicationItem += (*pCurrent_Char);
				++pCurrent_Char;

				if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

					if (GetLastError()==ERROR_CONDITION_SUCCESS){
						SetLastError(ERROR_CONDITION_PRED_FORMAT_INVALID);
						SetErrorStr(pCurrent_Char-1);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )||CFileScript::isValidSymbol(*pCurrent_Char)); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���

		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //�����ַ��������������

		}else{

			if (GetLastError()==ERROR_CONDITION_SUCCESS){
				SetLastError(ERROR_CONDITION_OPERATOR_INVALID);
				SetErrorStr(pCurrent_Char-1);
			}
			++pCurrent_Char;
		}
		
		break;
	}


}

Error_str  CExpCondition::GetLastErrorEx()
/*
������ȡ���ʽ�������ĵ�һ������ԭ�򣬷�����ԭ���ı�����

#define  ERROR_CONDITION_SUCCESS			          0x00001000
#define  ERROR_CONDITION_NO_EXP                       0x00001001
#define  ERROR_CONDITION_MISSING_OPERATOR             0x00001002
#define  ERROR_CONDITION_OPERATOR_INVALID             0x00001003
#define  ERROR_CONDITION_PRED_FORMAT_INVALID          0x00001004
#define  ERROR_CONDITION_MISSING_RIGHT_BRACKET        0x00001005
#define  ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST       0x00001006
#define  ERROR_CONDITION_MISSING_PREDITEM             0x00001007

*/
{
	switch (GetLastError())
	{
	case ERROR_CONDITION_SUCCESS:
		return "SUCCESS";
	case ERROR_CONDITION_NO_EXP:
		return "No condition expression in this object";
	case ERROR_CONDITION_MISSING_OPERATOR:
		return "Missing operator.  Near by "+str_error_exp;
	case ERROR_CONDITION_PRED_FORMAT_INVALID:
		return "PRED's format error.  Near by "+str_error_exp;
	case  ERROR_CONDITION_OPERATOR_INVALID:
		return "Operator invalid. Near by "+str_error_exp;
	case ERROR_CONDITION_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by "+str_error_exp;
	case ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST:
		return "Condition Expression includes invalid predication identify.  Near by "+str_error_exp;
	case ERROR_CONDITION_MISSING_PREDITEM : 
		return "Missing predication item.  Near by "+str_error_exp;
	default:
		return GetLastErrorStr();
	}

}

void CExpCondition::SetErrorStr (const char* p)
/*

*/
{
	str_error_exp.clear();
	
	const char* q=p;

	int i=0;

	while ((q-i)!=str_conditon.c_str() && (isalpha( *(q-i)) || isdigit( *(q-i) ))){

		i++;
	}
	
	string s(q-i);
	str_error_exp=s;
}


bool CExpCondition::GetConditionValue()

//ȡ�������ʽ��ֵ����һ���ݹ麯�������
{
	if (str_conditon.empty()){

		SetLastError(ERROR_CONDITION_NO_EXP);
		return false;
	}
	
	pCurrent_Char=str_conditon.c_str();
	ParseElementThenGotoNext();
	SetLastError(ERROR_CONDITION_SUCCESS);
	SetLastErrorStr("");
	str_error_exp.clear();
	
	bool bresult = GetCondExpValueByOR( GetCondExpValueFromSubRight() );


	if (Cur_Element_Species != FINISHED && GetLastError()==ERROR_CONDITION_SUCCESS){

		SetLastError(ERROR_CONDITION_MISSING_OPERATOR);
		SetErrorStr(pCurrent_Char-1);
	}



	return bresult;

}

bool CExpCondition::GetSubCondExpValue()
/*
*/
{
	if (GetLastError()==ERROR_CONDITION_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
	bool bresult = GetCondExpValueByOR( GetCondExpValueFromSubRight() );
	return bresult;

}


bool  CExpCondition::GetCondExpValueByOR( const bool& left )
// ���ȼ���͵ļ��㺯��:���������
{
	bool result = left;
	if( Cur_Element_Species == OR ){
		ParseElementThenGotoNext(); 
		result = GetCondExpValueByOR(  GetCondExpValueFromSubRight() || left );
	}
	return result;
}

bool  CExpCondition::GetCondExpValueFromSubRight()
//���||���Ҳ���ʽ��ֵ
{
	if (GetLastError()==ERROR_CONDITION_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
	return GetCondExpValueByAND( GetCondExpValueByNOT() );

}

bool  CExpCondition::GetCondExpValueByAND( const bool& left )
//���ȼ��ε͵ļ��㺯��:����������
{
	bool result = left;
	if( Cur_Element_Species == AND ){
		ParseElementThenGotoNext(); 
		result = GetCondExpValueByAND( GetCondExpValueByNOT() && left );
	}
	return result;

}

bool  CExpCondition::GetCondExpValueByNOT()
//��ñ��ʽ��ֵ,���������
{
	bool result;
	if( Cur_Element_Species == NOT ){
		ParseElementThenGotoNext(); 
		result = !GetCondExpValueByNOT();
	}else if (Cur_Element_Species ==AND || Cur_Element_Species ==OR ){ //��Щ��ȡ��������ţ��ַ�ν�ʱ�ʶ���������֧ת������˵��ԭ���ʽ������ 5&&||3

		if (GetLastError()==ERROR_CONDITION_SUCCESS){
			SetLastError(ERROR_CONDITION_OPERATOR_INVALID);
		}

		ParseElementThenGotoNext(); 
		result = GetCondExpValueByNOT();	//��������ͬʱ��¼�쳣ֵ	
	}else{

		if (GetLastError()==ERROR_CONDITION_SUCCESS)
			SetErrorStr(pCurrent_Char-1);
		result = GetElementValue();
	}
	return result;
}

bool  CExpCondition::GetElementValue()
/*
��������õ�ǰ���ʽԪ�ص�ֵ��������()��ʶ���ӱ��ʽ��ν�ʱ�ʶ��
*/
{
	bool result;

	if( Cur_Element_Species == PREDICATION_ITEM ){
		result = ParseCurPredItem();
	}else if( Cur_Element_Species == LEFT_BRACKET ){ //������(�����ȼ�������()�ڱ��ʽ��ֵ

		ParseElementThenGotoNext();

		//����һ���µı��ʽ����ͷ����
		result=GetSubCondExpValue();

		if (Cur_Element_Species != RIGHT_BRACKET && GetLastError()==ERROR_CONDITION_SUCCESS)//���������һ�������������ϣ�����ԭ���ʽ���Ų�ƥ��
			SetLastError(ERROR_CONDITION_MISSING_RIGHT_BRACKET);
		
		ParseElementThenGotoNext();	

	}else{

		if (GetLastError()==ERROR_CONDITION_SUCCESS){
			SetLastError(ERROR_CONDITION_MISSING_PREDITEM);
			SetErrorStr(pCurrent_Char-1);
		}
		result=false;
	}
	return result;
}

bool  CExpCondition::ParseCurPredItem()
/*

*/
{
	bool result;
	string PredIdentify = Str_Cur_PredicationItem;
	ParseElementThenGotoNext();	

	if( pred_table.find( PredIdentify) == pred_table.end() ) {    

		if (GetLastError()==ERROR_CONDITION_SUCCESS)
			SetLastError(ERROR_CONDITION_PREDIDENTIFY_NOT_EXIST);

		result=false;   //˵���Ҳ����ò���������Ĭ��ֵ

	}else{ 

		CPredicationItem tmp=pred_table.find( PredIdentify)->second;
		
		result = tmp.GetValue();  //ȥ�����б���ȡ����ֵ

		if (GetLastError()==ERROR_CONDITION_SUCCESS && tmp.GetLastError()!=ERROR_PRED_SUCCESS){

			SetLastErrorStr("");
			SetLastError(tmp.GetLastError());
			Error_str e_str=PredIdentify+":";
		    e_str=e_str+tmp.GetLastErrorEx();
			SetLastErrorStr(e_str);
		}
	}

    return result;

}



//end
