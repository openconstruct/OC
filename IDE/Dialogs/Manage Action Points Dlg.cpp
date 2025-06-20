// Dialogs/ManageActionPointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\Construct.h"
#include "ManageActionPointsDlg.h"
#include "ImageEditorDlg.h"

// CManageActionPointsDlg dialog

IMPLEMENT_DYNAMIC(CManageActionPointsDlg, CDialogEx) // Changed base class

CManageActionPointsDlg::CManageActionPointsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManageActionPointsDlg::IDD, pParent) // Changed base class
{

}

CManageActionPointsDlg::~CManageActionPointsDlg()
{
}

void CManageActionPointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class
	DDX_Control(pDX, IDC_LIST1, m_ActionPointsList);
	DDX_Control(pDX, IDC_EDIT1, m_ActionName_edit);

	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_REMOVE, m_Remove);
	// Note: m_Cancel is declared in header but not in DDX_Control here.
	// If it's a standard IDCANCEL button, CDialog(Ex) handles it by default.
	// If it's a custom button, it would need DDX_Control(pDX, IDCANCEL_OR_CUSTOM_ID, m_Cancel);
}


BEGIN_MESSAGE_MAP(CManageActionPointsDlg, CDialogEx) // Changed base class
	ON_BN_CLICKED(IDC_ADD, &CManageActionPointsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_REMOVE, &CManageActionPointsDlg::OnRemove)
END_MESSAGE_MAP()


// CManageActionPointsDlg message handlers

void CManageActionPointsDlg::OnBnClickedButton1()
{
	// Verify it's not empty
	CString NewActionName; 
	m_ActionName_edit.GetWindowText(NewActionName);
	if (NewActionName == "")
	{
		CErrorDlg Dlg;
		Dlg.Error("Blank name", "Please enter a name for this image point to continue.");
		return;
	}

	CImageEditorDlg* imgDlg = m_pToolSettings->m_pImgEd->m_pImgEdDlg;

	CImageResource* currentImage = imgDlg->m_sourceImages.at(imgDlg->m_animation_frame_index);

	(imgDlg->m_newAction[currentImage])[NewActionName] = imgDlg->m_newHotspots[currentImage];
	
	UpdateList();

	m_ActionName_edit.SetWindowText("");
	m_ActionName_edit.SetFocus();
}

void CManageActionPointsDlg::OnRemove()
{
	// TODO FOR DAVO
	// Verify it's not empty
	CString ActionName; 
	
	m_ActionPointsList.GetText(m_ActionPointsList.GetCurSel(), ActionName);

	CImageEditorDlg* imgDlg = m_pToolSettings->m_pImgEd->m_pImgEdDlg;

	for(map<CImageResource*,  map<CString, CPoint> >::iterator ii = imgDlg->m_newAction.begin();
		ii!= imgDlg->m_newAction.end(); ii++)
	{
		if(ii->second.find(ActionName) != ii->second.end())
		{
			ii->second.erase( ii->second.find(ActionName));
		}
	}

	UpdateList();
}

void CManageActionPointsDlg::UpdateList()
{
	m_ActionPointsList.ResetContent();
	//ADD
	CImageEditorDlg* imgDlg = m_pToolSettings->m_pImgEd->m_pImgEdDlg;

	
	map<CString, int> actions;
	imgDlg->GetActionNames(actions);
	
	for( map<CString, int>::iterator ai = actions.begin(); ai != actions.end(); ai++ )
	{
		m_ActionPointsList.AddString(ai->first);
	}
	// TODO: Add your control notification handler code here

}


BOOL CManageActionPointsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); // Changed base class
	UpdateList();
	// SubclassChildControls(); // Removed Prof-UIS specific call
	return TRUE; // Typically return TRUE if focus not set manually
}
