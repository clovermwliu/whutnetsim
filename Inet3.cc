#include "Inet3.h"
#include <stdlib.h>
#include "random.h"
#define DEGREE_1_SLOPE .292 /* poly-fit under matlab */
//#define DEGREE_1_SLOPE .35
#define DEGREE_1_INTERCEPT 462  /* poly-fit under matlab */
#define START_DEGREE 2 /* related to constant below */

/********************************************************************************/
/* complementary cumulative distribution function of degree frequency vs. degree*/
/*(Eq. 1 in the TR)   \bar{F}(d) = e^c * d^(a*t + b)                            */
/********************************************************************************/

#define a    .00324
#define b  -1.223
#define c   -.711

/***********************************/
/* degree vs. rank growth powerlaw */
/* d = exp(p*t + q) * r^R          */
/***********************************/
#define R       -.87  
#define p       .0228  
#define q       6.5285
#define DR_Frac .02 /* the fraction of nodes this law applies to */

/*************************************/
/* pairsize within h hops growth law */
/* P(t,h) = exp(s(h)*t)*P(0,h)       */
/*                                   */
/* therefore,                        */
/* P(t,0) = exp(s(0)*t)*P(0,0)       */
/* t is computed as:                 */
/*   log(P(t,0)/P(0,0))/s(0)         */
/*************************************/
#define P00     3037   /* P(0,0) */     
#define s0      .0281  /* s(0) */

/**** NOT BEING USED IN INET-3.0 ****************/
/************************************************/
/* frequency vs. degree growth powerlaw         */
/* f = C(t)*d^O                                 */
/* C(t) will be computed from Exponential Law 1 */
/************************************************/
#define O       -2.2  /* exponent computed using all nodes */
#define O_no1   -2.42 /* exponent without degree 1 nodes */

CInet3::CInet3(float _frac,int   _seed,int _node_num,
			   IPAddr_t i ,SystemId_t id,const Linkp2p& link)
:CPlatTopoBase(_node_num,i,link,id),frac(_frac),seed(_seed),node_num(_node_num)
{
}

CInet3::~CInet3(void)
{
}
bool CInet3::GenerateTopo()
{
	 AllocateForNode();       //为GTNetS的拓扑新建节点
	 ConstructorHelper();
	 
	 return true;
}
bool CInet3::ConstructorHelper()
{
	node_type *node;                                  //inet的节点放置
	node = (node_type *)malloc(sizeof(node_type) * node_num);

	int degree_one = 0;
	degree_one = (int)((float)node_num * frac);
	if (degree_one == 0)
		degree_one = DEGREE_1_SLOPE * node_num + DEGREE_1_INTERCEPT;

	generate_degrees(node, node_num, degree_one, seed);//inet对node进行处理，确定每个点的度
	connect_nodes(node, node_num, seed);               //为每个节点连边
	free(node);
	return true;
}
void CInet3::AllocateForNode()
{
	for(int i=0;i<nodeCount;i++) 
	{
		Node* addNode;
		addNode = new Node(sid);
	}
}
int degree_compare(const void *a1, const void *a2)
{
	node_type *n1 = (node_type *)a1;
	node_type *n2 = (node_type *)a2;

	if (n1->degree < n2->degree)
		return 1;
	else if (n1->degree == n2->degree)
		return 0;
	else
		return -1;
}
void CInet3::generate_degrees(node_type *node1, int node_num, int degree_one, int seed)
{
#define DEBUG_generate_degrees

	int i;
	int degree, nodes, edges = 0, F;
	float t, C_t;
	int node_num_no1, degree_two = 0, rank;
	float D_FRAC[19], base;

	float ccdfIdeal;
	float ccdfActual;
	float diff;
	int numNodesToAdd;

	float ccdfSlope;

	/* calculate what month  (how many months since nov. '97) */
	/*  corresponds to this topology of size node_num.       */                                                        
	t = log((float)node_num/(float)P00)/s0;

	/* fprintf(stderr,"MONTH = %f\n", t); */

	/* using t calculated above, compute the exponent of the ccdf power law */
	ccdfSlope = (a * t) + b;

	/********************************************/
	/* reinitialize the random number generator */
	/********************************************/
	random_reset();

	/***************************/
	/* generate degree 1 nodes */
	/***************************/
	node_num_no1 = node_num - degree_one;
	nodes = 0;
	for (i=node_num-1; i>=node_num_no1; --i)
	{
		node1[i].nid = i;
		node1[i].degree = 1;
		node1[i].free_degree = 1;
	}

	nodes += degree_one;

	// for each degree see if we need to add any of it to obtain the appropriate value on the ccdf
	degree = 2;
	while (nodes <= node_num && degree < node_num)
	{
		// this is where we should be at
		ccdfIdeal = exp(c) * pow((float)degree,ccdfSlope);
		// this is were we are before adding nodes of this degree
		ccdfActual = (1.0 - ((float)nodes/(float)node_num));

		// calc how many nodes of this degree we need to add to get the ccdf right
		diff = ccdfActual - ccdfIdeal;
		//fprintf(stderr,"%f  %f  %f\n",ccdfIdeal,ccdfActual,diff);
		if (diff * (float)node_num > 1.0)   // we actually need to add a node at this degree
		{
			numNodesToAdd = (int)(diff * (float)node_num);
			for (i = node_num - nodes-1; i >= node_num - nodes - numNodesToAdd; --i)
			{
				//fprintf(stderr, "- %d  %d\n", degree,i);
				node1[i].nid = i;
				node1[i].degree = degree;
				node1[i].free_degree = degree;

			}
			nodes += numNodesToAdd;
		}  
		++degree;
	}

	/* use the degree-rank relationship to generate the top 3 degrees */
	for(i=0; i<3; ++i)
	{
		node1[i].nid = i;
		rank = i + 1;
		node1[i].degree = pow((float)(rank), (float)R)*exp(q)*pow(((float)node_num/(float)P00),(float)(p/s0));
		node1[i].free_degree = node1[i].degree;

		//fprintf(stderr,"* %d\n",node[i].degree);
	}

	qsort(node1, node_num, sizeof(node_type), degree_compare);

#ifdef DEBUG_generate_degrees
	cout<<"node[0].degree = "<<node1[0].degree;
#endif /* DEBUG_generate_degrees */

	/************************************/
	/* the sum of node degrees can't be */
	/* odd -- edges come in pairs       */
	/************************************/
	for(i=0,edges=0;i<node_num;++i)
		edges += node1[i].degree;
	if ( edges % 2 == 1 )
	{
		node1[0].degree++;
		node1[0].free_degree++;
	}

	for (i=0; i<node_num; ++i)
		node1[i].nid = i;

} 

