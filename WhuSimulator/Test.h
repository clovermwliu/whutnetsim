// trace [7/16/2010 mpiuser]
#pragma once
#include "Simulator.h"
#include <string>
using namespace std;

#include "FileConfig.h"
//#include "GenericConfigItem.h"
//using namespace WhuTNetSimConfigClass;

#define  ERROR_TASK_FILENOTEXSIT                     0x00060101

int TestMain(const string str_ini_file);
