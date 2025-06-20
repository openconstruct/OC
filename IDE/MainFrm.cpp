/*
	Construct - DirectX 9 Game and Application creation
    Copyright (C) 2007 Scirra

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
	Prof-UIS professional user interface library

	Used with kind permission of FOSS Software, Inc.

	www.prof-uis.com

	The Prof-UIS library in this source code is only to be used with this project.
	You MAY NOT use it for any other purpose.  If you wish to do so, please purchase
	an appropriate license from the above address.
*/

// File includes
#include "StdAfx.h"
#include "Construct.h"
#include "Structure\Export\Export.h"
#include "Dialogs\ExchangeDlg.h"
#include "MainFrm.h"
#include "ChildFrame.h"
#include "Utilities\CGFilTyp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// IMPLEMENT_SERIAL(CColorizedThemeOffice2003, CExtPaintManagerOffice2003, VERSIONABLE_SCHEMA|1 ); // Prof-UIS theme class removed

// About dialog
#include "Dialogs\AboutDlg.h"

// message_bar
#include "Dialogs\Error Dlg.h"

// First run
#include "Dialogs\FirstStartDlg.h"

// Language
CString g_Language;
CMainFrame* g_MainFrame;
CLayoutEditor *g_pLayoutView;

#include "UI Elements\Settings\SettingsDlg.h"
#include "UI Elements\Settings\AutoSaveOptions.h"

// Python Scripting
#include "Dialogs\ScriptDlg.h"

CIni* pINI;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx) // Changed base class

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx) // Changed base class
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW_TEMPLATE, OnFileTemplate)
	ON_COMMAND(ID_VIEW_LAYERBAR, OnLayerBar)
	ON_COMMAND(ID_VIEW_OBJECTBAR, OnObjectBar)
	ON_COMMAND(ID_VIEW_SIDEBAR, OnProjectBar)
	ON_COMMAND(ID_VIEW_PROPERTIESBAR, OnPropertiesBar)
	ON_COMMAND(ID_VIEW_ANIMBAR, OnAnimationBar)
	ON_COMMAND(ID_PREFERENCES, OnPreferences)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_VIEWVRAM, OnViewVRAM)
	ON_COMMAND(ID_FILE_NEWAPP, OnFileNewApplication)
	ON_COMMAND(ID_HELP, OnContextHelp)
	ON_COMMAND(ID_APPEARANCE_XP, OnXP)
	ON_COMMAND(ID_APPEARANCE_2003, On2003)
	ON_COMMAND(ID_APPEARANCE_2005, On2005)
	ON_COMMAND(ID_ISCLOSING, OnClosing)
	ON_COMMAND(ID_EXCHANGE, OnExchange)
	ON_COMMAND(ID_WEB_SCIRRACOM, OnWebScirra)
	ON_COMMAND(ID_WEB_FORUMS, OnWebForums)
	ON_COMMAND(ID_WEB_REPORTBUG, OnWebReportBug)
	ON_COMMAND(ID_WEB_HELP, OnWebHelp)
	ON_COMMAND(ID_WEB_UPDATE, OnWebUpdate)
	// ON_REGISTERED_MESSAGE( CExtControlBar::g_nMsgConstructPopupMenu, OnConstructPopupMenuCB ) // Prof-UIS specific message
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CDragObjects *g_dragInfo;

// The allocator
edittime_allocator g_allocator;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

PropertiesBar* g_PropertyBar;

CString gInsert;
int gInsertAuto = 0;
CLayerBar* g_pLayerBar;

WIN32_FIND_DATA wfd;
HANDLE hFind;

