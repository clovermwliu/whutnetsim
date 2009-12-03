#include "StdAfx.h"
#include "ExpressionParse.h"

namespace WhuTNetSimConfigClass{


//-----------------------------------------------------------------------------------------
// CExpressionParse��ʵ��
//-----------------------------------------------------------------------------------------

CExpressionParse::CExpressionParse()
/*
���캯��
*/
{

}

	
CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),Error_code(ERROR_EXP_SUCCESS),str_error_exp("")
/*
���캯��
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}

void CExpressionParse::Initial()
/*
��������ʼ������
*/
{
	parameter_table.clear();
	str_expression="";
	pCurrent_Char=NULL;
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	Error_code=ERROR_EXP_SUCCESS;
	str_error_exp="";

	ParseElementThenGotoNext();
}


void CExpressionParse::Initial(const std::string&  _expression ,
			                   const map< string, double>& _parameter_table)
/*
��������ʼ������
*/
{
	parameter_table= _parameter_table;
	str_expression= _expression;
	pCurrent_Char=str_expression.c_str();
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	Error_code=ERROR_EXP_SUCCESS;
	str_error_exp="";
	
	ParseElementThenGotoNext();

}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
������������ǰԪ�ص����ԣ�����Cur_Element_Species��Str_Cur_Identifier��dwCur_Value��ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
*/
{
	if (!pCurrent_Char){
		if (Error_code==ERROR_EXP_SUCCESS)
			SetFirstError(ERROR_EXP_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}

	while(true){
		
		if( isspace( *pCurrent_Char) ) {
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
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���
			            
		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //�����ַ��������������
		
		}else{                             
	
			Cur_Element_Species = NUMBER;   //�������֣�������תΪ��ֵ��ʽ��ŵ�mValue������������ .3������С��
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				SetFirstError(ERROR_EXP_NUMBER_FORMAT_INVALID);
				SetErrorStr(pCurrent_Char);
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}
		
		break;
	}

}

string CExpressionParse::GetFirstErrorEx()
/*
������ȡ���ʽ�������ĵ�һ������ԭ�򣬷�����ԭ���ı�����

#define  ERROR_EXP_SUCCESS			                 0x00000000
#define  ERROR_EXP_DIVISOR_IS_ZERO                   0x00000001
#define  ERROR_EXP_SIGN_UNKNOWN                      0x00000002
#define  ERROR_EXP_INVAILD_PAPAMETER                 0x00000003
#define  ERROR_EXP_MISSING_RIGHT_BRACKET             0x00000004
#define  ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS   0x00000005
#define  ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS     0x00000006
#define  ERROR_EXP_USE_NONSUPPORT_FUNCS              0x00000007
#define  ERROR_EXP_CALL_SUBFUNCS_FAIL                0x00000008
#define  ERROR_EXP_NO_EXP                            0x00000009
#define  ERROR_EXP_MISSING_OPERATOR                  0x0000000a
#define  ERROR_EXP_NUMBER_FORMAT_INVALID             0x0000000b

*/
{
	switch (Error_code)
	{
	case ERROR_EXP_SUCCESS:
		return "SUCCESS";
	case ERROR_EXP_DIVISOR_IS_ZERO:
		return "Divisor is zero. Near by:"+str_error_exp;
	case ERROR_EXP_SIGN_UNKNOWN:
		return "Sign of expression is unknown.  Near by:"+str_error_exp;
	case ERROR_EXP_INVAILD_PAPAMETER:
		return "Including invalid parameter.  Near by"+str_error_exp;
	case ERROR_EXP_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by"+str_error_exp;
	case ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS:
		return "Expression includes reserve character.  Near by"+str_error_exp;
	case ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS:
		return "Sub-Functions includes invalid parameters.  Near by"+str_error_exp;
	case ERROR_EXP_USE_NONSUPPORT_FUNCS:
		return "Including non-support sub-functions.  Near by"+str_error_exp;
	case ERROR_EXP_CALL_SUBFUNCS_FAIL:
		return "Parameters dose not match on sub-functions's request @:"+str_error_exp;
	case ERROR_EXP_NO_EXP:
		return "No expression in this object";
	case ERROR_EXP_MISSING_OPERATOR:
		return "Missing operator.  Near by"+str_error_exp;
	case ERROR_EXP_NUMBER_FORMAT_INVALID:
		return "Number format error.  Near by"+str_error_exp;
	default:
		return "UNKNOWN_ERROR";
	}

}



bool CExpressionParse::SetParamValue(const string& param, double value)
/*
���������²����б��ֵ
*/
{
	map<string,double>::iterator iter=parameter_table.find( param);
	if( iter == parameter_table.end() )//û���������
		return false;

	iter->second=value;

	return true;

}




double CExpressionParse::GetExpValue()
/*
����������һ�����ʽ��ֵ����һ���ݹ麯�������
��ע�������������������������
      (1) �����ı��ʽ����һ�ν���ʱ��
	  (2) һ��������( )�ڵ��ӱ��ʽʱ��see to GetElementValue()
	  (3) һ���Ӻ����Ĳ�����һ�ӱ��ʽʱ��see to GetParameterValueForSupportFuncs()
*/
{

	double result = GetExpValueByAddOrMinusExp( GetExpValueFromSubRight() );
	return result;
}

double CExpressionParse::GetExpValueByAddOrMinusExp( const double& left )
/*
���������ȼ���͵ļ��㺯��:����Ӽ���������+/-�ŵ���ֵ������GetExpValueFromSubRight()�������+/-�ŵ���ֵ������ͻ��
������left�� +/-�ŵ���ֵ
*/
{
	double result = left;
	if( Cur_Element_Species == ADD_OR_POSITIVE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByAddOrMinusExp( left + GetExpValueFromSubRight() );
	}else if( Cur_Element_Species == SUBTRACT_OR_NEGATIVE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByAddOrMinusExp( left - GetExpValueFromSubRight() );
	}
	return result;
}
double CExpressionParse::GetExpValueFromSubRight()
/*
���������+/- ���Ҳ���ʽ��ֵ
*/
{

	if (Error_code==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char);
	}
	return GetExpValueByMulOrDivExp( GetSingedValueFromSubRight() );
}

