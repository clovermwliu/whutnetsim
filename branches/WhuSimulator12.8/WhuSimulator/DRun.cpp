// DRun.cpp : 实现文件
//

#include "stdafx.h"
#include "WhuSimulator.h"
#include "DRun.h"
#include "Simulator.h"
#include "Route.h"


// trace [7/16/2010 mpiuser]
#include "Test.h"

// CDRun 对话框

IMPLEMENT_DYNAMIC(CDRun, CDialog)

CDRun::CDRun(CWnd* pParent /*=NULL*/)
	: CDialog(CDRun::IDD, pParent)
	, str_ini_file(_T(""))
{

}

CDRun::~CDRun()
{
}

void CDRun::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, str_ini_file);
}


BEGIN_MESSAGE_MAP(CDRun, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDRun::OnBnClickedRun)
	ON_BN_CLICKED(IDC_BUTTON2, &CDRun::OnBnOpenInitFile)
END_MESSAGE_MAP()


// CDRun 消息处理程序

void CDRun::OnBnClickedRun()
{
	//// TODO: 在此添加控件通知处理程序代码

	//CSimulator* s = new CSimulator();
	//s->InitEventList();
	//s->Run();
	//delete s;

	// trace [7/16/2010 mpiuser]
	//写到配置文件
	UpdateData(TRUE);
	std::string strStl;
	strStl=str_ini_file.GetBuffer(0);
	TestMain(strStl);
	MessageBox("运行完毕");
}
BOOL CDRun::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pRunApp = (CWhuSimulatorApp*)AfxGetApp();
	//str_ini_file = "D:\\input.sci";
	str_ini_file = "D:\\test.sci";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDRun::OnBnOpenInitFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog
		opendlg(TRUE,_T("TXT"),_T("*.TXT"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("文本文件(*.TXT)|*.TXT|All Files (*.*)|*.*||"));

	opendlg.m_ofn.lpstrInitialDir=_T("D:\\"); //设置对话框的默认目录为

	if( opendlg.DoModal ()==IDOK )
	{
		str_ini_file = opendlg.GetPathName();
		UpdateData(FALSE);
	}
}
