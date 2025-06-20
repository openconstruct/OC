/////////////////////////////////////////////////////////////////////////////
// Error Dialog
//
// Last revision: 1.11
// 5th May 2008
//
// Changes:
// - Change error to message box
// - Change title for updates
//
/////////////////////////////////////////////////////////////////////////////

// File includes
#include "StdAfx.h"
#include "..\Construct.h"
#include "Error Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrorDlg dialog

CErrorDlg::CErrorDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CErrorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrorDlg)
	//}}AFX_DATA_INIT
}

void CErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_MESSAGE, m_Message);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CErrorDlg, CDialogEx)
	//{{AFX_MSG_MAP(CErrorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrorDlg message handlers

BOOL CErrorDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	m_Message.SetWindowText(errorMessage);

	// Title of the dialog
	CString title;
	title.Format("Construct : %s", E_TITLE);

	if (update) {
		title.Format("Construct : Update available");
		m_OK.SetWindowText("Download");
		m_Cancel.SetWindowText("Later");
	}

	else
		m_Cancel.ShowWindow(SW_HIDE);

	SetWindowText(title);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CErrorDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == 0x43 && GetKeyState(VK_CONTROL) < 0 ) 
	{
		if (OpenClipboard())
		{
			EmptyClipboard();
			HGLOBAL hClipboardData;
			hClipboardData = GlobalAlloc(GMEM_DDESHARE, errorMessage.GetLength() +1 );

			char* pchData;
			pchData = (char*)GlobalLock(hClipboardData);		  
			strcpy(pchData, LPCSTR(errorMessage));

			GlobalUnlock(hClipboardData);

			SetClipboardData(CF_TEXT, hClipboardData);		  
			CloseClipboard();
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

INT_PTR CErrorDlg::UpdateMessage(LPCSTR title, LPCSTR message)
{
	errorTitle = title;
	errorMessage = message;

	update = true;

	return DoModal();
}