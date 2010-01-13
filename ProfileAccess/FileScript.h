#ifndef FILESCRIPT_H_
#define FILESCRIPT_H_

#include <ctype.h>
#include <map>

#include "FileConfig.h"
#include "ExpressionParse.h"
#include "PredicationItem.h"
#include "ExpCondition.h"
#include "GenericConfigItem.h"


using namespace std;

namespace WhuTNetSimConfigClass{

#define  ERROR_SCRIPT_SUCCESS              0x00020000
#define  ERROR_SCRIPT_INITIAL_EXP_FAIL     0x00020001
#define  ERROR_SCRIPT_EXP_NOT_FOUND        0x00020002
#define  ERROR_SCRIPT_INITIAL_PRED_FAIL    0x00020003
#define  ERROR_SCRIPT_PRED_NOT_FOUND       0x00020004
#define  ERROR_SCRIPT_INITIAL_COND_FAIL    0x00020005
#define  ERROR_SCRIPT_COND_NOT_FOUND       0x00020006
#define  ERROR_SCRIPT_CONDEXP_INVALID      0x00020007

class CFileScript :	public CFileConfig
{

	friend class CItemLine;
	template <typename ValueType> friend class CGenericConfigItem;


public:
	CFileScript(const string& strFilePath);
	~CFileScript(void);

public:

	bool InitExpressionBySectionName(const string& expsection,
		                             CExpressionParse& exp,
		                             const string& expform="Formula");

    // void WriteExpSection();


	bool InitPredicationItemBySectionName(const string& pred_item_section,
		                                  CPredicationItem& pred,
										  const string& keyleft="LeftExp",
										  const string& keyright="RightExp",
										  const string& keyop="Operator");

	// void WritePredItemSection();

	bool InitConditionBySectionName(const string& condsection,
		                            CExpCondition& cond,
									const string& condkey="Condition");

	// void WriteConditionSection();

	unsigned long GetLastError() {return dwErr_code;}
	string GetLastErrorEx();

private:

	unsigned long dwErr_code;
	string err_str;
	void SetLastError(unsigned long e) {dwErr_code=e;}
	void SetLastErrorStr(string s) {err_str=s;}

};


}//end of FILESCRIPT_H_

#endif
