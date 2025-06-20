/////////////////////////////////////////////////////////////////////////////
// Install Settings Dialog
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\Construct.h"
#include "Install Settings Dlg.h"

// CInstallSettingsDlg dialog
IMPLEMENT_DYNAMIC(CInstallSettingsDlg, CDialogEx) // Changed base class

CInstallSettingsDlg::CInstallSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInstallSettingsDlg::IDD, pParent) // Changed base class
{
	m_OSSetting = 0;
}

CInstallSettingsDlg::~CInstallSettingsDlg()
{
}

void CInstallSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_LICENSE, m_License);
	DDX_Control(pDX, IDC_OS, m_OS);
	DDX_Control(pDX, IDC_FULLSCREEN, m_Fullscreen);
	DDX_Control(pDX, IDC_PATH, m_InstallPath);
}

BEGIN_MESSAGE_MAP(CInstallSettingsDlg, CDialogEx) // Changed base class
	ON_BN_CLICKED(IDOK, &CInstallSettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CInstallSettingsDlg message handlers
BOOL CInstallSettingsDlg::OnInitDialog() 
{
	// Startup
	CDialogEx::OnInitDialog(); // Changed base class

	m_OS.SetCurSel(m_OSSetting);
	m_License.SetWindowText(m_LicenseText);
	m_InstallPath.SetWindowText(m_Path);
	if (m_bFullscreen) m_Fullscreen.SetCheck(BST_CHECKED);

	// SubclassChildControls(); // Removed Prof-UIS specific call

	return TRUE;
}
void CInstallSettingsDlg::OnBnClickedOk()
{
	m_License.GetWindowText(m_LicenseText);
	m_InstallPath.GetWindowText(m_Path);
	m_OSSetting = m_OS.GetCurSel();
	if (m_Fullscreen.GetCheck() == BST_CHECKED) m_bFullscreen = true;
	else m_bFullscreen = false;

	OnOK();
}
