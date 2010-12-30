// DTopo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WhuSimulator.h"
#include "DTopo.h"


// CDTopo �Ի���

IMPLEMENT_DYNAMIC(CDTopo, CDialog)

CDTopo::CDTopo(CWnd* pParent /*=NULL*/)
	: CDialog(CDTopo::IDD, pParent)
	, m_topo_path(_T(""))
{

}

CDTopo::~CDTopo()
{
}

void CDTopo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_topo_path);
}


BEGIN_MESSAGE_MAP(CDTopo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CDTopo::OnBnLoadTopo)
	ON_BN_CLICKED(IDC_BUTTON1, &CDTopo::OnBnOpenLoadTopoPath)
	ON_BN_CLICKED(IDC_BUTTON3, &CDTopo::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDTopo ��Ϣ�������

void CDTopo::OnBnLoadTopo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if(pTApp->graph->LoadTopo(m_topo_path) == -1)
	{
		CString strTemp = _T("topoFile could not be opened:");
		strTemp.Format("%s", strTemp);
		MessageBox(strTemp);
		return;
	}
	MessageBox("�����ļ��ѵ��룡");
}

void CDTopo::OnBnOpenLoadTopoPath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog
		opendlg(TRUE,_T("TXT"),_T("*.TXT"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("�ı��ļ�(*.TXT)|*.TXT|All Files (*.*)|*.*||"));

	opendlg.m_ofn.lpstrInitialDir=_T("D:\\"); //���öԻ����Ĭ��Ŀ¼Ϊ

	if( opendlg.DoModal ()==IDOK )
	{
		m_topo_path = opendlg.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CDTopo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	pTApp = (CWhuSimulatorApp*)AfxGetApp();

	m_topo_path = "D:\\topo.txt";
	//m_topo_path = ".\\topo.txt";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//  [8/2/2010 Administrator]
void CDTopo::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//Ҫ��֤ͼ�Ѿ�����
	if(pTApp->graph->IsConnected())
		MessageBox("��ͨ��");
	else
		MessageBox("����ͨ��");
}
