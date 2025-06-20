/////////////////////////////////////////////////////////////////////////////
// Insert object Dialog
//
// Last revision: 1.1
// October 2008
//
// Changes:
// - Rewrote dialog
//
/////////////////////////////////////////////////////////////////////////////

// File includes
#include "StdAfx.h"
#include "..\Construct.h"
#include "..\Utilities\QArray.h"
#include "Insert Object Dlg.h"
#include <shlwapi.h>
#include "ExchangeDlg.h"
#include <commctrl.h>

// Object information
#include "Object Info Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\MainFrm.h"

extern CPluginMap g_Plugins;

static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   // lParamSort contains a pointer to the list view control.
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText(lParam2, 0);

   return strcmp(strItem2, strItem1);
}

/////////////////////////////////////////////////////////////////////////////
// InsertObjectDialog dialog

InsertObjectDialog::InsertObjectDialog(CApplication& application_)
	: CDialogEx(InsertObjectDialog::IDD, NULL), // Changed base class
	application(application_)
{
	m_curSel = -1;
	onclicklisttimeractivated = false;
}

void InsertObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class

	DDX_Control(pDX, IDC_GETMORE, exchange);
	DDX_Control(pDX, IDC_OBJECTS, objects);
	DDX_Control(pDX, IDOK, ok);
	DDX_Control(pDX, IDCANCEL, cancel);
	DDX_Control(pDX, IDC_LISTOBJECTS, list_objects);
	DDX_Control(pDX, IDC_INFO, info);
	DDX_Control(pDX, IDC_FILTER, filter);
	DDX_Control(pDX, IDC_NAME, name);
	DDX_Control(pDX, IDC_FOLDER, folders);
}

BEGIN_MESSAGE_MAP(InsertObjectDialog, CDialogEx) // Changed base class
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OBJECTS, OnClickList1)
	ON_BN_CLICKED(IDC_LISTOBJECTS, OnChangeObjectView)
	ON_BN_CLICKED(IDC_GETMORE, OnExchange)
	ON_NOTIFY(NM_DBLCLK, IDC_OBJECTS, OnDblclkList1)
	ON_NOTIFY(WM_LBUTTONUP, IDC_OBJECTS, OnSelected) // This seems unusual for LVN_ITEMACTIVATE or similar

	ON_WM_SIZE()
	ON_WM_DESTROY()

	ON_NOTIFY(NM_RCLICK, IDC_OBJECTS, OnRClickList)
	// ON_LBN_DBLCLK(IDC_CATEGORIES, OnChangeCategory) // Replaced by CTabCtrl
	// ON_LBN_SELCHANGE(IDC_CATEGORIES, OnSelchangeCategories) // Replaced by CTabCtrl
	ON_NOTIFY(TCN_SELCHANGE, 1000, &InsertObjectDialog::OnSelchangeCategoriesTabCtrl) // Assuming 1000 is ID for CTabCtrl 'tabs'
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FILTER, &InsertObjectDialog::OnBnClickedFilter)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void InsertObjectDialog::OnSelchangeCategoriesTabCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnSelchangeCategories(); // Call original logic
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// InsertObjectDialog message handlers

extern CString gInsert;
extern int   gInsertAuto;
extern CString insert_name;
extern CString insert_folder;

