#include "StdAfx.h"
#include "FileConfig.h"
#include <assert.h>


using namespace std;

namespace WhuTNetSimConfigClass{



//----------------------------------------------------------------------------------
// Sym��FileConfig
// FileConfig ���ʵ��
//----------------------------------------------------------------------------------	
	
CFileConfig::CFileConfig(const string& strFilePath)//�ڳ�ʼ���б��жԳ�ԱfileName��ֵ
: fileName(strFilePath), _SectionNum(0), _ItemNum(0),CErrorHandler()
{
	//nothing to do;
}

CFileConfig::~CFileConfig(void)
{
}



bool  CFileConfig::LoadFile()
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
		}

		CFileConfig::iterator iter;

		for (iter=begin();iter!=end();++iter){
			++_ItemNum;
		}

		for (iter=begin();iter!=end();iter=iter.GotoNextSection()){
			SectionList.push_back(iter.GetCurSectionIter());
			++_SectionNum;
		}
		
	infileStream.close();
	
	}else{

		SetLastError(ERROR_FILE_NOT_EXSITING);
		SetLastErrorStr(fileName);
		return false;
	}

	return true;
}


bool CFileConfig::LoadFile(int& SectionNum,int& ConfigurationItemNum)
/*
�������򿪲���ȡһ�������ļ����ļ�·��������CFileConfig::fileName��
������[OUT]SectionNum:��ǰ�ļ��е�sections����
      [OUT]ConfigurationItemNum:��ǰ�ļ��е�������Ŀ����

���أ��򿪳ɹ��򷵻�1
���򷵻ش�����
����ʱ�䣺2009/11/11
*/
{
	bool tmp=LoadFile();
	SectionNum=_SectionNum;
	ConfigurationItemNum=_ItemNum;
	return tmp;

}



bool CFileConfig::UpdateFile(bool bIngronCancelLine)
/*
�������򿪲���������һ�������ļ����ļ�·��������CFileConfig::fileName��
������bIngronCancelLineΪtrue�򽫴���@�������в���д���ļ�
���أ��򿪳ɹ��򷵻�1
      ���򷵻ش�����
*/
{

	if (!BackupFile() && GetLastError()==ERROR_FILE_WRITE_FAIL){

		return false;
	}
		
	ofstream fileStream(fileName.c_str(), std::ofstream::trunc);

	size_t start;

	if (fileStream) {
		list<string>::iterator iter;

		bool bIngronNext=false;
		
		for (iter = fileData.begin();iter != fileData.end();++iter){

			if (bIngronCancelLine){

				if (CFileConfig::iterator::IsBlankLine(*iter)){
					continue;
				}

				start=(*iter).find_first_not_of(STR_TAB);
				if ((*iter).at(start)==CHAR_REM){
					bIngronNext=true;
					continue;
				}
				if ((*iter).at(start)==CHAR_REMARK_LINE && bIngronNext==true){
					continue;
				}

				fileStream << (*iter) << endl;
				bIngronNext=false;

			}else{

				fileStream << (*iter) << endl;
			}
		}
		fileStream.close();
		return true;
		
	}else{

		SetLastError(ERROR_FILE_WRITE_FAIL);
		SetLastErrorStr(fileName);
		return false;
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
//����Ƕ�׵��������RebindingIterator����������iter_endָ�����һ����Ч������ĺ�һ����ʵ���ϵ��ļ�β��sectionΪ���һ����Чsection����������iter_end������
*/
{
	iter_end.RebindingIterator(fileData);
	while (!iter_end.end())
	{
		++iter_end;
	}
	return iter_end;
}


int CFileConfig::GetKeyNamesBySectionName(const string& section, list<string>& lnames)
/*
������ȡsection�ڵ�������Чkey,����lnames�У�����key������
������[out]lnames
*/
{
	int i=0;

	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) return 0;

	//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
	CFileConfig::iterator iter(fileData,*quickiter);

	while (match(section, iter.GetCurSection())){
		++i;
		lnames.push_back(iter.GetCurKey());
		++iter;
	}

	return i;
}

int CFileConfig::GetConfigItemBySectionName(const string& section,list<string>& litems)
/*
������ȡsection�ڵ�������Ч������,����litems�У�����item������
������[out]litems
*/

{
	int i=0;

	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) return 0;

	//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
	CFileConfig::iterator iter(fileData,*quickiter);

	if (iter.GetCurIndexIter()!=fileData.end())
	{
		while (match(section, iter.GetCurSection())){
			++i;
			litems.push_back(*(iter.GetCurIndexIter()));
			++iter;
		}

		return i;
	}

	return 0;
}