LRESULT CMainFrame::OnConstructPopupMenuCB(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void CMainFrame::ObjectTypesChanged()
{
	InitialUpdateAllEditors();
}

void CMainFrame::OnClosing()
{
	OnClose();
}

void CMainFrame::OnExchange()
{
	CExchangeDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnWebScirra()
{
	ShellExecute(NULL, "open", "http://www.scirra.com/", NULL, NULL, 0);
}

void CMainFrame::OnWebForums()
{
	ShellExecute(NULL, "open", "http://scirra.com/phpBB2/", NULL, NULL, 0);
}

void CMainFrame::OnWebReportBug()
{
	ShellExecute(NULL, "open", "http://scirra.com/bugs.php", NULL, NULL, 0);
}

void CMainFrame::OnWebHelp()
{
	ShellExecute(NULL, "open", "http://scirra.com/learn.php", NULL, NULL, 0);
}

void CMainFrame::OnWebUpdate()
{
	DoUpdateCheck(false);
}

void CMainFrame::InitialUpdateAllEditors()
{
	CWnd* clientWindow = GetActiveFrame();//GetWindow(GW_CHILD);
	while (clientWindow != NULL)
	{
		CChildFrame* frame = dynamic_cast<CChildFrame*>(clientWindow);
		if(frame)
			frame->InitialUpdateAllEditors();
		clientWindow = clientWindow->GetWindow(GW_HWNDNEXT);
	}
}

bool FileExists(LPCSTR Name)
{
	return GetFileAttributes(Name) != 0xFFFFFFFF;
}

//////////////////////////////////////////////////////
// PROF-UIS CODE
//////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	BOOL toSend = true;

	bIsMinimized = false;

	CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );

	// Prof-UIS Paint Manager and PopupMenuWnd settings removed.
	// MFC uses CMFCVisualManager for themes.
	// Example: CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));

	// Window placement persistence
	::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
	m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
	m_dataFrameWP.showCmd = SW_HIDE;

	g_PropertyBar = &m_PropertiesBar;
	g_pLayerBar = &m_LayerBar;
	g_dragInfo = new CDragObjects();

	newAppNumber = 0;
	m_pPApp = NULL;
	has_been_closed = false;
}

CMainFrame::~CMainFrame()
{
	CApplication *app;
	POSITION pos = m_apps.GetHeadPosition();
	delete g_dragInfo;

	while (pos != NULL) 
	{
		// iterate through objects to choose one to select
		app = m_apps.GetNext(pos);
		delete app;
	}
}

// void CMainFrame::RecalcLayout(BOOL bNotify) // Base CFrameWnd handles this.
// {
// 	CMDIFrameWndEx :: RecalcLayout( bNotify );
// }

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) ) // Changed base class
		return FALSE;

	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE/*|WS_EX_LAYOUT_RTL*/); // WS_EX_LAYOUT_RTL removal depends on RTL strategy
	cs.lpszClass =
		::AfxRegisterWndClass(
			CS_BYTEALIGNCLIENT,
			::LoadCursor( NULL, IDC_ARROW ),
			NULL,
			NULL
			);
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	return TRUE;
}

#define ID_RIBBON_MINIMIZE_RESTORE      5000

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if( nID == ID_RIBBON_MINIMIZE_RESTORE )
	{
		if( nCode == CN_UPDATE_COMMAND_UI )
		{
			((CCmdUI *)pExtra)->Enable();
			((CCmdUI *)pExtra)->SetCheck( m_Ribbon.RibbonPage_ExpandedModeGet() );
		}
		else if( pExtra == NULL )
		{
			m_Ribbon.RibbonPage_ExpandedModeSet( ! m_Ribbon.RibbonPage_ExpandedModeGet(), true );
		}
		return TRUE;
	}
	// Prof-UIS specific RTL handling and CExtNcFrameImpl calls removed.
	// if( nID == ID_RTL )
	// {
	// ...
	// }

	CMDIFrameWndEx::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ); // Changed base class
	return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// if( m_Ribbon.TranslateMainFrameMessage(pMsg) ) // CMFCRibbonBar handles this internally or differently
	// 	return TRUE;
	return CMDIFrameWndEx::PreTranslateMessage(pMsg); // Changed base class
}

BOOL CMainFrame::DestroyWindow() 
{
	CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );

	// Prof-UIS specific state saving removed.
	// m_Ribbon.CustomizeStateSave(pApp->m_pszRegistryKey,	pApp->m_pszProfileName,	pApp->m_pszProfileName);
	// CExtControlBar::ProfileBarStateSave(this, pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName);
	// g_PaintManager.PaintManagerStateSave(pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName);
	// g_CmdManager->ProfileWndRemove(GetSafeHwnd());
	// MFC equivalent for docking state: SaveMDIState(pApp->m_pszProfileName);
	
	return CMDIFrameWndEx::DestroyWindow(); // Changed base class
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	// Prof-UIS specific NcFrameImpl calls and logic removed.
	// bool bDwmMode = NcFrameImpl_IsDwmCaptionReplacement();
	// window placement persistence
	if( m_dataFrameWP.showCmd != SW_HIDE )
	{
		// if( bDwmMode )
		// 	NcFrameImpl_NcLock( true );
		SetWindowPlacement( &m_dataFrameWP );
		CMDIFrameWndEx::ActivateFrame( m_dataFrameWP.showCmd ); // Changed base class
		m_dataFrameWP.showCmd = SW_HIDE;
		// if( bDwmMode )
		// {
		// 	NcFrameImpl_NcLock( false );
		// 	NcFrameImpl_RecalcNcFrame();
		// }
		return;
	}
	CMDIFrameWndEx::ActivateFrame( nCmdShow ); // Changed base class
	// if( bDwmMode )
	// 	NcFrameImpl_RecalcNcFrame();
}

