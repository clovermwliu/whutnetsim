#include "StdAfx.h"
#include "FileConfig.h"


using namespace std;

namespace WhuTNetSimConfigClass{


CFileConfig::CFileConfig(const string& strFilePath)//在初始化列表中对成员fileName赋值
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
描述：打开并读取一个配置文件，文件路径存在于CFileConfig::fileName中
返回：打开成功则返回1
否则返回错误码
开发时间：2009/11/11
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
描述：打开并读取一个配置文件，文件路径存在于CFileConfig::fileName中
参数：[OUT]SectionNum:当前文件中的sections个数
      [OUT]ConfigurationItemNum:当前文件中的配置项目个数

返回：打开成功则返回1
否则返回错误码
开发时间：2009/11/11
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
//以下是FileConfig中迭代器的实现
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
描述：设置iter_cur_index指向第一个Configuration Line，并将其对应的section名读取到CFileConfig::iterator::section中

*/
{ 
  iter_cur_index = pStr->begin(); 
  section.erase();
  if (IsValidConfigurationLine()!=CONFIGURATION_LINE)
	  operator++();
}



bool CFileConfig::iterator:: end() 
/*
描述：判断iter_cur_index是否指向文件尾

*/
{
	return (pStr->end() == iter_cur_index);
}


const string CFileConfig::iterator::GetCurKey() 
/*
描述：返回iter_cur_index指向配置行=号左边的字符串
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
描述：返回iter_cur_index指向配置行=号右边的字符串
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
描述：将value跟新到当前iter_cur_index指向的配置行
*/
{
	size_t pos = iter_cur_index->find('=');
	*iter_cur_index = iter_cur_index->substr(0, pos + 1);
	iter_cur_index->append(value);
}


CFileConfig::iterator& CFileConfig::iterator::operator=(const CFileConfig::iterator& rhs)
/*
描述：重载操作符operator= 
      完成两个 CFileConfig::iterator的复制，形参为=的左操作数，返回给*this 的引用      
*/
{
	section = rhs.section;
	pStr = rhs.pStr;
	iter_cur_index = rhs.iter_cur_index;
	return *this;
}

bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
描述：重载操作符operator==
返回：若左右两个操作数的 pStr, section 及 iter_cur_index均相同则返回 true    
*/
{
	return (lhs.pStr==rhs.pStr)&&(lhs.iter_cur_index==rhs.iter_cur_index)&&(lhs.section==rhs.section);

}
bool operator!=(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
描述：重载操作符operator!=
返回：若左右两个操作数的 pStr, section 及 iter_cur_index任意一个不相同则返回 true    
*/      
{
	return !(lhs==rhs);

}


CFileConfig::iterator& CFileConfig::iterator::operator++()
/*
描述：重载操作符operator++ 
      完成一个 CFileConfig::iterator的自增操作。内部移动iter_cur_index，使之指向pStr容器内一个合法的configuration line
备注：这是前缀操作符++的实现
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
描述：重载操作符operator-- 
完成一个 CFileConfig::iterator的自减操作。内部移动iter_cur_index，使之指向pStr容器内一个合法的configuration line
备注：这是前缀操作符++的实现
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

		size_t end = (*iter_cur_index).find(CHAR_SECTION_END);
		section = (*iter_cur_index).substr(start + 1, end - 1);
		return SECTION_LINE; // Section line.

	}else{

		return CONFIGURATION_LINE; // Found a Valid Configuration Line
	}

}












//end of FileConfig.cpp

}