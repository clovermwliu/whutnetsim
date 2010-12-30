// WhuSimulatorDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "DTopo.h"
#include "DRun.h"

// CWhuSimulatorDlg �Ի���
class CWhuSimulatorDlg : public CDialog
{
// ����
public:
	CWhuSimulatorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WHUSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	CDTopo   m_topo;
	CDRun    m_run;
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};
