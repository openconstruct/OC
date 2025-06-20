#ifndef _ABOUTDLG_H_
#define _ABOUTDLG_H_

#include <afxdialogex.h> // Required for CDialogEx

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();


	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_People;
	CStatic	m_BuildDate;

	CButton     m_OK;
	CButton   m_Group;
	//}}AFX_DATA

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
