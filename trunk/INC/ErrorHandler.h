#ifndef	ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include <iostream>
#include <string>
#include <sstream>

#include "G_common_defs.h"



using namespace std;

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

	virtual Error_str GetLastErrorEx()=0; //Ҫ���������ʵ�ָú�����һ��ģ�GetLastErrorEx() {return GetLastErrorStr();}
    //
	Error_t GetLastError() {return error_code;}
	Error_str GetLastErrorIdentify() {stringstream stream_value(stringstream::out); stream_value << hex<<error_code;return "ERROR "+stream_value.str();}


protected:

	Error_str GetLastErrorStr() {return err_str;}
	//
	void SetLastError(Error_t dwErrcode) {error_code=dwErrcode;}
	void SetLastErrorStr(Error_str str) {err_str=str;}

private:

	Error_t error_code; //������
	Error_str err_str;  //����������Ϣ

};

//end of ERRORHANDLER_H_




#endif