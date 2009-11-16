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
	virtual ~CFileConfig(void);// CFileConfig�п��ܻᱻ�������ࡣ����һ�������ָ��ɾ��һ��������Ķ���ʱ��Ϊ��֤��������������������ã���Ҫ�����������������Ϊ�麯��

	int ReadFile();
	int ReadFile(int& Num_of_sections,int& Num_of_items);

	const char* GetFileName() { return fileName.c_str(); }


protected:

	//������FileConfig�����һ�����������ͣ�����������׼C++�е������ĵ�����һ��
	class iterator  
	{
	public:
		
		iterator(vector<string>& vStrData); 

		void begin();
		bool end();


		const std::string& get_section() { return section; } //section��parse()�б�ȡ�������ú�
		const std::string get_name();// //����ĳһ��������=����ߵ��ַ���
		const std::string get_value();  //����ĳһ��������=���ұߵ��ַ���
		std::vector<std::string>::iterator get_next() { return ++index; }

		void insert(std::size_t pos, const std::string& str) { (*index).insert(pos, str); }
		void replace_value(const std::string& value);



		iterator& operator=(const iterator& rhs);
		iterator& operator++();
		iterator& operator++(int) { return operator++(); }

		operator std::vector<std::string>::iterator() { return iter_cur_index; }

	protected:
		int IsValidConfigurationLine( ); //����ǰiter_cur_index�Ƿ�ָ��һ��Configuration Line
	private:
		string section ;    //�뵱ǰiter_cur_indexָ����ı��ж�Ӧ��section���ݡ�iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
		
		list<string> * pStr; //��õ������󶨵�һ��˳������ָ�룬������������ǰ��д洢�������ļ�������
		
		list<string>::iterator iter_cur_index; //��¼��ǰ��������λ�á�ÿ�ζԵ����������������Լ�����ʱ�����������ø�ֵ��iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
	};


protected:

	string fileName;
    
	list<string> fileData;

};

}

#endif /*FILECONFIG_H_*/





