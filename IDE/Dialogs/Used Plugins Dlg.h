#pragma once

#include "..\resource.h"
#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx

class UsedPluginDialog : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(UsedPluginDialog) // Added DECLARE_DYNAMIC
public:

	UsedPluginDialog(CApplication*);  

protected:

	enum { IDD = IDD_USEDPLUGINS };
	CButton information; // Changed from CExtButton
	CButton remove;      // Changed from CExtButton
	CButton close;       // Changed from CExtButton
	CListCtrl objects;   // Already standard

	CApplication* application;

	CDlgAnchor dialog_anchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan dialog_manager;   // Potentially Prof-UIS or other 3rd party - left for now

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnInformation();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};