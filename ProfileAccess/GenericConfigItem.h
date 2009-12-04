#ifndef GENERICCONFIGITEMLINE_H_
#define GENERICCONFIGITEMLINE_H_

#include "FileConfig.h"
#include "ExpressionParse.h"
#include <sstream>

using namespace std;

namespace WhuTNetSimConfigClass{

template <typename ValueType>
class CGenericConfigItem : public CItemLine
{
public:

	CGenericConfigItem(CFileConfig& file,
	                  const string& section,
		              const string& key,
		              const string& remark,
		              const ValueType& valuedef)
	: CItemLine(file, section, key, remark)
	, cur_value(valuedef)
	{
		SetValueToFile(GetItemValueToString(cur_value),remark);
	}

	CGenericConfigItem(CFileConfig& file,
		               const string& section,
		               const string& key)
	: CItemLine(file, section, key)
	{
		if (!GetValue()) Initcur_value(cur_value);
	
	}
	
	virtual ~CGenericConfigItem() {}

   	//method 1
	ValueType& MyValue() { return cur_value; } //返回值切不能是const

	//method 2
	void SaveItToFile(){SetValueToFile(GetItemValueToString(cur_value),remark);}

	//method 3
	virtual void ChangeRemarkToFile(const string& newremark) {SetLastError(ERROR_CONFIG_ITEM_SUCCESS);SetValueToFile(GetItemValueToString(cur_value),newremark);}

	//method 4
	bool Cancel() {Initcur_value(cur_value);return _Cancel();}


private:

	virtual bool ChangeValueInThisItem(const string& value)
	/* 
	描述：将value依照ValueType转化设置cur_value
	*/
	{
		ItemValueLoad(cur_value, value);
		SetLastError(ERROR_CONFIG_ITEM_SUCCESS);
		return true;		
	}
	
	bool GetValue( )
	/* 
	描述：根据section,key的值去绑定的pFileCach中去读取value值，并按valueType类型设置cur_value，同时将该项的注释赋值给remark
	注意：section,key,remark及pFileCach均为父类ItemLine中定义
	*/
	{
		string valuestr;
		if (!GetValueFromFile(valuestr)){ //GetValueFromFile在父类ItemLine中定义
		    //Initcur_value(cur_value);
    		return false;
		}
		return ChangeValueInThisItem(valuestr);
	}

	void SetValue(const ValueType& value)
	{
		cur_value = value;
		SetValueToFile(GetItemValueToString(cur_value),remark); //SetValueToFilee在父类ItemLine中定义，GetItemValueToString()是一个模板函数
	}

    //以下重载一批操作符，以方便操作CGenericConfigItem的对象

public:
	//赋值操作符
	ValueType& operator=(const ValueType& rhs) { SetValue(rhs); return MyValue(); } //赋值,e.g.  CGenericConfigItem<double> a(...),a=0.02
	
