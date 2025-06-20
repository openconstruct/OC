#include "stdafx.h"
#include "ProjectBar.h"
#include "..\MainFrm.h"

#include "..\Dialogs\Used Plugins Dlg.h"
#include "..\Dialogs\Family Manager dialog\Family Manager Dlg.h"
#include "..\Dialogs\MenuEditorDlg.h"
#include "..\Dialogs\ImageEditorDlg.h"
#include "..\Utilities\FECFileDialog.h"

#include "..\Structure\Export\Export.h"

#include "..\Construct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* pMainWnd;
extern PropertiesBar* g_PropertyBar;

IMPLEMENT_DYNAMIC(ProjectBar, CDockablePane) // Added IMPLEMENT_DYNAMIC

ProjectBar::ProjectBar() :
	stop_expand(false),
	dragging_object(false)
{
}

extern CMultiDocTemplate* pDocTemplate;

BEGIN_MESSAGE_MAP(ProjectBar, CDockablePane) // Changed base class
	ON_WM_CREATE()

	ON_NOTIFY(NM_CLICK, 100, OnSingleClick)
	ON_NOTIFY(NM_RCLICK, 100, OnRightClick)
	ON_NOTIFY(NM_DBLCLK, 100, OnDoubleClick)
	ON_NOTIFY(TVN_ITEMEXPANDING, 100, OnExpanding)
	ON_NOTIFY(TVN_BEGINLABELEDIT, 100, OnBeginEditLabel)
	ON_NOTIFY(TVN_ENDLABELEDIT, 100, OnEditLabel)

	// ON_REGISTERED_MESSAGE(WM_XHTMLTREE_BEGIN_DRAG, OnBeginDrag) // Removed CXHtmlTree specific message
	// ON_REGISTERED_MESSAGE(WM_XHTMLTREE_END_DRAG, OnEndDrag)   // Removed CXHtmlTree specific message
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// Main update function
////////////////////////////////////////////////////////////////////////////

int ProjectBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1) // Changed base class
		return -1;

	if (!tree.Create(WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_EDITLABELS,
		CRect(1, 1, 1, 1), this, 100)) // Control ID 100 for the tree
		return -1;

	tree.ModifyStyleEx(0, WS_EX_STATICEDGE);

	font.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
	tree.SetFont(&font);
	// tree.always_shift = false; // CXHtmlTree specific member, remove
	
	images.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_PROJECT);
	images.Add(&bitmap, RGB(0, 0, 0));

	TreeView_SetImageList(tree, images, TVSIL_NORMAL);

	// cleanup
	bitmap.DeleteObject();

	return 0;
}

void ProjectBar::AddApplication(CApplication* application)
{
	// add this application to the tree
	application->tree_items.application = tree.InsertItem(application->file_information.name, image_application, image_application);
	tree.SetItemData(application->tree_items.application, (DWORD)application);

	// layouts
	application->tree_items.layouts = tree.InsertItem("Layouts", image_stub, image_stub, application->tree_items.application); // HTML removed

	// event sheets
	application->tree_items.event_sheets = tree.InsertItem("Event sheets", 0, image_stub, application->tree_items.application); // HTML removed

	// folders
	application->tree_items.folders = tree.InsertItem("Objects", image_stub, image_stub, application->tree_items.application); // HTML removed

	// global variables
	application->tree_items.global_variables = tree.InsertItem("Global variables", image_stub, image_stub, application->tree_items.application); // HTML removed

	// file resources
	application->tree_items.files = tree.InsertItem("Files", image_stub, image_stub, application->tree_items.application); // HTML removed

	// sound resources
	application->tree_items.sounds = tree.InsertItem("Sounds", image_stub, image_stub, application->tree_items.application); // HTML removed

	// music resources
	application->tree_items.music = tree.InsertItem("Music", image_stub, image_stub, application->tree_items.application); // HTML removed

	// font resources
	application->tree_items.fonts = tree.InsertItem("Fonts", image_stub, image_stub, application->tree_items.application); // HTML removed

	// icon resources
	application->tree_items.icons = tree.InsertItem("Icons", image_stub, image_stub, application->tree_items.application); // HTML removed

	// menu resources
	application->tree_items.menus = tree.InsertItem("Menus", image_stub, image_stub, application->tree_items.application); // HTML removed
}

