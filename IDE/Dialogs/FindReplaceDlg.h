#pragma once

// CFindReplaceDlg dialog
#include "..\resource.h"
#include "afxwin.h"
#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx

class CFindReplaceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFindReplaceDlg)

public:
	CFindReplaceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindReplaceDlg();

// Dialog Data
	enum { IDD = IDD_FINDREPLACE };
	EventVector* m_pEventList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton	m_Search;
	CButton	m_Bookmark;
	CButton	m_Replace;
	CButton	m_ReplaceAll;

	CButton		m_Actions;    // Already CButton (standard)
	CButton		m_Conditions; // Already CButton (standard)

	CEdit	m_Find;
	CEdit	m_ReplaceWith;
	CListCtrl	m_List;       // Already CListCtrl (standard)
	EventSheetEditor* pChrono;

	// Search stuff
	CString m_Text;
	void DoSearch(CEditorEvent* pEvent, int& Number);

	// Handlers
	afx_msg void OnBnClickedSearch();
	afx_msg BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	CString StripHTML(LPCSTR String);
	afx_msg void OnNMDblclkResults(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBookmark();
	afx_msg void OnReplace();
	afx_msg void OnReplaceAll();
};
