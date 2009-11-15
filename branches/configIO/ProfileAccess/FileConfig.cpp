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


//end of FileConfig.cpp

}