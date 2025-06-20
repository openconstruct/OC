#pragma once
#include <vector> // For std::vector
using std::vector; // Make vector usable without std:: prefix
#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx


/////////////////////////////////////////////////////////////////////////////
// CNewBehaviorDlg dialog

class CNewBehaviorDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CNewBehaviorDlg) // Added DECLARE_DYNAMIC
// Construction
public:
	CNewBehaviorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewBehaviorDlg)
	enum { IDD = IDD_NEWMOVEMENT };
	CListCtrl	m_List;    // Already standard
	CButton	m_Cancel;  // Changed from CExtButton
	CButton	m_OK;      // Changed from CExtButton
	CEdit	m_Desc;    // Changed from CExtEdit

	vector<int> behaviors;

	//}}AFX_DATA

	CImageList  m_Images;

	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBehaviorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	CString Text;
	int     ID;

// Implementation
protected:

	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Generated message map functions
	//{{AFX_MSG(CNewBehaviorDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelChange(NMHDR* pNM, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};