#pragma once

#include "..\Utilities\anchor.h"
#include "..\Utilities\dlgman.h"
#include "..\UI Elements\grouplistctrl.h" // CGroupListCtrl - Custom, assumed to be CListCtrl compatible or separately refactored
#include "Popup dialogs\ACE Filter Dlg.h"  // CACEFilterDlg - Custom dialog, assumed independent or separately refactored
#include <afxdialogex.h> // Required for CDialogEx
#include <afxcmn.h> // Required for CTabCtrl

/////////////////////////////////////////////////////////////////////////////
// CInsertObject dialog

class InsertObjectDialog : public CDialogEx // Changed from CExtNCW<CExtResizableDialog>
{
	friend class CCategoryTab; // This class is not defined here, might be an issue if it relies on InsertObjectDialog being CExtNCW

public:
	InsertObjectDialog(CApplication&);  

	enum { IDD = IDD_INSERTOBJECTS };

	CButton ok, cancel, exchange, list_objects, filter; // Changed from CExtButton
	CGroupListCtrl objects;  // Assuming CGroupListCtrl is CListCtrl compatible or refactored separately
	CStatic tab_rect;        // Changed from CExtLabel
	CEdit info;              // Changed from CExtEdit
	CEdit name;              // Already CEdit (standard)
	CComboBox folders;         // Already CComboBox (standard)

	bool onclicklisttimeractivated;

	// CCtrlMessageBar	first_run; // Prof-UIS: Functionality will be removed

	class CObjectTab : public CTabCtrl // Changed from CExtTabFlatWnd
	{
		// virtual bool OnTabWndSelectionChange(LONG nOldItemIndex, LONG nNewItemIndex, bool bPreSelectionTest)
		// This exact signature might not match CTabCtrl notifications (usually TCN_SELCHANGE/TCN_SELCHANGING via WM_NOTIFY)
		// This will need adjustment in how it's called or handled.
		// For now, keeping the body to illustrate intent, but it will likely need rework.
		// It's better to handle TCN_SELCHANGE in the parent dialog's message map for the CTabCtrl ID.
		// This override will likely not be called as is.
		/*
		virtual bool OnTabWndSelectionChange(LONG nOldItemIndex, LONG nNewItemIndex, bool bPreSelectionTest)
		{
			// This logic needs to move to the parent dialog's TCN_SELCHANGE handler for the tab control
			bool bRetVal = true; // Assuming selection change is allowed by default
			if( bRetVal )
			{  
				LRESULT End = 0;
				((InsertObjectDialog*)GetParent())->OnSelchangeCategories();
			}
			return bRetVal;
		}
		*/
	}  tabs;

	// Resizable - dlgAnchor and dlgMan are potentially Prof-UIS or other 3rd party. Left for now.
	CDlgAnchor dlgAnchor;
	CDlgMan dlgMan;   

	long m_curSel;
	CxImage resizeIcon;

	// Object map
	CApplication& application;

	// Imagelists
	CImageList m_LargeImages, 
		       small_images;

	void LoadObjects();
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnTimer(UINT_PTR nIDEvent);

	CACEFilterDlg filter_dialog;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeObjectView();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnRClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeCategory();
	afx_msg void OnSelchangeCategories();
	afx_msg void OnExchange();
	afx_msg void OnBnClickedFilter();
	afx_msg void OnSelected(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};