#pragma once
#include "afxwin.h"
#include <afxdialogex.h> // Required for CDialogEx

// CNewProjectDlg dialog
class CNewProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewProjectDlg)

public:
	CNewProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProjectDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CComboBox m_Profile;
	CButton m_EditProfiles;
	CEdit m_Name;
	CButton m_OK;
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedEditprofiles();
};