BOOL InsertObjectDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog(); // Changed base class

	// Add resizing - dlgMan and dlgAnchor are potentially Prof-UIS or other 3rd party. Left for now.
	BOOL First = dlgMan.Load(m_hWnd, "Software\\Construct\\InsertObjectDialog099");
	// first_run (CCtrlMessageBar) functionality removed
	/*
	if (!First)
	{
		first_run.Attach(this);
		first_run.SetHighlightOnMouseOver();
		first_run.SetResize();
		first_run.SetText(_T(IO_FIRSTRUN));
	}
	*/

    dlgAnchor.Init(m_hWnd); // Potentially Prof-UIS

	dlgAnchor.Add(IDOK, ANCHOR_RIGHT | ANCHOR_TOP);
	dlgAnchor.Add(IDCANCEL, ANCHOR_RIGHT | ANCHOR_TOP);
	dlgAnchor.Add(IDC_LISTOBJECTS, ANCHOR_RIGHT | ANCHOR_TOP);
	dlgAnchor.Add(IDC_GETMORE, ANCHOR_RIGHT | ANCHOR_TOP);
	dlgAnchor.Add(IDC_FILTER, ANCHOR_RIGHT | ANCHOR_BOTTOM);
	dlgAnchor.Add(IDTOPTEXT, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP);

	dlgAnchor.Add(IDC_NAME, ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT);
	dlgAnchor.Add(IDC_FOLDER, ANCHOR_TOP | ANCHOR_RIGHT | ANCHOR_BOTTOM);

	dlgAnchor.Add(IDC_OBJECTS, ANCHOR_TOPLEFT | ANCHOR_BOTTOMRIGHT);
	dlgAnchor.Add(IDC_INFO, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_BOTTOM);
	dlgAnchor.Add(IDC_TOOLBOX, ANCHOR_RIGHT | ANCHOR_BOTTOM);
	dlgAnchor.Add(10, ANCHOR_LEFT | ANCHOR_BOTTOM | ANCHOR_RIGHT); // Assuming 10 is IDC_TABBOX placeholder for tabs

	CRect Rect;
	// Get rect of placeholder static control for tab positioning, assuming ID 10 (IDC_TABBOX)
	CWnd* pTabPlaceholder = GetDlgItem(10);
	if (pTabPlaceholder) {
		pTabPlaceholder->GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		pTabPlaceholder->ShowWindow(SW_HIDE); // Hide placeholder
	}
	else { // Fallback if placeholder not found (original logic was based on 'objects' list view)
		objects.GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		Rect.top = Rect.bottom -1; // This positioning might be suboptimal now
		Rect.bottom = Rect.top + 22; // Adjust height for standard tabs
	}
	
	tabs.Create(WS_CHILD | WS_VISIBLE | TCS_BOTTOM | WS_CLIPSIBLINGS, Rect, this, 1000); // 1000 is control ID for CTabCtrl
	tabs.SetFont(GetFont());

	dlgAnchor.Add(tabs.GetSafeHwnd(), ANCHOR_LEFT | ANCHOR_BOTTOM | ANCHOR_RIGHT); // Anchor the tab control by HWND
	
	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;
	CString tabTexts[] = {OT_ALL, OT_AUDIO, OT_CONTROLS, _T("Data & Files") /*OT_DATAFILES had issues*/, OT_GAME, OT_GRAPHICS, OT_INPUT, OT_INTERNET, OT_OTHER, OT_SYSTEM};
	for(int i = 0; i < sizeof(tabTexts)/sizeof(CString); ++i) {
		tcItem.pszText = tabTexts[i].GetBuffer(0);
		tabs.InsertItem(i, &tcItem);
		tabTexts[i].ReleaseBuffer();
	}
	tabs.SetCurSel(0);

	// Load objects
	LoadObjects(); // This function populates m_LargeImages and small_images
	ListView_SetImageList(objects.m_hWnd, m_LargeImages, LVSIL_NORMAL);		
	ListView_SetImageList(objects.m_hWnd, small_images, LVSIL_SMALL);	

	// Name of dialog
	CString dialogTitle;
	dialogTitle.Format("Construct : %s", IO_TITLE);
	SetWindowText(dialogTitle);

	// Set texts
	cancel.SetWindowText(CANCEL);
	ok.SetWindowText(INSERT);
	ok.EnableWindow(FALSE);
	exchange.SetWindowText(IO_EXCHANGE);
	info.SetWindowText(IO_CLICKFORINFO);
	list_objects.SetWindowText(IO_LIST_OBJECTS);
	filter.SetWindowText(IO_SEARCH);

	OnChangeCategory();

	if (g_MainFrame->m_INI.GetInt("InsertObject", "List", 0) == 1)
	{
		list_objects.SetCheck(BST_CHECKED);
		OnChangeObjectView();
	}

	objects.SetScrollPos(SB_VERT, 0);
	objects.SetScrollPos(1, 0);
	objects.Invalidate();

	// fill object folders
	vector<CApplication::ObjectFolder>::iterator folder = application.object_folders.begin();

	for ( ; folder != application.object_folders.end(); folder++)
		folders.AddString(folder->name);

	// select default
	folders.SetCurSel(0);

	// Refresh
	SetTimer(10, 200, NULL);

	return TRUE; 
}

void InsertObjectDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 10)
	{
		KillTimer(10);

		objects.SetScrollPos(SB_VERT, 0);
		objects.SetScrollPos(1, 0);
		objects.Invalidate();
	}

	else
	{
		onclicklisttimeractivated = false;
		KillTimer(500);
		CString give;
		name.GetWindowText(give);
		name.SetSel(0, give.GetLength());
		name.SetFocus();
	}
}

void InsertObjectDialog::OnExchange()
{
	CExchangeDlg Dlg;
	Dlg.DoModal();
}

void InsertObjectDialog::OnOK() 
{
	OnDblclkList1(0,0);

	CDialogEx::OnOK(); // Changed base class
}

void InsertObjectDialog::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (ok.IsWindowEnabled()) 
	{
		gInsert = "";
		POSITION pos = objects.GetFirstSelectedItemPosition();

		if (pos != NULL)
		{
			while (pos) 
			{
				int nItem = objects.GetNextSelectedItem(pos);

				LVITEM lvi;
				lvi.mask = LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;

				if (ListView_GetItem(objects.m_hWnd, &lvi)) 
				{
					CPlugin Plugin = GetPluginByFile((const char*)lvi.lParam);
					gInsert = Plugin.m_FileName;

					name.GetWindowText(insert_name);
					folders.GetLBText(folders.GetCurSel(), insert_folder);

					bool bGame = false;
					if (application.runtime != CApplication::rt_application) bGame = true;

					// common objects
					long count = g_MainFrame->m_INI.GetInt(bGame ? "RecentObjects" : "RecentObjectsApp", Plugin.m_FileName, 0);
					g_MainFrame->m_INI.WriteInt(bGame ? "RecentObjects" : "RecentObjectsApp", Plugin.m_FileName, count + 1);

					if (Plugin.m_Flags & OF_NODRAW)
						gInsertAuto = 1;				
				}
			}
		}

		EndDialog(0);
	}
}

void InsertObjectDialog::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = objects.GetFirstSelectedItemPosition();

	if (pos == NULL) 
	{
		if (m_curSel != -1) 
		{
			m_curSel = -1;
			info.SetWindowText(IO_CLICKFORINFO);
			
			ok.EnableWindow(FALSE);
		}
	}

	else 
	{
		while (pos) 
		{
			int nItem = objects.GetNextSelectedItem(pos);
			if (nItem != m_curSel) 
			{
				m_curSel = nItem;
				LVITEM lvi;
				lvi.mask = LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				if (ListView_GetItem(objects.m_hWnd, &lvi)) 
				{
					// New, use CPlugin
					CPlugin Plugin = GetPluginByFile((const char*)lvi.lParam);

					ok.EnableWindow(TRUE);
				
					CString Text;
					Text.Format("%s: %s\r\n%s: %s\r\n%s: %s", IO_AUTHOR, Plugin.m_Author, IO_VERSION, Plugin.m_Version, IO_DESCRIPTION, Plugin.m_Description);
					info.SetWindowText(Text);

					CString give = Plugin.m_Name;

					give = application.GetUniqueObjectTypeName(give);

					name.SetWindowText(give);

					SetTimer(500, 1000, NULL);
				}

				else 
					ok.EnableWindow(FALSE);
			}

			break;
		}
	}

	*pResult = 0;
}

void InsertObjectDialog::OnSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString give;
	name.GetWindowText(give);
	name.SetSel(0, give.GetLength());
	name.SetFocus();
}

void InsertObjectDialog::OnChangeObjectView() 
{
	if (list_objects.GetCheck())
	{
		::SetWindowLong(objects.GetSafeHwnd(), GWL_STYLE,
		::GetWindowLong(objects.GetSafeHwnd(), GWL_STYLE) | LVS_SMALLICON);
	}

	else
	{
		::SetWindowLong(objects.GetSafeHwnd(), GWL_STYLE,
		::GetWindowLong(objects.GetSafeHwnd(), GWL_STYLE) & ~(LVS_SMALLICON));
	}
}

void InsertObjectDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy); // Changed base class
	
	dlgAnchor.OnSize();	// Potentially Prof-UIS

	Invalidate();
}

