//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:

//更改人：李玉
//更改时间：2010-3-3
#pragma once

#ifndef __CTOPOANALYSIS__H__ 
#define __CTOPOANALYSIS__H__ 
#include "PlatTopoBase.h"

class CTopoAnalysis
{
public:
	CTopoAnalysis(CPlatTopoBase* base);
public:
	~CTopoAnalysis(void);
	void AnalysisDegree();
	void ComBetweenness();
    int cal_btns();
	int print_btns();
	int GetOneStartPathNum();

private:
	int search_sps(size_t start);
	int zero_array();

private:
	size_t _size;

	typedef struct _des_node
	{
		size_t _index;
		int _step;
		std::vector<size_t> _pre_id;  //最短路径上，该节点的前一节点在pList数组的位置
		_des_node() : _step(0) { _pre_id.clear(); }
	}des_node;

	des_node *_p_list;

	int **_p_spc;        //存储最短路径的矩阵

	/*
	*	存储节点在_p_list中的位置
	*/
	int *_hit;
	CPlatTopoBase* platTopo;
	int shortestPathNum;
};
#endif