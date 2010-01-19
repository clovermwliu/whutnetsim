#include "StdAfx.h"
#include "FileScript.h"
#include "ExpressionParse.h"


//-----------------------------------------------------------------------------------------
// CExpressionParse��ʵ��
//-----------------------------------------------------------------------------------------

CExpressionParse::CExpressionParse()
/*
���캯��
*/
:dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),CErrorHandler()
{

}

	
CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table,
								   const map< string,void*>& _remote_call_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI),CErrorHandler(),
  remote_call_addrs_table( _remote_call_table)
/*
���캯��
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}


CExpressionParse::CExpressionParse(const CExpressionParse& rhs)
/*
����ʹ��pCurrent_Charָ�룬���ʽ��������Ҫ�ض�λ��ָ�� 
*/
{

	parameter_table=rhs.parameter_table;
	remote_call_addrs_table=rhs.remote_call_addrs_table;
	dwCur_Value=rhs.dwCur_Value;
	Str_Cur_Identifier=rhs.Str_Cur_Identifier;
	Cur_Element_Species=rhs.Cur_Element_Species;
	SetLastError(ERROR_EXP_SUCCESS);	
	SetLastErrorStr("");

	if (!rhs.str_expression.empty()){

		str_expression=rhs.str_expression;
		pCurrent_Char=str_expression.c_str();
		ParseElementThenGotoNext();
	}

}


CExpressionParse& CExpressionParse::operator=(const CExpressionParse& rhs)
/*
����ʹ��pCurrent_Charָ�룬���ʽ��������Ҫ�ض�λ��ָ�� 
*/
{
	parameter_table=rhs.parameter_table;
	remote_call_addrs_table=rhs.remote_call_addrs_table;
	dwCur_Value=rhs.dwCur_Value;
	Str_Cur_Identifier=rhs.Str_Cur_Identifier;
	Cur_Element_Species=rhs.Cur_Element_Species;
	SetLastError(ERROR_EXP_SUCCESS);	
	SetLastErrorStr("");
	if (!rhs.str_expression.empty()){

		str_expression=rhs.str_expression;
		pCurrent_Char=str_expression.c_str();
		ParseElementThenGotoNext();
	}
	return *this;
}


void CExpressionParse::clear()
/*
��������ʼ������
*/
{
	parameter_table.clear();
	remote_call_addrs_table.clear();
	str_expression="";
	pCurrent_Char=NULL;
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");

	//ParseElementThenGotoNext();
}


void CExpressionParse::Initial(const std::string&  _expression ,
			                   const map< string, double>& _parameter_table,
							   const map< string,void*>& _remote_call_table)