int CFileConfig::GetConfigItemBySectionName(const string& section)
/*
���������ظ�section����Ч�����������
*/

{
	int i=0;

	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) return 0;

	//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
	CFileConfig::iterator iter(fileData,*quickiter);

	if (iter.GetCurIndexIter()!=fileData.end())
	{
		while (match(section, iter.GetCurSection())){
			++i;
			++iter;
		}
		return i;
	}
	
	return 0;
}

int CFileConfig::GetSectionNames(list<string>& lnames)
/*
������ȡ�ļ���ǰȫ��section���֣�����lnames�У�����_sectionNum
*/
{
	int i=0;
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		++i;
		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		lnames.push_back(sectionG);
	}
	assert(i==GetSectionNum());
	return i;

}


bool CFileConfig::match(const char* pLstr, const char* pRstr)
/*
�������ַ���ƥ��
*/
{
	if (0 == strcmp(pLstr, pRstr))	return true;
	return false;

}
bool CFileConfig::match(string lstr,string rstr)
/*
�������ַ���ƥ��
*/
{
	return lstr==rstr;

}

bool CFileConfig::CancelConfigLine(const string& section, const string& key)
/*
��������fileData��section��key��һ��ǰ���"@"����
���أ����������fileData��δ��������section,key����򷵻�false;
*/
{
	
	//����sectionList��ƥ��section�������section������ֱ�ӷ���ʧ��
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}
 
	if (quickiter==SectionList.end()) return false;
			
	//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
	CFileConfig::iterator iter(fileData,*quickiter);
	
	while (!iter.end()){

		if (match(section, iter.GetCurSection()) && match(key, iter.GetCurKey()))	{//match(section, iter.GetCurSection())�����٣���section��ǰû����Ч������ʱ��iterֱ��ָ������section�е���Ч������ȥ��
			
			iter.InsertSubstr(0,STR_REM);
			_ItemNum--;
			if (GetConfigItemBySectionName(section)==0) _SectionNum--;
			return true;
		}
    	iter++;
	}

	return false;
}

list<string>::iterator  CFileConfig::SetValueWithoutRemark(const string& section, 
						                                   const string& key,
							                               const string& value)
/*
��������section��key������valueֵ�����ظ���������fileData�еĵ�����
*/

{
	
	bool itemExisting=true;
	//����sectionList��ƥ��section�������section������ֱ�ӷ���ʧ��
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) itemExisting=false;

    if (itemExisting){

		//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
		CFileConfig::iterator iter(fileData,*quickiter);

		while (!iter.end())
		{
			if (match(section, iter.GetCurSection()) &&	match(key,iter.GetCurKey()))
			{
				iter.ReplaceCurValue(value);
				return iter.GetCurIndexIter();
			}
			iter++;
		}
    }

	//���û�����section,key������������֮
    
	if (!CFileConfig::iterator::IsBlankLine(key)){

		string item = key;
		item.append("=");
		item.append(value);
		return AddConfigLine(section, item);
	
	}else{

		return fileData.end();
	}
	
}



void CFileConfig::SetValue(const string& section, 
			               const string& key, 
			               const string& value,
			               const string& remark)
/*
����:������������
*/
{
	list<string>::iterator iter=SetValueWithoutRemark(section,key,value);

	if (iter==fileData.end()) return;
	

	string str=REMARK_SYM;
	str.append("{");
	str.append(key);
	str.append(STR_EQUALS);
	str.append(value);
	str.append("}");
	str.append(STR_TAB);

	if (!CFileConfig::iterator::IsBlankLine(remark)){
		str.append(remark);
	}else{
		str.append("(NONE	)");

	}

	if ((++iter)!=fileData.end()){

		size_t start = (*iter).find_first_not_of(STR_TAB);

		if ((*iter).at(start)==CHAR_REMARK_LINE){
			list<string>::iterator iter2=fileData.erase(iter); //����ԭ�������ע�ͣ�ע��λ�ڸ����������һ��
			fileData.insert(iter2,str);
		}else{
			fileData.insert(iter,str);//���������ע�ͣ�ע��λ�ڸ����������һ��
		}
		return;
	}

	fileData.insert(iter,str); //����������ļ�β��ֱ�����ļ�β���ע��
	return;
	
}

