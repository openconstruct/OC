#pragma once
#include "afxwin.h"
#include <afxdialogex.h> // Required for CDialogEx

class CInstallSettingsDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CInstallSettingsDlg)

public:
	CInstallSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstallSettingsDlg();
	BOOL OnInitDialog();

	enum { IDD = IDD_INSTALLER };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();

	// Controls
	CButton		m_OK;     // Changed from CExtButton
	CButton		m_Cancel; // Changed from CExtButton
	CEdit			m_License;
	CComboBox		m_OS;
	CButton			m_Fullscreen;
	CEdit			m_InstallPath;

	// Output
	CString		m_LicenseText;
	CString		m_Path;
	int			m_OSSetting;
	bool		m_bFullscreen;

	DECLARE_MESSAGE_MAP()
};
