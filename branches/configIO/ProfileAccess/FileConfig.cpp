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


//end of FileConfig.cpp

}