list<string>::iterator  CFileConfig::AddConfigLine(const string& section, const string& item)
/*
���������section����������Ŀ��item:key=value��,���ظ���������fileData�еĵ�����
*/
{
	
	bool SectionExisting=true;

	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) SectionExisting=false;
	
	if (SectionExisting){

		CFileConfig::iterator iter(fileData,*quickiter);

		if (GetConfigItemBySectionName(section)==0) _SectionNum++;

		_ItemNum++;
		
		if (iter.end()){
			fileData.push_back(item);
			return --fileData.end();
			
		}else	{
			
			fileData.insert(iter.GetCurIndexIter(), item);//ǰ����
			return --(iter.GetCurIndexIter());
		
		}
	
	}

	// �½�����������
	string newsection = STR_SECTION_BEGIN;
	newsection.append(section);
	newsection.append(STR_SECTION_END);
	fileData.push_back(newsection);

	list<string>::iterator iiter=(--fileData.end());
 	SectionList.push_back(iiter);
	_SectionNum++;

	fileData.push_back(item);
	_ItemNum++;
	return --fileData.end();
}


bool CFileConfig::GetValue(const string& section,
			               const string& key,
			               string& value,
						   string& remark)
/*
����������section��key��fileData�ϲ��Ҹ�������������ע�ͷ��ظ�remark,����Ŀ��ֵ��һ��string�����ظ�value
������[out]remark,value
���أ����������ڣ���������false,remark��value����Ϊ�մ�
*/
{
	//����sectionList��ƥ��section�������section������ֱ�ӷ���ʧ��
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) { //û��ƥ���section

		remark="";
		value="";
		return false;
	}

	//Ѹ�ٹ���һ��CFileConfig::iterator��ָ���section�ĵ�һ����Ч�������ƥ��key
	CFileConfig::iterator iter(fileData,*quickiter);

	while (!iter.end())
	{
		if (match(section, iter.GetCurSection()) &&	match(key,iter.GetCurKey()))
		{
			value=iter.GetCurValue();//���value

			//����ȥ��ø����ע��
			string remark= *(iter.GetNextIndexIter()); //Ĭ������£�ע��Ӧ�ý�����������
			if(!CFileConfig::iterator::IsBlankLine(remark)){
				
				size_t start = remark.find_first_not_of(STR_TAB);
				if (remark.at(start)!=CHAR_REMARK_LINE) remark="";
			}
			return true;
		}
		iter++;
	}

	//���û�����section,key����
	remark="";
	value="";
	return false;

}

bool CFileConfig::BackupFile()
/*
�������Ե�ǰ�����ļ����б��ݡ���UpdateFile()�б����ã��Ա�֤���������ļ�ʱ���ᶪʧԭ��������Ϣ��

*/
{
	string bakfile=fileName+".bak";
	ifstream infileStream(fileName.c_str(),ios::binary);
	ofstream outfileStream(bakfile.c_str(),ios::binary);

	if(!infileStream){ 

		SetLastError(ERROR_FILE_NOT_EXSITING);
		SetLastErrorStr(fileName);
		return false;
	}
	if(!outfileStream) {

		SetLastError(ERROR_FILE_WRITE_FAIL);
		SetLastErrorStr(bakfile);
		return false;
	}

	outfileStream  <<   infileStream.rdbuf(); 

	infileStream.close();
	outfileStream.close();
	
	return true;

}

Error_str CFileConfig::GetLastErrorEx()
/*
#define SUCCESS_NO_ERROR         0x00010000
#define ERROR_FILE_NOT_EXSITING	 0x00010001
#define ERROR_FILE_WRITE_FAIL    0x00010002
*/
{
	switch (error_code)
	{
	case SUCCESS_NO_ERROR:
		return "Parse config file successfully";
	case ERROR_FILE_NOT_EXSITING:
		return "File does not exist:"+err_str;
	case ERROR_FILE_WRITE_FAIL:
		return "Write file fail:"+err_str;
	default:
		return "Unknown";

	}
}



