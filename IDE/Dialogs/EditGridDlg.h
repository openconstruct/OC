#pragma once
#include <afxdialogex.h> // Required for CDialogEx

// CEditGridDlg dialog
class CEditGridDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditGridDlg)

public:
	CEditGridDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditGridDlg();

// Dialog Data
	enum { IDD = IDD_PICED_RESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_OK;
	CButton m_Cancel;
	CButton m_SnapTo; // Assuming this is a checkbox or button
	CEdit m_Height;
	CEdit m_Width;

	CString m_OldWidth, m_OldHeight;
	bool m_OldSnapTo;

	afx_msg void OnOK();
	afx_msg BOOL OnInitDialog();
};
