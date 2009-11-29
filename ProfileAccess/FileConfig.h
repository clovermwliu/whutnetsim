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
#define STR_SECTION_BEGIN      "["
#define CHAR_SECTION_END       ']'
#define STR_SECTION_END        "]"
#define CHAR_REMARK_LINE       '#'
#define STR_REMARK_LINE        "#"
#define REMARK_SYM             "#REMARK ON:"
#define CHAR_REM               '@'
#define STR_REM                "@"

#define CHAR_BLANK        ' '
#define STR_TAB           "\t"
#define CHAR_TAB          '\t'

#define CHAR_DASH         '-'
#define STR_DASH          "_"
#define CHAR_EQUALS       '='
#define STR_EQUALS        "="
//+

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
////Sym��FileConfig
// FileConfig ��Ķ���
//----------------------------------------------------------------------------------
class CFileConfig
{
public://��������I

	friend class CItemLine;

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig�п��ܻᱻ�������ࡣ����һ�������ָ��ɾ��һ��������Ķ���ʱ��Ϊ��֤��������������������ã���Ҫ�����������������Ϊ�麯��
    
	//���º������Զ���������ļ����д�ȡ
	int LoadFile();
	int LoadFile(int& SectionNum,int& ConfigurationItemNum);
	int UpdateFile();

public:

	//������FileConfig�����һ�����������ͣ�����������׼C++�е������ĵ�����һ��
	class iterator  
	{
	public: //���޷������������⹫�������������ڳ����ⲿʹ�ñ�����������һ��list<string>����

		friend class CFileConfig;

		iterator() {section="",pStr=NULL,iter_cur_index=NULL,iter_cur_section=NULL;}
		iterator(list<string>& vStrData); 
		iterator(list<string>& vStrData,const list<string>::iterator& SectionIter);
        //
		void begin();

       //����һ������������֧��FileConfig::iterator
		iterator& operator=(const iterator& rhs);
		friend bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		friend bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		
    	iterator& operator++();
		iterator& operator--();//prefix operators
		
		iterator& operator++(int) { return operator++(); }
		iterator& operator--(int) { return operator--(); }//postfix operators

		string operator* ();
		//
		iterator& GotoNextSection();//��++���������ƣ����ص�ǰ�����������ã�ָ����һ��section�е�һ��������
	    //operator list<string>::iterator() { return iter_cur_index; }
	
	private:
		
		bool end();	

		const string& GetCurSection() { return section; } //����secion�����ã����䱾��
		const string GetCurKey();// //����ĳһ��������=����ߵ��ַ���
		const string GetCurValue();  //����ĳһ��������=���ұߵ��ַ���

		list<string>::iterator GetCurSectionIter() {return iter_cur_section;}
		list<string>::iterator GetCurIndexIter() {return iter_cur_index;}
		list<string>::iterator GetNextIndexIter() { return iter_cur_index++; }
		
		void InsertSubstr(size_t pos, const string& substr) { (*iter_cur_index).insert(pos, substr); }
		void ReplaceCurValue(const string& value);

	private:
		void RebindingIterator(list<string>& vStrData);//���½���������������һ��list<string>��������  �����������ƻ���������Ҫ�޸ģ�
		int IsValidConfigurationLine( ); //����ǰiter_cur_index�Ƿ�ָ��һ��Configuration Line
		static bool IsBlankLine(string str);

	private:
		string section ;    //�뵱ǰiter_cur_indexָ����ı��ж�Ӧ��section���ݡ�iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
		list<string> * pStr; //��õ������󶨵�һ��˳������ָ�룬������������ǰ��д洢�������ļ�������
	    list<string>::iterator iter_cur_section;
		list<string>::iterator iter_cur_index; //��¼��ǰ��������λ�á�ÿ�ζԵ����������������Լ�����ʱ�����������ø�ֵ��iter_cur_indexֻ��ָ�����������У�����ָ����У�ע���м��ڱ����
	};


public://��������II

    CFileConfig::iterator& begin(); //����Ƕ�׵��������RebindingIterator��������iter_begָ���һ�����������iter_beg������
	CFileConfig::iterator& end();   //����Ƕ�׵��������RebindingIterator����������iter_endָ�����һ����Ч������ĺ�һ����ʵ���ϵ��ļ�β��sectionΪ���һ����Чsection����������iter_end������
	//const char* GetFileName() { return fileName.c_str(); }
	const string& GetFileName() {return fileName;}
	int GetConfigItemNum() {return _ItemNum;}
	int GetSectionNum() {return _SectionNum;}

	static bool match(const char* pLstr, const char* pRstr);
	static bool match(string lstr,string rstr);

protected: //��������

	//���º�������ά��fileData��SectionList
	bool CancelConfigLine(const string& section, 
		                  const string& key);

	void SetValue(const string& section, 
		          const string& key,
				  const string& value,
				  const string& remark);
	
	bool GetValue(const string& section,
		          const string& key,
				  string& value,
				  string& remark);
	
	list<string>::iterator AddConfigLine(const string& section, const string& line);
	list<string>::iterator SetValueWithoutRemark(const string& section, const string& key, const string& value);


	//���º�������ά��itemline_list
	void AddItemLine(CItemLine* item) { itemline_list.push_back(item); }


	//���º���������ά��SectionList


	//
	int BackupFile();//��ԭ�������ļ����ݣ���updatefile�����б��Զ�����


protected://��Ա����

	string fileName;
	int _SectionNum, _ItemNum;

	list<string> fileData;
	list<list<string>::iterator>  SectionList;// ��¼fileData�и���section���ַ����ĵ�ַ
	list<CItemLine *> itemline_list;

	iterator iter_beg,iter_end;// CFileConfig��Ĭ�ϵ�����������������CFileConfig::begin��CFileConfig::endά��

};


//----------------------------------------------------------------------------------
//Sym��CItemLine
// ConfigItem ��Ķ���
//----------------------------------------------------------------------------------

class CItemLine
{
public:

	 CItemLine(CFileConfig& file, 
		       const string& section,
		       const string& key,
		       const string& remark);

	 CItemLine(CFileConfig& file, 
		       const string& section,
		       const string& key);

	virtual ~CItemLine(void);

public:

	const string& GetSection() const { return section; }
	const string& GetKey()	 const { return key; }
	const string& GetRemark() const {return remark;}
	//��ȡvalue�ķ��������������ж��壬��ΪMyValue();
	
	bool Cancel() {remark="";return pFileCach->CancelConfigLine(section,key);}

	virtual bool ChangeValueInThisItem(const string& value) {return false;}//�麯���ӿڣ�ʵ�ʶ�����������
	
	virtual void ChangeRemarkToFile(const string& newremark) { return; }
	void ChangeRemarkInThisItem(const string& newremark) {remark=newremark;}
	
protected:
	
	void SetValueToFile(const string& newvalue,const string& newremark="") {pFileCach->SetValue(section,key,newvalue,newremark);}
	bool GetValueFromFile(string& newvalue) { return pFileCach->GetValue(section,key,newvalue,remark);}
	
protected:

	string section;
	string key;
	string remark;

	CFileConfig* pFileCach;

};

//End of CItemLine


}

#endif /*FILECONFIG_H_*/





