#pragma once
#include "afxwin.h"


// CDTopo �Ի���

class CDTopo : public CDialog
{
	DECLARE_DYNAMIC(CDTopo)

public:
	CDTopo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDTopo();

// �Ի�������
	enum { IDD = IDD_DIALOG_TOPO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnLoadTopo();
public:
	afx_msg void OnBnOpenLoadTopoPath();
public:
	CWhuSimulatorApp* pTApp;//ȫ�ֱ�����APPָ��
	CString m_topo_path;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButton3();
};
