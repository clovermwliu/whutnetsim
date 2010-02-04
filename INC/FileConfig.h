#ifndef FILECONFIG_H_
#define FILECONFIG_H_


#include <string>
#include <list>
#include <fstream>
#include <iostream>

#include "ErrorHandler.h"

using namespace std;


//Error Code
#define SUCCESS_NO_ERROR         ERROR_NO_ERROR
#define ERROR_FILE_NOT_EXSITING	 0x00010001
#define ERROR_FILE_WRITE_FAIL    0x00010002

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
#define CHAR_PARAM             '$'
#define STR_PARAM              "$"
#define CHAR_REMOTE_CALL_PARAM '&'
#define STR_REMOTE_CALL_PARAM  "&"

#define CHAR_BLANK        ' '
#define STR_TAB           "\t"
#define CHAR_TAB          '\t'
#define STR_EQUALS        "="

#define CHAR_SEPERATOR    ','
#define STR_SEPERATOR     ","
#define CHAR_SEPERATOR_2  ':'
#define STR_SEPERATOR_2   ":"



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
class CFileConfig :public CErrorHandler
{
public://��������I

	friend class CItemLine;

	CFileConfig(const string& strFilePath);
	virtual ~CFileConfig(void);// CFileConfig�п��ܻᱻ�������ࡣ����һ�������ָ��ɾ��һ��������Ķ���ʱ��Ϊ��֤��������������������ã���Ҫ�����������������Ϊ�麯��
    
	//���º������Զ���������ļ����д�ȡ
	bool LoadFile();
	bool LoadFile(int& SectionNum,int& ConfigurationItemNum);
	bool UpdateFile(bool bIngronCancelLine=true);

public:	//������FileConfig�����һ�����������ͣ�����������׼C++�е������ĵ�����һ��
	class iterator  
	{
	public: //���޷������������⹫�������������ڳ����ⲿʹ�ñ�����������һ��list<string>����

		friend class CFileConfig;

		iterator() {section="",pStr=NULL;}
	
	private: //�����ֹ��췽������CFileConfigʹ�ã�������
		iterator(list<string>& vStrData); 
		iterator(list<string>& vStrData,const list<string>::iterator& SectionIter);
        //
	public:
		void begin();//��������λ��ָ��һ����Ч������
		const string& GetCurSection() { return section; } //����secion�����ã����䱾��
		const string GetCurKey();// //����ĳһ��������=����ߵ��ַ���

        //����һ������������֧��FileConfig::iterator
		iterator& operator=(const iterator& rhs);
		friend bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		friend bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs);
		
    	iterator& operator++();
		iterator& operator--();//prefix operators
	    iterator& operator++(int) { return operator++(); }
		iterator& operator--(int) { return operator--(); }//postfix operators
		//
		iterator& GotoNextSection();//��++���������ƣ����ص�ǰ�����������ã�ָ����һ��section�е�һ��������
	 	
		string operator *();//ȡ���ݲ�����
		operator list<string>::iterator () {return iter_cur_index;} //���ò�����
	
	private:
		
		bool end();//�жϸõ������Ƿ�ָ���ļ�β	
		const string GetCurValue();  //����ĳһ��������=���ұߵ��ַ���

		list<string>::iterator GetCurSectionIter() {return iter_cur_section;}
		list<string>::iterator GetCurIndexIter() {return iter_cur_index;}
		list<string>::iterator GetNextIndexIter() { return iter_cur_index++; }
		
		void InsertSubstr(size_t pos, const string& substr) { (*iter_cur_index).insert(pos, substr); }
		void ReplaceCurValue(const string& value);
		void RebindingIterator(list<string>& vStrData);//���½���������������һ��list<string>��������  �����������ƻ�����������Ҫ�޸ģ�
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
	//
	const string& GetFileName() {return fileName;}
	int GetConfigItemNum() {return _ItemNum;}
	int GetSectionNum() {return _SectionNum;}
	int GetKeyNamesBySectionName(const string& section, list<string>& lnames);//ȡsection�ڵ�������Чkey,����lnames�У�����key������
	int GetConfigItemBySectionName(const string& section,list<string>& litems);//ȡsection�ڵ�������Ч������,����litems�У�����item������
	int GetConfigItemBySectionName(const string& section);//���ظ�section����Ч�����������
	int GetSectionNames(list<string>& lnames);//ȡ�ļ���ǰȫ��section���֣�����lnames�У�����_sectionNum

	static bool match(const char* pLstr, const char* pRstr);
	static bool match(string lstr,string rstr);

public://��������III����������

	virtual Error_str GetLastErrorEx();

protected: //��������

	CFileConfig(const CFileConfig&); //��ΪSectionList���ŵ��Ǹ���ǰ�����йصĵ�ַ���������CFileConfig�����ֹ���ƣ������ƺ�Ķ����е�SectionList��Ȼָ��ԭ����fileData���Ӷ����δ֪����

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
	
	//void AddItemLine(CItemLine* item) { itemline_list.push_back(item); }
	//void DeleteItemLine(CItemLine* item);


	//���º���������ά��SectionList

	//
	bool BackupFile();//��ԭ�������ļ����ݣ���updatefile�����б��Զ�����

protected://��Ա����

	string fileName;
	int _SectionNum, _ItemNum;

	list<string> fileData;

	list<list<string>::iterator>  SectionList;// ��¼fileData�и���section���ַ����ĵ�ַ
	//list<CItemLine *> itemline_list;
	iterator iter_beg,iter_end;// CFileConfig��Ĭ�ϵ�����������������CFileConfig::begin��CFileConfig::endά��
};


//----------------------------------------------------------------------------------
//Sym��CItemLine
// ConfigItem ��Ķ���
//----------------------------------------------------------------------------------

#define  ERROR_CONFIG_ITEM_SUCCESS       ERROR_NO_ERROR
#define  ERROR_CONFIG_ITEM_NOT_EXIST     0x00020002


class CItemLine :public CErrorHandler
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
	const string& GetKey()	 const {return key; }
	const string& GetRemark() const {return remark;}
	//��ȡvalue�ķ��������������ж��壬��ΪMyValue();
	
	
	virtual void ChangeRemarkToFile(const string& newremark) { return; }
	void ChangeRemarkInThisItem(const string& newremark) {remark=newremark;SetLastError(ERROR_CONFIG_ITEM_SUCCESS);}

	Error_str GetLastErrorEx();

protected:
	
	virtual bool ChangeValueInThisItem(const string& value) {return false;}//�麯���ӿڣ�ʵ�ʶ�����������

    bool _Cancel() {remark="";SetLastError(ERROR_CONFIG_ITEM_SUCCESS);return pFileCach->CancelConfigLine(section,key);}
	
	void SetValueToFile(const string& newvalue,const string& newremark="") {pFileCach->SetValue(section,key,newvalue,newremark);SetLastError(ERROR_CONFIG_ITEM_SUCCESS);}
	bool GetValueFromFile(string& newvalue);

protected:

	string section;
	string key;
	string remark;

	CFileConfig* pFileCach;

};//End of CItemLine




#endif /*FILECONFIG_H_*/