// void CMainFrame::_AdjustRTL() // Prof-UIS specific, commented out from header
// {
// 	ASSERT_VALID( this );
// 	if( (GetExStyle()&WS_EX_LAYOUT_RTL) != 0 )
// 	{
// 		m_Ribbon.ModifyStyleEx( 0, WS_EX_LAYOUTRTL, 0 );
//
// 		g_ResourceManager->SetLangLayout( LAYOUT_RTL );
// 	}
// 	else
// 	{
// 		m_Ribbon.ModifyStyleEx( WS_EX_LAYOUTRTL, 0, 0 );
//
// 		g_ResourceManager->SetLangLayout( LAYOUT_LTR );
// 	}
// 	m_Ribbon.Ribbon_UpdateLayout( true );
// 	CExtNcFrameImpl::NcFrameImpl_RecalcNcFrame();
// 	CExtNcFrameImpl::NcFrameImpl_SetupRgn();
// 	if( IsWindowVisible() )
// 		RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN );
// }


//////////////////////////////////////////////////////
// END PROF-UIS CODE
//////////////////////////////////////////////////////

void CMainFrame::OnContextHelp()
{
	CPath path;
	path.SetToCurrentDirectory();
	CString Help = path.GetFullPath();

	Help += "Construct.chm";

	ShellExecute(NULL, "open", Help, NULL, path.GetFullPath(), SW_SHOWMAXIMIZED);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Did the MDI Client create successfully?
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1) // Changed base class
		return -1;	

	CPath path;
	CWinApp * pApp = ::AfxGetApp();
	// g_CmdManager->ProfileSetup(pApp->m_pszProfileName, GetSafeHwnd()); // Prof-UIS Command Manager

	LoadLanguage("English (UK)");

	gInsert = "";

	path.SetToCurrentDirectory();	

	CGCFileTypeAccess FileType;
	CString csTempText;

	FileType.SetExtension("cap");

	csTempText = path.GetFullPath();
	csTempText += "Construct.exe";
	csTempText += " \"%1\"";
	FileType.SetShellOpenCommand(csTempText);
	FileType.SetDocumentShellOpenCommand(csTempText);

	FileType.SetDocumentClassName("Construct.Application");

	csTempText = path.GetFullPath();
	csTempText += "capico.ico";
	csTempText += ",0";
	FileType.SetDocumentDefaultIcon(csTempText);

	// set the necessary registry entries	
	FileType.RegSetAllInfo();

	// INI
	TCHAR szPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath);
	CString IP;
	IP.Format("%s\\Scirra\\", szPath);
	CreateDirectory(IP, NULL);
	IP += "Construct.ini";
	m_INIPath=IP;
	m_INI.SetPathName(m_INIPath);

	// First run wizard?
	if (m_INI.GetInt("General", "Run", 0) == 0)
	{
		CString language = "English (UK)";

		WritePrivateProfileString("General", "Run", "1", m_INIPath);
		WritePrivateProfileString("General", "Language", language, m_INIPath);
		WritePrivateProfileString("General", "ShowMaximized", "Y", m_INIPath);
	}

	if (!m_INI.IsSectionExist("RecentObjects")) {
		m_INI.WriteInt("RecentObjects", "MouseKeyb.csx", 2);
		m_INI.WriteInt("RecentObjects", "Sprite.csx", 2);
		m_INI.WriteInt("RecentObjects", "TileBackground.csx", 2);
		m_INI.WriteInt("RecentObjects", "Box.csx", 2);
		m_INI.WriteInt("RecentObjects", "DirectSound.csx", 2);
		m_INI.WriteInt("RecentObjects", "FrameView.csx", 2);
		m_INI.WriteInt("RecentObjects", "Image.csx", 2);
		m_INI.WriteInt("RecentObjects", "Text.csx", 2);
	}
	
	if (!m_INI.IsSectionExist("Recent")) {
		m_INI.WriteString("Recent", "1", "None");
		m_INI.WriteString("Recent", "2", "None");
		m_INI.WriteString("Recent", "3", "None");
		m_INI.WriteString("Recent", "4", "None");
		m_INI.WriteString("Recent", "5", "None");
		m_INI.WriteString("Recent", "6", "None");
		m_INI.WriteString("Recent", "7", "None");
		m_INI.WriteString("Recent", "8", "None");
		m_INI.WriteString("Recent", "9", "None");
		m_INI.WriteString("Recent", "10", "None");
	}

	if (!m_INI.IsSectionExist("RecentObjectsApp")) {
		m_INI.WriteInt("RecentObjectsApp", "Button.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "ComboBox.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "Edit.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "ListBox.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "Array.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "FrameView.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "Image.csx", 2);
		m_INI.WriteInt("RecentObjectsApp", "File.csx", 2);
	}

	if (!m_INI.IsSectionExist("ImageEditor")) 
	{
		m_INI.WriteInt("ImageEditor", "Tool", ID_BRUSH);

		m_INI.WriteDouble("ImageEditor", "Hardness", 1);
		m_INI.WriteDouble("ImageEditor", "Angle", 0);
		m_INI.WriteDouble("ImageEditor", "Thickness", 100);
		m_INI.WriteDouble("ImageEditor", "Size",1);
		m_INI.WriteDouble("ImageEditor", "Step",1);
		m_INI.WriteDouble("ImageEditor", "LineThickness",1);
		m_INI.WriteBool("ImageEditor", "Smooth", 0);
		m_INI.WriteDouble("ImageEditor", "Opacity", 255);

		m_INI.WriteInt("ImageEditor", "Colour1", RGB(255,0,0));
		m_INI.WriteInt("ImageEditor", "Colour2", RGB(0,0,255));
		m_INI.WriteInt("ImageEditor", "Colour1Alpha", 255);
		m_INI.WriteInt("ImageEditor", "Colour2Alpha", 255);
	}

	g_Language = m_INI.GetString("General", "Language", "English (UK)");
	g_Language += ".txt";

	// Ribbon Bar creation using CMFCRibbonBar
	if (!m_Ribbon.Create(this))
	{
		TRACE0("Failed to create ribbon bar\n");
		return -1;
	}
	// m_Ribbon.Init(); // Prof-UIS specific ribbon initialization
	// _InitRibbonBar(); // This was for CExtRibbonNode, commented out
	// m_Ribbon.Ribbon_PageSelectionSet(0, true); // Prof-UIS specific selection
	// MFC Ribbon population would happen here (AddCategory, AddPanel, AddElement, etc.)
	// For now, it will be an empty ribbon.

	// CProfStudioPropertyGridCtrl& m_PGC = m_PropertiesBar.m_Grid; // m_PropertiesBar is a custom class
	// m_PropertiesBar.m_pStore = &(m_PGC.m_PS); // Assuming m_PropertiesBar handles its grid internally
	m_PropertiesBar.m_pGrid = &m_PGC;

	// MDI tabs
	tabs.Create(this, CRect(0,0,0,0), UINT(IDC_STATIC), WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, __ETWS_MDI_DEFAULT | __ETWS_HIDE_ICONS | __ETWS_ITEM_DRAGGING);
	tabs.ModifyTabWndStyle( 0, __ETWS_SHOW_BTN_TAB_LIST|__ETWS_ENABLED_BTN_TAB_LIST );

	// Bars - Assuming these custom classes (ProjectBar, PropertiesBar, CLayerBar, AnimatorBar)
	// will have their base classes changed to CDockablePane or similar MFC compatible docking class.
	// Their Create() methods might need adjustment.
	// For animator_parent (now CDockablePane), its Create method needs to be standard.
	// The animator (CWnd) would then be created as a child of animator_parent's wrapper window.

	// Example for a CDockablePane derived class:
	// if (!project_bar.Create(BAR_PROJECT, this, CRect(0,0,200,200), TRUE, 123, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	// TRACE0("Failed to create project_bar\n");
	// project_bar.EnableDocking(CBRS_ALIGN_ANY);
	// DockPane(&project_bar);

	// Simplified creation for now, actual docking and sizing will be very different from Prof-UIS.
	project_bar.Create(BAR_PROJECT, this, 123);
	m_PropertiesBar.Create(BAR_PROPERTIES, this, 124);
	m_LayerBar.Create(BAR_LAYERS, this, 125);

	// For animator_parent (now CDockablePane)
	if (!animator_parent.Create(BAR_ANIMATOR, this, CRect(0,0,200,200), TRUE, 127, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		TRACE0("Failed to create animator_parent dockable pane\n");
	else {
		animator.Create(IDD_ANIMATORBAR, &animator_parent); // animator is a CWnd, parented to the CDockablePane
		animator.ShowWindow(SW_SHOW);
		animator_parent.SetChildView(&animator); // Or similar method to embed animator in the pane
	}

	// UpdateTheme(); // Prof-UIS theme switching removed for now. Standard MFC themes apply.

	///////////////////
	// SETUP DOCKING //
	///////////////////
	EnableDocking(CBRS_ALIGN_ANY); // Enable docking for the main frame
	EnableAutoHidePanes(CBRS_ALIGN_ANY); // Enable auto-hide for docking panes

	// Assuming ProjectBar, PropertiesBar, CLayerBar are now CDockablePane derived
	project_bar.EnableDocking(CBRS_ALIGN_ANY);
	m_PropertiesBar.EnableDocking(CBRS_ALIGN_ANY);
	m_LayerBar.EnableDocking(CBRS_ALIGN_ANY);
	animator_parent.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&project_bar);
	DockPane(&m_PropertiesBar, AFX_IDW_DOCKBAR_LEFT);
	DockPane(&m_LayerBar);
	m_LayerBar.DockToWindow(&project_bar, CBRS_ALIGN_BOTTOM); // Example of tabbed docking
	DockPane(&animator_parent);


	g_MainFrame = this;

	// CExtControlBar::FrameEnableDocking(this);       // Replaced by EnableDocking
	// CExtControlBar::FrameInjectAutoHideAreas(this);	// Replaced by EnableAutoHidePanes

	// Status Bar - using CMFCStatusBar
	if (!m_wndStatusBar.CreateEx(this, SBT_TOOLTIPS)) // SBT_TOOLTIPS for tooltip support on panes
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators,  sizeof(indicators)/sizeof(UINT));
	// CMFCStatusBar uses SetPaneInfo to set width, style, ID
	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, 70); // Example for first pane
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_CAPS, SBPS_STRETCH, 0); // Example stretch pane
	// Add more panes as needed using SetPaneInfo or AddElement/AddExtendedElement for complex panes

	// Load settings - MFC's LoadMDIState would be used for docking layout, ribbon state.
	// m_Ribbon.CustomizeStateLoad(pApp->m_pszRegistryKey,	pApp->m_pszProfileName,	pApp->m_pszProfileName); // Prof-UIS
	// CExtControlBar::ProfileBarStateLoad(...) // Prof-UIS
	// LoadMDIState(pApp->m_pszProfileName); // MFC equivalent for docking/toolbar states

	// Prof-UIS specific docking markers and grid settings removed.
	// CExtControlBar::g_eResizablePanelDockingType = CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2005;
	// CExtPropertyGridComboBoxBar* pWnd = STATIC_DOWNCAST(CExtPropertyGridComboBoxBar, m_PGC.GetChildByRTC(RUNTIME_CLASS(CExtPropertyGridComboBoxBar)));
	// pWnd->ShowWindow(SW_HIDE);
	// CExtPropertyGridTipBar* pTipBar = STATIC_DOWNCAST(CExtPropertyGridTipBar, m_PGC.GetChildByRTC(RUNTIME_CLASS(CExtPropertyGridTipBar)));
	// if(pTipBar != NULL)
	// 	pTipBar->HeightMinSet(55);
	// m_PGC.Invalidate();
	// m_PropertiesBar.RedrawWindow();


	SetTimer(500, 500, NULL);
	// ActivateFrame(0); // ActivateFrame is called by the framework

	// Auto save
	if (m_INI.GetInt("AutoSave", "On", 0) == 1)
	{
		m_AutoSaveTime = m_INI.GetInt("AutoSave", "Time", 0);
		m_bAutoSave = true;

		if (m_INI.GetInt("AutoSave", "On", 0) == 1)
			m_bAutoBackup = true;
		else
			m_bAutoBackup = false;

		KillTimer(999);
		SetTimer(999, (m_AutoSaveTime * 60) * 1000, NULL);
	}

	else 
	{
		m_bAutoSave = false;
		m_bAutoBackup = false;
		m_AutoSaveTime = 0;
		KillTimer(999);
	}

	// Disable menu fading
	CExtPopupMenuWnd::g_nDefaultFadeOutAnimationStepCount = 0;
	CExtPopupMenuWnd::g_nDefaultFadeOutAnimationEllapse = 0;


	/*
	CXAudio2SetupParameters setup;

	setup.cacheMode = CXAudio2::CACHE_OFF;

	// initialise audio
	audio.Init(setup);
	*/


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

