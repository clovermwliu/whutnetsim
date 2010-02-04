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

#ifndef __appfilescript_h__
#define __appfilescript_h__


#include <fstream> 
#include "G_common_defs.h"
#include "ErrorHandler.h"
#include "FileScript.h"


#define DEFAULT_VALUE                                    1
#define ERROR_APPSCRIPT_SUCCESS			              ERROR_NO_ERROR
#define ERROR_APPSCRIPT_NODES_EMPTY                    0x00060101
#define ERROR_APPSCRIPT_GET_NODES                     0x00060102
#define ERROR_APPSCRIPT_GET_NODES_SIZE                 0x00060103
#define ERROR_APPSCRIPT_GET_NODES_SECTION               0x00060104
#define ERROR_APPSCRIPT_GET_APP_TYPE               0x00060105
#define ERROR_APPSCRIPT_GET_APP_PARA              0x00060106
#define ERROR_APPSCRIPT_GET_APP_SET               0x00060107
#define ERROR_APPSCRIPT_GET_WORM_SET              0x00060108
#define ERROR_APPSCRIPT_GET_WORM_MODEL_PATH              0x00060109
#define ERROR_APPSCRIPT_GET_WORM_STARTID             0x0006010A
#define ERROR_APPSCRIPT_GET_WORM_NODE_BY_ID             0x0006010B
#define ERROR_APPSCRIPT_GET_TCPSERVER_NODE_BY_ID             0x0006010C
#define ERROR_APPSCRIPT_GET_TCPSEND_NODE_BY_ID             0x0006010D
#define ERROR_APPSCRIPT_GET_NODE_ID                     0x0006010E
#define ERROR_APPSCRIPT_GET_WORM_SCANRATE              0x0006010F
#define ERROR_APPSCRIPT_WORM_MODEL_PARA              0x00060110
#define ERROR_APPSCRIPT_WORM_WHEN_CONSTRUCT_MODEL              0x00060111
#define ERROR_APPSCRIPT_GET_TCPSERVER_SET              0x00060112
#define ERROR_APPSCRIPT_GET_TCPSEND_PARA              0x00060113
#define ERROR_APPSCRIPT_GET_TCPSEND_BINDPORT          0x00060114
#define ERROR_APPSCRIPT_GET_TCPSERVER_BINDPORT        0x00060115
#define ERROR_APPSCRIPT_GET_TCPSEND_TARGETID          0x00060116
#define ERROR_APPSCRIPT_GET_TCPSEND_COUNTNUM          0x00060117
#define ERROR_APPSCRIPT_GET_TCPSEND_SET               0x00060118
#define ERROR_APPSCRIPT_GET_TCPSEND_ISSTART           0x00060119
#define ERROR_APPSCRIPT_GET_TCPSEND_STARTTTIME        0x0006011A
#define ERROR_APPSCRIPT_GET_WORM_MODEL_TYPE           0x0006011B
#define ERROR_APPSCRIPT_GET_WORM_MODEL_DPREF_FILE     0x0006011C
#define ERROR_APPSCRIPT_OPEN_DPREF_FILE               0x0006011D
#define ERROR_APPSCRIPT_GET_WORM_STARTTYPE            0x0006011E



class CAppFileScript:public CFileScript
{
	friend class CItemLine;
	template <typename ValueType> friend class CGenericConfigItem;

public:
	CAppFileScript( const string& appfile );
	~CAppFileScript(void);

	virtual Error_str GetLastErrorEx();

	int ReadAppbyInifile();    //读应用脚本

	int WriteAppbyInifile();  //写应用脚本

private:
	//TCPSend
	int InitTCPSend(int node_id, string appSec, string paraSec, string settingSec);
	//TCPServer
	int InitTCPServer(int node_id, string appSec, string paraSec, string settingSec);
	//wormudp
	vector<double> * pVulDistribute;
	bool bInitVul;
	int InitWormUDP(int node_id, string appSec, string paraSec, string settingSec);



};
#endif

