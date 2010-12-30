#pragma once
#include "afxwin.h"


// CDTopo 对话框

class CDTopo : public CDialog
{
	DECLARE_DYNAMIC(CDTopo)

public:
	CDTopo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDTopo();

// 对话框数据
	enum { IDD = IDD_DIALOG_TOPO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnLoadTopo();
public:
	afx_msg void OnBnOpenLoadTopoPath();
public:
	CWhuSimulatorApp* pTApp;//全局变量的APP指针
	CString m_topo_path;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButton3();
};