//------------------------------------------------------------------------
//Sym��Iterator
//������FileConfig�е�������ʵ��
//------------------------------------------------------------------------


CFileConfig::iterator::iterator(list<string>& vStrData)
/*
Constructor:��ʼ��һ��CFileConfig::iterator��CFileConfig::iterator�������ָ�������ļ���һ����Ч������
*/
{
	section="";
	pStr = & vStrData;
	iter_cur_index = pStr->begin();
	iter_cur_section=pStr->end();
	while(IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE){             
		iter_cur_index++;
	}
}

CFileConfig::iterator::iterator(list<string>& vStrData,const list<string>::iterator& SectionIter)
/*
Constructor:Ѹ�ٳ�ʼ��һ��CFileConfig::iterator�������β���һ��ָ��vStrData������ĳ��section�ڱ���еĵ�����
            CFileConfig::iterator�������ָ��ýڵ�һ����Ч������,
			��ýڵ�ǰû����Ч��������ָ������һ������Ч������Ľڵĸ���Ч�������ָ���ļ�β
*/
{
	size_t start = (*SectionIter).find_first_not_of(CHAR_SECTION_BEGIN);
	size_t end = (*SectionIter).find(CHAR_SECTION_END);
	section =(*SectionIter).substr(start, end - 1);

	pStr = & vStrData;
	iter_cur_index = SectionIter;
	iter_cur_section = SectionIter;
	do{
		iter_cur_index++;

	}while(IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE);            

}


void CFileConfig::iterator:: begin() 
/*
����������iter_cur_indexָ���һ��Configuration Line���������Ӧ��section����ȡ��CFileConfig::iterator::section��

*/
{ 
  if (pStr==NULL) return;

  iter_cur_index = pStr->begin(); 
  iter_cur_section=pStr->end();;
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
	iter_cur_section = rhs.iter_cur_section;
	return *this;
}

bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
���������ز�����operator==
���أ������������������� pStr, section �� iter_cur_index����ͬ�򷵻� true    
*/
{
	return (lhs.pStr==rhs.pStr)&&(lhs.iter_cur_index==rhs.iter_cur_index)&&(lhs.section==rhs.section)&&(lhs.iter_cur_section == rhs.iter_cur_section);

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
	if (pStr==NULL) return *this;

    if (iter_cur_index !=pStr ->end() )
	{
		do
		{
			iter_cur_index++;
			
			if (iter_cur_index==pStr->end()){
				
				section="";
				iter_cur_section=pStr->end();
			}
			

		} while (IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE);
	}
	return *this;
}

CFileConfig::iterator& CFileConfig::iterator::operator--()
/*
���������ز�����operator-- 
���һ�� CFileConfig::iterator���Լ��������ڲ��ƶ�iter_cur_index��ʹָ֮��pStr������һ���Ϸ���configuration line
��ע������ǰ׺������--��ʵ��
*/
{
	if (pStr==NULL) return *this;
	
	list<string>::iterator ittmp;

	if (iter_cur_index !=pStr ->begin() )
	{
		do
		{
			iter_cur_index--;

		} while (IsValidConfigurationLine()!=CONFIGURATION_LINE && iter_cur_index !=pStr ->begin() );

		if(iter_cur_index != pStr->begin()){

			ittmp=iter_cur_index;
			while (IsValidConfigurationLine()!=SECTION_LINE){

				if (iter_cur_index !=pStr ->begin()){
					
					iter_cur_index--;
				}else{
					section="";
					iter_cur_section=pStr->end();
					break;
				}

			}
		}
		iter_cur_index=ittmp;
	}
	return *this;
}

string CFileConfig::iterator::operator *()
{
	if (pStr==NULL) return "";

	if (!(iter_cur_index==pStr->end())){
		return *iter_cur_index;
	}else{
		return "";
	}
}

