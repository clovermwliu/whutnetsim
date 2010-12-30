// DTopo.cpp : 实现文件
//

#include "stdafx.h"
#include "WhuSimulator.h"
#include "DTopo.h"


// CDTopo 对话框

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


// CDTopo 消息处理程序

void CDTopo::OnBnLoadTopo()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(pTApp->graph->LoadTopo(m_topo_path) == -1)
	{
		CString strTemp = _T("topoFile could not be opened:");
		strTemp.Format("%s", strTemp);
		MessageBox(strTemp);
		return;
	}
	MessageBox("拓扑文件已导入！");
}

void CDTopo::OnBnOpenLoadTopoPath()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog
		opendlg(TRUE,_T("TXT"),_T("*.TXT"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("文本文件(*.TXT)|*.TXT|All Files (*.*)|*.*||"));

	opendlg.m_ofn.lpstrInitialDir=_T("D:\\"); //设置对话框的默认目录为

	if( opendlg.DoModal ()==IDOK )
	{
		m_topo_path = opendlg.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CDTopo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pTApp = (CWhuSimulatorApp*)AfxGetApp();

	m_topo_path = "D:\\topo.txt";
	//m_topo_path = ".\\topo.txt";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//  [8/2/2010 Administrator]
void CDTopo::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	//要保证图已经导入
	if(pTApp->graph->IsConnected())
		MessageBox("连通！");
	else
		MessageBox("不连通！");
}
