#if !defined(AFX_CParametersDlg_H__DF4A5448_731B_4787_BF32_0326D849E058__INCLUDED_)
#define AFX_CParametersDlg_H__DF4A5448_731B_4787_BF32_0326D849E058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ParametersDlg.h : header file
#include "..\Utilities\anchor.h"
#include "..\Utilities\dlgman.h"
// #include "..\UI Elements\StackedWndCtrl.h" // CStackedWndCtrl - Major Prof-UIS dependency, will be placeholder CWnd
#include "..\UI Elements\TellTaleButton.h"   // Assuming standard CButton or custom CWnd based
#include "..\UI Elements\JazzUpTellTaleButton.h"// Assuming standard CButton or custom CWnd based
#include "..\Structure\SystemObject.h"
#include "..\UI Elements\ParamScintilla.h"
#include "..\MainFrm.h"
#include "AddVariableDlg.h"
#include "..\Utilities\helper.h"

#include "..\Editors\Scintilla\include\Scintilla.h"

// Application structure for Object map
#include "..\Structure.h"

// Scintilla header
#include "..\Editors\Scintilla\ScintillaWnd.h"

// Other includes
#include "..\Editors\Event Sheet Editor\Event Sheet Editor.h"
#include "..\Structure\CTypeChecker.h"

/////////////////////////////////////////////////////////////////////////////
// CParametersDlg dialog

class CComboItemData
{
public:
	ACESEntry2* theEntry;
	CBehavior* mov;

	int index;
};

#include <vector> // For std::vector
using std::vector; // Make vector usable without std:: prefix

class CParametersDlg : public CDialogEx // Changed from CExtResizableDialog
{
	DECLARE_DYNAMIC(CParametersDlg) // Added DECLARE_DYNAMIC
public:
	vector<CComboItemData> m_combodata;

// Construction
public:
	void ResizeScintilla(CScintillaWnd& scintWin);
	// friend class CStackedWndCtrl; // Commented out due to CStackedWndCtrl being replaced by CWnd

	CLayout* layout;
	CApplication* application;

	int GetType();
	CObjType* GetObjectType();
	CParametersDlg(CWnd* pParent = NULL);   // standard constructor

	CEventEditor* m_pEventEditor;

	CImageList    m_ObjectImages;
	CImageList    m_ObjectImagesLarge;

	vector<HWND> m_scintillaList;

	// CCtrlMessageBar	message_bar; // Prof-UIS, commented out

	void OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);

// Dialog Data
	//{{AFX_DATA(CParametersDlg)
	enum { IDD = IDD_PARAMETERS };
	CComboBox	m_AceCombo; // Changed from CExtComboBox
	CListCtrl	m_Expand;   // Already standard
	//CButton	m_Toolbox; // Was CExtButton, assuming it's a CButton. DDX will confirm.

	// Error checking
	CStringArray m_Errors;
	void SetScintillaColour(CScintillaWnd* pWindow, long Colour);

	CEdit m_Parameter; // Changed from CExtEdit
	CStatic m_Desc;    // Changed from CExtLabel


	TDS_PANE Current;
	CWnd	m_Stack; // Changed from CStackedWndCtrl - MAJOR CHANGE, functionality will break

	// Imagelists
	CImageList imageList; // Already standard

	CScintillaWnd* p_scintWin;

	CObjTypeMap* objectMap;

	CTypeChecker m_TypeChecker;

	CWnd* pWndFocusParam;

	CObjType* m_ID;
	CObjType* m_selfType;		// Object type that 'Self' refers to

	// Current parameter
	int currentParameter;
	CString m_lastDesc;

	CDlgAnchor dlgAnchor;
	CDlgMan dlgMan;   

	// Action/Condition info
	int oid;					// object ID
	int aceID;					// action/condition ID
	int mid;
	CStringArray strings;		// parameters
	vector<int> types;

	// Type checking
	CScintillaWnd* m_pWindow; // Scintilla is 3rd party, not Prof-UIS
	CStatic m_Number;  // Changed from CExtLabel

	// Parameter tooltip
	bool              m_ParameterTooltipIsVisible;
	bool              m_QuotesOpen;
	CParamTooltipCtrl m_Tooltip;

	// ACES Entry
	ACESEntry2* m_pACEEntry;

	// Parameter functions
	void CreateParameter(int i, CWnd* pParent,TDS_PANE& current, int type=-1, CString Default = "");
	bool CheckParameterValid(const CString& in, CString& msg);
	void ChangeParameterType();

	CScintillaWnd*		CreateScintillaParameter(CWnd* pParent, LPCSTR DefaultText, int iID);
	CButton*	CreateColourParameter(CWnd* pParent, int iDefaultColour); // Changed from CExtColorButton*
	CComboBox*		CreateComboBoxParameter(CWnd* pParent, LPCSTR Items = NULL, bool bVariable = false); // Changed from CExtComboBox*

	CHotKeyCtrl*		CreateHotKeyParameter(CWnd* pParent, int KeyID); // Already standard

	// Force close
	bool bForce;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParametersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	// Scintilla specific messages, not Prof-UIS related
	afx_msg void OnChar(NMHDR* pNMHDR, LRESULT* pResult,CScintillaWnd& scintWin);
	afx_msg void OnChange(NMHDR* pNMHDR, LRESULT* pResult,CScintillaWnd& scintWin);
	afx_msg void OnFocusChange(UINT nID);

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CParametersDlg)
	bool OnFinish(); // This is custom, not an override in this form
	virtual BOOL OnInitDialog();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point); // Already standard message
	afx_msg LRESULT OnParamChanged(WPARAM w, LPARAM l);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnToolbox();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditchangeAce();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void ReleaseMFCStuff();
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CParametersDlg_H__DF4A5448_731B_4787_BF32_0326D849E058__INCLUDED_)
