// Dialogs\ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\Construct.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx) // Changed base class

CProgressDlg::CProgressDlg(CString status, CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDlg::IDD, pParent) // Changed base class
{
	m_StatusText = status;
	progress = 0;	
}

void CProgressDlg::Start(CString status)
{
	if (status != "")
		m_StatusText = status;

	Create(IDD_PROGRESSDLG); // This should be fine, CDialogEx::Create
	// SetProgress(0); // SetProgress calls m_Status.SetWindowText, ensure m_Status is valid (OnInitDialog usually handles DDX)
                   // If Create() doesn't run full DDX, this might be an issue here.
                   // Usually, one would call DoModal or Create then ShowWindow, and OnInitDialog would setup controls.
                   // For a modeless dialog started with Create, OnInitDialog IS called.
                   // This call to SetProgress(0) should be okay if OnInitDialog has run or will run before actual painting.
                   // To be safe, ensure control interaction happens after OnInitDialog.
                   // The original code calls SetProgress(0) after Create() which should be okay.
	SetProgress(0);
}

BOOL CProgressDlg::OnInitDialog()
{
	BOOL ret = CDialogEx::OnInitDialog(); // Changed base class

	CenterWindow();
	ShowWindow(SW_SHOW);

	CString status;
	status.Format("%s... %d%%", m_StatusText, progress);
	m_Status.SetWindowText(status);
	m_Status.Invalidate();
	m_Progress.SetRange(0, 100);
	m_Progress.SetPos(0);
	m_Progress.Invalidate();

	// SubclassChildControls(); // Removed Prof-UIS specific call

	return ret;
}

void CProgressDlg::SetProgress(int prog)
{
	CString status;
	status.Format("%s... %d%%", m_StatusText, prog);
	m_Status.SetWindowText(status);
	m_Status.Invalidate();
	m_Progress.SetPos(prog);
}

void CProgressDlg::Finish()
{
	DestroyWindow();
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class
	DDX_Control(pDX, IDC_STATUS, m_Status);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx) // Changed base class
END_MESSAGE_MAP()


// CProgressDlg message handlers
