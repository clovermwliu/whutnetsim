
#ifndef __dumbbell1_h__ 
#define __dumbbell1_h__ 

#include "common-defs.h"
#include "node.h"
#include "HiberTopoBase.h"
class Node;
class Linkp2p;
class Queue;

//Doc:ClassXRef
class Dumbbell1 : public CHiberTopoBase
{
public:

	Dumbbell1(Count_t l, Count_t r,Mult_t m,IPAddr_t lip, IPAddr_t rip,
			 const Linkp2p& link = Linkp2p::Default(), SystemId_t id = 0);
  ////Doc:Method
  //Dumbbell1(Count_t, Count_t, Mult_t, SystemId_t = 0);
  //  
  //Dumbbell1(Count_t, Count_t, Mult_t, const Linkp2p&, SystemId_t = 0);
  // 
  //Dumbbell1(Count_t, Count_t, Mult_t, 
  //         IPAddr_t leftIP, IPAddr_t rightIP, SystemId_t = 0);
  //  
  //Dumbbell1(Count_t, Count_t, Mult_t, 
  //         IPAddr_t leftIP, IPAddr_t rightIP,         // Specify ip addresses
  //         const Linkp2p&, SystemId_t = 0); // Specify link for leafs
 
	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										const Location& BoundBoxRightUpper,
										BoxType  type);
private:
  //NodeId_t   first;      // There is a global node list, we don't need one 
  //NodeId_t   last;       // Just log first/last+1 entries
  Count_t    leftCount;  // Number of left side nodes
  Count_t    rightCount; // Number of right side nodes
  IPAddr_t   leftip;
  IPAddr_t   rightip;
  CPlatTopoBase *left;
  CPlatTopoBase *right;

  //Linkp2p    routerLink;
  Mult_t     bound;          //´ø¿í
  
private:
  //void ConstructorHelper(Count_t, Count_t, Mult_t, 
  //                       IPAddr_t, IPAddr_t,
  //                       const Linkp2p&, SystemId_t);
};

#endif


