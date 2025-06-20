#pragma once


#include "..\..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx

// CEditEventGroup dialog

class CEditEventGroup : public CDialogEx
{
	DECLARE_DYNAMIC(CEditEventGroup)

public:
	CEditEventGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditEventGroup();

	virtual void OnOK();

	bool m_Active;
	CString m_Title;
	CString m_Description;

	// CCtrlMessageBar	tips; // Prof-UIS: Functionality will be removed
	CDlgAnchor dlgAnchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dlgMan;       // Potentially Prof-UIS or other 3rd party - left for now

	CButton m_OK, m_Cancel;
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_EGROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