void ProjectBar::UpdateApplication(CApplication* application)
{
	// first of all, loop layouts
	POSITION position = application->layouts.GetHeadPosition();

	CLayout* layout = NULL;
	CLayout* last_layout = NULL;

	while (position)
	{
		last_layout = layout;
		layout = application->layouts.GetNext(position);
		
		if (layout->tree == NULL)
		{
			// add it, not yet added
			HTREEITEM after;
			if (last_layout) after = last_layout->tree;
			else after = TVI_FIRST;

			layout->tree = tree.InsertItem(layout->GetName(), image_layout, image_layout, application->tree_items.layouts, after);
			tree.SetItemData(layout->tree, (LPARAM)layout);

			// add linked event sheet
			layout->event_sheet->tree[layout] = tree.InsertItem(layout->event_sheet->GetName(), image_event_sheet, image_event_sheet, layout->tree);
			tree.SetItemData(layout->event_sheet->tree[layout], (LPARAM)layout->event_sheet);
		}
	}

	list<EventSheet*>::iterator sheet = application->event_sheets.begin();

	// check event sheets
	for( ; sheet != application->event_sheets.end(); sheet++)
	{	
		if (!(*sheet)->second_tree)
		{
			(*sheet)->second_tree = tree.InsertItem((*sheet)->GetName(), image_event_sheet, image_event_sheet, application->tree_items.event_sheets);
			tree.SetItemData((*sheet)->second_tree, (LPARAM)(*sheet));
		}
	}

	// check object folders
	vector<CApplication::ObjectFolder>::iterator folder = application->object_folders.begin();

	for ( ; folder != application->object_folders.end(); folder++)
	{
		if (folder->tree == NULL)
		{
			folder->tree = tree.InsertItem(folder->name, image_object_folder, image_object_folder, application->tree_items.folders);
			tree.SetItemData(folder->tree, (LPARAM)application);
		}
	}

	// now do objects in those folders
	POSITION object_position = application->object_types.GetStartPosition();
	long key;
	CObjType* object;

	for (int i = 0; i < application->object_types.GetCount(); i++)
	{
		application->object_types.GetNextAssoc(object_position, key, object);
	
		if (!object->tree)
		{
			HBITMAP bitmap = object->small_image.MakeBitmap();
			int icon = ImageList_Add(images.m_hImageList, bitmap, NULL);
			DeleteObject(bitmap);

			folder = application->object_folders.begin();

			for ( ; folder != application->object_folders.end(); folder++)
			{
				if (folder->name == object->GetFolder())
				{
					object->tree = tree.InsertItem(object->GetName(), icon, icon, folder->tree);
					tree.SetItemData(object->tree, (LPARAM)object);
					tree.Invalidate();

					break;
				}
			}
		}
	}

	// check global variables
	list<CApplication::GlobalVariable>::iterator variable = application->global_variables.begin();

	for ( ; variable != application->global_variables.end(); variable++)
	{
		if (variable->tree == NULL)
		{
			CString name;
			name.Format("%s (%s)", variable->name, variable->value);

			variable->tree = tree.InsertItem(name, image_global_variable, image_global_variable, application->tree_items.global_variables);
			tree.SetItemData(variable->tree, (LPARAM)&(*variable));
		}
	}

	// file resources
	list<BinaryResource>::iterator binary = application->resources.file_resources.begin();

	for ( ; binary != application->resources.file_resources.end(); binary++)
	{
		if (!binary->tree)
		{
			binary->tree = tree.InsertItem(binary->GetName(), image_binary_resource, image_binary_resource, application->tree_items.files);
			tree.SetItemData(binary->tree, (LPARAM)&*binary);
		}
	}

	// sound resources
	list<BinaryResource>::iterator sound = application->resources.sound_resources.begin();

	for ( ; sound != application->resources.sound_resources.end(); sound++)
	{
		if (!sound->tree)
		{
			sound->tree = tree.InsertItem(sound->GetName(), image_sound_resource, image_sound_resource, application->tree_items.sounds);
			tree.SetItemData(sound->tree, (LPARAM)&*sound);
		}
	}

	// music resources
	list<BinaryResource>::iterator music = application->resources.music_resources.begin();

	for ( ; music != application->resources.music_resources.end(); music++)
	{
		if (!music->tree)
		{
			music->tree = tree.InsertItem(music->GetName(), image_music_resource, image_music_resource, application->tree_items.music);
			tree.SetItemData(music->tree, (LPARAM)&*music);
		}
	}

	// font resources
	list<FontResource>::iterator font = application->resources.font_resources.begin();

	for ( ; font != application->resources.font_resources.end(); font++)
	{
		if (!font->tree)
		{
			font->tree = tree.InsertItem(font->GetName(), image_font_resource, image_font_resource, application->tree_items.fonts);
			tree.SetItemData(font->tree, (LPARAM)&*font);
		}
	}

	// icon resources
	list<IconResource>::iterator icon = application->resources.icon_resources.begin();

	for ( ; icon != application->resources.icon_resources.end(); icon++)
	{
		if (!icon->tree)
		{
			icon->tree = tree.InsertItem(icon->GetName(), image_icon_resource, image_icon_resource, application->tree_items.icons);
			tree.SetItemData(icon->tree, (LPARAM)&*icon);
		}
	}

	// menu resources
	list<MenuResource>::iterator menu = application->resources.menu_resources.begin();

	for ( ; menu != application->resources.menu_resources.end(); menu++)
	{
		if (!menu->tree)
		{
			menu->tree = tree.InsertItem(menu->GetName(), image_menu_resource, image_menu_resource, application->tree_items.menus);
			tree.SetItemData(menu->tree, (LPARAM)&*menu);
		}
	}
}