double CExpressionParse::GetExpValueByMulOrDivExp( const double& left )
/*
���������ȼ��ε͵ļ��㺯��:����˳���������*,/�ŵ���ֵ������GetSingedValueFromSubRight()�������*,/�ŵ���ֵ�����������
������left�� *,/�ŵ���ֵ
��ע��*,/���ұ��ʽ���ܴ���+/-�ţ������伫�ԣ�����ڳ˻������ǰ����ȷ����ֵ�ļ���
      �������Ϊ0�򷵻ؼ�����Ϊ1.#INF
*/
{

	double result = left;
	if( Cur_Element_Species == MULTIPLY ){
		ParseElementThenGotoNext();

		if (Error_code==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char);
		}

		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 

#define DIVZERO 

#ifdef DIVZERO
	
		if (Error_code==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char);
		}
		
		double dwDiv=GetSingedValueFromSubRight();
		if(dwDiv ==0 && Error_code==ERROR_EXP_SUCCESS){
			SetFirstError(ERROR_EXP_DIVISOR_IS_ZERO);
		}
		result = GetExpValueByMulOrDivExp( left / dwDiv );
#else
		result = GetExpValueByMulOrDivExp( left / GetSingedValueFromSubRight() );
#endif
		
	}
	return result;
}
double CExpressionParse::GetSingedValueFromSubRight()
/*
��������ñ��ʽ��ֵ�������伫��,��Ϊ�˳�������ֵ
*/
{
	double result;
	if( Cur_Element_Species == ADD_OR_POSITIVE ){
		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();
	}else if( Cur_Element_Species == SUBTRACT_OR_NEGATIVE ){
		ParseElementThenGotoNext(); 
		result = - GetSingedValueFromSubRight();
	}else if (Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==DIVIDE || Cur_Element_Species==POWER){ //��Щ�ȷǼ��Է��ţ��ַǲ�����ʶ�����Ӻ����ĺϷ��ַ��������֧ת������˵��ԭ���ʽ������ 5+*3
		
		if (Error_code==ERROR_EXP_SUCCESS){
			SetFirstError(ERROR_EXP_SIGN_UNKNOWN);
		}

		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();	//����+�Ŵ���ͬʱ��¼�쳣ֵ	
	}else{

		if (Error_code==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char);
		result = GetExpValueByPowerExp();
	}
	return result;
}
double CExpressionParse::GetExpValueByPowerExp()
/*
���������ȼ���ߵļ��㺯��:�����ݡ����� GetElementValue()��õ�����ֵ���ݹ���û��ָ����ֵ
��ע��ָ����Ȼ�������ݵ���ʽ����a^b^c���ݹ���ù����ջ���ȼ���b^c���ڼ���a^b^c��
      ���ȡ�õ�������һ�����������^������ζ�Ÿõ�����1���ݣ���ݹ���ڡ���ʵ�ϣ�����+,-,*,/������κ�һ�������������Կ����ݱ��ʽ��ʽ
	  �������������֣���()��ʶ���ӱ��ʽ��������ʶ�����Ӻ���
      
*/
{
	if (Error_code==ERROR_EXP_SUCCESS)
		SetErrorStr(pCurrent_Char);

	double result = GetElementValue();  //ȡ����
	
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 

		result = pow( result, GetExpValueByPowerExp());
	}
	return result;
}
double CExpressionParse::GetElementValue()
/*
��������õ�ǰ���ʽԪ�ص�ֵ�����������֣���()��ʶ���ӱ��ʽ��������ʶ�����Ӻ���
*/
{
	double result;
	if( Cur_Element_Species == NUMBER ){
		result = dwCur_Value;
		ParseElementThenGotoNext();	
	}else if( Cur_Element_Species == LEFT_BRACKET ){ //������(�����ȼ�������()�ڱ��ʽ��ֵ

		ParseElementThenGotoNext();

		if (Error_code==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char);

		result = GetExpValue();                      //����һ���µı��ʽ����ͷ����
		if (Cur_Element_Species != RIGHT_BRACKET && Error_code==ERROR_EXP_SUCCESS)//���������һ�������������ϣ�����ԭ���ʽ���Ų�ƥ��
			SetFirstError(ERROR_EXP_MISSING_RIGHT_BRACKET);
		ParseElementThenGotoNext();	
	}else{

		result = ParseCurIdentifier();
	}
	return result;
}

