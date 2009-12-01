#ifndef CPP_EVAL_H_DFF520DB406EDCF31AB9A538F7E1C3BD_20040721__
#define CPP_EVAL_H_DFF520DB406EDCF31AB9A538F7E1C3BD_20040721__

#include <cmath>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <numeric>


using namespace std;

namespace cpp_eval
{

//template function declaration
template< typename number >
number eval( const char* expression );

template< typename number >
number eval( const char* expression, const map< string, number >& variables );

template< typename number, typename functions >
number eval( const char* expression, const map< string, number >& variables, functions& funcs );

//Class eval_exception
class eval_exception: public ::std::logic_error
{
public:
	eval_exception( const char* desc ): logic_error(desc)
	{}
};


//Class dummy_functions
template< typename number >
class dummy_functions
{
public:
	number operator()( const char*, const ::std::vector< number >& params )
	{
		throw eval_exception( "unexisting function called" );
		// return number();

	}
};


//template function implement
template< typename number >
number eval( const char* expression )
{
	::std::map< ::std::string, number > variables;
	dummy_functions<number> funcs;
	return eval( expression, variables, funcs );
}

template< typename number >
number eval( const char* expression, const map< string, number >& variables )
{
	dummy_functions<number> funcs;
	return eval( expression, variables, funcs );
}

template< typename number, typename functions >
number eval( const char* expression, const ::std::map< ::std::string, number >& variables, functions& funcs )
{
	return evaler<number, functions>( variables, funcs )( expression );
}


//Class evaler
template< typename number, typename functions >
class evaler
{
	const map< string, number >& mVariables;
	
	functions& mFuncs;
	
	const char* mCurrent;
	
	enum Type
	{
		ADD_OR_POSITIVE = '+',
		SUBTRACT_OR_NEGATIVE = '-',
		MULTIPLY = '*',
		DIVIDE = '/',
		POWER = '~',
		LEFT_BRACKET = '(',
		RIGHT_BRACKET = ')',
		PARAMETER_SEPERATOR = ',',
		IDENTIFIER = 257,
		NUMBER = 258,
		FINISHED = 259
	};
	Type mType;
	
	std::string mIdentifier;
	
	number mValue;
	
	void look_ahead()
	{
		for(;;)
		{
			if( isspace( *mCurrent ) ) //���ֿո�
			{
				++mCurrent;
				continue;
			}
			else if( *mCurrent == '*' && *( mCurrent + 1 ) == '*' ) //���ַ���**
				mType = POWER, mCurrent += 2;
			else if( *mCurrent == ADD_OR_POSITIVE || *mCurrent == SUBTRACT_OR_NEGATIVE || //��������������Ż�,��
				*mCurrent == MULTIPLY || *mCurrent == DIVIDE ||
				*mCurrent == LEFT_BRACKET || *mCurrent == RIGHT_BRACKET ||
				*mCurrent == PARAMETER_SEPERATOR )
				mType = ( Type )*mCurrent, ++mCurrent;
			else if( isalpha( *mCurrent ) )  //�ж��ַ� *mCurrent �Ƿ�ΪӢ����ĸ����ΪӢ����ĸa-z��A-Zʱ�����ط���ֵ�����򷵻���
			{
				mType = IDENTIFIER;
				mIdentifier.clear();
				mIdentifier += *mCurrent;
				++mCurrent;
				while( isalpha( *mCurrent ) || isdigit( *mCurrent ) )
					mIdentifier += *mCurrent, ++mCurrent;               //ȥ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��ţ��������������
			}
			else if( *mCurrent == 0)//��Ϊʲô����0����
				mType = FINISHED;
			else                   //��������
			{
				mType = NUMBER;
				istringstream iss( mCurrent );
				iss >> mValue;                   //������תΪ��ֵ��ʽ��ŵ�mValue
				if( ! iss )
					throw eval_exception( "error in number format" );
				mCurrent += iss.rdbuf()->pubseekoff( 0, ios::cur, ios::in );
			}
			break;
		}
	}


	void match( Type type )  //Type��ǰ�����ö������
	{
		if( mType == type )
			look_ahead();
		else
			throw eval_exception( "unmatched token" );
	}


	number expression()
	{//expression -> higher_expression expression_R

		number result = expression_R( higher_expression() );
		return result;
	}

