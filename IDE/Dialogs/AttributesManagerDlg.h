#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <afxdialogex.h> // Required for CDialogEx

#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party

// CAddNewTraitDlg dialog

class CAddNewTraitDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CAddNewTraitDlg)

public:
	CAddNewTraitDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddNewTraitDlg();

	CButton m_Add, m_Remove, m_Edit, m_Close; // Changed from CExtButton

	BOOL OnInitDialog();

	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;

	bool TraitNameExists(CString Name);
	void ConfirmDeleteSelectedItems();
	void RefreshTraitList();
	bool TraitNameIsBuiltIn(CString Name);

	CApplication* application;

// Dialog Data
	enum { IDD = IDD_ATTRIBUTEMANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Name;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAdd();
	CListCtrl m_List;
public:
	afx_msg void OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnListKeyDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRename();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLvnBeginlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult);
};
