#include "AppFileScript.h"
#include "G_common_defs.h"
#include "node.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "application-tcpserver.h"
#include "application-tcpsend.h"

using namespace std;

CAppFileScript::CAppFileScript( const string& appfile ):CFileScript(appfile)
{


}


CAppFileScript::~CAppFileScript(void)
{


}

int CAppFileScript::ReadAppbyInifile()    //读取应用脚本文件
{
	list<string> l;
	
	CFileConfig scriptfile(fileName);
	scriptfile.LoadFile();

	int num = GetKeyNamesBySectionName("AppNodes", l);
	for ( int i=0; i<num; i++ )
	{
		char c[4];
		string s;
		string id;

		sprintf(c,"%d",i);
		s = c;
		CGenericConfigItem<string> _nodeapp(scriptfile,"AppNodes","node"+s);
		id = _nodeapp.MyValue();
		
		CGenericConfigItem<string> _nodenew(scriptfile,id,"type");
		string nodenew = _nodenew.MyValue();
		CGenericConfigItem<string> _nodenewpara(scriptfile,id,"para");
		string nodenewpara=_nodenewpara.MyValue();
		CGenericConfigItem<string> _nodenewsetting(scriptfile,id,"setting");
		string nodenewsetting=_nodenewsetting.MyValue();

		if ( nodenew == "TCPServer" )
		{
			TCPServer* server;
			if ( nodenewpara != "NULL" )
			{

			}
			else
			{
				server = (TCPServer*)Node::GetNode(atof(c))->AddApplication(TCPServer());
			}

			if ( nodenewsetting == "BindAndListen" )
			{
				char* portnum; 
				CGenericConfigItem<string> __para(scriptfile, id+nodenewsetting, "para1" );
				string _para = __para.MyValue();
				portnum = (char*)_para.c_str();

				server->BindAndListen(atoi(portnum)); // Bind to port 80

			}
		}
		else if ( nodenew == "TCPSend" )
		{
			if ( nodenewpara != "NULL" )
			{
				int paranum = GetKeyNamesBySectionName(nodenewpara, l);
				if ( paranum == 1 )
				{

				}
				else if ( paranum == 3 )
				{
					char* peerid;
					char* portnum; 
					char* countnum;

					CGenericConfigItem<string> __para1(scriptfile, nodenewpara, "para1" );
					string _para1 = __para1.MyValue();
					peerid = (char*)_para1.c_str();

					CGenericConfigItem<string> __para2(scriptfile, nodenewpara, "para2" );
					string _para2 = __para2.MyValue();
					portnum = (char*)_para2.c_str();

					CGenericConfigItem<string> __para3(scriptfile, nodenewpara, "para3" );
					string _para3 = __para3.MyValue();
					countnum = (char*)_para3.c_str();

					TCPSend*  client = (TCPSend*)Node::GetNode(atof(c))->AddApplication(
						TCPSend(Node::GetNode(atof(peerid))->GetIPAddr(), atof(portnum), Constant(atof(countnum))));

					if ( nodenewsetting == "Start" )
					{
						char* startnum; 
						CGenericConfigItem<string> __para(scriptfile, id+nodenewsetting, "para1" );
						string _para = __para.MyValue();
						startnum = (char*)_para.c_str();
						client->Start(atoi(startnum));
					}
				}
			}
			else
			{
				
			}

		}
		else
		{

		}
	}


	

	return GetLastError();
}

int CAppFileScript::WriteAppbyInifile()     //新取应用脚本文件
{


	return GetLastError();
}

Error_str CAppFileScript::GetLastErrorEx()
{
	switch ( GetLastError() )
	{
	case ERROR_APPSCRIPT_FILENOTEXSIT:
		return "INIFILE NOT EXIST!";

	}


}