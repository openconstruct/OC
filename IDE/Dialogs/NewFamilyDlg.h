#pragma once
#include <afxdialogex.h> // Required for CDialogEx

// CNewFamilyDlg dialog

#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party

class CNewFamilyDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CNewFamilyDlg)

public:
	CNewFamilyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewFamilyDlg();

	CListCtrl m_List;     // Already standard
	CButton m_OK, m_Cancel, m_Manage; // Changed from CExtButton
	CImageList m_Images;  // Already standard
	CApplication* application;

	// Name and icon of chosen family
	CString m_Name;
	CxImage small_image;
	CxImage m_Image;

	CString m_Path;

	// Resizable
	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

// Dialog Data
	enum { IDD = IDD_NEWFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnOK();
	BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	void OnRefresh();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedManage();
public:
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};
