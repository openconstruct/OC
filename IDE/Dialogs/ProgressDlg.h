#pragma once
#include "afxwin.h"
#include "afxcmn.h"     // For CProgressCtrl
#include "afxwin.h"     // For CStatic (though often included via stdafx or afxdialogex)
#include "..\resource.h"
#include <afxdialogex.h> // Required for CDialogEx


// CProgressDlg dialog

class CProgressDlg : public CDialogEx // Changed base class
{
	DECLARE_DYNAMIC(CProgressDlg)

	BOOL OnInitDialog();

public:
	CProgressDlg(CString status = "", CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

	void SetProgress(int prog);
	void Start(CString status = "");
	void Finish();

// Dialog Data
	enum { IDD = IDD_PROGRESSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Status;       // Changed from CExtLabel
	CProgressCtrl m_Progress; // Already standard
	CString m_StatusText;

	int progress;
};
