// Dialogs\EditGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\Construct.h"
#include "EditGridDlg.h"

// CEditGridDlg dialog
IMPLEMENT_DYNAMIC(CEditGridDlg, CDialogEx)

CEditGridDlg::CEditGridDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditGridDlg::IDD, pParent)
{

}

CEditGridDlg::~CEditGridDlg()
{
}

void CEditGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Base class call first
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_WIDTH, m_Width);
	DDX_Control(pDX, IDC_HEIGHT, m_Height);
	// Assuming IDC_SNAPTO is the resource ID for m_SnapTo checkbox
	// If m_SnapTo is not associated with a dialog control ID in the .rc file, this line might be an issue
	// or if IDC_SNAPTO is not a valid ID. For now, we assume it exists.
	DDX_Control(pDX, IDC_SNAPTOGRID, m_SnapTo); // Common ID for "Snap to Grid" is IDC_SNAPTOGRID or similar
                                            // If it's different, this ID needs to be correct.
                                            // Will use IDC_SNAPTOGRID as a placeholder.
}

BEGIN_MESSAGE_MAP(CEditGridDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditGridDlg::OnOK)
END_MESSAGE_MAP()

// CEditGridDlg message handlers
BOOL CEditGridDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	m_Width.SetWindowText(m_OldWidth);
	m_Height.SetWindowText(m_OldHeight);

	// Ensure m_SnapTo is valid before calling SetCheck, which requires the window handle.
	// This implies m_SnapTo should have been subclassed by DoDataExchange or similar before OnInitDialog if accessed this early.
	// However, standard DDX_Control in DoDataExchange (called by base OnInitDialog) should handle this.
	if (m_SnapTo.GetSafeHwnd()) // Check if the window is valid
	{
		if (m_OldSnapTo)
			m_SnapTo.SetCheck(BST_CHECKED);
		else
			m_SnapTo.SetCheck(BST_UNCHECKED); // Explicitly uncheck
	}


	// SubclassChildControls(); // Removed Prof-UIS specific call

	return TRUE;
}

void CEditGridDlg::OnOK()
{
	m_Width.GetWindowTextA(m_OldWidth);
	m_Height.GetWindowTextA(m_OldHeight);

	if (m_SnapTo.GetSafeHwnd()) // Check if the window is valid
	{
		if (m_SnapTo.GetCheck() == BST_CHECKED)
			m_OldSnapTo = true;
		else
			m_OldSnapTo = false;
	}
	else {
		// Default or error if control not found
		m_OldSnapTo = false;
	}
	
	CDialogEx::OnOK();
}