/*
��������ʼ������
*/
{
	parameter_table= _parameter_table;
	remote_call_addrs_table= _remote_call_table;
	str_expression= _expression;
	pCurrent_Char=str_expression.c_str();
	dwCur_Value=0;
	Str_Cur_Identifier="";
	Cur_Element_Species=BEGININI;
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");

	ParseElementThenGotoNext();

}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
������������ǰԪ�ص����ԣ�����Cur_Element_Species��Str_Cur_Identifier��dwCur_Value��ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
*/
{
	/*if (!pCurrent_Char){
		if (error_code==ERROR_EXP_SUCCESS)
			SetLastError(ERROR_EXP_NO_EXP);
		Cur_Element_Species = FINISHED;
		return;
	}*/

	assert(pCurrent_Char);

	while(true){

		if (static_cast<int>(*pCurrent_Char)>256 || static_cast<int>(*pCurrent_Char)<0){

			if (GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
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

					if (GetLastError()==ERROR_EXP_SUCCESS){
						SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
						SetErrorStr(pCurrent_Char);
					}
					++pCurrent_Char;
					break;

				}
			} while(isalpha(  *pCurrent_Char ) || isdigit(  *pCurrent_Char )|| CFileScript::isValidSymbol(*pCurrent_Char)); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���
			            
		}else if( *pCurrent_Char == 0){
			Cur_Element_Species = FINISHED; //�����ַ��������������
		
		}else{                             
	
			Cur_Element_Species = NUMBER;   //�������֣�������תΪ��ֵ��ʽ��ŵ�mValue������������ .3������С��
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				if (GetLastError()==ERROR_EXP_SUCCESS){
					SetLastError(ERROR_EXP_NUMBER_FORMAT_INVALID);
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

Error_str CExpressionParse::GetLastErrorEx()
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
#define  ERROR_EXP_OVERFLOW                          0x0000000c

*/
{
	switch (GetLastError())
	{
	case ERROR_EXP_SUCCESS:
		return "SUCCESS";
	case ERROR_EXP_DIVISOR_IS_ZERO:
		return "Divisor is zero. Near by:"+GetLastErrorStr();
	case ERROR_EXP_SIGN_UNKNOWN:
		return "Sign of expression is unknown.  Near by "+GetLastErrorStr();
	case ERROR_EXP_INVAILD_PAPAMETER:
		return "Expression includes invalid parameter.  Near by "+GetLastErrorStr();
	case ERROR_EXP_MISSING_RIGHT_BRACKET:
		return "Missing right bracket.  Near by "+GetLastErrorStr();
	case ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS:
		return "Expression includes reserve character.  Near by "+GetLastErrorStr();
	case ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS:
		return "Sub-Functions includes invalid parameters.  Near by "+GetLastErrorStr();
	case ERROR_EXP_USE_NONSUPPORT_FUNCS:
		return "Expression includes  non-support sub-function:"+GetLastErrorStr();
	case ERROR_EXP_CALL_SUBFUNCS_FAIL:
		return "Parameters don't match on sub-functions's request @:"+GetLastErrorStr();
	case ERROR_EXP_NO_EXP:
		return "No expression in this object";
	case ERROR_EXP_MISSING_OPERATOR:
		return "Missing operator.  Near by "+GetLastErrorStr();
	case ERROR_EXP_NUMBER_FORMAT_INVALID:
		return "Expression's format error.  Near by "+GetLastErrorStr();
	case  ERROR_EXP_OVERFLOW:
		return "Overflow!";
	default:
		return "UNKNOWN_ERROR";
	}

}

void CExpressionParse::SetErrorStr (const char* p)
/*

*/
{
	SetLastErrorStr("");
	const char* q=p;

	int i=0;

	while ((q-i)!=str_expression.c_str() && (isalpha( *(q-i)) || isdigit( *(q-i) ))){

		i++;
	}

	string s(q-i);
	SetLastErrorStr(s);
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

bool CExpressionParse::SetRemoteCallAddrs(const string& param, void* p)
/*
����������Զ�̵��ú����ĵ�ַ
*/
{
	map<string,void*>::iterator iter=remote_call_addrs_table.find( param);
	if( iter == remote_call_addrs_table.end() )//û���������
		return false;

	iter->second=p;

	return true;

}

int   CExpressionParse::GetCommonParamName(vector<string>& param_name)
/*
��������õ�ǰ���ʽ�������ͨ����������������
������[OUT] param_name������ʼ���õĲ�����
*/
{
	map<string,double>::iterator iter=parameter_table.begin();

	while (iter!=parameter_table.end()){

		param_name.push_back(iter->first);
		++iter;
		
	}

	return static_cast<int> (parameter_table.size());


}

int  CExpressionParse:: GetRemoteParamName(vector<string>& param_name)
/*
��������õ�ǰ���ʽ�����Զ�̸�ֵ����������������
������[OUT] param_name������ʼ���õ�Զ�̸�ֵ������
*/
{

	map<string,void*>::iterator iter=remote_call_addrs_table.begin();

	while (iter!=remote_call_addrs_table.end()){

		param_name.push_back(iter->first);
		++iter;

	}

	return static_cast<int> (remote_call_addrs_table.size());


}



double CExpressionParse::GetExpValue()
/*
����������һ�����ʽ��ֵ����һ���ݹ麯�������
��ע�������������������������
      (1) �����ı��ʽ����һ�ν���ʱ��
*/
{

	if (str_expression.empty()){

		SetLastError(ERROR_EXP_NO_EXP);
		return DEFAULT_VALUE;
	}
		
	pCurrent_Char=str_expression.c_str();
	ParseElementThenGotoNext();
	SetLastError(ERROR_EXP_SUCCESS);
	SetLastErrorStr("");
	
	double result = GetExpValueByAddOrMinusExp( GetExpValueFromSubRight() );

	if (Cur_Element_Species != FINISHED && GetLastError()==ERROR_EXP_SUCCESS){

		SetLastError(ERROR_EXP_MISSING_OPERATOR);
		SetErrorStr(pCurrent_Char-1);
	}

	return result;
}

double CExpressionParse::GetSubExpValue()
/*
����������һ�����ʽ��ֵ����һ���ݹ麯�������
��ע�������������������������
     (1) һ��������( )�ڵ��ӱ��ʽʱ��see to GetElementValue()
     (2) һ���Ӻ����Ĳ�����һ�ӱ��ʽʱ��see to GetParameterValueForSupportFuncs()
*/

{
	if (GetLastError()==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
	}
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

	if (GetLastError()==ERROR_EXP_SUCCESS){
		SetErrorStr(pCurrent_Char-1);
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

		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char-1);
		}

		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 

#define DIVZERO 

#ifdef DIVZERO
	
		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetErrorStr(pCurrent_Char-1);
		}
		
		double dwDiv=GetSingedValueFromSubRight();
		if(dwDiv ==0 && GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_DIVISOR_IS_ZERO);
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
		
		if (GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_SIGN_UNKNOWN);
		}

		ParseElementThenGotoNext(); 
		result = GetSingedValueFromSubRight();	//����+�Ŵ���ͬʱ��¼�쳣ֵ	
	}else{

		if (GetLastError()==ERROR_EXP_SUCCESS)
			SetErrorStr(pCurrent_Char-1);
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
	if (GetLastError()==ERROR_EXP_SUCCESS)
		SetErrorStr(pCurrent_Char-1);

	double result = GetElementValue();  //ȡ����
	
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 

		result = pow( result, GetSingedValueFromSubRight());
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

		//result = GetExpValue();                      //����һ���µı��ʽ����ͷ����
		result=GetSubExpValue();

		if (Cur_Element_Species != RIGHT_BRACKET && GetLastError()==ERROR_EXP_SUCCESS)//���������һ�������������ϣ�����ԭ���ʽ���Ų�ƥ��
			SetLastError(ERROR_EXP_MISSING_RIGHT_BRACKET);
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
	if(Cur_Element_Species !=IDENTIFIER && GetLastError()==ERROR_EXP_SUCCESS ){
		SetLastError(ERROR_EXP_IDENTIFIER_INCLUDE_RESERVECHARS); 
		SetErrorStr(pCurrent_Char-1);
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
       
		
		result = GetValueFromCurSubFunc( identifier,param );        //����֧�ֺ����⴦������Ӻ���
	
	}else{                                                   //û�н�������˵������һ��������ʶ��
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    
			
			if (remote_call_addrs_table.find(identifier) == remote_call_addrs_table.end()){  

				if (GetLastError()==ERROR_EXP_SUCCESS)
					SetLastError(ERROR_EXP_INVAILD_PAPAMETER);

				result=DEFAULT_VALUE;   //˵�������б���ͳ̺������ñ���Ҳ����ò���������Ĭ��ֵ
 			
			}else{

				pFunGet pRemoteFuncs =static_cast<pFunGet>(remote_call_addrs_table.find(identifier)->second);  //ȥԶ�̺������ñ����Զ�̺�����ò���ֵ
     			result=pRemoteFuncs();

			}

		}else{ 
			result = parameter_table.find( identifier )->second;                  //ȥ�����б���ȡ����ֵ
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
					
				   if (GetLastError()==ERROR_EXP_SUCCESS)
						SetLastError(ERROR_EXP_INVAILD_PAPAMETER_IN_SUBFUNCS );
				   
				   continue;
					
			}
		}

		if (Cur_Element_Species == FINISHED){  //����ȱ��������

			if (GetLastError()==ERROR_EXP_SUCCESS)
				SetLastError(ERROR_EXP_MISSING_RIGHT_BRACKET );

			break;
		}
		//result.push_back( GetExpValue() );//ÿ�����������Ա�������һ���ӱ��ʽ
		result.push_back(GetSubExpValue());
	}
	return result;
}


