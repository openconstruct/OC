#include "StdAfx.h"
#include "LayerBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLayerBar* g_pLayerBar;

/////////////////////////////////////////////////////////////////////////////
// CLayerBar
IMPLEMENT_DYNAMIC(CLayerBar, CDockablePane) // Added IMPLEMENT_DYNAMIC

CLayerBar::CLayerBar()
{
}

CLayerBar::~CLayerBar()
{
}


BEGIN_MESSAGE_MAP(CLayerBar, CDockablePane) // Changed base class
	//{{AFX_MSG_MAP(CLayerBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define IDC_LAYERLISTBOX 124

/////////////////////////////////////////////////////////////////////////////
// CLayerBar message handlers

int CLayerBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1) // Changed base class
		return -1;
	
	CRect rc;

	// Init the control's size to cover the entire client area
	// GetClientRect(rc); // Not needed if m_layerDlg is sized in OnSize
	if (!m_layerDlg.Create(IDD_BARDIALOG, this))
	{
		TRACE0("Failed to create layer dialog\n");
		return -1;
	}
	m_layerDlg.ShowWindow(SW_SHOW);

	return 0;
}


void CLayerBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy); // Changed base class

	// Resize m_layerDlg to fill the pane
	if (m_layerDlg.GetSafeHwnd()) {
		m_layerDlg.MoveWindow(0, 0, cx, cy);
	}
}