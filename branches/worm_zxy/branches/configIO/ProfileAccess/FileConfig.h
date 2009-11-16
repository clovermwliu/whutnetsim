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
	//����source,����ÿһ��string,���ַ�������#��[  ]Ϊ��ʼ����β���ķ���true������soouce������������Ƿ��ַ�����flase
	
	ConfigCach(std::vector<std::string>& source); 

	~ConfigCach(void);

	const std::string& get_section() { return section; } //section��parse()�б�ȡ�������ú�
	const std::string get_name();// //����ĳһ��������=����ߵ��ַ���
	const std::string get_value();  //����ĳһ��������=���ұߵ��ַ���
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
	virtual ~CFileConfig(void);// CFileConfig�п��ܻᱻ�������ࡣ����һ�������ָ��ɾ��һ��������Ķ���ʱ��Ϊ��֤��������������������ã���Ҫ�����������������Ϊ�麯��

	int ReadFile();
	int ReadFile(int& Num_of_sections,int& Num_of_items);

	const char* GetFileName() { return fileName.c_str(); }


protected:

protected:

	string fileName;

};

}

#endif /*FILECONFIG_H_*/





