
#ifndef __STDTS_H__ 
#define __STDTS_H__ 

#pragma once
#include "HiberTopoBase.h"
#include "PlatTopoBase.h"
#include <string>
using namespace std;

class CStdTS : public CHiberTopoBase
{
public:
	CStdTS(const string _file);
	~CStdTS(void);
public:
	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
};
#endif