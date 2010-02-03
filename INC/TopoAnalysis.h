#pragma once

#ifndef __CTOPOANALYSIS__H__ 
#define __CTOPOANALYSIS__H__ 
#include "PlatTopoBase.h"

class CTopoAnalysis
{
public:
	CTopoAnalysis(void);
public:
	~CTopoAnalysis(void);
	void AnalysisDegree(CPlatTopoBase* base);
};
#endif