// WhuSimulator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "Graph.h"


// CWhuSimulatorApp:
// �йش����ʵ�֣������ WhuSimulator.cpp
//
class CGraph;
class CRoute;
class CWhuSimulatorApp : public CWinApp
{
public:
	CWhuSimulatorApp();
	CGraph* graph;

	// trace [7/16/2010 mpiuser]
	bool bTrace;

	//void NodeBoundRoute(CRoute* route);

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWhuSimulatorApp theApp;