	number expression_R( const number& left )
	{//expression_R -> + higher_expression expression_R | - higher_expression expression_R | /e/

		number result = left;
		if( mType == ADD_OR_POSITIVE )
			match( ADD_OR_POSITIVE ), result = expression_R( left + higher_expression() );
		else if( mType == SUBTRACT_OR_NEGATIVE )
			match( SUBTRACT_OR_NEGATIVE ), result = expression_R( left - higher_expression() );
		return result;
	}
	number higher_expression()
	{//higher_expression -> sign_expression higher_expression_R

		return higher_expression_R( sign_expression() );
	}
	number higher_expression_R( const number& left )
	{//higher_expression_R -> * sign_expression higher_expression_R | / sign_expression higher_expression_R | /e/

		number result = left;
		if( mType == MULTIPLY )
			match( MULTIPLY ), result = higher_expression_R( left * sign_expression() );
		else if( mType == DIVIDE )
			match( DIVIDE ), result = higher_expression_R( left / sign_expression() );
		return result;
	}
	number sign_expression()
	{//sign_expression -> + sign_expression | - sign_expression | power_expression

		number result;
		if( mType == ADD_OR_POSITIVE )
			match( ADD_OR_POSITIVE ), result = sign_expression();
		else if( mType == SUBTRACT_OR_NEGATIVE )
			match( SUBTRACT_OR_NEGATIVE ), result = - sign_expression();
		else if (mType ==MULTIPLY || mType ==MULTIPLY || mType==POWER)
			throw eval_exception( "invaild expression" );		
		else
			result = power_expression();
		return result;
	}
	number power_expression()
	{//power_expression -> factor ** power_expression | factor

		number result = factor();  //ȡ��ǰ���ӱ��ʽ��ֵ
		if( mType == POWER )
			match( POWER ), result = std::pow( result, static_cast<int>( power_expression() ) );
		return result;
	}
	number factor()
	{//factor -> number | ( expression ) | lang_structure

		number result;
		if( mType == NUMBER )
			result = mValue, match( NUMBER );
		else if( mType == LEFT_BRACKET )
			match( LEFT_BRACKET ), result = expression(), match( RIGHT_BRACKET ); //������(�����ȼ�������()�ڱ��ʽ��ֵ
		else
			result = lang_structure();
		return result;
	}
	number lang_structure()
	{//lang_structure -> identifier lang_tail

		std::string id = mIdentifier;
		match( IDENTIFIER );
		return lang_tail( id );
	}
	number lang_tail( const std::string& id )
	{//lang_tail -> ( parameter_list | /e/

		number result;
		if( mType == LEFT_BRACKET )    //���һ�����ʽȡ�귢�ֺ������һ������˵������һ���Ӻ���
		{
			match( LEFT_BRACKET );
			std::vector<number> param = parameter_list();   
			result = mFuncs( id.c_str(), param );           //ת������Ӻ���
		}
		else                                                //û�н�������˵������һ����������
		{
			if( mVariables.find( id ) == mVariables.end() )     //ȥ�����б���ȥ����ֵ
				throw eval_exception( "cannot find variable" );
			result = mVariables.find( id )->second;
		}
		return result;
	}
	std::vector<number> parameter_list()
	{//parameter_list -> ) | expression parameter_tail

		std::vector<number> result;
		for(;;)
		{
			if( mType == RIGHT_BRACKET )
			{
				match( RIGHT_BRACKET );
				break;
			}
			result.push_back( expression() );
			parameter_tail( result );
		}
		return result;
	}
	void parameter_tail( std::vector<number>& param )
	{//parameter_tail -> , expression parameter_tail | /e/

		if( mType == PARAMETER_SEPERATOR )
			match( PARAMETER_SEPERATOR ), param.push_back( expression() ), parameter_tail( param );
	}


public:
	evaler( const ::std::map< ::std::string, number >& variables, functions& funcs )
		: mVariables( variables ), mFuncs( funcs )
	{}
	number operator()( const char* expr )
	{
		mCurrent = expr;
		look_ahead();
		number result = expression();
		if( mType != FINISHED )
			throw eval_exception( "unexpect expression format" );
		return result;
	}
};

//

class functions
{
public:
	int operator()( const char* name, const ::std::vector< double >& params )
	{
		if( stricmp( name, "return1" ) == 0 )
		{
			if( params.size() != 0 )
				throw 1;
			return 1;
		}
		else if( stricmp( name, "negative" ) == 0 )
		{
			if( params.size() != 1 )
				throw 1;
			return - params[ 0 ];
		}
		else if( stricmp( name, "sum" ) == 0 )
		{
			return accumulate( params.begin(), params.end(), 0 );
		}
	}
};

};

#endif
