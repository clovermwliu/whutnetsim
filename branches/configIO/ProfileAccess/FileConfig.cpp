#include "StdAfx.h"
#include "FileConfig.h"
#include <assert.h>


using namespace std;

namespace WhuTNetSimConfigClass{



//----------------------------------------------------------------------------------
// Sym：FileConfig
// FileConfig 类的实现
//----------------------------------------------------------------------------------	
	
CFileConfig::CFileConfig(const string& strFilePath)//在初始化列表中对成员fileName赋值
: fileName(strFilePath), _SectionNum(0), _ItemNum(0),CErrorHandler()
{
	//nothing to do;
}

CFileConfig::~CFileConfig(void)
{
}



bool  CFileConfig::LoadFile()
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
描述：打开并读取一个配置文件，文件路径存在于CFileConfig::fileName中
参数：[OUT]SectionNum:当前文件中的sections个数
      [OUT]ConfigurationItemNum:当前文件中的配置项目个数

返回：打开成功则返回1
否则返回错误码
开发时间：2009/11/11
*/
{
	bool tmp=LoadFile();
	SectionNum=_SectionNum;
	ConfigurationItemNum=_ItemNum;
	return tmp;

}



bool CFileConfig::UpdateFile(bool bIngronCancelLine)
/*
描述：打开并重新生成一个配置文件，文件路径存在于CFileConfig::fileName中
参数：bIngronCancelLine为true则将带有@的配置行不回写入文件
返回：打开成功则返回1
      否则返回错误码
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
描述：调用嵌套迭代器类的RebindingIterator函数，将iter_beg指向第一个配置项，返回iter_beg的引用
*/
{
	iter_beg.RebindingIterator(fileData);
	return iter_beg;
}


CFileConfig::iterator&  CFileConfig::end()
/*
//调用嵌套迭代器类的RebindingIterator函数，并将iter_end指向最后一个有效配置项的后一个（实际上的文件尾，section为最后一个有效section），并返回iter_end的引用
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
描述：取section节的所有有效key,放在lnames中，返回key的数量
参数：[out]lnames
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

	//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
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
描述：取section节的所有有效配置项,放在litems中，返回item的数量
参数：[out]litems
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

	//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
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
描述：返回该section节有效配置项的数量
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

	//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
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
描述：取文件当前全部section名字，放在lnames中，返回_sectionNum
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
描述：字符串匹配
*/
{
	if (0 == strcmp(pLstr, pRstr))	return true;
	return false;

}
bool CFileConfig::match(string lstr,string rstr)
/*
描述：字符串匹配
*/
{
	return lstr==rstr;

}

bool CFileConfig::CancelConfigLine(const string& section, const string& key)
/*
描述：在fileData中section，key这一项前面加"@"符号
返回：如果遍历完fileData均未发现满足section,key的项，则返回false;
*/
{
	
	//先在sectionList中匹配section，如果该section不存在直接返回失败
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}
 
	if (quickiter==SectionList.end()) return false;
			
	//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
	CFileConfig::iterator iter(fileData,*quickiter);
	
