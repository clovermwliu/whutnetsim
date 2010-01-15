#ifndef	ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include <iostream>
#include <string>
#include <sstream>

//#include "G_Common_def.h"
typedef unsigned long   Error_t;
typedef std::string     Error_str;


using namespace std;

namespace WhuTNetSimConfigClass{

#define ERROR_NO_ERROR 0x00000001

class CErrorHandler
{
public:
	
	CErrorHandler()
	:error_code(ERROR_NO_ERROR),err_str("")
	{

	}

	virtual ~CErrorHandler(void) {}

public:

	Error_t GetLastError() {return error_code;}
	virtual Error_str GetLastErrorEx()=0;
	Error_str GetLastErrorIdentify() {stringstream stream_value(stringstream::out); stream_value << hex<<error_code;return "ERROR "+stream_value.str();}


protected:

	void SetLastError(Error_t dwErrcode) {error_code=dwErrcode;}
	void SetLastErrorStr(Error_str str) {err_str=str;}

protected:

	Error_t error_code;
	Error_str err_str;

};

}//end of ERRORHANDLER_H_




#endif