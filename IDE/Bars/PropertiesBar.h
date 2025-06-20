#pragma once

#include "PropBar.h" // Assuming this does not bring in Prof-UIS itself, or will be refactored too
#include <afxdockablepane.h> // For CDockablePane
#include <afxpropertygridctrl.h> // For CMFCPropertyGridCtrl (potential replacement, but not used directly yet)
#include <vector>       // For std::vector
#include <map>          // For std::map
#include <list>         // For std::list
// Using directives for STL containers
using std::vector;
using std::map;
using std::list;


#define PROPTYPE_EDIT		0
#define PROPTYPE_STATIC		1
#define PROPTYPE_BUTTON		2
#define PROPTYPE_VALUE		3
#define PROPTYPE_COMBO		4
#define PROPTYPE_FLOAT		5
#define PROPTYPE_COLOR		6
#define PROPTYPE_CHECKBOX	7
#define PROPTYPE_FONT		8
#define PROPTYPE_PERCENT	9
#define PROPTYPE_SLIDER		10
#define PROPTYPE_FRAME		11

// struct HyperLinkClickInfo { // Depends on CExtPropertyItem, CProfStudioPropertyGridCtrl - Commented out
// 	CExtPropertyItem* pItem;
// 	CProfStudioPropertyGridCtrl* pGrid;
// };

// CExtGridCellHyperLinkEx, CExtGridCellCurrencyEx, CExtGridCellNumberEx and their DYNCREATE/IMPLEMENT macros
// are Prof-UIS specific grid cell types. Commented out.
/*
class CExtGridCellHyperLinkEx : public CExtGridCellHyperLink 
{
    DECLARE_DYNCREATE(CExtGridCellHyperLinkEx);
    IMPLEMENT_ExtGridCell_Clone( CExtGridCellHyperLinkEx, CExtGridCellHyperLink );
    CExtGridCellHyperLinkEx(
        CExtGridDataProvider * pDP = NULL
        );
public:

    virtual bool OnHyperLinkOpen(
        __EXT_MFC_SAFE_LPCTSTR strURL,
        e_OpenMode_t eOM = __EOM_OPEN,
        bool bAllowErrorAlert = false
        ) const 
    {
		// Trigger the onchange
		HyperLinkClickInfo* info = (HyperLinkClickInfo*)LParamGet();
		info->pGrid->DoItemChange(info->pItem);
        return true;
    }
};

class CExtGridCellCurrencyEx : public CExtGridCellCurrency
{
    DECLARE_DYNCREATE(CExtGridCellCurrencyEx);
    IMPLEMENT_ExtGridCell_Clone( CExtGridCellCurrencyEx, CExtGridCellCurrency );
    CExtGridCellCurrencyEx(
        CExtGridDataProvider * pDP = NULL
        );
public:
	void TextGet( CExtSafeString & strCopy ) const;
};

class CExtGridCellNumberEx : public CExtGridCellNumber
{
    DECLARE_DYNCREATE(CExtGridCellNumberEx);
    IMPLEMENT_ExtGridCell_Clone( CExtGridCellNumberEx, CExtGridCellNumber );
    CExtGridCellNumberEx(
        CExtGridDataProvider * pDP = NULL
        );
public:
	void TextGet( CExtSafeString & strCopy ) const;
};
*/

/////////////////////////////////////////////////////////////////////////////
// CPropertiesBar window

#include "..\Construct Doc.h"
#include "..\Editors\LayoutEditor.h"

class PropertiesBar : public CDockablePane // Changed base class from CExtControlBar
{
	DECLARE_DYNAMIC(PropertiesBar) // Added DECLARE_DYNAMIC
// Construction
public:
	PropertiesBar();

	// Animation information
	CAnimation* m_pAnimation;
	int			m_iIndex;

	void AddAnimationProperties();
	void AddLayoutProperties(CLayout* layout);
	void AddLayerProperties(CLayer* layer);
	void AddResourceProperties(ApplicationResource* resource, CApplication* application, list<ApplicationResource>* stored);

// Implementation
public:
	virtual ~PropertiesBar();

	void Update(CLayoutEditor* view, long type, CObjList* sel, CLayout* pLayout, CObjMap *objs, CApplication* app = NULL, long expand = 0, CLayer* pLayer = NULL, ApplicationResource* pResource = NULL, list<ApplicationResource>* stored = NULL);
	void Refresh();

	template <typename M, typename C>
	bool SelectionMembersEqual(M C::* member);

	long		m_type;
	CLayoutEditor*	m_view;
	CObjList m_sel;
	CObjMap* m_objects;

	bool multiSelect;

	vector<CObj*> vSelectedObjects;
	vector<CObjType*> vSelectedTypes;
	vector<OINFO*> vSelectedOINFOs;

	map<CString, bool>* curExpandSave;

	// CExtPropertyStore* m_pStore;				// Prof-UIS: Commented out
	// CExtPropertyStore* m_pCurrentStore;			// Prof-UIS: Commented out
	// CProfStudioPropertyGridCtrl* m_pGrid;		// Prof-UIS: Commented out
	CStatic m_GridPlaceholder; // Placeholder for CProfStudioPropertyGridCtrl m_Grid;
	CStatic* m_pGridPlaceholder; // Placeholder for CProfStudioPropertyGridCtrl* m_pGrid;


