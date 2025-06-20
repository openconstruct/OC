// Dialogs\AngleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AngleDlg.h"
#include "..\resource.h"

// CAngleDlg dialog

IMPLEMENT_DYNAMIC(CAngleDlg, CDialogEx) // Changed base class

CAngleDlg::CAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngleDlg::IDD, pParent) // Changed base class
{

}

CAngleDlg::~CAngleDlg()
{
}

void CAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // Changed base class
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_ANGLES, m_AddAngles);
}

BEGIN_MESSAGE_MAP(CAngleDlg, CDialogEx) // Changed base class
	ON_BN_CLICKED(IDOK, &CAngleDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CAngleDlg::OnBnClickedOk()
{
	CString text;
	m_AddAngles.GetWindowText(text);
	m_Angles.push_back(atof(text));

	CDialogEx::OnOK(); // Changed base class
}

BOOL CAngleDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog(); // Changed base class

	m_OK.SetWindowText(ADD);
	m_Cancel.SetWindowText(CANCEL);

	CString title;
	title.Format("Construct : %s", AB_ADDNEWANGLE);
	SetWindowText(title);

	m_AngleIcons.Create(16, 16, ILC_COLOR24 | ILC_MASK, 3, 3);
	ImageList_Add(m_AngleIcons, LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ANGLEICONS)), NULL);

	m_AddAngles.SetImageList(&m_AngleIcons);
	m_AddAngles.SetWindowText("0");

	for (int i = 0; i < 8; i++)
	{
		COMBOBOXEXITEM insItem;
		insItem.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_SELECTEDIMAGE; 
		insItem.iItem = i;
		insItem.iImage = i;
		insItem.iSelectedImage = i;

		CString Text;
		Text.Format("%d", i * 45);
		insItem.pszText = Text.GetBuffer(0);
		m_AddAngles.InsertItem(&insItem);
	}

	m_AddAngles.SetFocus();

	return TRUE;
}
