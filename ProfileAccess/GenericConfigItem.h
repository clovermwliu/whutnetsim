#ifndef GENERICCONFIGITEMLINE_H_
#define GENERICCONFIGITEMLINE_H_

#include "FileConfig.h"
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
	ValueType& MyValue() { return cur_value; } //

	//method 2
	void SaveItToFile(){SetValueToFile(GetItemValueToString(cur_value),remark);}

	//method 2
	virtual void ChangeRemarkToFile(const string& newremark) {SetValueToFile(GetItemValueToString(cur_value),newremark);}


private:

	virtual bool ChangeValueInThisItem(const string& value)
	/* 
	描述：将value依照ValueType转化设置cur_value
	*/
	{
		ItemValueLoad(cur_value, value);
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
	ValueType& operator=(const ValueType& rhs) { SetValue(rhs); return MyValue(); } //赋值,e.g.  CGenericConfigItem<double> a=0.02
	
	//定义自增操作符，如果ValueType是数值，则结果+1；如果ValueType是string则什么都不做；如果ValueType是bool则取反
	ValueType & operator++() { ItemIncrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator++(int) { return operator++(); }
	//同理定义自减操作符
	ValueType & operator--() { ItemDecrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator--(int) { return operator--(); }
	//等于，不等于操作符
	bool operator==(const ValueType& rhs) const { return rhs == cur_value; }
	bool operator!=(const ValueType& rhs) const { return !(rhs == cur_value); }
	//调用操作符
	operator const ValueType() { return MyValue(); }

private:
	
	ValueType cur_value;
};

//end of CGenericConfigItem

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


template <class T>
void ItemValueLoad(T& item, const string& value)
/*
描述：将value依照item的类型T转化设置item
*/
{
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


}

#endif /*GENERICCONFIGITEMLINE_H_*/