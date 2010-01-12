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



class CFileScript :	public CFileConfig
{

	friend class CItemLine;
	template <typename ValueType> friend class CGenericConfigItem;


public:
	CFileScript(const string& strFilePath);
	~CFileScript(void);

public:

	bool InitExpressionBySectionName(string expsection,
		                             CExpressionParse& exp,
		                             string expform="Formula");

    // void WriteExpSection();


	bool InitPredicationItemBySectionName(string pred_item_section,
		                                  CPredicationItem& pred,
										  string keyleft="LeftExp",
										  string keyright="RightExp",
										  string keyop="Operator");

	// void WritePredItemSection();

	bool InitConditionBySectionName(string condsection,
		                            CExpCondition& cond,
									string condkey="Condition");

	// void WriteConditionSection();






};


}//end of FILESCRIPT_H_

#endif