double CExpressionParse::ParseCurIdentifier()
/*
����������������ʶ�����Ӻ�����
*/
{
	if(Cur_Element_Species !=IDENTIFIER && Error_code==ERROR_EXP_SUCCESS ){
		SetFirstError(ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS); 
		SetErrorStr(pCurrent_Char);
	}
	string str = Str_Cur_Identifier;
	ParseElementThenGotoNext();	
	return GetValueFromCurIdentifier( str);
}


double CExpressionParse::GetValueFromCurIdentifier( const string& identifier )
/*
�������Ӳ����б�parameter_table�л�ò�����ʶ����ֵ
��ע�����һ���ַ������(����������ַ������ǲ�����ʶ���������Ӻ���������������"sin("��˵��sin��һ���Ӻ���
*/
{
	double result;
	if( Cur_Element_Species == LEFT_BRACKET )               //���һ�����ʽȡ�귢�ֺ������һ������˵������һ���Ӻ���
	{
		ParseElementThenGotoNext();	
		vector<double> param = GetParameterValueForSupportFuncs();   
       
		//GUOCHI
		result = GetValueFromCurSubFunc( identifier,param );        //����֧�ֺ����⴦������Ӻ���
	
	}else{                                                   //û�н�������˵������һ��������ʶ��
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    //ȥ�����б���ȥ����ֵ
			
			if (Error_code==ERROR_EXP_SUCCESS)
				SetFirstError(ERROR_EXP_INVAILD_PAPAMETER);
			result=DEFAULT_VALUE;                                              //˵�������б����Ҳ����ò���������Ĭ��ֵ
		
		}else{
			result = parameter_table.find( identifier )->second;
		}
		
	}
	return result;
}


vector<double> CExpressionParse::GetParameterValueForSupportFuncs()
/*
����������Ӻ�����Ҫ�ĸ���������ֵ
��ע���Ӻ�����ÿ�����������Ա�������һ���ӱ��ʽ���𲽼�������ӱ��ʽ��ֵ���������vector����
*/
{	vector<double> result;
	while( true)
	{
		if( Cur_Element_Species == RIGHT_BRACKET ){  //���������ű����Ӻ��������б��������ѭ����һ�μ�ת��˷�֧����������Ӻ�����������

			ParseElementThenGotoNext();	
			break;
		}

		if( Cur_Element_Species == PARAMETER_SEPERATOR ){

			ParseElementThenGotoNext();
 			if(Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==DIVIDE || Cur_Element_Species ==POWER ||
		       Cur_Element_Species==RIGHT_BRACKET || Cur_Element_Species==PARAMETER_SEPERATOR || Cur_Element_Species==FINISHED){
					if (Error_code==ERROR_EXP_SUCCESS)
						SetFirstError(ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS );
			}
		}
		result.push_back( GetExpValue() );//ÿ�����������Ա�������һ���ӱ��ʽ
	}
	return result;
}


double  CExpressionParse::GetValueFromCurSubFunc(const string& name, const vector< double >& params)
/*

*/
{
	if (name== "sum" || name== "SUM"){

		return accumulate( params.begin(), params.end(), (double)0 );
	
	}else if (name=="exp" || name=="EXP"){

		if (params.size()>1 || params.size()==0){
			if(Error_code==ERROR_EXP_SUCCESS){

				SetFirstError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		return exp(params[0]);
	}

	if(Error_code==ERROR_EXP_SUCCESS)
		SetFirstError(ERROR_EXP_USE_NONSUPPORT_FUNCS);

	return DEFAULT_VALUE; //Ĭ��ֵ����
}



}//end of ExpressionParse.cpp