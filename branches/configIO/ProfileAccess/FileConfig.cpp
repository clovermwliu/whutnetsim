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

			size_t start = line.find_first_not_of(CHAR_TAB);

			if (string::npos != start && line.at(start)!=CHAR_REMARK_LINE && line.at(start)!=CHAR_REM){

				++_ItemNum;

			}else if (line.at(start)==CHAR_SECTION_BEGIN){

				++_SectionNum; 
			}
		}

		infileStream.close();
	
	}else{

		return ERROR_FILE_NOT_EXSITING;
	}

	// Now parse everything into the attatched objects.
	//parse();

	return 1;
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
{
	std::ofstream fileStream(fileName.c_str(), std::ofstream::trunc);
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


//------------------------------------------------------------------------
//������FileConfig�е�������ʵ��
//------------------------------------------------------------------------


CFileConfig::iterator::iterator(list<string>& vStrData)
/*
Constructor
*/
{
	pStr = & vStrData;
	iter_cur_index = pStr->begin();
	while(IsValidConfigurationLine()){             
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
  if (IsValidConfigurationLine()!=CONFIGURATION_LINE)
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
	size_t pos = (*iter_cur_index).find('=');
	if (pos!=string::npos)
		return (*iter_cur_index).substr(0, pos);
	else
		return (*iter_cur_index);
}


const std::string CFileConfig::iterator::GetCurValue()
/*
����������iter_cur_indexָ��������=���ұߵ��ַ���
*/
{
	size_t pos = (*iter_cur_index).find('=');
	if (string::npos != pos)
		return (*iter_cur_index).substr(pos + 1);
	else
		return "";
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

		} while (IsValidConfigurationLine()!=CONFIGURATION_LINE);
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