int is_graph_connectable(node_type *node1, int node_num)
{
  int i;
  int F_star, F = 0, degree_one = 0;

  for (i=0; i<node_num; ++i)
  {
    if (node1[i].degree == 1)
      ++degree_one;
    else
      F += node1[i].degree;
  }  

  //fprintf(my_stderr, "is_graph_connectable: F = %d, degree_one = %d\n", F, degree_one);

  F_star = F - 2*(node_num - degree_one - 1);
  if (F_star < degree_one)
    return 0;

  return 1;
}    
/*************************************/
/* the main node connection function */
/*************************************/
void CInet3::connect_nodes(node_type *node1, int node_num, int seed)
{
	int i, j, k, degree_g2;
	int *G, *L, G_num, L_num, g, l;
	int *p_array, p_array_num, *id, *flag;

	int added_node;
	int **weighted_degree_array;
	double distance;
	int *freqArray;

	/************************/
	/* satisfaction testing */
	/************************/
	if (!is_graph_connectable(node1, node_num))
	{
		fprintf(stderr, "Warning: not possible to generate a connected graph with this degree distribution!\n");
	}

	p_array_num = 0;
	degree_g2 = 0;
	for (i=0; i<node_num; ++i)
	{
		node1[i].nnp = (node_type **)malloc(sizeof(node_type *)*node1[i].degree);
		if (!node1[i].nnp)
		{
			fprintf(stderr, "connect_nodes: no memory for node[%d].nnp (%d bytes)!\n", i, sizeof(node_type *)*node1[i].degree);
			exit(-1);
		}
		for (j=0; j<node1[i].degree; ++j)
			node1[i].nnp[j] = NULL;

		/* the probability array needs be of size = the sum of all degrees of nodes that have degrees >= 2 */
		if (node1[i].degree > 1)
			p_array_num += node1[i].degree;


		/* set the position of the first node of degree 1 */
		if (node1[i].degree == 1 && node1[i-1].degree != 1)
			degree_g2 = i;
	}

	//fprintf(my_stderr, "connect_nodes: degree_g2 = %d\n", degree_g2);

	G = (int *)malloc(sizeof(int)*degree_g2);
	if (!G)
	{
		fprintf(stderr, "connect_nodes: no memory for G (%d bytes)!\n", sizeof(int)*degree_g2);
		exit(-1);
	}

	L = (int *)malloc(sizeof(int)*degree_g2);
	if (!L)
	{
		fprintf(stderr, "connect_nodes: no memory for L (%d bytes)!\n", sizeof(int)*degree_g2);
		exit(-1);
	}

	//fprintf(my_stderr, "connect_nodes: allocating %d element array for p_array.\n", p_array_num);

	/* we need to allocate more memory than just p_array_num because of our added weights       */
	/* 40 is an arbitrary number, probably much higher than it needs to be, but just being safe */
	p_array = (int *)malloc(sizeof(int)*p_array_num*40);
	if (!p_array)
	{
		fprintf(stderr, "connect_nodes: no memory for p_array (%d bytes)!\n", (sizeof(int)*p_array_num));
		exit (-1);
	}

	id = (int *)malloc(sizeof(int)*degree_g2);
	if (!id)
	{
		fprintf(stderr, "connect_nodes: no memory for id (%d bytes)!\n", sizeof(int)*node_num);
		exit(-1);
	}

	flag = (int *)malloc(sizeof(int)*degree_g2);
	if (!flag)
	{
		fprintf(stderr, "no memory for flag (%d bytes)!\n", sizeof(int)*degree_g2);
		exit(-1);
	}

	/* weighted_degree_array is a matrix corresponding to the weight that we multiply  */
	/* the standard proportional probability by. so weighted_degree_array[i][j] is the */
	/* value of the weight in P(i,j). the matrix is topDegree x topDegree in size,     */
	/* where topDegree is just the degree of the node withh the highest outdegree.     */
	weighted_degree_array = (int**)malloc(sizeof(int*) * node1[0].degree + 1);
	for (i = 0; i <= node1[0].degree; ++i)
	{
		weighted_degree_array[i] = (int*)malloc(sizeof(int) * node1[0].degree + 1);
		if (!weighted_degree_array[i])
		{
			fprintf(stderr, "no memory for weighted_degree index %d!\n", i);
			exit(-1);
		}
	}

	/* determine how many nodes of a given degree there are.  */
	freqArray = (int*)malloc(sizeof(int) * node1[0].degree +1);

	// fill the freq array
	for (i = 0; i <= node1[0].degree; ++i)
		freqArray[i] = 0;

	for (i = 0; i < node_num; ++i)
		freqArray[node1[i].degree]++;


	/* using the frequency-degree relationship, calculate weighted_degree_array[i][j] for a all i,j */
	for (i = 1; i <= node1[0].degree; ++i)
	{
		for (j = 1; j <=  node1[0].degree; ++j)
		{
			if (freqArray[i] && freqArray[j]) // if these degrees are present in the graph
			{
				distance = pow(( pow( log((double)i/(double)j), 2.0) + pow( log((double)freqArray[i]/(double)freqArray[j]), 2.0)), .5);
				if (distance < 1)
					distance = 1;

				weighted_degree_array[i][j] = distance/2 * j;
			}
		}
	}

	/********************************/
	/* randomize the order of nodes */
	/********************************/
	for (i=0; i<degree_g2; ++i)
		id[i] = i;

	i = degree_g2;
	while (i>0)
	{
		j = random_uniform_int(seed, 0, i-1); /* j is the index to the id array! */
		L[degree_g2 - i] = id[j];

		for (; j<i-1; ++j)
			id[j] = id[j+1];
		--i;
	}

	/* do not randomize the order of nodes as was done in Inet-2.2. */
	/* we just want to build the spanning tree by adding nodes in   */
	/* in monotonically decreasing order of node degree             */	
	for(i=0;i<degree_g2;++i)
		L[i] = i; 

	/************************************************/
	/* Phase 1: building a connected spanning graph */
	/************************************************/
	G_num = 1;
	G[0] = L[0];
	L_num = degree_g2 - 1;

	while (L_num > 0)
	{   
		j = L[1]; 
		added_node = j;

		/******************************/
		/* fill the probability array */
		/******************************/
		l = 0;
		for (i=0; i<G_num; ++i)
		{
			if (node1[G[i]].free_degree)
			{
				if (node1[G[i]].free_degree > node1[G[i]].degree)
				{
					fprintf(stderr, "connect_nodes: problem, node %d(nid=%d), free_degree = %d, degree = %d, exiting...\n", G[i], node1[G[i]].nid, node1[G[i]].free_degree, node1[G[i]].degree);
					exit(-1);
				}

				for(j=0; j < weighted_degree_array[ node1[added_node].degree ][ node1[G[i]].degree ]; ++j, ++l)
					p_array[l] = G[i];
			}
		}

		/**************************************************************/
		/* select a node randomly according to its degree proportions */
		/**************************************************************/
		g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */

		/*****************************************************/
		/* redirect -- i and j are indices to the node array */
		/*****************************************************/
		i = p_array[g];
		j = added_node;

		/*if (node[i].nid == 0)
		fprintf(stderr, "phase I: added node %d\n", node[j].nid);*/

		node1[i].nnp[node1[i].degree - node1[i].free_degree] = node1+j;
		node1[j].nnp[node1[j].degree - node1[j].free_degree] = node1+i;
		Node::nodes[i]->AddDuplexLink(Node::nodes[j], link);

		/* add l to G and remove from L */
		G[G_num] = j;
		for (l=1; l < L_num; ++l)
			L[l] = L[l+1];

		--(node1[i].free_degree);
		--(node1[j].free_degree);
		++G_num;
		--L_num;
	}

	// fprintf(stderr, "DONE!!\n");
	/*************************************************************************/
	/* Phase II: connect degree 1 nodes proportionally to the spanning graph */
	/*************************************************************************/
	for (i=degree_g2; i<node_num; ++i)
	{
		/******************************/
		/* fill the probability array */
		/******************************/
		l = 0;
		for (j=0; j<degree_g2; ++j)
		{
			if (node1[j].free_degree)
			{
				for (k = 0; k < weighted_degree_array[ 1 ][ node1[j].degree ]; ++k, ++l)
					p_array[l] = j;
			}
		} 

		g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */
		j = p_array[g];

		node1[i].nnp[node1[i].degree - node1[i].free_degree] = node1+j;
		node1[j].nnp[node1[j].degree - node1[j].free_degree] = node1+i;
		Node::nodes[i]->AddDuplexLink(Node::nodes[j], link);

		--(node1[i].free_degree);
		--(node1[j].free_degree);
	}

	// fprintf(stderr, "DONE!!\n");
	/**********************************************************/
	/* Phase III: garbage collection to fill all free degrees */
	/**********************************************************/
	for (i=0; i<degree_g2; ++i)
	{
		for (j=i+1; j<degree_g2; ++j)
			flag[j] = 1;
		flag[i] = 0; /* no connection to itself */

		/********************************************************************/
		/* well, we also must eliminate all nodes that i is already         */
		/* connected to. bug reported by shi.zhou@elec.qmul.ac.uk on 8/6/01 */
		/********************************************************************/
		for (j = 0; j < (node1[i].degree - node1[i].free_degree); ++j)
			if ( node1[i].nnp[j] - node1 < degree_g2 )
				flag[ node1[i].nnp[j] - node1 ] = 0;

		if ( !node1[i].nnp[0] )
		{
			fprintf(stderr, "i = %d, degree = %d, free_degree = %d, node[i].npp[0] null!\n", i, node1[i].degree, node1[i].free_degree );
			exit(-1);
		}

		flag[node1[i].nnp[0] - node1] = 0; /* no connection to its first neighbor */

		if (node1[i].free_degree < 0)
		{
			fprintf(stderr, "we have a problem, node %d free_degree %d!\n", i, node1[i].free_degree);
			exit(-1);
		}

		while (node1[i].free_degree)
		{      
			/******************************/
			/* fill the probability array */
			/******************************/
			l = 0;
			for (j=i+1; j<degree_g2; ++j)
			{
				if (node1[j].free_degree && flag[j])
				{
					for (k = 0; k < weighted_degree_array[ node1[i].degree ][ node1[j].degree ]; ++k, ++l)
						p_array[l] = j;
				}
			}

			if (l == 0)
				break;

			g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */
			j = p_array[g];

			/*if ( node[i].nid == 0 )
			fprintf(stderr, "phase III: added node %d!\n", node[j].nid);*/

			node1[i].nnp[node1[i].degree - node1[i].free_degree] = node1+j;
			node1[j].nnp[node1[j].degree - node1[j].free_degree] = node1+i;
			Node::nodes[i]->AddDuplexLink(Node::nodes[j], link);

			--(node1[i].free_degree);
			--(node1[j].free_degree);

			flag[j] = 0;
		}

		if (node1[i].free_degree)
		{
			fprintf(stderr, "connect_nodes: node %d has degree of %d with %d unfilled!\n", node1[i].nid, node1[i].degree, node1[i].free_degree);
		}
	}
}
void CInet3::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
							        const Location& BoundBoxRightUpper,
							        BoxType  type)
{
	const NodeVec_t& nodes = Node::GetNodes();

	//随机生成位置
	for (NodeVec_t::size_type nodeNum=0;nodeNum!=nodes.size();nodeNum++)
	{
	int x =  random(10000);
	int y =  random(10000);
	nodes[nodeNum]->SetLocation(x,y);
	}
}
