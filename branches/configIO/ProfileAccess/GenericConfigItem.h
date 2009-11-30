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
	ValueType& MyValue() { return cur_value; } //����ֵ�в�����const

	//method 2
	void SaveItToFile(){SetValueToFile(GetItemValueToString(cur_value),remark);}

	//method 3
	virtual void ChangeRemarkToFile(const string& newremark) {SetValueToFile(GetItemValueToString(cur_value),newremark);}

	//method 4
	bool Cancel() {Initcur_value(cur_value);return _Cancel();}


private:

	virtual bool ChangeValueInThisItem(const string& value)
	/* 
	��������value����ValueTypeת������cur_value
	*/
	{
		ItemValueLoad(cur_value, value);
		return true;		
	}
	
	bool GetValue( )
	/* 
	����������section,key��ֵȥ�󶨵�pFileCach��ȥ��ȡvalueֵ������valueType��������cur_value��ͬʱ�������ע�͸�ֵ��remark
	ע�⣺section,key,remark��pFileCach��Ϊ����ItemLine�ж���
	*/
	{
		string valuestr;
		if (!GetValueFromFile(valuestr)){ //GetValueFromFile�ڸ���ItemLine�ж���
		    //Initcur_value(cur_value);
    		return false;
		}
		return ChangeValueInThisItem(valuestr);
	}

	void SetValue(const ValueType& value)
	{
		cur_value = value;
		SetValueToFile(GetItemValueToString(cur_value),remark); //SetValueToFilee�ڸ���ItemLine�ж��壬GetItemValueToString()��һ��ģ�庯��
	}

    //��������һ�����������Է������CGenericConfigItem�Ķ���

public:
	//��ֵ������
	ValueType& operator=(const ValueType& rhs) { SetValue(rhs); return MyValue(); } //��ֵ,e.g.  CGenericConfigItem<double> a(...),a=0.02
	
	//�������������������ValueType����ֵ������+1�����ValueType��string��ʲô�����������ValueType��bool��ȡ��
	ValueType operator++() { ItemIncrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator++(int) { return operator++(); }
	//ͬ�����Լ�������
	ValueType operator--() { ItemDecrease(cur_value); SetValue(cur_value); return  MyValue(); }
	ValueType operator--(int) { return operator--(); }
	//���ڣ������ڲ��������Ƚϲ�����
	bool operator==(const ValueType& rhs) const { return rhs == cur_value; }
	bool operator!=(const ValueType& rhs) const { return !(*this ==rhs); }
	bool operator==(const CGenericConfigItem& rhs) const {return (section==rhs.section)&&(key==rhs.key)&&(remark==rhs.remark)&&(cur_value==rhs.cur_value);}
	bool operator!=(const CGenericConfigItem& rhs) const {return !(*this==rhs);}

	bool operator>(const ValueType& rhs) const {return cur_value>rhs;}
	bool operator>=(const ValueType& rhs) const {return cur_value>=rhs;}
	bool operator<(const ValueType& rhs) const {return cur_value<rhs;}
	bool operator<=(const ValueType& rhs) const {return cur_value<=rhs;}
	//���ò�����
	operator const ValueType() { return MyValue(); }//use like:  CGenericConfigItem<int> item(...); int i=item;/*i=item.MyValue()*/
	//�������������
	ValueType operator+(const ValueType& rhs) { return ItemAdd(cur_value,rhs);}
	friend ValueType operator+ (const ValueType& lhs, CGenericConfigItem& item) {return ItemAdd(lhs,item.MyValue());}
	ValueType operator-(const ValueType& rhs) { return ItemMinus(cur_value,rhs);}
	friend ValueType operator- (const ValueType& lhs, CGenericConfigItem& item) {return ItemMinus(lhs,item.MyValue());}
	ValueType operator*(const ValueType& rhs) { return ItemMul(cur_value,rhs);}
	ValueType operator/(const ValueType& rhs) { return ItemDiv(cur_value,rhs);}
	//
private:
	
	ValueType cur_value;

};//end of CGenericConfigItem


//������һЩģ�庯������֧��CGenericConfigItem

template <class T>
void Initcur_value( T& value)
/*
����������T�����͸�value����ֵ
*/
{
	value=0;
}

void Initcur_value( string& value)
/*
����������T�����͸�value����ֵ
*/
{
	value="";
}

void Initcur_value(bool& value)
/*
����������T�����͸�value����ֵ
*/
{
	value=false;
}


template <class T>
void ItemValueLoad(T& item, const string& value)
/*
��������value����item������Tת������item
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
��������value����item������Tת������item,���item�������string������ֱ�Ӹ�ֵ
*/
{
	item = value;
}

template <class T>
string GetItemValueToString(const T& item)
/*
��������������T�ı���item���ַ�����ʽ��string��
*/
{
	stringstream stream_value(std::stringstream::out);
	stream_value << item;
	return stream_value.str();
}
template <class T>
void ItemIncrease(T& item)
/*
�������������������T����ֵ���ͣ���int,double,float�ȣ���item���+1
*/
{
	++item;
}


void ItemIncrease(std::string& item)
/*
* �����������item��string������ʲô������
*/
{
	return;

}


void ItemIncrease(bool& item)
/*
���������item��bool������ȡ��
*/
{
	item = !item;
}


template <class T>
void ItemDecrease(T& item)
/*
�������Լ����������T����ֵ���ͣ���int,double,float�ȣ���item���-1
*/
{
	--item;
}


void ItemDecrease(std::string& item)
/*
* �����������item��string������ʲô������
*/
{
	return;

}


void ItemDecrease(bool& item)
/*
���������item��bool������ȡ��
*/
{
	item = !item;
}

template <class T>
T ItemAdd(const T& lhs,const T& rhs)
/*
������������������
*/
{
	return lhs+rhs;
}

bool ItemAdd(const bool& lhs,const bool& rhs)
/*
����������bool���͵�����ӵ�ͬ��������������
*/
{
	return (bool)lhs^rhs;
}

template <class T>
T ItemMinus(const T& lhs,const T& rhs)
/*
������������������
*/
{
	return lhs-rhs;
}

string ItemMinus(const string & lhs, const string& rhs)
/*
�������������string�ļ���������lhs��ȥ�� rhs
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
����������bool���͵�����ӵ�ͬ��������������ȡ��
*/
{
	return !(lhs^rhs);
}

template <class T>
T ItemMul(const T& lhs,const T& rhs)
/*
������������������
*/
{
	return lhs*rhs;
}

string ItemMul(const string & lhs, const string& rhs)
/*
�������������string�ĳ˷���ʲô������
*/
{
	return lhs;
}

bool ItemMul(bool& lhs,const bool& rhs)
/*
����������bool���͵����ĳ˷���ʲô������
*/
{
	return lhs;
}

template <class T>
T ItemDiv(const T& lhs,const T& rhs)
/*
������������������
*/
{
	return lhs/rhs;
}

string ItemDiv(const string & lhs, const string& rhs)
/*
�������������string�ĳ�����ʲô������
*/
{
	return lhs;
}

bool ItemDiv(const bool& lhs,const bool& rhs)
/*
����������bool���͵����ĳ�����ʲô������
*/
{
	return lhs;
}





}//end namespace
#endif /*GENERICCONFIGITEMLINE_H_*/