void InsertObjectDialog::OnDestroy() 
{
	// first_run.Detach(); // CCtrlMessageBar (first_run) functionality removed

	ListView_SetImageList(objects.m_hWnd, NULL, LVSIL_SMALL);
	ListView_SetImageList(objects.m_hWnd, NULL, LVSIL_NORMAL);

	if (list_objects.GetCheck() == BST_CHECKED)
		g_MainFrame->m_INI.WriteInt("InsertObject", "List", 1);
	else
		g_MainFrame->m_INI.WriteInt("InsertObject", "List", 0);

	CDialogEx::OnDestroy(); // Changed base class
	
	dlgMan.Save(); // Potentially Prof-UIS
}

void InsertObjectDialog::OnRClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION listpos = objects.GetFirstSelectedItemPosition();
	if(listpos == NULL) { // Added check for no selection
		*pResult = 0;
		return;
	}
	int item = objects.GetNextSelectedItem(listpos);
	CString itemtext = objects.GetItemText(item, 0);

	if (itemtext == "") {
		*pResult = 0;
		return;
	}

	// Mouse position
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);

	CMenu menu;
	if(menu.LoadMenu(IDR_OBJECT)) // Assuming IDR_OBJECT is a standard menu resource
	{
		CMenu* pSubMenu = menu.GetSubMenu(0); // Assuming the desired popup is the first submenu
		if(pSubMenu)
		{
			UINT ChosenItem = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
														cursorPosition.x, cursorPosition.y, this);
			// Open info dialog
			if (ChosenItem == ID_OBJECTINFO)
			{
				CObjectInfoDlg objInfo;
				objInfo.objname = itemtext;
				objInfo.DoModal();
			}
			// Open website address
			else if (ChosenItem == ID_MENU_VISITURL)
			{
				CPlugin Plugin = GetPluginByFile((const char*)objects.GetItemData(item));
				ShellExecute(NULL, "open", Plugin.m_WebLink, NULL, NULL, NULL);
			}
		}
	}
	// delete popup; CExtPopupMenuWnd was heap allocated, CMenu is stack allocated.

	*pResult = 0;
}

void InsertObjectDialog::LoadObjects()
{
	// Create imagelists
	m_LargeImages.Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 10); // Added ILC_MASK
	small_images.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 10);  // Added ILC_MASK

	map<int, CPlugin>::iterator i = g_Plugins.begin();

	for ( ; i != g_Plugins.end(); i++) 
	{
		CString lowerName = i->second.m_Name;
		lowerName.MakeLower();

		if (!i->second.m_bMovement && lowerName != "tag")
		{
			bool bGame = false;
			if (application.runtime != CApplication::rt_application) bGame = true;

			if (!bGame && i->second.m_Category == OT_GAME)
				continue;

			// Extract the icon
			if(i->second.m_LargeIcon) // Add NULL check
				ImageList_Add(m_LargeImages, i->second.m_LargeIcon, NULL);
			if(i->second.m_SmallIcon) // Add NULL check
				ImageList_Add(small_images, i->second.m_SmallIcon, NULL);
		}
	}
}

