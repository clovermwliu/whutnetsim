#include "StdAfx.h"
#include "FileConfig.h"


using namespace std;

namespace WhuTNetSimConfigClass{


CFileConfig::CFileConfig(const string& strFilePath)//在初始化列表中对成员fileName赋值
: fileName(strFilePath)
{
	//nothing to do;
}

CFileConfig::~CFileConfig(void)
{
}



int  CFileConfig::ReadFile()
/*
描述：打开并读取一个配置文件，文件路径存在于CFileConfig::fileName中
返回：打开成功则返回1
否则返回错误码
开发时间：2009/11/11
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
描述：打开并读取一个配置文件，文件路径存在于CFileConfig::fileName中
参数：[OUT]Num_of_sections:当前文件中的sections个数
      [OUT]Num_of_items:当前文件中的配置项目个数

返回：打开成功则返回1
否则返回错误码
开发时间：2009/11/11
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
//以下是FileConfig中迭代器的实现
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
描述：设置iter_cur_index指向第一个Configuration Line，并将其对应的section名读取到CFileConfig::iterator::section中

*/
{ 
  iter_cur_index = pStr->begin(); 
  section.erase();
  if (IsValidConfigurationLine()!=CONFIGURATION_LINE)
	  operator++();
}



bool void CFileConfig::iterator:: end() 
/*
描述：判断iter_cur_index是否指向文件尾

*/
{
	return (pStr->end() == iter_cur_index);
}


int CFileConfig::iterator::IsValidConfigurationLine()
/*

描述：遍历pStr指向的容器，解析每一个string。
      如遇到空字符串或以#和@开头的行则略过，自增iter_cur_index，使之指向pStr容器的下一行
	  如遇到[ ]为开始（结尾）的，表示遇到一个新的section line,则将节名存放到CFileConfig::iterator::section中，并自增iter_cur_index，使之指向pStr容器的下一行

返回：#define CONFIGURATION_LINE     0x00000001
	  #define SECTION_LINE           0x00000010
	  #define REMAK_LINE             0x00000100
	  #define CANCEL_LINE            0x00001000
	  #define FILE_END_LINE          0x00010000
	  #define BLANK_LINE             0x00100000

*/

{
	if (iter_cur_index==pStr->end()) //当前iter_cur_index已指向文件尾
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