double  CExpressionParse::GetValueFromCurSubFunc(const string& name, const vector< double >& params)
/*

*/
{	
	if (name== "sum" || name== "SUM"){

		return accumulate( params.begin(), params.end(), (double)0 );
	
	}else if(name == "avg" || name == "AVG" || name == "mean" || name == "MEAN"){

		
		if (params.size()!=0){

			return accumulate( params.begin(), params.end(), (double)0 )/static_cast<double>(params.size());
		
		}else{

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;

		}
		
	}else if(name == "min"|| name=="MIN"){

		
		if (params.size()==0){

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;
		}
		
		vector<double>::const_iterator it=params.begin();
		double result=params[0];

		while (it!=params.end()){

			if (result>(*it)) result=*it;
			++it;
		}
        
		return  result;
	
	}else if(name == "max"|| name=="MAX"){


		if (params.size()==0){

			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}

			return DEFAULT_VALUE;
		}

		vector<double>::const_iterator it=params.begin();
		double result=params[0];

		while (it!=params.end()){

			if (result<(*it)) result=*it;
			++it;
		}

		return  result;
	
	}else if (name == "factorial" || name == "FACTORIAL" || name == "fa"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return DEFAULT_VALUE;
		
		double dwresult= DEFAULT_VALUE;
		double i= floor(params[0]);

		while(  i> 1 ){
			dwresult *= i;
			i -= 1;
		}

		return dwresult;

	}else if (name == "sqrt" || name == "SQRT"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return exp((double)DEFAULT_VALUE);

		return sqrt(params[0]);

	}else if (name=="exp" || name=="EXP"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0) return exp((double)DEFAULT_VALUE);
		
		return exp(params[0]);
	
	}else if (name == "logXY" || name == "LOGXY"){

		if (params.size()!=2 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==1 )	return log(params[0]);
		
		if (params.size()==0 )	return DEFAULT_VALUE;
	
		return log(params[0])/log(params[1]);

	}else if (name == "ln" || name == "log"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;

		return log(params[0]);

	}else if (name == "lg" || name == "log10"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return log10(params[0]);

	}else if (name == "sin" || name == "SIN"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return sin(params[0]);

	}else if (name == "asin" || name == "ASIN"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return asin(params[0]);

	}else if (name == "cos" || name == "COS"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return cos(params[0]);

	}else if (name == "acos" || name == "ACOS"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return acos(params[0]);

	}else if (name == "tan" || name == "tg" || name == "TAN" || name=="TG"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return tan(params[0]);

	}else if (name == "atan" || name == "atg" || name == "ATAN" || name=="ATG"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return atan(params[0]);

	}else if (name == "pi" || name == "PI"){
		
		if (params.size()!=0 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		return (double)3.1415926535897932384626433832795; 

	}else if (name == "abs" || name == "ABS"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return abs(params[0]);
	
	}else if (name == "floor" || name == "FLOOR")	{

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return floor(params[0]);

	}else if (name == "ceil" || name == "CEIL")	{

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==0)  return DEFAULT_VALUE;
		return ceil(params[0]);

	}else if (name == "mod" || name == "MOD")	{

		if (params.size()!=2){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}

		if (params.size()==1)  return params[0];
		if (params.size()==0)  return DEFAULT_VALUE;

		return fmod(params[0],params[1]);

	}else if (name == "sinh" || name == "SINH"){

		if (params.size()!=1){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return sinh(params[0]);

	}else if (name == "cosh" || name == "COSH"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return cosh(params[0]);

	}else if (name == "tanh" || name == "tgh" || name == "TANH" || name=="TGH"){

		if (params.size()!=1 ){
			if(GetLastError()==ERROR_EXP_SUCCESS){
				SetLastError(ERROR_EXP_CALL_SUBFUNCS_FAIL);
				SetErrorStr(name.c_str());
			}
		}
		if (params.size()==0)  return DEFAULT_VALUE;
		return tanh(params[0]);

	}else{

		if(GetLastError()==ERROR_EXP_SUCCESS){
			SetLastError(ERROR_EXP_USE_NONSUPPORT_FUNCS);
			SetLastErrorStr(name);
		}

		return DEFAULT_VALUE; //Ĭ��ֵ����

	}

	
}



//end of ExpressionParse.cpp