extern CString g_LanguageID;

void CMainFrame::OnPreferences() 
{
	pINI = &m_INI;

	CSettingsDialog Dlg;

	CAutoSave* pAutoSave = (CAutoSave*)Dlg.AddPage(RUNTIME_CLASS(CAutoSave),
                _T("Auto save"), IDD_PROPERTY_PAGE_2, 
                _T("Auto save settings"));

	pAutoSave->m_AutoSaveTime = m_INI.GetInt("AutoSave", "Time", 0);
	
	Dlg.SetTitle("Construct Preferences");
	Dlg.SetLogoText("Construct");    

	int oldTheme = m_INI.GetInt("General", "Theme", 4);

	if (Dlg.DoModal() == IDOK)
	{
		if (m_INI.GetInt("General", "Theme", 4) != oldTheme)
			UpdateTheme();

		// Auto save preferences
		if (m_INI.GetInt("AutoSave", "On", 0) == 1) 
		{
			m_bAutoSave = true;

			if (m_INI.GetInt("AutoBackup", "On", 0) == 1) 
				m_bAutoBackup = true;
			else
				m_bAutoBackup = false;

			SetTimer(999, (m_AutoSaveTime * 60) * 1000, NULL);
		}

		else
		{
			m_bAutoSave = false;
			m_bAutoBackup = false;

			KillTimer(999);
		}
	}
}

