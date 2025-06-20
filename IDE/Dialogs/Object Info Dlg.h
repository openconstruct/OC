#pragma once
#include <afxdialogex.h> // Required for CDialogEx
#include "afxcmn.h"     // For CListCtrl
#include "afxwin.h"     // For CEdit, CStatic, CComboBox, CButton

#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party

class CEmptyListCtrl : public CListCtrl
{
// Construction
public:
	CFont m_Font;

	CEmptyListCtrl()
	{
		m_Font.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
	}

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg dialog

class CObjectInfoDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CObjectInfoDlg) // Added DECLARE_DYNAMIC
// Construction
public:
	CObjectInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectInfoDlg)
	enum { IDD = IDD_OBJECTINFO };
	CButton		m_Close;    // Changed from CExtButton
	CComboBox	m_AceType;  // Changed from CExtComboBox
	CEmptyListCtrl	m_AceList; // Custom control, base is CListCtrl
	CEdit		m_Info;     // Changed from CExtEdit
	CStatic			m_Icon;     // Changed from CExtLabel
	CStatic			m_Name;     // Changed from CExtLabel

	// Object
	CString objname;

	// Resizable
	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnChangeACE();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};