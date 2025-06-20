// mybar.cpp : implementation file
//

#include "stdafx.h"
#include "PropertiesBar.h"
#include "../MainFrm.h"

// IMPLEMENT_DYNCREATE for Prof-UIS grid cell classes commented out
// IMPLEMENT_DYNCREATE(CExtGridCellHyperLinkEx, CExtGridCellHyperLink);
// IMPLEMENT_DYNCREATE(CExtGridCellCurrencyEx, CExtGridCellCurrency);
// IMPLEMENT_DYNCREATE(CExtGridCellNumberEx, CExtGridCellNumber);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(PropertiesBar, CDockablePane) // Added IMPLEMENT_DYNAMIC

extern PropertiesBar* propertyWindow;
extern CImageList* m_treeImages;
extern CMainFrame* pMainWnd;

#include "..\ChildFrame.h"
#include "..\Construct.h"

// Image editor include
#include "..\Editors\ImageEditor.h"

extern CLayerBar* g_pLayerBar;
/////////////////////////////////////////////////////////////////////////////
// CProperties Bar
class CPerformanceTimer {
public:
	__int64 start;
	__int64 freq;

	map<CString, int> timers;

	CString curTimer;

	CPerformanceTimer() {
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	}

	void Start(CString name) {
		curTimer = name;
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	}

	void Stop() {
		__int64 c;
		QueryPerformanceCounter((LARGE_INTEGER*)&c);

		timers[curTimer] += c - start;
	}

	void QuickReport() {
		CString report = "Timers report:\n\n";
		
		map<CString, int>::iterator i = timers.begin();

		for ( ; i != timers.end(); i++) {
			CString line;
			
			float ms = ((float)(i->second) * 1000.0f) / (float)freq;

			line.Format("%s : %.2f ms\n", i->first, ms);
			report += line;
		}

		MessageBox(NULL, report, "Timers quickreport", MB_OK);
	}

};

#define IDC_PROPERTYTREE			11065



PropertiesBar::PropertiesBar()
{
	curExpandSave = NULL;
	m_pSelectedObjType = NULL;
	m_pSelectedObj = NULL;
}

PropertiesBar::~PropertiesBar()
{
	// Delete any allocated property stores
//	vector<CExtPropertyStore*>::iterator i = m_pMultiStore.begin(); // m_pMultiStore commented out in .h

	//for ( ; i != m_pMultiStore.end(); i++)
	//	delete *i;
}

BEGIN_MESSAGE_MAP(PropertiesBar, CDockablePane) // Changed base class
	//{{AFX_MSG_MAP(PropertiesBar)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN() // Kept standard message, specific Prof-UIS context menu gone
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyBar message handlers (Comment should be CPropertiesBar)

BOOL PropertiesBar::OnEraseBkgnd(CDC* pDC) 
{
      return TRUE;
}

void PropertiesBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy); // Changed base class
	if (m_GridPlaceholder.GetSafeHwnd()) { // Resize placeholder
		m_GridPlaceholder.MoveWindow(0, 0, cx, cy);
	}
}

int PropertiesBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1) // Changed base class
		return -1;

	// Create placeholder static control instead of Prof-UIS grid
	if (!m_GridPlaceholder.Create("Properties Grid Placeholder - Prof-UIS CProfStudioPropertyGridCtrl removed",
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		CRect(0,0,0,0), this, IDC_STATIC)) // Use IDC_STATIC or a new ID
	{
		TRACE0("Failed to create property grid placeholder\n");
		return -1;
	}
	m_pGridPlaceholder = &m_GridPlaceholder; // Point the placeholder pointer

	return 0;
}

enum OPTION_CONTROL_TYPE {
	OPTION_CONTROL_EDIT,
	OPTION_CONTROL_STATIC,
	OPTION_CONTROL_BUTTON,
	OPTION_CONTROL_VALUE,
	OPTION_CONTROL_COMBO,
	OPTION_CONTROL_FLOAT,
	OPTION_CONTROL_COLOR,
	OPTION_CONTROL_CHECKBOX,
	OPTION_CONTROL_RADIO,
	OPTION_CONTROL_FORCE_DWORD = 0x7fffffff
};

// Refresh by repeating the last Update()
void PropertiesBar::Refresh()
{
	// Update(m_view, m_type, &m_sel, m_CurrentLayout, m_objects, m_pCurrentApplication, 0, pLastLayer, NULL);
	// Update method body is now commented out due to grid replacement
}

template <typename M, typename C>
bool PropertiesBar::SelectionMembersEqual(M C::* member)
{
	return ptrmem_members_equal(vSelectedObjects.begin(),
							    vSelectedObjects.end(),
								&CObj::editObject,
								member);
}

void PropertiesBar::ShowTip(std::string const tip)
{
	// tips.Attach(m_pGridPlaceholder->GetActiveGrid()); // m_GridPlaceholder is placeholder, tips is removed

	// tips.SetHighlightOnMouseOver();

	// tips.SetWrapText();
	// tips.SetText(tip.c_str());
	MessageBox(CString(tip.c_str()), "Property Info", MB_OK); // Simple replacement
}

void PropertiesBar::AddAnimationProperties()
{
	// All logic depends on Prof-UIS grid, commented out
	/*
	CExtPropertyItem* pRoot;
	// ... rest of the function ...
	*/
}

void PropertiesBar::AddLayoutProperties(CLayout* layout)
{
	// All logic depends on Prof-UIS grid, commented out
	/*
	CExtPropertyItem* pRoot;
	// ... rest of the function ...
	*/
}

void PropertiesBar::AddLayerProperties(CLayer* layer)
{
	// All logic depends on Prof-UIS grid, comment out
	/*
	CExtPropertyItem* pRoot;
	// ... rest of the function ...
	*/
}

void PropertiesBar::AddResourceProperties(ApplicationResource* resource, CApplication* application, list<ApplicationResource>* stored)
{
	// All logic depends on Prof-UIS grid, comment out
	/*
	CExtPropertyItem* pRoot;
	// ... rest of the function ...
	*/
}

void PropertiesBar::Update(CLayoutEditor* view, long type, CObjList* sel, CLayout* pLayout, CObjMap *objects, CApplication* app, long expand, CLayer* layer, ApplicationResource* resource, list<ApplicationResource>* stored) 
{
	// This entire function is for populating the Prof-UIS property grid.
	// Since the grid is replaced by a placeholder, this function's body is mostly commented out.
	/*
	if(app) m_pCurrentApplication = app;
	CPerformanceTimer timer; // This is a local helper, can stay if not problematic

	// ... (rest of the logic that interacts with m_pStore, m_pGridPlaceholder, Insert...Item, Get...Cell) ...

	// Refresh grid
	// m_pGridPlaceholder->PropertyStoreSynchronize(); // This functionality is lost
	// m_pGridPlaceholder->RecalcLayout(); // This functionality is lost

	// CExtPropertyGridWnd* pGridWnd = m_pGridPlaceholder->GetActiveGrid(); // This functionality is lost
	// pGridWnd->SetScrollPos(SB_VERT, 0); // This functionality is lost
	*/
}


void PropertiesBar::Update_OINFO_Properties(editInfo* info)
{
	// This function relies on OINFO::Properties which is a vector<CPropItem>
	// CPropItem itself might be fine, but this function is usually called by Update()
	// which is now mostly commented out.
	/*
	CPropItem item;
	// ... rest of the function ...
	*/
}