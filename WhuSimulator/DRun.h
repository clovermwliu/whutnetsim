#pragma once


// CDRun �Ի���

class CDRun : public CDialog
{
	DECLARE_DYNAMIC(CDRun)

public:
	CDRun(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDRun();

	CWhuSimulatorApp* pRunApp;

// �Ի�������
	enum { IDD = IDD_DIALOG_RUN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRun();
public:
	virtual BOOL OnInitDialog();

	// trace [7/16/2010 mpiuser]
	//string str_ini_file;
public:
	CString str_ini_file;
public:
	afx_msg void OnBnOpenInitFile();
};