void ProjectBar::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// check what it is
	HTREEITEM selected = tree.GetSelectedItem();

	int image, selected_image;
	tree.GetItemImage(selected, image, selected_image);

	// get cursor position to check what we're double clicking
	POINT cursor;
	GetCursorPos(&cursor);

	tree.ScreenToClient(&cursor);
	
	TVHITTESTINFO hit_test;

	hit_test.pt.x = cursor.x;
	hit_test.pt.y = cursor.y;

	tree.HitTest(&hit_test);

	if(hit_test.flags & TVHT_ONITEMLABEL && image == 2)
		stop_expand = true;

	// layout
	if (image == image_layout)
	{
		CLayout* layout = (CLayout*)tree.GetItemData(selected);
		CApplication* application = layout->application;

		CString title;
		title.Format("%s : %s", application->file_information.name, layout->GetName());

		// check if this layout is already open
		HWND client_window;
		client_window = ::GetWindow(pMainWnd->m_hWndMDIClient, GW_CHILD);

		while (client_window != NULL)
		{
			CChildFrame* window = (CChildFrame*)CWnd::FromHandle(client_window);
			if(window->layout == layout)
			{
				if (window->m_tabs.SelectionGet() != 0)
					window->m_tabs.SelectionSet(0, true, true);

				::BringWindowToTop(client_window);

				return;
			}

			client_window = :: GetWindow(client_window, GW_HWNDNEXT);
		}

		// not open, create
		CChildFrame* child_frame = new CChildFrame(application, layout, layout->event_sheet);
		
		CCreateContext context;
		context.m_pCurrentFrame = child_frame;
		context.m_pNewViewClass = RUNTIME_CLASS(CLayoutEditor);
		context.m_pNewDocTemplate = pDocTemplate;
		context.m_pCurrentDoc = pDocTemplate->CreateNewDocument();
		CRect rc(CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);

		if (child_frame->Create(NULL, title, WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE, rc, pMainWnd, &context) == 0)
			return;

		child_frame->MDIMaximize();

		last_opened = child_frame; // store last opened window for use
	}

	// event sheet
	if (image == image_event_sheet)
	{
		EventSheet* event_sheet = (EventSheet*)tree.GetItemData(selected);
		CApplication* application = event_sheet->application;

		CString title;
		title.Format("%s : %s", application->file_information.name, event_sheet->GetName());

		// check if this layout is already open
		HWND client_window;
		client_window = ::GetWindow(pMainWnd->m_hWndMDIClient, GW_CHILD);

		while (client_window != NULL)
		{
			CChildFrame* window = (CChildFrame*)CWnd::FromHandle(client_window);
			if(window->event_sheet->identifier == event_sheet->identifier)
			{
				if (window->m_tabs.ItemGetCount() != 1)
					if (window->m_tabs.SelectionGet() != 1)
						window->m_tabs.SelectionSet(1, true, true);

				last_opened = window; // store last opened window for use

				::BringWindowToTop(client_window);
				return;
			}

			client_window = :: GetWindow(client_window, GW_HWNDNEXT);
		}

		// have a look at the parent
		CLayout* layout = NULL;
		int parent_image, parent_selected_image;
		tree.GetItemImage(tree.GetParentItem(selected), parent_image, parent_selected_image);

		if (parent_image == image_layout)
			layout = (CLayout*)tree.GetItemData(tree.GetParentItem(selected));

		// see if only one layout uses this event sheet
		else
		{
			POSITION position = application->layouts.GetHeadPosition();
			CLayout* find_layout;
			CLayout* used_layout;

			int used = 0;

			while(position)
			{
				find_layout = application->layouts.GetNext(position);

				if (find_layout->event_sheet->identifier == event_sheet->identifier)
				{
					used_layout = find_layout;
					used++;
				}
			}

			if (used == 1)
				layout = used_layout;
		}

		CChildFrame* child_frame;

		if (!layout)
		{
			// not open, create
			child_frame = new CChildFrame(application, NULL, event_sheet);
			
			CCreateContext context;
			context.m_pCurrentFrame = child_frame;
			context.m_pNewViewClass = RUNTIME_CLASS(EventSheetEditor);
			context.m_pNewDocTemplate = pDocTemplate;
			context.m_pCurrentDoc = pDocTemplate->CreateNewDocument();
			CRect rc(CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);

			if (child_frame->Create(NULL, title, WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE, rc, pMainWnd, &context) == 0)
				return;

			child_frame->m_tabs.ItemRemove(0);

			child_frame->m_tabs.SelectionSet(0, true, true);

			child_frame->MDIMaximize();

			last_opened = child_frame; // store last opened window for use
		}

		else
		{
			child_frame = new CChildFrame(application, layout, layout->event_sheet);
			
			CCreateContext context;
			context.m_pCurrentFrame = child_frame;
			context.m_pNewViewClass = RUNTIME_CLASS(CLayoutEditor);
			context.m_pNewDocTemplate = pDocTemplate;
			context.m_pCurrentDoc = pDocTemplate->CreateNewDocument();
			CRect rc(CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);

			if (child_frame->Create(NULL, title, WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE, rc, pMainWnd, &context) == 0)
				return;

			child_frame->m_tabs.SelectionSet(1, true, true);

			child_frame->MDIMaximize();

			last_opened = child_frame; // store last opened window for use
		}
	}

	// global variable
	else if (image == image_global_variable)
	{
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		CApplication::GlobalVariable* variable = (CApplication::GlobalVariable*)tree.GetItemData(selected);

		CAddVariableDlg dialog;
		CString old = dialog.Name = variable->name;
		dialog.Type = variable->type;
		dialog.Value = variable->value;
		dialog.Caption = "Construct : Edit global variable";

		if (dialog.DoModal() == IDOK)
		{
			if (dialog.Name != old)
			{
				 bool found = false;

				for (list<CApplication::GlobalVariable>::iterator i = application->global_variables.begin(); i != application->global_variables.end(); i++)
					if (i->name == dialog.Name)
						found = true;

				if (found)
				{
					MessageBox(CString("The global variable name '") + dialog.Name + "' already exists.  Please choose a unique name.", "Global variables", MB_OK | MB_ICONEXCLAMATION);
					return;
				}
			}
				
			list<CApplication::GlobalVariable>::iterator g = find(application->global_variables.begin(), application->global_variables.end(), old);

			g->name = dialog.Name;
			g->type = dialog.Type;
			g->value = dialog.Value;

			CString name;
			name.Format("%s (%s)", g->name, variable->value);

			tree.SetItemText(g->tree, name);
		}
	}

	// icon resource
	else if (image == image_icon_resource)
	{
		try
		{
			CImageEditorDlg dialog;
			dialog.m_Icon = true;
			dialog.application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			dialog.EditImage(&((IconResource*)tree.GetItemData(selected))->icon, false);
			dialog.application->ChangeModifiedStatus();
		}

		catch (CD3DError e) 
		{
			MessageBox("Image editor error", e.GetString());
		}

		catch (...)
		{
			MessageBox("Image editor error", "Error opening the image editor.  Please report this bug to Scirra.");
		}
	}

	// menu resource
	else if (image == image_menu_resource)
	{
		CMenuEditorDlg menu_editor;
		menu_editor.m_pMenu = (MenuResource*)tree.GetItemData(selected);
		if(menu_editor.DoModal() == IDOK)
		{
			menu_editor.m_Editor.SaveToResource((MenuResource*)tree.GetItemData(selected));
			((CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected))))->ChangeModifiedStatus();
		}
	}
}

