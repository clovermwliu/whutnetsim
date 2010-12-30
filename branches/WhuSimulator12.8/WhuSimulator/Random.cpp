// Random.cpp: implementation of the CRandom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Random.h"
#include <stdlib.h>



unsigned CRandom::dwOldSeed=0;


CRandom::CRandom()
{

}

CRandom::~CRandom()
{

}

//////////////////////////////////////////////////////////////////////
// Common
//////////////////////////////////////////////////////////////////////

void CRandom::SetRandomSeed(unsigned s)
{
	unsigned Timeseed=(unsigned)time( NULL );
	if (s==0) {
		srand(Timeseed);
		dwOldSeed=Timeseed;
	}else{
		srand(s);
		dwOldSeed=s;
	}

}

int CRandom::Random(void)
{
	return rand();
}


//////////////////////////////////////////////////////////////////////
// Special probability distribution
//////////////////////////////////////////////////////////////////////

double
CRandom::Random_U()
{
	long low;
	low=(long)(rand()<<16)+rand();
	return(low * 4.656612875e-10);

}


/*

int
CRandom::Random_U(int lower ,int upper)
{
//	ASSERT(lower<=upper);
	double b2;
	b2 = (double)upper - lower + 1.0;
	b2 = (double) b2 *  Random_U() - 0.5;
	return(lower + (int)b2);
}

*/

int
CRandom::Random_U(int lower ,int upper)
{
	//	ASSERT(lower<=upper);
	return (int)(lower+ (upper - lower + 1) * Random_U());
}

double
CRandom::Random_U(double lower ,double upper)
/* stream number */
/* lower and upper bounds */
{
//	ASSERT(lower<=upper);
	return(lower+ (upper - lower) * Random_U());
}


long
CRandom::Random_P(double lumda)
/* lumda:average arrivals in unit time */
{
	long  n = -1;
	double e, p = 1.0;

	e = exp((double) -lumda);
	do {
		p *= Random_U();
		n++;
	} while (p >= e);
	return(n);
}


double
CRandom::Random_E(double mean)
/* mean value */
{ 
	return(-mean * log(Random_U()));
}

double
CRandom::Random_Erlang(double m, double s)
/* mean and standard deviation */
{
//	ASSERT(s<=m);
	int i, k;
	double z;
	z = m / s;
	k = (int) z * z;
	z = 1.0;
	for (i = 0; i < k; i++){
		z *= Random_U();
	}
	return(-(m/k) * log(z));
}

double
CRandom::Random_N (double m, double s)
/* mean and standard deviation */
{
	double v1, v2, w, z1;
	double z2 = 0.0;

	if (z2 != 0.0) {
		z1 = z2;
		z2 = 0.0;
	}

	else {
		do {
			v1 = 2.0 * Random_U() - 1.0;
			v2 = 2.0 * Random_U() - 1.0;
			w = v1 * v1 + v2 * v2;
		} while (w >= 1.0);

		w = sqrt((-2.0 * log(w)) / w);
		z1 = v1 * w;
		z2 = v2 * w;
	}

	return(m + z1 * s);
}

double
CRandom::Random_N_Log(double m, double s)
{
	return (m * exp(s*Random_N()));
}

double
CRandom::Random_Pareto(double scale, double shape)
{
	return (scale * pow (Random_U(), -1.0/shape));
}


//////////////////////////////////////////////////////////////////////
// Big Random number
//////////////////////////////////////////////////////////////////////
void
CRandom::Random_LargeNum(void * pNum,
						 unsigned int NumofBits)

{
//	ASSERT(pNum);
//	ASSERT(NumofBits>0);

	int Quotient=NumofBits/32;
	int Residue=NumofBits%32;
	int i;

	for(i=0;i<Quotient;i++){
		*((unsigned int *)pNum+i)=((unsigned int)rand()<<16)+rand();
	}

	if (Residue>15) {
		*((unsigned short *)((unsigned int *)pNum+i))=(unsigned int)rand();
		*(((unsigned short *)((unsigned int *)pNum+i))+1)=(unsigned int)rand()>>(31-Residue);
	}
	if (Residue<15 && Residue>0){
		*((unsigned short *)((unsigned int *)pNum+i))=(unsigned int)rand()>>(15-Residue);
	}

}

//////////////////////////////////////////////////////////////////////
// Asistant
//////////////////////////////////////////////////////////////////////

void 
CRandom::random_reset(void)
{
	srand(dwOldSeed);
}

int
CRandom::Round(double Num)
{
	return (int)(Num+0.5);
}

double
CRandom::Round(double Num,
			   double ReservedDecimalDigits)

{
	int tmp=pow((double)10,ReservedDecimalDigits);
	return ((double)((int)(Num*tmp+0.5)))/tmp;
}

//////////////////////////////////////////////////////////////////////
// Private member functions
//////////////////////////////////////////////////////////////////////

