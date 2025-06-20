#pragma once

#include "..\UI Elements\GroupListCtrl.h" // CGroupListCtrl - Custom, assumed to be CListCtrl compatible or separately refactored
#include "..\Utilities\anchor.h" // CDlgAnchor - Potentially Prof-UIS or other 3rd party
#include "..\Utilities\dlgman.h"   // CDlgMan - Potentially Prof-UIS or other 3rd party
#include <afxdialogex.h> // Required for CDialogEx

class ObjectSelectorDialog : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(ObjectSelectorDialog)

public:

	ObjectSelectorDialog(CApplication* application_, CLayout* layout_, CString previous_, int dll_filter_ = -1, bool show_families_ = true, bool show_attributes_ = false, int exclude_ = -1); 

	enum { IDD = IDD_OBJECTSELECTOR };

	CApplication*	application;
	CLayout*		layout;
	int				dll_filter;
	CString			previous;
	bool			show_families;
	bool			show_attributes;
	int				exclude;

	CImageList		images;    // Already standard
	CGroupListCtrl	objects;   // Custom control, leave as is
	CButton		ok, cancel;    // Changed from CExtButton
	CDlgAnchor		dialog_anchor; // Potentially Prof-UIS or other 3rd party - left for now
	CDlgMan			dialog_manager;   // Potentially Prof-UIS or other 3rd party - left for now

	class SelectedObject
	{
	public:

		SelectedObject() :
			is_trait(false),
			is_family(false)
		{
		}

		bool is_trait;
		bool is_family;

		Family* family;
		CObjType* type;
		CString trait;

	} selected;

protected:

	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};