void ProjectBar::OnSingleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// check what it is
	HTREEITEM selected = tree.GetSelectedItem();

	int image, selected_image;
	tree.GetItemImage(selected, image, selected_image);
		
	switch (image)
	{
	// nothing
	case image_stub:
		break;

	// application
	case image_application:

		{
			CApplication* application = (CApplication*)tree.GetItemData(selected);
			g_PropertyBar->Update(NULL, TYPE_APP, NULL, NULL, NULL, application);
		}

		break;

	// layout
	case image_layout:

		{
			CLayout* layout = (CLayout*)tree.GetItemData(selected);
			g_PropertyBar->Update(NULL, TYPE_LAYOUT, NULL, layout, NULL);
		}

		break;

	// file properties
	case image_binary_resource:

		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.file_resources);
		}

		break;

	// font properties
	case image_font_resource:

		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.font_resources);
		}

		break;

	// icon properties
	case image_icon_resource:

		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.icon_resources);
		}

		break;

	// menu properties
	case image_menu_resource:
		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.menu_resources);
		}

		break;

	// sound properties
	case image_sound_resource:
		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.sound_resources);
		}

		break;

	// music properties
	case image_music_resource:
		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.music_resources);
		}

		break;

	// objects
	default:
		{
		}
	}
}

void ProjectBar::OnRightClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	POINT cursor;
	GetCursorPos(&cursor);
	UINT chosen = 0;

	CPoint select = cursor;
	tree.ScreenToClient(&select);
	tree.SelectItem(tree.HitTest(select));

	// check what it is
	HTREEITEM selected = tree.GetSelectedItem();

	int image, selected_image;
	tree.GetItemImage(selected, image, selected_image);

	// create a blank menu
	CMenu menu;
	menu.CreatePopupMenu();
		
	switch (image)
	{
	// stub
	case image_stub:

		// layouts stub
		if (tree.GetItemText(selected).Find("Layouts") != -1) // Find will work on plain text
		{
			menu.AppendMenu(MF_STRING, 1, "Add layout");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add layout
			if (chosen == 1)
				((CApplication*)(tree.GetItemData(tree.GetParentItem(selected))))->AddLayout();
		}

		// event sheets stub
		if (tree.GetItemText(selected).Find("Event sheets") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add event sheet");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add layout
			if (chosen == 1)
			{
				CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));

				EventSheet* sheet = new EventSheet(application);
				sheet->identifier = application->event_sheet_unique_identifier++;

				CString name;
				name.Format("Event sheet %d", sheet->identifier);

				sheet->SetName(application->GetUniqueEventSheetName(name));

				application->event_sheets.push_back(sheet);

				application->ChangeModifiedStatus();

				UpdateApplication(application);
			}
		}

		// object folders stub
		if (tree.GetItemText(selected).Find("Objects") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add object folder");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add object folder
			if (chosen == 1)
			{
				CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));

				CApplication::ObjectFolder folder;
				folder.name = application->GetUniqueObjectFolderName("New folder");

				application->object_folders.push_back(folder);

				application->ChangeModifiedStatus();

				UpdateApplication(application);
				//update object bar
				last_opened->object_bar.Refresh();
			}
		}

		// global variables stub
		if (tree.GetItemText(selected).Find("Global variables") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add global variable");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add global variable
			if (chosen == 1)
			{
				CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));

				CAddVariableDlg dialog;
				dialog.Name.Format("Global %d", application->global_variables.size() + 1);
				dialog.Caption = "Construct : Add global variable";
	
				if (dialog.DoModal() == IDOK)
				{
					bool found = false;

					for (list<CApplication::GlobalVariable>::iterator i = application->global_variables.begin(); i != application->global_variables.end(); i++)
						if (i->name == dialog.Name)
							found = true;
					
					if (found)
						MessageBox(CString("The global variable name '") + dialog.Name + "' already exists.  Please choose a unique name.", "Global variables", MB_OK | MB_ICONEXCLAMATION);
					else
					{
						application->global_variables.push_back(CApplication::GlobalVariable(dialog.Name, dialog.Type, dialog.Value, application->m_varID++));

						application->ChangeModifiedStatus();

						UpdateApplication(application);
					}
				}
			}
		}

		// menus stub
		if (tree.GetItemText(selected).Find("Menus") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add menu");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add menu
			if (chosen == 1)
			{
				CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));

				MenuResource menu_res; // Changed variable name to avoid conflict
				CString name = application->resources.GetUniqueResourceName((list<ApplicationResource>*)&application->resources.menu_resources, "Menu");
				menu_res.SetName(name);

				application->resources.menu_resources.push_back(menu_res);

				application->ChangeModifiedStatus();

				UpdateApplication(application);
			}
		}

		// files stub
		if (tree.GetItemText(selected).Find("Files") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add file(s)");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add file
			if (chosen == 1)
			{
				CFECFileDialog dialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT);
				if (dialog.DoModal() == IDOK)
				{ 
					CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));
				
					POSITION position = dialog.GetStartPosition();

					while (position)
					{
						CString next = dialog.GetNextPathName(position);
						CPath path;
						path.SetPath(next);
	
						BinaryResource resource;
						resource.SetName(path.GetFileName());
						resource.path = next;

						if (application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.file_resources, resource.GetName()))
						{
							CString error;
							error.Format("Resource '%s' already exists; either rename the existing resource, or rename the file you are trying to import.", resource.GetName());

							MessageBox(error, "Cannot add resource");

							continue;
						}

						resource.id = application->resources.resource_id++;

						application->resources.file_resources.push_back(resource);
					}

					application->ChangeModifiedStatus();

					UpdateApplication(application);
				}
			}
		}

		// sounds stub
		if (tree.GetItemText(selected).Find("Sounds") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add sound(s)");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add sound
			if (chosen == 1)
			{
				CFECFileDialog dialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, "Wave files (*.wav)|*.wav|OGG files (*.ogg)|*.ogg||");
				if (dialog.DoModal() == IDOK)
				{ 
					CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));
				
					POSITION position = dialog.GetStartPosition();

					while (position)
					{
						CString next = dialog.GetNextPathName(position);
						CPath path;
						path.SetPath(next);
	
						BinaryResource resource;
						resource.SetName(path.GetFileName());
						resource.path = next;

						if (application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.sound_resources, resource.GetName()))
						{
							CString error;
							error.Format("Resource '%s' already exists; either rename the existing resource, or rename the file you are trying to import.", resource.GetName());

							MessageBox(error, "Cannot add resource");

							continue;
						}

						resource.id = application->resources.resource_id++;

						application->resources.sound_resources.push_back(resource);
					}

					application->ChangeModifiedStatus();

					UpdateApplication(application);
				}
			}
		}

		// music stub
		if (tree.GetItemText(selected).Find("Music") != -1)
		{
			menu.AppendMenu(MF_STRING, 1, "Add music");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			// add music
			if (chosen == 1)
			{
				CFECFileDialog dialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, "MP3 files (*.mp3)|*.mp3|WMA files (*.wma)|*.wma|Wave files (*.wav)|*.wav||");
				if (dialog.DoModal() == IDOK)
				{ 
					CApplication* application = (CApplication*)(tree.GetItemData(tree.GetParentItem(selected)));
				
					POSITION position = dialog.GetStartPosition();

					while (position)
					{
						CString next = dialog.GetNextPathName(position);
						CPath path;
						path.SetPath(next);
	
						BinaryResource resource;
						resource.SetName(path.GetFileName());
						resource.path = next;

						if (application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.music_resources, resource.GetName()))
						{
							CString error;
							error.Format("Resource '%s' already exists; either rename the existing resource, or rename the file you are trying to import.", resource.GetName());

							MessageBox(error, "Cannot add resource");

							continue;
						}

						resource.id = application->resources.resource_id++;

						application->resources.music_resources.push_back(resource);
					}

					application->ChangeModifiedStatus();

					UpdateApplication(application);
				}
			}
		}

		break;

	// application
	case image_application:

		{
			menu.AppendMenu(MF_STRING, 1, "Add layout");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 2, "Manage families");
			menu.AppendMenu(MF_STRING, 3, "View used plugins");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 4, "Close project");
			menu.AppendMenu(MF_STRING, 5, "Preview project");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

			CApplication* application = (CApplication*)tree.GetItemData(selected);

			// add layout
			if (chosen == 1)
				application->AddLayout();

			// family manager
			if (chosen == 2)
			{
				FamilyManagerDialog dialog(application);
				dialog.DoModal();
				application->ChangeModifiedStatus(true);

				pMainWnd->m_PropertiesBar.Refresh();	
			}

			// used plugins
			if (chosen == 3)
			{
				UsedPluginDialog dialog(application);
				dialog.DoModal();
			}

			// close project
			else if (chosen == 4)
			{
				// confirm close
				if (!application->CloseSafely()) return;

				HWND client_window = ::GetWindow(pMainWnd->m_hWndMDIClient, GW_CHILD);
				vector<HWND> destroy;

				while (client_window != NULL)
				{
					CChildFrame* window = (CChildFrame*)CWnd::FromHandle(client_window);

					if(window->application == application)
						destroy.push_back(client_window);
				
					client_window = ::GetWindow(client_window, GW_HWNDNEXT);
				}

				// destroy windows
				for (int i = 0; i < destroy.size(); i++)
					::DestroyWindow(destroy[i]);

				// update display
				pMainWnd->Invalidate();

				client_window = ::GetWindow(pMainWnd->m_hWndMDIClient, GW_CHILD);

				while (client_window != NULL)
				{
					CChildFrame* window = (CChildFrame*)CWnd::FromHandle(client_window);
					window->Invalidate();	
					client_window = ::GetWindow(client_window, GW_HWNDNEXT);
				}

				pMainWnd->tabs.Invalidate();

				tree.DeleteItem(tree.GetSelectedItem());

				// Update bars
				g_PropertyBar->Update(NULL, NULL, NULL, NULL, NULL, NULL);
				pMainWnd->m_LayerBar.m_layerDlg.layout_editor = NULL;
				pMainWnd->m_LayerBar.m_layerDlg.RefreshLayers();
				pMainWnd->animator.UpdateAnimations(NULL, NULL, NULL, 1);
		
				CApplication* locate;
				POSITION position = pMainWnd->m_apps.GetHeadPosition();

				for (int i = 0; i < pMainWnd->m_apps.GetCount(); i++)
				{
					POSITION old_position = position;
					locate = pMainWnd->m_apps.GetNext(position);

					if (locate->m_appID == application->m_appID)
					{
						pMainWnd->m_apps.RemoveAt(old_position);
						delete locate;
						return;
					}
				}
			}

			// preview
			if (chosen == 5)
			{
				CExport preview;
				preview.main_frame = pMainWnd;
				preview.Preview(application);
			}
		}

		break;

	// layouts
	case image_layout:

			menu.AppendMenu(MF_STRING, 1, "Preview layout");
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 2, "Clone layout");
			menu.AppendMenu(MF_STRING, 3, "Remove layout");

			chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

		// preview layout
		if (chosen == 1)
		{
			CLayout* layout = (CLayout*)tree.GetItemData(selected);

			CExport preview;
			preview.main_frame = pMainWnd;
			preview.Preview(layout->application, layout->identifier);
		}

		// clone layout
		if (chosen == 2)
		{
			CLayout* layout = (CLayout*)tree.GetItemData(selected);
			CLayout* new_layout = new CLayout();

			CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
			CArchive save(&sf, CArchive::store);
			layout->Serialize(save);
			save.Close();

			sf.SeekToBegin();

			CArchive load(&sf, CArchive::load);
			new_layout->Serialize(load);
			load.Close();

			long index = layout->application->layouts.GetCount();

			// create an original name
			CString name = layout->GetName();
			int old_index = name.ReverseFind(' ');

			if (old_index > -1)
				name = name.Left(old_index);

			CString add = name + " 2";
			int number = 3;
		
			while (layout->application->LayoutNameExists(add)) 
				add.Format("%s %d", name, number++);
		
			new_layout->application = layout->application;
			new_layout->m_WndName = add;
			new_layout->SetName(add);
			new_layout->identifier = layout->application->m_frameID++;
	
			// event sheet
			EventSheet* event_sheet = new EventSheet(layout->application);

			event_sheet->identifier = layout->application->event_sheet_unique_identifier++,		
			event_sheet->SetName(layout->application->GetUniqueEventSheetName(new_layout->m_WndName));

			layout->application->event_sheets.push_back(event_sheet);

			new_layout->event_sheet = layout->application->event_sheets.back();
			layout->application->layouts.AddTail(new_layout);

			layout->application->ChangeModifiedStatus();

			UpdateApplication(layout->application);
		}

		// remove layout
		if (chosen == 3)
		{
			CLayout* layout = (CLayout*)tree.GetItemData(selected);

			CString title;
			title.Format("%s : %s", layout->application->file_information.name, layout->GetName());

			tree.DeleteItem(selected);

			pMainWnd->CloseChildWindows(layout);

			layout->application->RemoveLayout(layout);
		}

		break;

	// event sheets
	case image_event_sheet:

		menu.AppendMenu(MF_STRING, 1, "Remove event sheet");

		chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

		// remove event sheet
		if (chosen == 1)
		{
			EventSheet* sheet = (EventSheet*)tree.GetItemData(selected);

			sheet->application->RemoveEventSheet(sheet->identifier, true);
		}

		break;

	// object folders
	case image_object_folder:

		menu.AppendMenu(MF_STRING, 1, "Remove object folder");

		chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

		// remove object folder
		if (chosen == 1)
		{
			CString name = tree.GetItemText(selected);

			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

			application->RemoveObjectFolder(name);
		}

		break;

	// global variable
	case image_global_variable:

		menu.AppendMenu(MF_STRING, 1, "Edit global variable");
		menu.AppendMenu(MF_STRING, 2, "Remove global variable");

		chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

		// edit global variable
		if (chosen == 1)
		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

			CApplication::GlobalVariable* variable = (CApplication::GlobalVariable*)tree.GetItemData(selected);

			CAddVariableDlg dialog;
			CString old = dialog.Name = variable->name;
			dialog.Type = variable->type;
			dialog.Value = variable->value;
			dialog.Caption = "Construct : Edit global variable";

			if (dialog.DoModal() == IDOK)
			{
				if (dialog.Name != old)
				{
					bool found = false;

					for (list<CApplication::GlobalVariable>::iterator i = application->global_variables.begin(); i != application->global_variables.end(); i++)
						if (i->name == dialog.Name)
							found = true;

					if (found)
					{
						MessageBox(CString("The global variable name '") + dialog.Name + "' already exists.  Please choose a unique name.", "Global variables", MB_OK | MB_ICONEXCLAMATION);
						return;
					}
				}
					
				list<CApplication::GlobalVariable>::iterator g = find(application->global_variables.begin(), application->global_variables.end(), old);

				g->name = dialog.Name;
				g->type = dialog.Type;
				g->value = dialog.Value;

				CString name;
				name.Format("%s (%s)", g->name, variable->value);

				tree.SetItemText(g->tree, name);
			}
		}

		// delete global variable
		else if (chosen == 2)
		{
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

			CApplication::GlobalVariable* variable = (CApplication::GlobalVariable*)tree.GetItemData(selected);

			if (MessageBox(CString("This will remove all actions, conditions and expressions relating to the global variable.  Are you sure you want to delete the global variable '") + variable->name + "'?", "Delete global bariable", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				list<CApplication::GlobalVariable>::iterator i = find(application->global_variables.begin(), application->global_variables.end(), variable->name);

				EraseGlobalVariableReferences(application, i->identifier);

				tree.DeleteItem(variable->tree);

				application->global_variables.erase(i);
			}

		}

		break;

	// resources
	case image_binary_resource:
	case image_font_resource:
	case image_icon_resource:
	case image_menu_resource:
	case image_music_resource:
	case image_sound_resource:

		menu.AppendMenu(MF_STRING, 1, "Remove");

		if (image == image_music_resource || image == image_sound_resource)
		{
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 2, "Preview");
		}

		chosen = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, this);

		// remove resource
		if (chosen == 1)
		{
			// file
			if (image == image_binary_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				BinaryResource* resource = (BinaryResource*)tree.GetItemData(selected);
				list<BinaryResource>::iterator locate = find(application->resources.file_resources.begin(), application->resources.file_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.file_resources.erase(locate);

				application->ChangeModifiedStatus();
			}

			// font
			if (image == image_font_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				FontResource* resource = (FontResource*)tree.GetItemData(selected);
				list<FontResource>::iterator locate = find(application->resources.font_resources.begin(), application->resources.font_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.font_resources.erase(locate);

				application->ChangeModifiedStatus();
			}

			// icon
			if (image == image_icon_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				IconResource* resource = (IconResource*)tree.GetItemData(selected);

				if (resource->GetName() == "Default")
				{
					MessageBox("This icon cannot be deleted as it is the default icon.", "Cannot remove icon");
					return;
				}

				list<IconResource>::iterator locate = find(application->resources.icon_resources.begin(), application->resources.icon_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.icon_resources.erase(locate);

				application->ChangeModifiedStatus();
			}

			// menu
			if (image == image_menu_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				MenuResource* resource = (MenuResource*)tree.GetItemData(selected);

				if (resource->GetName() == "Default")
				{
					MessageBox("This menu cannot be deleted as it is the default menu.", "Cannot remove menu");
					return;
				}

				list<MenuResource>::iterator locate = find(application->resources.menu_resources.begin(), application->resources.menu_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.menu_resources.erase(locate);

				application->ChangeModifiedStatus();
			}

			// sound
			if (image == image_sound_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				BinaryResource* resource = (BinaryResource*)tree.GetItemData(selected);
				list<BinaryResource>::iterator locate = find(application->resources.sound_resources.begin(), application->resources.sound_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.sound_resources.erase(locate);

				application->ChangeModifiedStatus();
			}

			// music
			if (image == image_music_resource)
			{
				CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));
				BinaryResource* resource = (BinaryResource*)tree.GetItemData(selected);
				list<BinaryResource>::iterator locate = find(application->resources.music_resources.begin(), application->resources.music_resources.end(), *resource);

				tree.DeleteItem(resource->tree);
				application->resources.music_resources.erase(locate);

				application->ChangeModifiedStatus();
			}
		}

		// preview
		if (chosen == 2)
		{
			BinaryResource* resource = (BinaryResource*)tree.GetItemData(selected);

			resource->Preview();
		}

		break;
	}
}

void ProjectBar::OnExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (stop_expand)	
	{
		stop_expand = false;
		*pResult = TRUE;

		return;
	}

	*pResult = 0;
}