// Show about dialog
void CMainFrame::OnAbout() 
{
	CAboutDlg about;
	about.DoModal();
}

// SHOW/HIDE LAYER BAR
void CMainFrame::OnLayerBar() 
{
	BOOL bShow = m_LayerBar.IsVisible();
	ShowControlBar(&m_LayerBar, !bShow, FALSE);
}

// SHOW/HIDE PROJECT BAR
void CMainFrame::OnProjectBar() 
{
	BOOL bShow = project_bar.IsVisible();
	ShowControlBar(&project_bar, !bShow, FALSE);
}

// SHOW/HIDE PROPERTIES BAR
void CMainFrame::OnPropertiesBar() 
{
	BOOL bShow = m_PropertiesBar.IsVisible();
	ShowControlBar(&m_PropertiesBar, !bShow, FALSE);
}

void CMainFrame::OnObjectBar()
{
	CChildFrame* frame = dynamic_cast<CChildFrame*>(GetActiveFrame());
	if(frame)
	{
		BOOL bShow = frame->object_bar_parent.IsVisible();
		ShowControlBar(&frame->object_bar_parent, !bShow, FALSE);
	}
}

// SHOW/HIDE ANIMATION BAR
void CMainFrame::OnAnimationBar() 
{
	BOOL bShow = animator_parent.IsVisible();
	ShowControlBar(&animator_parent, !bShow, FALSE);
}

