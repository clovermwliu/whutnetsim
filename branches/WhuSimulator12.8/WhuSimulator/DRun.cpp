// DRun.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WhuSimulator.h"
#include "DRun.h"
#include "Simulator.h"
#include "Route.h"


// trace [7/16/2010 mpiuser]
#include "Test.h"

// CDRun �Ի���

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


// CDRun ��Ϣ�������

void CDRun::OnBnClickedRun()
{
	//// TODO: �ڴ���ӿؼ�֪ͨ����������

	//CSimulator* s = new CSimulator();
	//s->InitEventList();
	//s->Run();
	//delete s;

	// trace [7/16/2010 mpiuser]
	//д�������ļ�
	UpdateData(TRUE);
	std::string strStl;
	strStl=str_ini_file.GetBuffer(0);
	TestMain(strStl);
	MessageBox("�������");
}
BOOL CDRun::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	pRunApp = (CWhuSimulatorApp*)AfxGetApp();
	//str_ini_file = "D:\\input.sci";
	str_ini_file = "D:\\test.sci";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDRun::OnBnOpenInitFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog
		opendlg(TRUE,_T("TXT"),_T("*.TXT"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("�ı��ļ�(*.TXT)|*.TXT|All Files (*.*)|*.*||"));

	opendlg.m_ofn.lpstrInitialDir=_T("D:\\"); //���öԻ����Ĭ��Ŀ¼Ϊ

	if( opendlg.DoModal ()==IDOK )
	{
		str_ini_file = opendlg.GetPathName();
		UpdateData(FALSE);
	}
}