void ProjectBar::OnBeginEditLabel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	HTREEITEM selected = pTVDispInfo->item.hItem;

	int image, selected_image;
	tree.GetItemImage(selected, image, selected_image);

	if (image == image_stub || image == image_global_variable)
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void ProjectBar::OnEditLabel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	// don't allow NULL names!
	CString name = pTVDispInfo->item.pszText;

	if (name == "")
	{
		*pResult = 0;
		return;
	}

	HTREEITEM selected = pTVDispInfo->item.hItem;

	int image, selected_image;
	tree.GetItemImage(selected, image, selected_image);

	switch (image)
	{

	case image_stub:
		*pResult = 0;
		return;

	// renaming application
	case image_application:
	{
		CApplication* application = (CApplication*)tree.GetItemData(selected);
		application->ChangeName(name);
		g_PropertyBar->Update(NULL, TYPE_APP, NULL, NULL, NULL, application, NULL, NULL, NULL);
	} break;

	// renaming layout
	case image_layout:
	{
		CLayout* layout = (CLayout*)tree.GetItemData(selected);
		layout->SetName(name);
		g_PropertyBar->Update(NULL, TYPE_LAYOUT, NULL, layout, NULL, layout->application, NULL, NULL, NULL);
	} break;

	// renaming event sheet
	case image_event_sheet:
	{
		EventSheet* sheet = (EventSheet*)tree.GetItemData(selected);

		CApplication* application = sheet->application;

		list<EventSheet*>::iterator find_sheet = application->event_sheets.begin();

		// check event sheets
		for( ; find_sheet != application->event_sheets.end(); find_sheet++)
		{	
			if ((*find_sheet)->GetName() == name)
			{
				*pResult = 0;
				return;
			}
		}

		sheet->SetName(name);
	} break;

	// renaming object folder
	case image_object_folder:
	{
		if (tree.GetItemText(selected) != "Default")
		{
			// get application
			CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

			application->RenameObjectFolder(tree.GetItemText(selected), name);
			//update object bar
			last_opened->object_bar.Refresh();
		}

		else
		{
			*pResult = 0;
			return;
		}
	} break;

	// renaming file resource
	case image_binary_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.file_resources, name))
		{
			((BinaryResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.file_resources);
		}

		else
			*pResult = 0;
	} break;

	// renaming font resource
	case image_font_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.font_resources, name))
		{
			((FontResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.font_resources);
		}

		else
			*pResult = 0;
	} break;

	// renaming icon resource
	case image_icon_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.icon_resources, name))
		{
			if (((IconResource*)tree.GetItemData(selected))->GetName() != "Default")
				((IconResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.icon_resources);
		}

		else
			*pResult = 0;
	} break;

	// renaming menu resource
	case image_menu_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.menu_resources, name))
		{
			if (((MenuResource*)tree.GetItemData(selected))->GetName() != "Default")
				((MenuResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.menu_resources);
		}

		else
			*pResult = 0;
	} break;

	// renaming sound resource
	case image_sound_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.sound_resources, name))
		{
			((BinaryResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.sound_resources);
		}

		else
			*pResult = 0;
	} break;

	// renaming music resource
	case image_music_resource:
	{
		// get application
		CApplication* application = (CApplication*)tree.GetItemData(tree.GetParentItem(tree.GetParentItem(selected)));

		if (!application->resources.ResourceNameExists((list<ApplicationResource>*)&application->resources.music_resources, name))
		{
			((BinaryResource*)tree.GetItemData(selected))->SetName(name);

			g_PropertyBar->Update(NULL, TYPE_RESOURCE, NULL, NULL, NULL, application, NULL, NULL, (ApplicationResource*)tree.GetItemData(selected), (list<ApplicationResource>*)&application->resources.music_resources);
		}

		else
			*pResult = 0;
	} break;

	default:
		// rename object
		CObjType* type = (CObjType*)tree.GetItemData(selected);
		type->SetName(name);
		//update object bar
		last_opened->object_bar.Refresh();
		break;
	}

	*pResult = 1;
}

