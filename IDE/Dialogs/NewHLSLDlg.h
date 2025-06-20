#pragma once
#include <vector> // For std::vector
using std::vector; // Make vector usable without std::prefix
#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx
// #include "afxcmn.h" // For CListCtrl, CEdit - already included by stdafx.h or afxwin.h usually
// #include "afxwin.h" // For CButton - already included by stdafx.h

class CAddEffect
{
public:
	CString Name;
	CString Author;
	CString Description;
	double	Version;
};

// CNewHLSLDlg dialog
class CNewHLSLDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CNewHLSLDlg) // Added DECLARE_DYNAMIC
public:
	CNewHLSLDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewHLSLDlg();
	CListCtrl	m_List;    // Already standard
	CButton	m_Cancel;  // Changed from CExtButton
	CButton	m_OK;      // Changed from CExtButton
	CButton	m_Editor;  // Changed from CExtButton
	CEdit	m_Description; // Changed from CExtEdit

	vector<CAddEffect> m_Effects;

	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now
	CCustomTabCtrl m_Tabs; // Custom control - leave as is, may need separate refactoring if Prof-UIS derived

	float m_Version;
	
	CString m_Text, m_Filename;

	void OnChangeCategory();

// Dialog Data
	enum { IDD = IDD_NEWHLSL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
};