CFileConfig::iterator& CFileConfig::iterator::GotoNextSection()
/*
����������һ����������ָ����һ��section�е�һ��������
*/
{
	if (pStr==NULL) return *this;
	
	if (iter_cur_index !=pStr ->end() )
	{
		do
		{
			iter_cur_index++;
			if (iter_cur_index==pStr->end()){

				section="";
				iter_cur_section=pStr->end();
			}
		} while (IsValidConfigurationLine()!=SECTION_LINE && IsValidConfigurationLine()!=FILE_END_LINE);
		if (iter_cur_index !=pStr ->end()){

			do
			{
				iter_cur_index++;
				if (iter_cur_index==pStr->end()){

					section="";
					iter_cur_section=pStr->end();
				}

			} while (IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE);

		}
		
	}
	return *this;
}


void CFileConfig::iterator::RebindingIterator(list<string>& vStrData)
/*
���������½���������������һ��list<string>��������

*/

{
	section="";
	pStr = & vStrData;
	iter_cur_index = pStr->begin();
	iter_cur_section=pStr->end();
	while(IsValidConfigurationLine()!=CONFIGURATION_LINE && IsValidConfigurationLine()!=FILE_END_LINE){             
		iter_cur_index++;
	}

}


bool CFileConfig::iterator::IsBlankLine(string str)
/*
�������ж�str�Ƿ�Ϊһ�����У�������Щ���пո��TAB���ŵ��ַ�������Ϊ�ǿ���
*/
{

	for(string::size_type pos(0);pos!=string::npos;pos+=0){

		if ((pos=str.find(STR_TAB,pos))!=string::npos){

			str.replace(pos,1,"");
		}else{
			break;
		}
	}

	for(string::size_type   pos(0);pos!=string::npos;pos+=0){

		if ((pos=str.find(CHAR_BLANK,pos))!=string::npos){

			str.replace(pos,1,"");
		}else{
			break;
		}
	}

	return str.empty();

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
	  #define BLANK_LINE             0x01000000

*/

{
	if (iter_cur_index ==pStr->end()) return FILE_END_LINE; //��ǰiter_cur_index��ָ���ļ�β
		
	if (IsBlankLine(*iter_cur_index)) return BLANK_LINE;
		
	size_t start = (*iter_cur_index).find_first_not_of(STR_TAB);

	if ((*iter_cur_index).at(start)==CHAR_REMARK_LINE ){

		return REMAK_LINE;  // remark line;

	}else if ((*iter_cur_index).at(start)==CHAR_REM ){

		return CANCEL_LINE;	
	
	}else if (CHAR_SECTION_BEGIN == (*iter_cur_index).at(start)){

		size_t end = (*iter_cur_index).find(CHAR_SECTION_END);
		section = (*iter_cur_index).substr(start + 1, end - 1);
		iter_cur_section=iter_cur_index;

		return SECTION_LINE; // Section line.

	}else{

		return CONFIGURATION_LINE; // Found a Valid Configuration Line
	}

}


//end of CFileConfig

//----------------------------------------------------------------------------------
// Sym��ConfigItem
// ConfigItem ���ʵ��
//----------------------------------------------------------------------------------
CItemLine::CItemLine(CFileConfig& file, 
					 const string& section,
					 const string& key,
					 const string& remark)
					 : section(section) , key(key) , remark(remark),pFileCach(&file),CErrorHandler()
{
	//file.AddItemLine(this);

}


CItemLine::CItemLine(CFileConfig& file, 
					 const string& section,
					 const string& key)
					 : section(section) , key(key) , remark(""),pFileCach(&file),CErrorHandler()
{
	//file.AddItemLine(this);

}


CItemLine::~CItemLine(void)
{

}

bool CItemLine::GetValueFromFile(string& newvalue)
/*
�������������ļ��ж�������
*/

{ 
	if(!pFileCach->GetValue(section,key,newvalue,remark)){

		SetLastError(ERROR_CONFIG_ITEM_NOT_EXIST);
		Error_str tmp=section+":";
		tmp=tmp+key;		
		SetLastErrorStr(tmp);
		return false;
	}

	return true;
}


Error_str CItemLine::GetLastErrorEx()
/*

*/
{
	switch (error_code)
	{
	case ERROR_CONFIG_ITEM_SUCCESS:
		return "Config LineItem was found in ConfigCache successfully";
	case ERROR_CONFIG_ITEM_NOT_EXIST:
		return "Config LineItem does not exist:"+err_str;
	default:
		return "UNKNOWN";
	}

}





//end of FileConfig.cpp

}