LRESULT ProjectBar::OnBeginDrag(WPARAM wParam, LPARAM lParam)
{
	// XHTMLTREEMSGDATA and related drag/drop logic removed as it's specific to CXHtmlTree.
	// Standard CTreeCtrl uses TVN_BEGINDRAG notification.
	// This functionality will be lost unless re-implemented with standard MFC drag/drop for CTreeCtrl.
	/*
	XHTMLTREEMSGDATA *pMsg = (XHTMLTREEMSGDATA *) wParam;
	ASSERT(pMsg);

	XHTMLTREEDRAGMSGDATA *pData = (XHTMLTREEDRAGMSGDATA *) lParam;

	if (pMsg && pData)
	{
		// ... original logic ...
	}

	return 1;
	*/
	return 0; // Placeholder
}

LRESULT ProjectBar::OnEndDrag(WPARAM wParam, LPARAM lParam)
{
	// XHTMLTREEMSGDATA and related drag/drop logic removed.
	/*
	XHTMLTREEMSGDATA *pMsg = (XHTMLTREEMSGDATA *) wParam;
	ASSERT(pMsg);

	XHTMLTREEDRAGMSGDATA *pData = (XHTMLTREEDRAGMSGDATA *) lParam;

	LRESULT result = 0;

	if (pMsg)
	{
		if (pData)
		{
			// ... original logic ...
		}
	}

	return result;
	*/
	return 0; // Placeholder
}
