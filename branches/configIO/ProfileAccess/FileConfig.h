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
#define SECTION_BEGIN     "["
#define SECTION_END       "]"
#define REMARK_LINE       "#"
#define REM               "@"
#define REMARK_BEGIN      "//*"
#define REMARK_END        "*//"

#define CHAR_BLANK        ' '
#define CHAR_TAB          '\t'

#define CHAR_DASH         '-'
#define CHAR_EQUALS       '='



class ConfigCach
{
public:
	//遍历source,解析每一个string,空字符串或以#及[  ]为开始（结尾）的返回true。否则soouce遍历完或遇到非法字符返回flase
	
	ConfigCach(std::vector<std::string>& source); 

	~ConfigCach(void);

	const std::string& get_section() { return section; } //section在parse()中被取出并设置好
	const std::string get_name();// //返回某一个配置行=号左边的字符串
	const std::string get_value();  //返回某一个配置行=号右边的字符串
	std::vector<std::string>::iterator get_next() { return ++index; }

	void insert(std::size_t pos, const std::string& str) { (*index).insert(pos, str); }
	void replace_value(const std::string& value);

	void begin() { index = data->begin(); section.erase(); if (parse()) operator++();}
	bool end() { return (data->end() == index); }

	ConfigCach& operator=(const ConfigCach& rhs);
	ConfigCach& operator++();
	ConfigCach& operator++(int) { return operator++(); }
	operator std::vector<std::string>::iterator() { return index; }

protected:
	bool parse();

private:
	std::string section;
	std::vector<std::string>* data;
	std::vector<std::string>::iterator index;
};

	//end of ConfigCach




class CFileConfig
{
public:

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig有可能会被当作基类。当用一个基类的指针删除一个派生类的对象时，为保证派生类的析构函数被调用，需要将基类的析构函数设为虚函数

	int ReadFile();
	int ReadFile(int& Num_of_sections,int& Num_of_items);

	const char* GetFileName() { return fileName.c_str(); }


protected:

protected:

	string fileName;

};

}

#endif /*FILECONFIG_H_*/