	// void	SetGridCellIcon(CExtGridCell* pCell, HBITMAP bmpIcon); // Depends on CExtGridCell - Commented out

	 //For multi selections
	// vector<CExtPropertyStore*> m_pMultiStore; // Prof-UIS: Commented out

	CLayer* pLastLayer;

	long        curType;

	CObjType* m_pSelectedObjType; //unsafe, only use for movement properties
	CObj* m_pSelectedObj; //unsafe, only use for movement properties
    
    // Allow usage as a pointer    
	PropertiesBar* operator->() { return this; }

	CApplication*	m_pCurrentApplication;
	CLayout*		m_CurrentLayout;

	// A totally stupid hack for working around Prof-UIS hyper link classes being reset
	// list<HyperLinkClickInfo> hyperLinks; // Depends on HyperLinkClickInfo - Commented out

     // Allow usage as a pointer    
	operator PropertiesBar*() { return this; }

	CFont	m_font;

	// Property grid helpers - All these depend on CExtPropertyItem and CExtGridCell... types
	// They will need to be commented out or their bodies heavily modified.
	// For now, changing return types to void* or basic types and commenting bodies in CPP.
	void*		InsertCategory(CString label, CString desc, void* pRoot = NULL);
	void*		InsertAppItem(CString label, CString desc, void* pRoot = NULL);
	void*		InsertResourceItem(CString label, CString desc, CApplication* application, list<ApplicationResource>* stored, ApplicationResource* resource, void* pRoot = NULL);
	void*		InsertControlItem(CString label, CString desc, int player, void* pRoot = NULL);
	void*		InsertFrameItem(CString label, CString desc, void* pRoot = NULL);
	void*		InsertStdObjectItem(CString label, CString desc, CObj* obj, CObjType* pType, void* pRoot = NULL);
	void*		InsertPrivateVarItem(CString label, CString desc, CObj* obj, CObjType* pType, int valueIndex, void* pRoot = NULL);
	void*		InsertObjectItem(CString label, CString desc, CObj* obj, editInfo* data, CObjType* pType, OINFO* oInfo, void* pRoot = NULL);
	void*		InsertEffectItem(CString label, CString desc, CEffect* _pEffect, list<CEffect>& _effects, CObjType* _pType, CLayer* _pLayer, void* pRoot = NULL);
	void*		InsertBehaviorItem(CString label, CString desc, CBehavior* _pMov, CObjType* _pOwner, CLayout* _level, CLayoutEditor* _frame, void* pRoot = NULL);
	void*		InsertTraitItem(CString label, CString desc, CString _trait, CObjType* _pOwner, void* pRoot = NULL);
	void*		InsertFamilyItem(CString label, CString desc, CObjType* pType, void* pRoot = NULL);
	void*		InsertGlobalItem(CString label, CString desc, void* pRoot = NULL);
	void*		InsertContainerItem(CString label, CString desc, CContainer* pContainer, long oid, void* pRoot = NULL);
	void*		InsertTransitionItem(CString label, CString desc, CTransition* pTransition, void* pRoot = NULL);
	void*		InsertEventSheetItem(CString label, CString desc, EventSheet* pEventSheet, void* pRoot = NULL);
	void*		InsertAnimationItem(CString label, CString desc, CAnimation* pAnim, int frameIndex, void* pRoot = NULL);

	void*				InsertLayerItem(CString label, CString desc, void* pRoot = NULL);
	void*				InsertNeutralItem(CString label, CString desc, void* pRoot = NULL, bool compound = false);
	// The Get... methods returning CExtGridCell...* are problematic. They should return generic CWnd* or be removed.
	// For now, commenting them out as their return types are Prof-UIS specific.
/*
	CExtGridCellDropListComboBox*	GetCombo(CExtPropertyItem* pItem);
	CExtGridCellString*				GetEdit(CExtPropertyItem* pItem);
	CExtGridCellNumber*				GetInteger(CExtPropertyItem* pItem);
	CExtGridCellNumberEx*			GetFloat(CExtPropertyItem* pItem);
	CExtGridCellCurrency*			GetPercent(CExtPropertyItem* pItem);
	CExtGridCellCheckBox*			GetCheckBox(CExtPropertyItem* pItem);
	CExtGridCellColor*				GetColor(CExtPropertyItem* pItem);
	CExtGridCellHyperLinkEx*		GetHyperLink(CExtPropertyItem* pItem);
	CExtGridCellSlider*				GetSlider(CExtPropertyItem* pItem);
	CExtGridCellRadioButton*		GetRadio(CExtPropertyItem* pItem);
	CExtGridCellFont*				GetFont(CExtPropertyItem* pItem);
*/
	void* AddPropertyItemToTree(CPropItem& item, void* pRoot, CString& retStr, PropReturn& Return); // void* for CExtPropertyItem

	void							DoAppProperties();

	// tips
	// CCtrlMessageBar					tips; // Prof-UIS, commented out
	void							ShowTip(std::string const);

	static void Update_OINFO_Properties(editInfo* info);
	// Generated message map functions

	//{{AFX_MSG(CPropertiesBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};