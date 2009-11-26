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
#define BLANK_LINE             0x00100000



class CFileConfig
{
public:

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig�п��ܻᱻ�������ࡣ����һ�������ָ��ɾ��һ��������Ķ���ʱ��Ϊ��֤��������������������ã���Ҫ�����������������Ϊ�麯��

	int LoadFile();
	int LoadFile(int& SectionNum,int& ConfigurationItemNum);
	int UpdateFile();

	//const char* GetFileName() { return fileName.c_str(); }
	const string& GetFileName() {return fileName;}


public:

	//������FileConfig�����һ�����������ͣ�����������׼C++�е������ĵ�����һ��
	class iterator  
	{
	public:
		
		iterator() {section="",pStr=NULL,iter_cur_index=NULL;};
		iterator(list<string>& vStrData); 

		void begin();
		bool end();


		const string& GetCurSection() { return section; } //����secion�����ã����䱾��
		const string GetCurKey();// //����ĳһ��������=����ߵ��ַ���
		const string GetCurValue();  //����ĳһ��������=���ұߵ��ַ���
		void ReplaceCurValue(const string& value);

       //����һ������������֧��FileConfig::iterator
		iterator& operator=(const iterator& rhs);
		friend bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		friend bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		
		iterator& operator++();
		iterator& operator--();//prefix operators
		
		iterator& operator++(int) { return operator++(); }
		iterator& operator--(int) { return operator--(); }//postfix operators


		string operator* ();



	
		//operator list<string>::iterator() { return iter_cur_index; }

    	void RebindingIterator(list<string>& vStrData);//���½���������������һ��list<string>��������  �����������ƻ���������Ҫ�޸ģ�

	private:
		int IsValidConfigurationLine( ); //����ǰiter_cur_index�Ƿ�ָ��һ��Configuration Line

	private:
		string section ;    //�뵱ǰiter_cur_indexָ����ı��ж�Ӧ��section���ݡ�iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
		
		list<string> * pStr; //��õ������󶨵�һ��˳������ָ�룬������������ǰ��д洢�������ļ�������
		
		list<string>::iterator iter_cur_index; //��¼��ǰ��������λ�á�ÿ�ζԵ����������������Լ�����ʱ�����������ø�ֵ��iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
	};


public:

	CFileConfig::iterator& begin(); //����Ƕ�׵��������RebindingIterator��������iter_begָ���һ�����������iter_beg������
	CFileConfig::iterator& end();   //����Ƕ�׵��������RebindingIterator����������iter_endָ�����һ������Ч�ģ������������iter_end������

protected:

	int BackupFile();

	string fileName;
	int _SectionNum, _ItemNum;    
	list<string> fileData;

	iterator iter_beg,iter_end;// CFileConfig��Ĭ�ϵ�����������������CFileConfig::begin��CFileConfig::endά��

};

}

#endif /*FILECONFIG_H_*/





