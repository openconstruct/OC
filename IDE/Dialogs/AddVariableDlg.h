#pragma once
#include "afxwin.h"
#include <afxdialogex.h> // Required for CDialogEx

// CAddVariableDlg dialog
class CAddVariableDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CAddVariableDlg)

public:
	CAddVariableDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddVariableDlg();
	BOOL OnInitDialog();

	CButton m_OK, m_Cancel; // Changed from CExtButton
	// CCtrlMessageBar	tips; // Prof-UIS: Functionality will be removed or replaced by MessageBox

// Dialog Data
	enum { IDD = IDD_ADDVARIABLE };

	CString Name, Value, Caption;
	int Type;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Name;
	CEdit m_Value;
	afx_msg void OnBnClickedOk();

	CComboBox m_Type;
};