void CMainFrame::OnFileTemplate() 
{
	TemplateDialog dlg;
	dlg.DoModal();
}

void CMainFrame::OnViewVRAM()
{
	if (m_pPApp == NULL)
		return;

	const float megabyte = 1024 * 1024;
	float targetMb = (float)CD3DDisplay::vramtargetbytes / megabyte;
	float texMb = (float)CD3DDisplay::vramtexturebytes / megabyte;

	CString msg;
	msg.Format("VRAM usage by Construct:\n\n%.2fmb textures\n%.2fmb targets\n%.2fmb total\n\nApprox %d MB texture memory remaining", 
		texMb, targetMb, targetMb + texMb, m_pPApp->m_display->GetD3DDevice()->GetAvailableTextureMem() / (1024*1024));
	::MessageBox(NULL, msg, "VRAM usage", MB_OK | MB_ICONINFORMATION);
}

#include "Dialogs\UpdateDlg.h"
#include "Utilities\WebGrab.h"

void UpdateCheck(void* mainframe_)
{
	CMainFrame* mainframe = (CMainFrame*)mainframe_;

	mainframe->DoUpdateCheck(true);
}

void CMainFrame::DoUpdateCheck(bool silent)
{
	CWebGrab WG;
	WG.SetTimeOut(2000);

	CString Ver;
	WG.GetFile("http://www.scirra.com/update.php", Ver);
	
	CString VerNumber;
	CString VerMessage;
	AfxExtractSubString(VerNumber, Ver, 0, '|');
	AfxExtractSubString(VerMessage, Ver, 1, '|');

	if (atoi(VerNumber) > CONSTRUCT_VERSION_ID)
	{
		CErrorDlg Dlg;
		CString msg = "There is a new version of Construct available. Visit http://www.scirra.com to download the latest update.\r\n\r\n";
		msg += VerMessage;

		if (Dlg.UpdateMessage("New version available", msg) == IDOK)
			ShellExecute(NULL, "open", "http://www.scirra.com/download.php", NULL, NULL, 0);

		return;
	}
	else if (!silent) {
		::MessageBox(NULL, "Your version of Construct (" CONSTRUCT_VERSION_STRING ") appears to be up to date.", "Check for updates", MB_OK | MB_ICONINFORMATION);
	}

	if (silent) {
		CUpdateDlg Dlg;
		Dlg.DoModal();
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{	
	if (nIDEvent == 999)
	{
		// Auto save
		// Save all open applications
		CApplication *app;
		POSITION pos = m_apps.GetHeadPosition();

		bool bClose = true;
		while (pos != NULL) 
		{
			// iterate through objects to choose one to select
			app = m_apps.GetNext(pos);
			
			// Check save
			if (app->GetModifiedStatus())
			{
				if (app->file_information.file_path == "") continue;

				OnFileSave(app, true);	// Autosave
			}
		}
	}
	
	if (nIDEvent == 500)
	{
		//if(m_Maximize)
			ShowWindow(SW_SHOWMAXIMIZED);
		KillTimer(500);

		m_PropertiesBar.RedrawWindow();

	#ifndef DEBUGBUILD

		if (m_INI.GetInt("General", "UpdateCheck", -1) == -1) {

			int ret = MessageBox("Do you want Construct to automatically check for updates on startup?  This does not send any information"
				" about you or your computer to Scirra.  If you select Yes, you may be alerted by your firewall.", "Check for updates online", MB_ICONQUESTION | MB_YESNO);

			m_INI.WriteInt("General", "UpdateCheck", ret == IDYES);
		}

		if (m_INI.GetInt("General", "UpdateCheck", 0) == 1)
			_beginthread(UpdateCheck, 0, (void*)this);

	#endif
	}
	
	CExtNCW <CMDIFrameWnd >::OnTimer(nIDEvent);
}

void CMainFrame::OnClose() 
{
	// Loop apps check saved // Base call already changed to CMDIFrameWndEx in Block 1
	CApplication *app;
	POSITION pos = m_apps.GetHeadPosition();

	std::vector<CApplication*> apps_to_delete;

	bool bClose = true;
	while (pos != NULL) 
	{
		// iterate through objects to choose one to select
		POSITION oldPos = pos;
		app = m_apps.GetNext(pos);
		
		// Check save
		if (app->CloseSafely()) {
			apps_to_delete.push_back(app);
			m_apps.RemoveAt(oldPos);
		}
		else
			bClose = false;
	}

	if (!bClose)
	{
		std::vector<CApplication*>::iterator i = apps_to_delete.begin();

		for ( ; i != apps_to_delete.end(); ++i)
		{
			delete *i;
		}

		return;
	}

	//has_been_closed = true;	// stop layout editors crashing trying to access apps deleted above
	CMDIFrameWndEx::OnClose(); // Ensure this is CMDIFrameWndEx

	// Delete apps after closing main window & docs
	std::vector<CApplication*>::iterator i = apps_to_delete.begin();

	for ( ; i != apps_to_delete.end(); ++i)
	{
		delete *i;
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWndEx::OnSize(nType, cx, cy); // Base call already changed in Block 1
}

void CMainFrame::UpdateTheme()
{
	// Prof-UIS Theme switching logic removed.
	// MFC Visual Manager would be used here if theme switching is desired.
	// Example: CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	// AfxGetMainWnd()->RedrawWindow( NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE );

	// For now, this function will do nothing, or apply a default MFC theme if desired.
	// To apply a theme (VS 2008 look for example):
	// CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
    // CDockingManager::SetDockingMode(DT_SMART); // Optional: for smart docking markers
    // RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);


	// The INI writing for theme should also be removed or adapted if using MFC visual managers
	// m_INI.WriteInt("General", "Theme", new_theme_id);
}

// All OnXP(), OnOfficeXP(), On2003(), On2005(), On2008(), OnLunaBlue(), OnLunaSilver(), OnLunaBlack()
// methods are commented out as they are Prof-UIS specific theme handlers.

/*
void CMainFrame::OnXP() 
{
	// ... Prof-UIS specific ...
}

void CMainFrame::OnOfficeXP() 
{
	// ... Prof-UIS specific ...
}

void CMainFrame::On2003() 
{  
	// ... Prof-UIS specific ...
}

void CMainFrame::On2005() 
{   
	// ... Prof-UIS specific ...
}

void CMainFrame::On2008() 
{   
	// ... Prof-UIS specific ...
}

void CMainFrame::OnLunaBlue() 
{  
	// ... Prof-UIS specific ...
}

void CMainFrame::OnLunaSilver() 
{   
	// ... Prof-UIS specific ...
}

void CMainFrame::OnLunaBlack() 
{  
	// ... Prof-UIS specific ...
}
*/

void CMainFrame::OnPreview()
{
	CExport Export;
	Export.main_frame = this;
	Export.Preview(m_pPApp);
}

void CMainFrame::OnPreviewLayout(CLayout* layout)
{
	//CHECKME - why would layout be NULL here?
	// Parameter now ignored, uses same code as OnDebugLayout
	//if (layout == NULL) return;

	CExport Export;
	Export.main_frame = this;
	//Export.Preview(m_pPApp, layout->identifier);

	CChildFrame* pChildFrame = (CChildFrame*)CWnd::FromHandle(::GetWindow(m_hWndMDIClient, GW_CHILD));

	if (pChildFrame == NULL || pChildFrame->layout == NULL)
		MessageBox("There is no active layout to preview - make sure a layout is open!", "Preview layout");
	else
		Export.Preview(m_pPApp, pChildFrame->layout->identifier);
}

void CMainFrame::OnDebug()
{
	CExport Export;
	Export.main_frame = this;
	Export.PreviewDebug(m_pPApp);
}

void CMainFrame::OnDebugLayout()
{
	CExport Export;
	Export.main_frame = this;
	CChildFrame* pChildFrame = (CChildFrame*)CWnd::FromHandle(::GetWindow(m_hWndMDIClient, GW_CHILD));

	if (pChildFrame == NULL || pChildFrame->layout == NULL)
		MessageBox("There is no active layout to debug - make sure a layout is open!", "Preview layout");
	else
		Export.PreviewDebug(m_pPApp, pChildFrame->layout->identifier);
}

void CMainFrame::OnExportEXE()
{
	CExport Export;
	Export.main_frame = this;
	Export.ExportToEXE(m_pPApp);
}

void CMainFrame::CloseChildWindows(CLayout* pLayout)
{
	HWND ClientWindow;
	ClientWindow = ::GetWindow(m_hWndMDIClient, GW_CHILD);

	// Now find any layouts within this application
	while (ClientWindow != NULL)
	{
		CChildFrame* pWnd = (CChildFrame*)CWnd::FromHandle(ClientWindow);
		if(pWnd->layout == pLayout)
			pWnd->DestroyWindow();

		ClientWindow = :: GetWindow(ClientWindow, GW_HWNDNEXT);
	}

	// Update windows displaying this potentially
	tabs.RedrawWindow();
}

void CMainFrame::CloseChildWindows(EventSheet* pEventSheet)
{
	HWND ClientWindow;
	ClientWindow = ::GetWindow(m_hWndMDIClient, GW_CHILD);

	// Now find any layouts within this application
	while (ClientWindow != NULL)
	{
		CChildFrame* pWnd = (CChildFrame*)CWnd::FromHandle(ClientWindow);
		if(pWnd->event_sheet == pEventSheet)
			pWnd->DestroyWindow();

		ClientWindow = :: GetWindow(ClientWindow, GW_HWNDNEXT);
	}

	// Update windows displaying this potentially
	tabs.RedrawWindow();
}