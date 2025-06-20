// Dialogs\AddVariableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\Construct.h"
#include "AddVariableDlg.h"

// CAddVariableDlg dialog

IMPLEMENT_DYNAMIC(CAddVariableDlg, CDialogEx) // Changed base class

CAddVariableDlg::CAddVariableDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddVariableDlg::IDD, pParent) // Changed base class
{
	Type = 0;
	Value = "0";
}

CAddVariableDlg::~CAddVariableDlg()
{
}

void CAddVariableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_VARIABLE, m_Value);
	DDX_Control(pDX, IDC_COMBO1, m_Type);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
}


BEGIN_MESSAGE_MAP(CAddVariableDlg, CDialogEx) // Changed base class
	ON_BN_CLICKED(IDOK, &CAddVariableDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddVariableDlg message handlers

void CAddVariableDlg::OnBnClickedOk()
{
	m_Name.GetWindowTextA(Name);

	if (Name == "")
	{
		// CCtrlMessageBar (tips) functionality replaced by MessageBox
		MessageBox("Please enter a name for this variable.", "Missing Name", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_Value.GetWindowTextA(Value);
	Type = m_Type.GetCurSel();

	// If a number type (0), convert the value to a float then back to text.  This sets the value to 0
	// if the user accidentally types text for a number variable.
	if (Type == 0) {
		Value.Format("%f", atof(Value));
		
		while(Value.Right(1) == "0")
			Value = Value.Left(Value.GetLength() - 1);

		if (Value.Right(1) == ".")
			Value = Value.Left(Value.GetLength() - 1);
	}

	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CAddVariableDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); // Changed base class

	if (Caption != "")
		SetWindowText(Caption);

	m_Type.SetCurSel(Type);
	m_Name.SetWindowText(Name);
	m_Value.SetWindowText(Value);

	// SubclassChildControls(); // Removed Prof-UIS specific call

	return TRUE;
}