void InsertObjectDialog::OnChangeCategory() 
{
	bool bGame = false;
	if (application.runtime != CApplication::rt_application) bGame = true;

	int CategoryID = tabs.GetCurSel(); // Changed from CExtTabFlatWnd::SelectionGet()
	CString SelectedCategory;

	// Find category to use
	if ((CategoryID != 0) && (CategoryID != -1)) // Assuming 0 is "All" category
	{
		TCITEM tcItem;
		TCHAR buffer[256]; // Buffer for tab text
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = buffer;
		tcItem.cchTextMax = 256;
		if(tabs.GetItem(CategoryID, &tcItem))
			SelectedCategory = tcItem.pszText;

		SelectedCategory.Replace(_T("&&"), _T("&")); // ProfUIS used "&&" for literal "&" in tabs, MFC CTabCtrl does not.
		objects.RemoveAllGroups(); // This is CGroupListCtrl specific
	}
	// Otherwise show all
	else
	{
	{
		objects.AddGroup(0, OT_ALL); // CGroupListCtrl specific
		objects.AddGroup(1, OT_AUDIO); // CGroupListCtrl specific
		objects.AddGroup(2, OT_CONTROLS); // CGroupListCtrl specific
		objects.AddGroup(3, "Data & Files"); // CGroupListCtrl specific
		if (bGame) objects.AddGroup(4, OT_GAME); // CGroupListCtrl specific
		objects.AddGroup(5, OT_GRAPHICS); // CGroupListCtrl specific
		objects.AddGroup(6, OT_INPUT); // CGroupListCtrl specific
		objects.AddGroup(7, OT_INTERNET); // CGroupListCtrl specific
		objects.AddGroup(8, OT_OTHER); // CGroupListCtrl specific
		objects.AddGroup(9, OT_PROFESSIONAL); // CGroupListCtrl specific
		objects.AddGroup(10, OT_SYSTEM); // CGroupListCtrl specific
	}

	SelectedCategory.MakeLower();
	objects.DeleteAllItems();

	bool bAddPlugin = false;
	if (CategoryID == 0 || CategoryID == -1) bAddPlugin = true; // Assuming 0 is "All"

	// Prepare common object selection
	CQArray <intstr, intstr&> intstrArr;
	CStringArray oArray;
	g_MainFrame->m_INI.GetKeyNames(bGame ? "RecentObjects" : "RecentObjectsApp", &oArray);
	for (int k = 0; k < oArray.GetCount(); k++) // Changed loop variable
	{
		intstr obj;
		obj.variable = oArray.GetAt(k);
		obj.id = g_MainFrame->m_INI.GetInt(bGame ? "RecentObjects" : "RecentObjectsApp", obj.variable, 0);
		intstrArr.Add(obj);
	}

	intstrArr.QuickSort(FALSE); 

	map<int, CPlugin>::iterator plugin_iter = g_Plugins.begin(); // Changed loop variable

	// Image index
	int Index = 0; // Unused for image list index, 'Image' is used
	int Image = 0;

	// Loop all objects
	for ( ; plugin_iter != g_Plugins.end(); plugin_iter++)
		{
		CString lowerName = plugin_iter->second.m_Name;
		lowerName.MakeLower();

		if (plugin_iter->second.m_bMovement || lowerName == "tag")
		{
			Index++;
			continue;
		}

		CString Category = plugin_iter->second.m_Category;
		Category.MakeLower();

		// We want this object
		if (Category.Find(SelectedCategory, 0) != -1 || bAddPlugin)
			{
			// Add this plugin
			LVITEM InsertPlugin;

			if (CategoryID == 0 || CategoryID == -1) // Show all with groups (CGroupListCtrl specific)
			{
				InsertPlugin.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE | LVIF_GROUPID;
				InsertPlugin.state = 0;
				InsertPlugin.stateMask = 0;
				// InsertPlugin.iItem = ListView_GetItemCount(objects.m_hWnd) - 1; // Incorrect for new item, should be count
				InsertPlugin.iImage = Image;
				InsertPlugin.iSubItem = 0;
				InsertPlugin.lParam = (LPARAM)((const char*)plugin_iter->second.m_FileName);
				InsertPlugin.pszText = plugin_iter->second.m_Name.GetBuffer(0);

				// Now find a group (CGroupListCtrl specific)
				InsertPlugin.iGroupId = -1;

				// Audio
				if (Category.Find(OT_AUDIO.MakeLower(), 0) != -1) 
					InsertPlugin.iGroupId = 1;
				// Controls
				else if (Category.Find(OT_CONTROLS.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 2;
				// Data and files
				else if (Category.Find("data & files", 0) != -1)
					InsertPlugin.iGroupId = 3;
				// Game
				else if (Category.Find(OT_GAME.MakeLower(), 0) != -1) {
					if (!bGame) {
						plugin_iter->second.m_Name.ReleaseBuffer();
						Image++; // Still need to advance image index if we skip
						continue;
					}
					else InsertPlugin.iGroupId = 4;
				}
				// Graphics
				else if (Category.Find(OT_GRAPHICS.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 5;
				// Input
				else if (Category.Find(OT_INPUT.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 6;
				// Internet
				else if (Category.Find(OT_INTERNET.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 7;
				// Other
				else if (Category.Find(OT_OTHER.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 8;
				// Professional
				else if (Category.Find(OT_PROFESSIONAL.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 9;
				// System
				else if (Category.Find(OT_SYSTEM.MakeLower(), 0) != -1)
					InsertPlugin.iGroupId = 10;

				bool isCommon = false;
				// Is it a common object?
				for (int j = 0; j < min(intstrArr.GetCount(), 8); j++) 
					{
					if (!_stricmp(plugin_iter->second.m_FileName, intstrArr[j].variable))
					{ 
						isCommon = true;
						// This is a commonly used object
						if (InsertPlugin.iGroupId != -1) { // If it also belongs to another group
							LVITEM commonInsert = InsertPlugin; // copy
							commonInsert.iGroupId = 0; // Common group
							commonInsert.iItem = ListView_GetItemCount(objects.m_hWnd);
							if (lowerName != "directsound")
								ListView_InsertItem(objects.m_hWnd, &commonInsert);
						}
						else { // Only common
							InsertPlugin.iGroupId = 0;
						}
						break;
					}
					}

				// Add to its specific group if not already added as common OR if it belongs to multiple (handled by CGroupListCtrl logic)
				// The original logic might add items twice if common and also in current category. CGroupListCtrl might handle this.
				// Simplified: if it's common, it's in group 0. If also in specific, depends on CGroupListCtrl.
				// For now, ensure it's added if it matched a specific group and wasn't ONLY common.
				if(InsertPlugin.iGroupId != 0 || !isCommon) { // If it has a specific group, or isn't common at all
					if (lowerName != "directsound") {
						InsertPlugin.iItem = ListView_GetItemCount(objects.m_hWnd);
						ListView_InsertItem(objects.m_hWnd, &InsertPlugin);
						}
					}
				plugin_iter->second.m_Name.ReleaseBuffer();
			}
			else // Specific category selected, no groups in CListCtrl standard view
			{
				InsertPlugin.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				InsertPlugin.state = 0;
				InsertPlugin.stateMask = 0;
				InsertPlugin.iItem = ListView_GetItemCount(objects.m_hWnd);
				InsertPlugin.iImage = Image;
				InsertPlugin.iSubItem = 0;
				InsertPlugin.lParam = (LPARAM)((const char*)plugin_iter->second.m_FileName);
				InsertPlugin.pszText = plugin_iter->second.m_Name.GetBuffer(0);
				if (lowerName != "directsound") ListView_InsertItem(objects.m_hWnd, &InsertPlugin);
				plugin_iter->second.m_Name.ReleaseBuffer();
				}
			}

		Image++; // Increment for each potential image, even if skipped, to keep sync with imagelist
		Index++;
		}

	if (CategoryID == 0 || CategoryID == -1)
		objects.EnableGroups(); // This is a CGroupListCtrl specific function

	// Sort the list view items
	// MyCompareProc needs to be static or global for SortItems.
	// objects.SortItems(MyCompareProc, (LPARAM)&objects); // This is CListCtrl method

	// objects.SetScrollPos(0, 0); // This is for CWnd, not CListCtrl scrollbar specifically
	objects.Scroll(CSize(0,-50000)); // Scroll to top using CListCtrl::Scroll
	objects.Scroll(CSize(0,0));
}

void InsertObjectDialog::OnSelchangeCategories() 
{

	OnChangeCategory();
}

BOOL InsertObjectDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			OnOK();
			return TRUE;
		default:
			if(onclicklisttimeractivated)
			{
				KillTimer(500);
				SetTimer(500, 1000, NULL);

			}
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg); // Changed base class
}

void InsertObjectDialog::OnBnClickedFilter()
{
	bool bGame = false;
	if (application.runtime != CApplication::rt_application) bGame = true;

	if (filter_dialog.m_hWnd)
		filter_dialog.DestroyWindow();

	CPoint Pt;
	::GetCursorPos(&Pt);

	filter_dialog.m_bGame = bGame;
	filter_dialog.Create(IDD_FILTER, this);
	filter_dialog.SetWindowPos(NULL, Pt.x, Pt.y, -1, -1, SWP_NOSIZE);
	filter_dialog.ShowWindow(SW_SHOW);
}

void InsertObjectDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (filter_dialog.m_hWnd)
		filter_dialog.DestroyWindow();

	CDialogEx::OnLButtonUp(nFlags, point); // Changed base class
}
