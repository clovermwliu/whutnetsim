#pragma once
#ifndef FILECONFIG_H_
#define FILECONFIG_H_

#include <string>
//#include <vector>
#include <list>
#include <fstream>
#include <iostream>

using namespace std;

namespace WhuTNetSimConfigClass{


//----------------------------------------------------------------------------------
//Sym：CItemLine
// ConfigItem 类的定义
//----------------------------------------------------------------------------------

class CFileConfig;

class CItemLine
{
public:

	CItemLine(CFileConfig& file, 
		        const std::string& section,
				const std::string& key);
	
	virtual ~CItemLine(void);

public:

	const string& GetSection() const { return section; }
	const string& GetKey()	 const { return key; }

protected:

	string section;
	string key;
	
    CFileConfig* FileCach;



};

//End of CItemLine

//Error Code
#define ERROR_FILE_NOT_EXSITING	 0x00000000
#define SUCCESS_NO_ERROR         0x00000001
#define ERROR_FILE_WRITE_FAIL    0x00000002

//Config file's marker
#define CHAR_SECTION_BEGIN     '['
#define CHAR_SECTION_END       ']'
#define CHAR_REMARK_LINE       '#'
#define CHAR_REM               '@'
#define REMARK_BEGIN      "//*"
#define REMARK_END        "*//"

#define CHAR_BLANK        ' '
#define CHAR_TAB          "\t"

#define CHAR_DASH         '-'
#define CHAR_EQUALS       '='
//

/*config file sample

[section] //section line

key=value //configuration line

#This is ... //remark line

@key=value   // cancel configuration line


*/

#define CONFIGURATION_LINE     0x00000001
#define SECTION_LINE           0x00000010
#define REMAK_LINE             0x00000100
#define CANCEL_LINE            0x00001000
#define FILE_END_LINE          0x00010000
#define FILE_BEGIN_LINE        0x00100000
#define BLANK_LINE             0x01000000


//----------------------------------------------------------------------------------
////Sym：FileConfig
// FileConfig 类的定义
//----------------------------------------------------------------------------------
class CFileConfig
{
public:

	friend class CItemLine;

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig有可能会被当作基类。当用一个基类的指针删除一个派生类的对象时，为保证派生类的析构函数被调用，需要将基类的析构函数设为虚函数
    //
	int LoadFile();
	int LoadFile(int& SectionNum,int& ConfigurationItemNum);
	int UpdateFile();

public:

	//定义了FileConfig里面的一个迭代器类型，类似其他标准C++中的容器的迭代器一样
	class iterator  
	{
	public:

		friend class CFileConfig;

		iterator() {section="",pStr=NULL,iter_cur_index=NULL,iter_cur_section=NULL;}
		iterator(list<string>& vStrData); 
        //
		void begin();

       //重载一批操作符，以支持FileConfig::iterator
		iterator& operator=(const iterator& rhs);
		friend bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		friend bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		
    	iterator& operator++();
		iterator& operator--();//prefix operators
		
		iterator& operator++(int) { return operator++(); }
		iterator& operator--(int) { return operator--(); }//postfix operators

		string operator* ();
		//
		iterator& GotoNextSection();//与++操作符类似，返回当前迭代器的引用，指向下一个section中第一个配置项
	    //operator list<string>::iterator() { return iter_cur_index; }
	private:
		bool end();	

		const string& GetCurSection() { return section; } //返回secion的引用，即其本身
		list<string>::iterator GetCurSectionIter() {return iter_cur_section;}
		const string GetCurKey();// //返回某一个配置行=号左边的字符串
		const string GetCurValue();  //返回某一个配置行=号右边的字符串
		void ReplaceCurValue(const string& value);

	private:
		void RebindingIterator(list<string>& vStrData);//重新将本迭代器对象与一个list<string>的容器绑定  （这个函数设计还不合理，还要修改）
		int IsValidConfigurationLine( ); //看当前iter_cur_index是否指向一个Configuration Line
		bool IsBlankLine(string str);

	private:
		string section ;    //与当前iter_cur_index指向的文本行对应的section内容。iter_cur_index只会指向具体的配置行，不会指向空行，注释行及节标记行
		list<string> * pStr; //与该迭代器绑定的一个顺序容器指针，容器里的内容是按行存储的配置文件的文字
	    list<string>::iterator iter_cur_section;
		list<string>::iterator iter_cur_index; //记录当前迭代器的位置。每次对迭代器进行自增及自减操作时都会重新设置该值。iter_cur_index只会指向具体的配置行，不会指向空行，注释行及节标记行
	};


public:

    CFileConfig::iterator& begin(); //调用嵌套迭代器类的RebindingIterator函数，将iter_beg指向第一个配置项，返回iter_beg的引用
	CFileConfig::iterator& end();   //调用嵌套迭代器类的RebindingIterator函数，并将iter_end指向最后一个有效配置项的后一个（实际上的文件尾，section为最后一个有效section），并返回iter_end的引用
	//const char* GetFileName() { return fileName.c_str(); }
	const string& GetFileName() {return fileName;}
	int GetConfigItemNum() {return _ItemNum;}
	int GetSectionNum() {return _SectionNum;}

protected:

	int BackupFile();

	string fileName;
	int _SectionNum, _ItemNum;

	list<string> fileData;
	list<list<string>::iterator>  SecionList;// 记录fileData中各个section行字符串的地址

	iterator iter_beg,iter_end;// CFileConfig内默认的两个迭代器对象，由CFileConfig::begin和CFileConfig::end维护

};

}

#endif /*FILECONFIG_H_*/





