#include "StdAfx.h"
#include "FileConfig.h"


using namespace std;

namespace WhuTNetSimConfigClass{


CFileConfig::CFileConfig(const string& strFilePath)//�ڳ�ʼ���б��жԳ�ԱfileName��ֵ
: fileName(strFilePath)
{
	//nothing to do;
}

CFileConfig::~CFileConfig(void)
{
}



int  CFileConfig::ReadFile()
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

		while(getline(infileStream, line)){
			
			//data.push_back(line);
		}

		infileStream.close();
	
	}else{

		return ERROR_FILENOTEXSITING;
	}

	// Now parse everything into the attatched objects.
	//parse();

	return 1;
}


int CFileConfig::ReadFile(int& Num_of_sections,int& Num_of_items)
/*
�������򿪲���ȡһ�������ļ����ļ�·��������CFileConfig::fileName��
������[OUT]Num_of_sections:��ǰ�ļ��е�sections����
      [OUT]Num_of_items:��ǰ�ļ��е�������Ŀ����

���أ��򿪳ɹ��򷵻�1
���򷵻ش�����
����ʱ�䣺2009/11/11
*/
{
	string line;
	ifstream fileStream(fileName.c_str());
	if (fileStream){

		while(getline(fileStream, line)){
			
			//data.push_back(line);
		}

		fileStream.close();
	}else{

		return ERROR_FILENOTEXSITING;
	}

	// Now parse everything into the attatched objects.
	//parse();

	return 1;

}




/*
void CFileConfig::write()
{
	std::ofstream fileStream(fileName.c_str(), std::ofstream::trunc);
	if (fileStream) {
		std::vector<std::string>::iterator it;
		for (it = data.begin();it != data.end();it++)
		{
			fileStream << (*it) << std::endl;
		}

		fileStream.close();
	}
}
*/



//------------------------------------------------------------------------
//������FileConfig�е�������ʵ��
//------------------------------------------------------------------------


CFileConfig::iterator::iterator(vector<string>& vStrData)
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



bool void CFileConfig::iterator:: end() 
/*
�������ж�iter_cur_index�Ƿ�ָ���ļ�β

*/
{
	return (pStr->end() == iter_cur_index);
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

		size_t end = (*iter_cur_index).find(']');
		section = (*iter_cur_index).substr(start + 1, end - 1);
		return SECTION_LINE; // Section line.

	}else{

		return CONFIGURATION_LINE; // Found a Valid Configuration Line
	}

}












//end of FileConfig.cpp

}