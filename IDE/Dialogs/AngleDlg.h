#pragma once
#include "afxwin.h"
#include "..\resource.h"
#include <afxdialogex.h> // Required for CDialogEx
#include <vector>        // For std::vector
using std::vector;       // Make vector usable without std::prefix

// CAngleDlg dialog

class CAngleDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CAngleDlg)

public:
	CAngleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAngleDlg();

	vector<float> m_Angles;

	CButton m_OK, m_Cancel; // Changed from CExtButton
	CComboBoxEx m_AddAngles;  // Already standard

	void OnBnClickedOk();

	CImageList m_AngleIcons;

	BOOL OnInitDialog() ;

// Dialog Data
	enum { IDD = IDD_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