	//定义自增操作符，如果ValueType是数值，则结果+1；如果ValueType是string则什么都不做；如果ValueType是bool则取反
	ValueType operator++() { ItemIncrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator++(int) { return operator++(); }
	//同理定义自减操作符
	ValueType operator--() { ItemDecrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator--(int) { return operator--(); }
	//等于，不等于操作符及比较操作符
	bool operator==(const ValueType& rhs) const { return rhs == cur_value; }
	bool operator!=(const ValueType& rhs) const { return !(*this ==rhs); }
	bool operator==(const CGenericConfigItem& rhs) const {return (section==rhs.section)&&(key==rhs.key)&&(remark==rhs.remark)&&(cur_value==rhs.cur_value);}
	bool operator!=(const CGenericConfigItem& rhs) const {return !(*this==rhs);}

	bool operator>(const ValueType& rhs) const {return cur_value>rhs;}
	bool operator>=(const ValueType& rhs) const {return cur_value>=rhs;}
	bool operator<(const ValueType& rhs) const {return cur_value<rhs;}
	bool operator<=(const ValueType& rhs) const {return cur_value<=rhs;}
	//调用操作符
	operator const ValueType() { return MyValue(); }//use like:  CGenericConfigItem<int> item(...); int i=item;/*i=item.MyValue()*/
	//算数运算操作符
	ValueType operator+(const ValueType& rhs) { return ItemAdd(cur_value,rhs);}
	friend ValueType operator+ (const ValueType& lhs, CGenericConfigItem& item) {return ItemAdd(lhs,item.MyValue());}
	ValueType operator-(const ValueType& rhs) { return ItemMinus(cur_value,rhs);}
	friend ValueType operator- (const ValueType& lhs, CGenericConfigItem& item) {return ItemMinus(lhs,item.MyValue());}
	ValueType operator*(const ValueType& rhs) { return ItemMul(cur_value,rhs);}
	ValueType operator/(const ValueType& rhs) { return ItemDiv(cur_value,rhs);}
	//
private:
	
	ValueType cur_value;

private:

	virtual void SetLastError(unsigned long err) {Error_code=err;}

//以下是一些模板函数，以支持CGenericConfigItem

template <class T>
void Initcur_value( T& value)
	/*
	描述：根据T的类型给value赋初值
	*/
{
	value=0;
}

void Initcur_value( string& value)
	/*
	描述：根据T的类型给value赋初值
	*/
{
	value="";
}

void Initcur_value(bool& value)
	/*
	描述：根据T的类型给value赋初值
	*/
{
	value=false;
}

void Initcur_value(CExpressionParse& value)
{
	value.Initial();
}

template <class T>
void ItemValueLoad(T& item, const string& value)
	/*
	描述：将value依照item的类型T转化设置item
	*/
{
	if (value.empty()){

		Initcur_value(item);
		return;
	}

	stringstream stream_value(value, stringstream::in);
	stream_value >> item;	
}

void ItemValueLoad(string& item, const string& value)
	/*
	描述：将value依照item的类型T转化设置item,如果item本身就是string类型则直接赋值
	*/
{
	item = value;
}

void ItemValueLoad(CExpressionParse& item, const string& value)
/*
描述：将value依照CExpressionParse的要求赋值，value是一个形如 "expression $a,$b,$c"的字符串
备注：引导字符以第一次出现的字符为准，比如 $&a,则a被认为是由$引导的，且&自动忽略
*/
{
	string str_exp;
	string tmp;
	string str_param;
	map<string,double> v;
	map<string,void*> remote_call;

	size_t pos=value.find_first_of(CHAR_SEPERATOR_2);

	if (string::npos != pos){

		str_exp=value.substr(0,pos);
		tmp=value.substr(pos+1);

		string::iterator pchar=tmp.begin();

		while (pchar!=tmp.end())
		{
			if (*pchar==CHAR_BLANK || *pchar==CHAR_TAB||*pchar==CHAR_SEPERATOR || *pchar==CHAR_SEPERATOR_2){
				
				++pchar;
				continue;
			}

			str_param.clear();

			if (*pchar==CHAR_PARAM){ // 如果是$符号引导的参数

				++pchar;
				do 
				{
					if(*pchar==CHAR_BLANK || *pchar==CHAR_TAB ||*pchar==CHAR_PARAM ||*pchar == CHAR_REMOTE_CALL_PARAM){
						++pchar;
						continue;
					}

					str_param+=*pchar;
					++pchar;
				} while (pchar!=tmp.end() && *pchar!=CHAR_SEPERATOR); //注意两个条件的顺序

				if (pchar!=tmp.end())	++pchar;

				v.insert(pair<string, double>(str_param,1));
			
			}else if (*pchar == CHAR_REMOTE_CALL_PARAM){ //如果是&符号引导的参数
			
				++pchar;
				do 
				{
					if(*pchar==CHAR_BLANK || *pchar==CHAR_TAB ||*pchar == CHAR_REMOTE_CALL_PARAM || *pchar==CHAR_PARAM ){
						++pchar;
						continue;
					}

					str_param+=*pchar;
					++pchar;
				} while (pchar!=tmp.end() && *pchar!=CHAR_SEPERATOR); //注意两个条件的顺序

				if (pchar!=tmp.end())	++pchar;

				remote_call.insert(pair<string, void*>(str_param,NULL));
						
			}else{

				++pchar; //无引导字符，忽略
			}
		
		}

	}else{
		str_exp=value;
	}

	item.Initial(str_exp,v,remote_call);
}


template <class T>
string GetItemValueToString(const T& item)
	/*
	描述：返回类型T的变量item的字符串形式（string）
	*/
{
	stringstream stream_value(std::stringstream::out);
	stream_value << item;
	return stream_value.str();
}

string GetItemValueToString(CExpressionParse& item)
{
	return item.GetExpStr();
}

template <class T>
void ItemIncrease(T& item)
	/*
	描述：自增操作，如果T是数值类型，如int,double,float等，则item结果+1
	*/
{
	++item;
}


void ItemIncrease(std::string& item)
	/*
	* 描述：；如果item是string类型则什么都不做
	*/
{
	return;

}


void ItemIncrease(bool& item)
	/*
	描述：如果item是bool类型则取反
	*/
{
	item = !item;
}

void ItemIncrease(CExpressionParse& item)
	/*
	* 描述
	*/
{
	return;

}


template <class T>
void ItemDecrease(T& item)
	/*
	描述：自减操作，如果T是数值类型，如int,double,float等，则item结果-1
	*/
{
	--item;
}



void ItemDecrease(std::string& item)
	/*
	* 描述：；如果item是string类型则什么都不做
	*/
{
	return;

}


void ItemDecrease(bool& item)
	/*
	描述：如果item是bool类型则取反
	*/
{
	item = !item;
}

void ItemDecrease(CExpressionParse& item)
	/*
	* 描述：；如果item是string类型则什么都不做
	*/
{
	return;

}



template <class T>
T ItemAdd(const T& lhs,const T& rhs)
	/*
	描述：完成两个数相加
	*/
{
	return lhs+rhs;
}

bool ItemAdd(const bool& lhs,const bool& rhs)
	/*
	描述：两个bool类型的数相加等同于这两个数异或加
	*/
{
	return (bool)lhs^rhs;
}

CExpressionParse ItemAdd(const CExpressionParse& lhs,const CExpressionParse& rhs)
	/*
	描述：两个bool类型的数相加等同于这两个数异或加
	*/
{
	return lhs;
}


template <class T>
T ItemMinus(const T& lhs,const T& rhs)
	/*
	描述：完成两个数相减
	*/
{
	return lhs-rhs;
}

string ItemMinus(const string & lhs, const string& rhs)
	/*
	描述：完成两个string的减法，即从lhs中去除 rhs
	*/
{
	string str=lhs;
	for(string::size_type pos(0);pos!=string::npos;pos+=rhs.size()){

		if ((pos=lhs.find(rhs,pos))!=string::npos){
			str.replace(pos,rhs.size(),"");
		}else{
			break;
		}
	}
	return str;

}

bool ItemMinus(const bool& lhs,const bool& rhs)
	/*
	描述：两个bool类型的数相加等同于这两个数异或后取反
	*/
{
	return !(lhs^rhs);
}

CExpressionParse ItemMinus(const CExpressionParse& lhs,const CExpressionParse& rhs)
	/*
	描述：
	*/
{
	return lhs;
}


template <class T>
T ItemMul(const T& lhs,const T& rhs)
	/*
	描述：完成两个数相乘
	*/
{
	return lhs*rhs;
}

string ItemMul(const string & lhs, const string& rhs)
	/*
	描述：完成两个string的乘法，什么都不做
	*/
{
	return lhs;
}

bool ItemMul(bool& lhs,const bool& rhs)
	/*
	描述：两个bool类型的数的乘法，什么都不做
	*/
{
	return lhs;
}

CExpressionParse ItemMul(const CExpressionParse& lhs,const CExpressionParse& rhs)
	/*
	描述：
	*/
{
	return lhs;
}

template <class T>
T ItemDiv(const T& lhs,const T& rhs)
	/*
	描述：完成两个数相除
	*/
{
	return lhs/rhs;
}

string ItemDiv(const string & lhs, const string& rhs)
	/*
	描述：完成两个string的除法，什么都不做
	*/
{
	return lhs;
}

bool ItemDiv(const bool& lhs,const bool& rhs)
	/*
	描述：两个bool类型的数的除法，什么都不做
	*/
{
	return lhs;
}

CExpressionParse ItemDiv(const CExpressionParse& lhs,const CExpressionParse& rhs)
	/*
	描述：
	*/
{
	return lhs;
}



};//end of CGenericConfigItem







}//end namespace
#endif /*GENERICCONFIGITEMLINE_H_*/