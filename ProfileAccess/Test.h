#pragma once

#ifndef TEST_H_
#define TEST_H_


namespace WhuTNetSimConfigClass{


class CTest
{
public:
	CTest(void);

	~CTest(void);

public:

	static double GetValue1(void) {return (double)1; }

	static double GetValue2(void) {return (double)2; }

};

}


#endif