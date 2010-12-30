// Random.h: interface for the CRandom class.
#include <math.h>
#include <ctime>
#ifndef RANDOM_H_H 
#define RANDOM_H_H  

class CRandom  
{
public:

	//-----Common Random functions
	static void SetRandomSeed(unsigned s=0); //seed
	//
	static int Random(void);

	//-----Random generated obeys a special probability distribution

	//1) Uniformly distributed

	static double Random_U(); //Generate uniformly distributed random var on (0, 1)
	static int Random_U(int lower ,int upper); //Generate uniformly distributed random var on [lower, upper],integer
	static double Random_U(double lower ,double upper);//Generate uniformly distributed random var on [lower, upper],real
    
	//2) Poisson distributed
	static long Random_P (double lumda);//Generate Poisson distributed random :p(lumda)
	
	//3) Exponent distributed
	static double Random_E (double m);//Generate Exponent distributed random :E(lumda)
    
	//4) Erlang distributed
    static double Random_Erlang(double m, double s);//Generate Erlang distributed random :Erlang(mean,standard deviation)

	//5) Normal distributed
    static double Random_N (double m=0, double s=1); //Generate Normal distributed random :Erlang(mean,standard deviation),default N(0,1)
    
	//6)Log Normal distributed
	static double Random_N_Log(double m=0, double s=1);//Generate Log Normal distributed random 

	//7)Pareto distributed
    static double Random_Pareto(double scale, double shape);//Generate Pareto distributed random
	/*

#define random_pareto(sn, scale, shape) \
	((double) (scale) * pow(myrandom(sn), -1.0/(double)(shape)))
*/	
	
	//-----Big Random number
	static void Random_LargeNum(void * pNum,
		                        unsigned int NumofBits);//Generate NumofBits bits random number

	//-----Asistant functions
	static void random_reset(void); //Make the new seed to be equal to the last used one
	static int Round(double Num);   //Round off to integet 
	static double Round(double Num,
		                double ReservedDecimalDigits);//Round off to the ReservedDecimalDigits-th decimal place 

	//Construction / Destruction

	CRandom();
	virtual ~CRandom();

private:

	static unsigned dwOldSeed;

};

//
//
//
//#endif // !defined(AFX_RANDOM_H__BE9C67C2_3A3D_4B54_B85A_A25EFBB6C945__INCLUDED_)

#endif 





