	while (!iter.end()){

		if (match(section, iter.GetCurSection()) && match(key, iter.GetCurKey()))	{//match(section, iter.GetCurSection())不可少，当section当前没有有效配置项时，iter直接指向后面的section中的有效配置项去了
			
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
描述：给section的key项设置value值，返回该配置项在fileData中的迭代器
*/

{
	
	bool itemExisting=true;
	//先在sectionList中匹配section，如果该section不存在直接返回失败
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

		//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
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

	//如果没有这个section,key的项，则整体添加之
    
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
描述:设置新配置项
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
			list<string>::iterator iter2=fileData.erase(iter); //更新原来该项的注释，注释位于该配置项的下一行
			fileData.insert(iter2,str);
		}else{
			fileData.insert(iter,str);//给新项添加注释，注释位于该配置项的下一行
		}
		return;
	}

	fileData.insert(iter,str); //新项存在于文件尾，直接在文件尾添加注释
	return;
	
}

list<string>::iterator  CFileConfig::AddConfigLine(const string& section, const string& item)
/*
描述：添加section的新配置项目（item:key=value）,返回该配置项在fileData中的迭代器
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
			
			fileData.insert(iter.GetCurIndexIter(), item);//前插入
			return --(iter.GetCurIndexIter());
		
		}
	
	}

	// 新建整个配置项
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
描述：根据section和key在fileData上查找该配置项，将该项的注释返回给remark,该项目的值（一个string）返回给value
参数：[out]remark,value
返回：如果该项不存在，则函数返回false,remark和value均设为空串
*/
{
	//先在sectionList中匹配section，如果该section不存在直接返回失败
	list<list<string>::iterator> ::iterator quickiter;
	string sectionG;
	for(quickiter=SectionList.begin();quickiter!=SectionList.end();quickiter++){

		size_t start = (*(*quickiter)).find_first_not_of(CHAR_SECTION_BEGIN);
		size_t end = (*(*quickiter)).find(CHAR_SECTION_END);
		sectionG = (*(*quickiter)).substr(start, end - 1);
		if (sectionG==section)  break;
	}

	if (quickiter==SectionList.end()) { //没有匹配的section

		remark="";
		value="";
		return false;
	}

	//迅速构造一个CFileConfig::iterator，指向该section的第一个有效配置项，再匹配key
	CFileConfig::iterator iter(fileData,*quickiter);

	while (!iter.end())
	{
		if (match(section, iter.GetCurSection()) &&	match(key,iter.GetCurKey()))
		{
			value=iter.GetCurValue();//获得value

			//以下去获得该项的注释
			string remark= *(iter.GetNextIndexIter()); //默认情况下，注释应该紧跟着配置项
			if(!CFileConfig::iterator::IsBlankLine(remark)){
				
				size_t start = remark.find_first_not_of(STR_TAB);
				if (remark.at(start)!=CHAR_REMARK_LINE) remark="";
			}
			return true;
		}
		iter++;
	}

	//如果没有这个section,key的项
	remark="";
	value="";
	return false;

}

bool CFileConfig::BackupFile()
/*
描述：对当前配置文件进行备份。在UpdateFile()中被调用，以保证更新配置文件时不会丢失原有配置信息。

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
//Sym：Iterator
//以下是FileConfig中迭代器的实现
//------------------------------------------------------------------------


CFileConfig::iterator::iterator(list<string>& vStrData)
/*
Constructor:初始化一个CFileConfig::iterator，CFileConfig::iterator生成完后指向配置文件第一个有效配置项
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
Constructor:迅速初始化一个CFileConfig::iterator，传入形参是一个指向vStrData容器中某个section节标记行的迭代器
            CFileConfig::iterator生成完后指向该节第一个有效配置项,
			如该节当前没有有效配置项则指向后面第一个有有效配置项的节的该有效配置项，或指向文件尾
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
描述：设置iter_cur_index指向第一个Configuration Line，并将其对应的section名读取到CFileConfig::iterator::section中

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
描述：返回iter_cur_index指向配置行=号右边的字符串
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
	iter_cur_section = rhs.iter_cur_section;
	return *this;
}

bool operator==(const CFileConfig::iterator& lhs,const CFileConfig::iterator& rhs)
/*
描述：重载操作符operator==
返回：若左右两个操作数的 pStr, section 及 iter_cur_index均相同则返回 true    
*/
{
	return (lhs.pStr==rhs.pStr)&&(lhs.iter_cur_index==rhs.iter_cur_index)&&(lhs.section==rhs.section)&&(lhs.iter_cur_section == rhs.iter_cur_section);

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
描述：重载操作符operator-- 
完成一个 CFileConfig::iterator的自减操作。内部移动iter_cur_index，使之指向pStr容器内一个合法的configuration line
备注：这是前缀操作符--的实现
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
描述：返回一个迭代器，指向下一个section中第一个配置项
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
描述：重新将本迭代器对象与一个list<string>的容器绑定

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
描述：判断str是否为一个空行，包括那些仅有空格和TAB符号的字符串均认为是空行
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

描述：遍历pStr指向的容器，解析每一个string。
      如遇到空字符串或以#和@开头的行则略过，自增iter_cur_index，使之指向pStr容器的下一行
	  如遇到[ ]为开始（结尾）的，表示遇到一个新的section line,则将节名存放到CFileConfig::iterator::section中，并自增iter_cur_index，使之指向pStr容器的下一行

返回：#define CONFIGURATION_LINE     0x00000001
	  #define SECTION_LINE           0x00000010
	  #define REMAK_LINE             0x00000100
	  #define CANCEL_LINE            0x00001000
	  #define FILE_END_LINE          0x00010000
	  #define BLANK_LINE             0x01000000

*/

{
	if (iter_cur_index ==pStr->end()) return FILE_END_LINE; //当前iter_cur_index已指向文件尾
		
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
// Sym：ConfigItem
// ConfigItem 类的实现
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
描述：从配置文件中读配置项
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