// Test the dumbbell topology stock object.
// George F. Riley.  Georgia Tech, Fall 2002

#include <time.h>
#include "simulator.h"
#include "scheduler.h"
#include "dumbbell.h"
#include "linkp2p.h"
#include "ratetimeparse.h"
#include "tcp-tahoe.h"
#include "application-tcpsend.h"
#include "application-tcpserver.h"
#include "globalstats.h"
#include "droptail.h"
#include "ipv4.h"
#include "validation.h"
#include "TopoAnalysis.h"
#include "Ring1.h"
#include "PlatTopoBase.h"
#include "Clique1.h"
#include "WS.h"
#include "debug.h"
#include "star1.h"
#include "HiberTopoBase.h"
#include "dumbbell1.h"
#include "tree1.h"
#include "BA.h"
using namespace std;
//class CRing;
//class CPlatTopoBase;

int main(int argc, char** argv)
{
	//srand((int)time(0));

	Simulator s;

	vector<int> ivec;
	Count_t nc = 10;
	Node::DefaultShape(Node::CIRCLE);
	Linkp2p lk(Rate("10Mb"), Time("20ms"));

	/*CPlatTopoBase *r = new CRing(nc, IPAddr("192.168.0.1"), lk);
	r->GenerateTopo();
	r->SetLocationViaBoundBox(Location(0,0), Location(5,5));*/


	////树的测试
    //Count_t  treeNum=4;
	//CPlatTopoBase *p=new Star1(treeNum,lk,IPAddr("192.168.0.1"));                  //两个放一起
	//p->GenerateTopo();
	//p->StarSetLocationViaBoundBox(Location(0,0), Location(5,5));

	//Count_t  t,t2;
	//CPlatTopoBase *pleaf[4];

	//int i=-1;
	//for (t=0;t<treeNum;t++)
	//{
	//	Node* leaf=p->GetLeaf(t);

	//	pleaf[t]=new Star1(5,leaf,lk,IPAddr("192.168.0.1"));                  //两个放一起
	//	pleaf[t]->GenerateTopo();

	//	pleaf[t]->StarSetLocationViaBoundBox(Location(0,0), Location(5,5),M_PI*i/4,M_PI/(2));
	//	i=i+2;
	//}
	//CPlatTopoBase *p2[4];

	//i=-1;
	//for (t=0;t<treeNum;t++)
	//{
	//	for (t2=0;t2<5;t2++)
	//	{
	//		Node* leaf=pleaf[t]->GetLeaf(t2);

	//		p2[t]=new Star1(5,leaf,lk,IPAddr("192.168.0.1"));                  //两个放一起
	//		p2[t]->GenerateTopo();


	//		p2[t]->StarSetLocationViaBoundBox(Location(0,0), Location(5,5),M_PI*i/4,M_PI/(2));
	//		
	//	}
	//	i=i+2;
	//}


	//Node* leaf=pleaf[0]->GetLeaf(0);

	//CPlatTopoBase *other=new Star1(5,leaf,lk,IPAddr("192.168.0.1"));                  //两个放一起
	//other->GenerateTopo();

	//other->StarSetLocationViaBoundBox(Location(0,0), Location(5,5),-M_PI/4,M_PI/2);


	//Dumbbell测试
	//CHiberTopoBase *chiber = new Dumbbell1(10,10,2,IPAddr("192.168.0.1"),IPAddr("192.168.0.1"),lk,0);
 //   chiber->GenerateTopo();
	//chiber->SetLocationViaBoundBox(Location(0,0),Location(10,10));


	  //// 创建一颗树
	  //CPlatTopoBase *tree =new Tree1(3, 4, lk, IPAddr("192.168.1.1"));
	  //tree->GenerateTopo();
	  //tree->SetLocationViaBoundBox(Location(0,0), Location(10,5));

	//BA网络
	CPlatTopoBase *ring =new CRing(10);
	ring->GenerateTopo();
    ring->SetLocationViaBoundBox(Location(0,0),Location(90,100));

	if (!Validation::noAnimation)
    {
      s.StartAnimation(0, true);
      s.AnimationUpdateInterval(Time("10us")); // 10us initial update rate
    }

    s.Progress(1);
    s.StopAt(100);
    s.Run();
}
