#include "StdAfx.h"
#include "ExpressionParse.h"

namespace WhuTNetSimConfigClass{

CExpressionParse::CExpressionParse(const std::string&  _expression,
								   const map< string, double>& _parameter_table)
: parameter_table( _parameter_table ), str_expression( _expression ),pCurrent_Char(str_expression.c_str()),
  dwCur_Value(0),Str_Cur_Identifier(""),Cur_Element_Species(BEGININI)
/*
���캯��
*/
{
	ParseElementThenGotoNext();
}

CExpressionParse::~CExpressionParse(void)
{
}


void  CExpressionParse:: ParseElementThenGotoNext()
/*
������������ǰԪ�ص����ԣ�����Cur_Element_Species��Str_Cur_Identifier��dwCur_Value��ͬʱʹpCurrent_Charָ����һ��Ԫ�ص����ַ�
*/
{
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
	
			Cur_Element_Species = NUMBER;   //�������֣�������תΪ��ֵ��ʽ��ŵ�mValue
			istringstream iss( pCurrent_Char );
			iss >>dwCur_Value;                   
			
			if( ! iss ){
				
				//throw eval_exception( "error in number format" );
			}
			pCurrent_Char += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
		}
		
		break;
	}

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

	return GetExpValueByMulOrDivExp( GetSingedValueFromSubRight() );
}

double CExpressionParse::GetExpValueByMulOrDivExp( const double& left )
/*
���������ȼ��ε͵ļ��㺯��:����˳���������*,/�ŵ���ֵ������GetSingedValueFromSubRight()�������*,/�ŵ���ֵ�����������
������left�� *,/�ŵ���ֵ
��ע��*,/���ұ��ʽ���ܴ���+/-�ţ������伫�ԣ�����ڳ˻������ǰ����ȷ����ֵ�ļ���
*/
{

	double result = left;
	if( Cur_Element_Species == MULTIPLY ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByMulOrDivExp( left * GetSingedValueFromSubRight() );
	
	}else if( Cur_Element_Species == DIVIDE ){
		ParseElementThenGotoNext(); 
		result = GetExpValueByMulOrDivExp( left / GetSingedValueFromSubRight() );
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
	}else if (Cur_Element_Species ==MULTIPLY || Cur_Element_Species ==MULTIPLY || Cur_Element_Species==POWER){ //��Щ�ȷǼ��Է��ţ��ַǲ�����ʶ�����Ӻ����ĺϷ��ַ��������֧ת������˵��ԭ���ʽ������ 5+*3
		assert(0);		
	}else{
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
	double result = GetElementValue();  //ȡ����
	if( Cur_Element_Species == POWER ){
		ParseElementThenGotoNext(); 
		result = pow( result, static_cast<int>( GetExpValueByPowerExp() ) );
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
		result = GetExpValue();                      //����һ���µı��ʽ����ͷ����
		assert(Cur_Element_Species == RIGHT_BRACKET ); //���������һ�������������ϣ�����ԭ���ʽ���Ų�ƥ��
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
	assert(Cur_Element_Species ==IDENTIFIER );
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
//		result = funcs.GetValue( identifier,param );        //����֧�ֺ��������f��������Ӻ���
	
	}else{                                                   //û�н�������˵������һ��������ʶ��
		
		if( parameter_table.find( identifier ) == parameter_table.end() ) {    //ȥ�����б���ȥ����ֵ
			
			//GUOCHI 
			result=1;                                                          //˵�������б����Ҳ����ò���������Ĭ��ֵ
		
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
            
			//GUOCHI
			assert(Cur_Element_Species!=MULTIPLY && Cur_Element_Species!=DIVIDE && Cur_Element_Species!=POWER &&
				   Cur_Element_Species!=RIGHT_BRACKET && Cur_Element_Species!=PARAMETER_SEPERATOR && Cur_Element_Species!=FINISHED);
		}
		result.push_back( GetExpValue() );//ÿ�����������Ա�������һ���ӱ��ʽ
	}
	return result;
}


}//end of ExpressionParse.cpp