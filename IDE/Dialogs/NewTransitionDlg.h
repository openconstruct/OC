#pragma once
#include <afxdialogex.h> // Required for CDialogEx

// CNewTransitionDlg dialog

#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party

class CNewTransitionDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CNewTransitionDlg)

public:
	// Transition
	CTransition m_Transition;

	CNewTransitionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewTransitionDlg();
	virtual BOOL OnInitDialog();
	CListCtrl m_Transitions; // Already standard
	void OnOK();

	CButton m_OK, m_Cancel; // Changed from CExtButton

	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

// Dialog Data
	enum { IDD = IDD_NEWTRANSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
