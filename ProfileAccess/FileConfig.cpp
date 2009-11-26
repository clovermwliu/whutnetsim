#include "StdAfx.h"
#include "FileConfig.h"


using namespace std;

namespace WhuTNetSimConfigClass{


CFileConfig::CFileConfig(const string& strFilePath)//�ڳ�ʼ���б��жԳ�ԱfileName��ֵ
: fileName(strFilePath),
  _SectionNum(0),
  _ItemNum(0)
{
	//nothing to do;
}

CFileConfig::~CFileConfig(void)
{
}



int  CFileConfig::LoadFile()
/*
�������򿪲���ȡһ�������ļ����ļ�·��������CFileConfig::fileName��
���أ��򿪳ɹ��򷵻�1
���򷵻ش�����
����ʱ�䣺2009/11/11
*/
{
	string line;
	ifstream infileStream(fileName.c_str());
	if (infileStream){

		_SectionNum=0;
		_ItemNum=0;

		while(getline(infileStream, line)){
			
			fileData.push_back(line);

#define  TEST
#ifdef TEST

			if (line.empty())	continue;//���������⣡��������

			string::size_type start = line.find_first_not_of(CHAR_TAB);
			if (line.at(start)!=CHAR_REMARK_LINE && line.at(start)!=CHAR_REM && line.at(start)!=CHAR_SECTION_BEGIN){

				++_ItemNum;

			}else if (line.at(start)==CHAR_SECTION_BEGIN){

				++_SectionNum; 
			}

		}
#else


		CFileConfig::iterator iter;

		for (iter=begin();iter!=end();++iter){
			++_ItemNum;
		}
#endif
		infileStream.close();
	
	}else{

		return ERROR_FILE_NOT_EXSITING;
	}

	return SUCCESS_NO_ERROR;
}


int CFileConfig::LoadFile(int& SectionNum,int& ConfigurationItemNum)
/*
�������򿪲���ȡһ�������ļ����ļ�·��������CFileConfig::fileName��
������[OUT]SectionNum:��ǰ�ļ��е�sections����
      [OUT]ConfigurationItemNum:��ǰ�ļ��е�������Ŀ����

���أ��򿪳ɹ��򷵻�1
���򷵻ش�����
����ʱ�䣺2009/11/11
*/
{
	int tmp=LoadFile();
	SectionNum=_SectionNum;
	ConfigurationItemNum=_ItemNum;
	return tmp;

}



int CFileConfig::UpdateFile()
/*
�������򿪲���������һ�������ļ����ļ�·��������CFileConfig::fileName��

���أ��򿪳ɹ��򷵻�1
      ���򷵻ش�����
����ʱ�䣺2009/11/11
*/
{

	int tmp=BackupFile();
	if (tmp != SUCCESS_NO_ERROR){

		return tmp;
	}
	
	ofstream fileStream(fileName.c_str(), std::ofstream::trunc);
	if (fileStream) {
		list<string>::iterator iter;
		for (iter = fileData.begin();iter != fileData.end();iter++)
		{
			fileStream << (*iter) << endl;
		}

		fileStream.close();
		return 1;
	}else{
		return ERROR_FILE_WRITE_FAIL ;
	}
}


CFileConfig::iterator& CFileConfig::begin()
/*
����������Ƕ�׵��������RebindingIterator��������iter_begָ���һ�����������iter_beg������
*/
{
	iter_beg.RebindingIterator(fileData);
	return iter_beg;
}

CFileConfig::iterator&  CFileConfig::end()
/*
//����Ƕ�׵��������RebindingIterator����������iter_endָ�����һ������Ч�ģ������������iter_end������

*/
{
	iter_end.RebindingIterator(fileData);

	while (!iter_end.end())
	{
		++iter_end;
	}
	return iter_end;
}


int CFileConfig::BackupFile()
/*
�������Ե�ǰ�����ļ����б��ݡ���UpdateFile()�б����ã��Ա�֤���������ļ�ʱ���ᶪʧԭ��������Ϣ��

*/
{
    string bakfile=fileName+".bak";
  	ifstream infileStream(fileName.c_str(),ios::binary);
	ofstream outfileStream(bakfile.c_str(),ios::binary);

	if(!infileStream) 
		return ERROR_FILE_NOT_EXSITING;
	if(!outfileStream) 
		return ERROR_FILE_WRITE_FAIL;

	outfileStream  <<   infileStream.rdbuf(); 
	
	infileStream.close();
	outfileStream.close();
	return SUCCESS_NO_ERROR;

}

//------------------------------------------------------------------------
//������FileConfig�е�������ʵ��
//------------------------------------------------------------------------


CFileConfig::iterator::iterator(list<string>& vStrData)
/*
Constructor
*/
{
	section="";
	pStr = & vStrData;
	iter_cur_index = pStr->begin();
	while(IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE){             
		iter_cur_index++;
	}
}


void CFileConfig::iterator:: begin() 
/*
����������iter_cur_indexָ���һ��Configuration Line���������Ӧ��section����ȡ��CFileConfig::iterator::section��

*/
{ 
  iter_cur_index = pStr->begin(); 
  section.erase();
  if (IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE)
	  operator++();
}



bool CFileConfig::iterator:: end() 
/*
�������ж�iter_cur_index�Ƿ�ָ���ļ�β

*/
{
	return (pStr->end() == iter_cur_index);
}


const string CFileConfig::iterator::GetCurKey() 
/*
����������iter_cur_indexָ��������=����ߵ��ַ���
*/ 
{
	if (iter_cur_index!=pStr->end())
	{
		size_t pos = (*iter_cur_index).find('=');
		if (pos!=string::npos)
			return (*iter_cur_index).substr(0, pos);
		else
			return (*iter_cur_index);

	}else{
		return "";
	}

}


const std::string CFileConfig::iterator::GetCurValue()
/*
����������iter_cur_indexָ��������=���ұߵ��ַ���
*/
{
	if (iter_cur_index!=pStr->end()){

		size_t pos = (*iter_cur_index).find('=');
		if (string::npos != pos)
			return (*iter_cur_index).substr(pos + 1);
		else
			return "";
	}else{
		return "";
	}

	
}


void CFileConfig::iterator::ReplaceCurValue(const string& value)
/*
��������value���µ���ǰiter_cur_indexָ���������
*/
{
	size_t pos = iter_cur_index->find('=');
	*iter_cur_index = iter_cur_index->substr(0, pos + 1);
	iter_cur_index->append(value);
}


CFileConfig::iterator& CFileConfig::iterator::operator=(const CFileConfig::iterator& rhs)
/*
���������ز�����operator= 
      ������� CFileConfig::iterator�ĸ��ƣ��β�Ϊ=��������������ظ�*this ������      
*/
{
	section = rhs.section;
	pStr = rhs.pStr;
	iter_cur_index = rhs.iter_cur_index;
	return *this;
}

bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
���������ز�����operator==
���أ������������������� pStr, section �� iter_cur_index����ͬ�򷵻� true    
*/
{
	return (lhs.pStr==rhs.pStr)&&(lhs.iter_cur_index==rhs.iter_cur_index)&&(lhs.section==rhs.section);

}
bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
���������ز�����operator!=
���أ������������������� pStr, section �� iter_cur_index����һ������ͬ�򷵻� true    
*/      
{
	return !(lhs==rhs);

}


CFileConfig::iterator& CFileConfig::iterator::operator++()
/*
���������ز�����operator++ 
      ���һ�� CFileConfig::iterator�������������ڲ��ƶ�iter_cur_index��ʹָ֮��pStr������һ���Ϸ���configuration line
��ע������ǰ׺������++��ʵ��
*/
{
	if (iter_cur_index !=pStr ->end() )
	{
		do
		{
			iter_cur_index++;

		} while (IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE);
	}

	return *this;
}

CFileConfig::iterator& CFileConfig::iterator::operator--()
/*
���������ز�����operator-- 
���һ�� CFileConfig::iterator���Լ��������ڲ��ƶ�iter_cur_index��ʹָ֮��pStr������һ���Ϸ���configuration line
��ע������ǰ׺������++��ʵ��
*/
{
	if (iter_cur_index !=pStr ->begin() )
	{
		do
		{
			iter_cur_index--;

		} while (IsValidConfigurationLine()!=CONFIGURATION_LINE);
	}

	return *this;
}


string CFileConfig::iterator::operator* ()
/*

*/

{
	string strtmp=GetCurSection();

	if (iter_cur_index!=pStr->end())
	{
		strtmp=strtmp+CHAR_TAB;
		strtmp=strtmp+(*iter_cur_index);
	}
	return strtmp;
}

void CFileConfig::iterator::RebindingIterator(list<string>& vStrData)
/*
���������½���������������һ��list<string>��������

*/

{
	section="";
	pStr = & vStrData;
	iter_cur_index = pStr->begin();
	while(IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE){             
		iter_cur_index++;
	}

}


int CFileConfig::iterator::IsValidConfigurationLine()
/*

����������pStrָ�������������ÿһ��string��
      ���������ַ�������#��@��ͷ�������Թ�������iter_cur_index��ʹָ֮��pStr��������һ��
	  ������[ ]Ϊ��ʼ����β���ģ���ʾ����һ���µ�section line,�򽫽�����ŵ�CFileConfig::iterator::section�У�������iter_cur_index��ʹָ֮��pStr��������һ��

���أ�#define CONFIGURATION_LINE     0x00000001
	  #define SECTION_LINE           0x00000010
	  #define REMAK_LINE             0x00000100
	  #define CANCEL_LINE            0x00001000
	  #define FILE_END_LINE          0x00010000
	  #define BLANK_LINE             0x00100000

*/

{
	if (iter_cur_index==pStr->end()) //��ǰiter_cur_index��ָ���ļ�β
		return FILE_END_LINE; 

	size_t start = (*iter_cur_index).find_first_not_of(CHAR_TAB);

	if (string::npos == start){

		return BLANK_LINE; // Blank line;
	
	}else if ((*iter_cur_index).at(start)==CHAR_REMARK_LINE ){

		return REMAK_LINE;  // remark line;

	}else if ((*iter_cur_index).at(start)==CHAR_REM ){

		return CANCEL_LINE;	
	
	}else if (CHAR_SECTION_BEGIN == (*iter_cur_index).at(start)){

		size_t end = (*iter_cur_index).find(CHAR_SECTION_END);
		section = (*iter_cur_index).substr(start + 1, end - 1);
		return SECTION_LINE; // Section line.

	}else{

		return CONFIGURATION_LINE; // Found a Valid Configuration Line
	}

}












//end of FileConfig.cpp

}