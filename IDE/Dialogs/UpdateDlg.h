#pragma once
#include "afxcmn.h"     // For CProgressCtrl, CListCtrl
#include "afxwin.h"     // For CButton
#include <afxdialogex.h> // Required for CDialogEx

#include "..\Utilities\CWebUpdate.h" // Custom class, assumed independent or separately refactored

#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party

// CUpdateDlg dialog

class CUpdateDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateDlg();

	CWebUpdate m_UpdateObj; // Custom class

	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

// Dialog Data
	enum { IDD = IDD_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl	m_Progress; // Already standard
	CListCtrl		m_Files;    // Already standard
	CButton		m_Download, m_Cancel; // Changed from CExtButton
	HRESULT			hr;

	CStringArray	m_URLs;
	CStringArray	m_Local;

	// Download in progress
	CString			m_RemoteURL;
	CString			m_LocalFile;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDownload();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
