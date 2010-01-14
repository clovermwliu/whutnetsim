#ifndef FILESCRIPT_H_
#define FILESCRIPT_H_

#include <ctype.h>
#include <map>

#include "FileConfig.h"
#include "ExpressionParse.h"
#include "PredicationItem.h"
#include "ExpCondition.h"
#include "ElementCustom.h"
#include "GenericConfigItem.h"


using namespace std;

namespace WhuTNetSimConfigClass{

#define CHAR_UNDERLINE        '_'
#define CHAR_INVERTED_COMMA   '\''
#define CHAR_INVERTED_COMMA2  '\"'


#define  ERROR_SCRIPT_SUCCESS                         SUCCESS_NO_ERROR
#define  ERROR_SCRIPT_INITIAL_EXP_FAIL                0x00010101
#define  ERROR_SCRIPT_EXP_NOT_FOUND                   0x00010102
#define  ERROR_SCRIPT_INITIAL_PRED_FAIL               0x00010103
#define  ERROR_SCRIPT_PRED_NOT_FOUND                  0x00010104
#define  ERROR_SCRIPT_INITIAL_COND_FAIL               0x00010105
#define  ERROR_SCRIPT_COND_NOT_FOUND                  0x00010106
#define  ERROR_SCRIPT_CONDEXP_INVALID                 0x00010107
#define  ERROR_SCRIPT_CUNSTOM_ELEMENT_NOT_FOUND       0x00010108
#define  ERROR_SCRIPT_CUNSTOM_ELEMENT_INVALID         0x00010109

class CFileScript :	public CFileConfig
{

	friend class CItemLine;
	template <typename ValueType> friend class CGenericConfigItem;


public:
	CFileScript(const string& strFilePath);
	~CFileScript(void);

public:

	bool InitExpressionBySectionName(const string& exp_section,
		                             CExpressionParse& exp,
		                             const string& expform="Formula");

    // void WriteExpSection();


	bool InitPredicationItemBySectionName(const string& pred_item_section,
		                                  CPredicationItem& pred,
										  const string& keyleft="LeftExp",
										  const string& keyright="RightExp",
										  const string& keyop="Operator");

	// void WritePredItemSection();

	bool InitConditionBySectionName(const string& cond_section,
		                            CExpCondition& cond,
									int id=0,
									const string& condkey="Condition");

	// void WriteConditionSection();

	bool InitCustomElementBySectionName(const string& cust_ele_section,
		                                CElementCustom& cust_ele,
										const string& keyoption="Option",
										const string& keydefault="Default");




	unsigned long GetLastError() {return dwErr_code;}
	string GetLastErrorEx();

public:


	static bool isValidSymbol(const char c);

private:

	unsigned long dwErr_code;
	string err_str;
	void SetLastError(unsigned long e) {dwErr_code=e;}
	void SetLastErrorStr(string s) {err_str=s;}

};


}//end of FILESCRIPT_H_

#endif
