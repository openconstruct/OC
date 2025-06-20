// Dialogs/EditEventGroup.cpp : implementation file
//

#include "stdafx.h"
#include "EditEventGroup.h"

// CEditEventGroup dialog
IMPLEMENT_DYNAMIC(CEditEventGroup, CDialogEx)

CEditEventGroup::CEditEventGroup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditEventGroup::IDD, pParent)
{
	m_Active = true;
}

void CEditEventGroup::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_OK);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditEventGroup, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CEditEventGroup message handlers
BOOL CEditEventGroup::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	// Resizing anchors
	// dlgMan and dlgAnchor are potentially Prof-UIS or other 3rd party. Left for now.
	BOOL First = dlgMan.Load(this->m_hWnd, "Software\\Construct\\EditEventGroup");
	// CCtrlMessageBar (tips) functionality removed
	/*
	if (!First)
	{
		tips.Attach(this);
		tips.SetHighlightOnMouseOver();
		tips.SetResize();
		tips.SetWrapText();
		tips.SetText("An event group is a group of events which can be activated or deactivated at runtime.  This serves the purpose of not only keeping event sheets clean, but allowing certain segments to be toggled on and off depending on what's going on.");
	}
	*/

	// Translations
	m_Cancel.SetWindowText(CANCEL);
	GetDlgItem(IDC_TITLELABEL)->SetWindowText(EG_GROUPTITLE);
	GetDlgItem(IDC_DESCRIPTION)->SetWindowText(EG_DESCRIPTION);
	((CButton*)GetDlgItem(IDC_DISABLED))->SetWindowText(EG_DISABLEBYDEFAULT);
	CString Title;
	Title.Format("Construct : %s", EG_TITLE);
	SetWindowText(Title);

	// Editing
	if (!m_Active) ((CButton*)GetDlgItem(IDC_DISABLED))->SetCheck(BST_CHECKED);
	GetDlgItem(IDC_DESCRIPTION)->SetWindowText(m_Description);
	GetDlgItem(IDC_TITLE)->SetWindowText(m_Title);

	// SubclassChildControls(); // Removed Prof-UIS specific call

	return TRUE;
}

CEditEventGroup::~CEditEventGroup()
{
}

void CEditEventGroup::OnDestroy() 
{
	// tips.Detach(); // CCtrlMessageBar (tips) functionality removed

	CDialogEx::OnDestroy();
	
	// dlgMan might be Prof-UIS specific.
	dlgMan.Save();	
}

void CEditEventGroup::OnOK()
{
	m_Active = !((CButton*)GetDlgItem(IDC_DISABLED))->GetCheck();
	GetDlgItem(IDC_DESCRIPTION)->GetWindowText(m_Description);
	GetDlgItem(IDC_TITLE)->GetWindowText(m_Title);

	if (m_Title == "")
	{
		// CCtrlMessageBar (tips) functionality removed
		/*
		tips.Detach();
		tips.Attach(this);
		tips.SetHighlightOnMouseOver();
		tips.SetResize();
		tips.SetWrapText();
		tips.SetText("Please enter a name for this event group.");
		*/
		MessageBox("Please enter a name for this event group.", "Missing Title", MB_OK | MB_ICONEXCLAMATION);


		return;
	}

	CDialogEx::OnOK();
}