// WhuSimulatorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WhuSimulator.h"
#include "WhuSimulatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWhuSimulatorDlg �Ի���




CWhuSimulatorDlg::CWhuSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWhuSimulatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWhuSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB3, m_tab);
}

BEGIN_MESSAGE_MAP(CWhuSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB3, &CWhuSimulatorDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CWhuSimulatorDlg ��Ϣ�������

BOOL CWhuSimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_tab.InsertItem(0,_T("��������"));  //��Ӳ���һѡ�
	m_tab.InsertItem(1,_T("����"));  //��Ӳ�����ѡ�

	//�����Ի���,���ҽ�IDD_TEST_DIALOG�ؼ���Ϊ������
	m_topo.Create(IDD_DIALOG_TOPO, GetDlgItem(IDD_WHUSIMULATOR_DIALOG));
	m_run.Create(IDD_DIALOG_RUN ,GetDlgItem(IDD_WHUSIMULATOR_DIALOG));

	CRect rs;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rs);
	GetDlgItem(IDC_STATIC)->ClientToScreen(&rs);
	ScreenToClient(&rs);

	//�ֱ��������غ���ʾ	
	m_tab.ShowWindow(SW_SHOW);
	m_topo.ShowWindow(SW_SHOW);
	m_run.ShowWindow(SW_HIDE);
    
	////�����ӶԻ���ߴ粢�ƶ���ָ��λ��
	m_topo.MoveWindow(&rs);
	m_run.MoveWindow(&rs);
	//����Ĭ�ϵ�ѡ�///
	m_tab.SetCurSel(0); 

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CWhuSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWhuSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CWhuSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWhuSimulatorDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_tab.GetCurSel();
	switch(CurSel)
	{
	case 0://��������
		m_topo.ShowWindow(SW_SHOW);
		m_run.ShowWindow(SW_HIDE);
		break;
	case 1://���ؿ�������
		m_topo.ShowWindow(SW_HIDE);
		m_run.ShowWindow(SW_SHOW);
		break;
	default:
		break;
	} 
	*pResult = 0;
}
