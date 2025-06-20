#pragma once

#include "..\Utilities\Palette\ToolSettings.h"
#include "afxwin.h"
#include <afxdialogex.h> // Required for CDialogEx

// CManageActionPointsDlg dialog

class CManageActionPointsDlg : public CDialogEx // Changed base class
{


	DECLARE_DYNAMIC(CManageActionPointsDlg)

public:
	void UpdateList();
	CToolSettings* m_pToolSettings;
	CManageActionPointsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManageActionPointsDlg();

	CButton m_Add, m_Remove, m_OK, m_Cancel; // Changed from CExtButton

// Dialog Data
	enum { IDD = IDD_MANAGE_ACTION_POINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnRemove();
public:
	CListBox m_ActionPointsList;
public:
	CEdit m_ActionName_edit;
	BOOL OnInitDialog();
public:
};
