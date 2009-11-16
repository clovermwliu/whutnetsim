#pragma once
#ifndef FILECONFIG_H_
#define FILECONFIG_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

namespace WhuTNetSimConfigClass{


//Error Code
#define ERROR_FILENOTEXSITING	 0x00000000

//Config file's marker
#define CHAR_SECTION_BEGIN     '['
#define CHAR_SECTION_END       ']'
#define CHAR_REMARK_LINE       '#'
#define CHAR_REM               '@'
#define REMARK_BEGIN      "//*"
#define REMARK_END        "*//"

#define CHAR_BLANK        ' '
#define CHAR_TAB          '\t'

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
#define BLANK_LINE             0x00100000



class CFileConfig
{
public:

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig有可能会被当作基类。当用一个基类的指针删除一个派生类的对象时，为保证派生类的析构函数被调用，需要将基类的析构函数设为虚函数

	int ReadFile();
	int ReadFile(int& Num_of_sections,int& Num_of_items);

	const char* GetFileName() { return fileName.c_str(); }


protected:

	//定义了FileConfig里面的一个迭代器类型，类似其他标准C++中的容器的迭代器一样
	class iterator  
	{
	public:
		
		iterator(vector<string>& vStrData); 

		void begin();
		bool end();


		const std::string& get_section() { return section; } //section在parse()中被取出并设置好
		const std::string get_name();// //返回某一个配置行=号左边的字符串
		const std::string get_value();  //返回某一个配置行=号右边的字符串
		std::vector<std::string>::iterator get_next() { return ++index; }

		void insert(std::size_t pos, const std::string& str) { (*index).insert(pos, str); }
		void replace_value(const std::string& value);



		iterator& operator=(const iterator& rhs);
		iterator& operator++();
		iterator& operator++(int) { return operator++(); }

		operator std::vector<std::string>::iterator() { return iter_cur_index; }

	protected:
		int IsValidConfigurationLine( ); //看当前iter_cur_index是否指向一个Configuration Line
	private:
		string section ;    //与当前iter_cur_index指向的文本行对应的section内容。iter_cur_index只会指向具体的配置行，不会指向空行，注释行及节标记行
		
		list<string> * pStr; //与该迭代器绑定的一个顺序容器指针，容器里的内容是按行存储的配置文件的文字
		
		list<string>::iterator iter_cur_index; //记录当前迭代器的位置。每次对迭代器进行自增及自减操作时都会重新设置该值。iter_cur_index只会指向具体的配置行，不会指向空行，注释行及节标记行
	};


protected:

	string fileName;
    
	list<string> fileData;

};

}

#endif /*FILECONFIG_H_*/





