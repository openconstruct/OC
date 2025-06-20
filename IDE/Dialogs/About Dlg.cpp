
// File includes
#include "StdAfx.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD, nullptr)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PEOPLE, m_People);
	DDX_Control(pDX, IDC_BUILD, m_BuildDate);
	DDX_Control(pDX, IDOK, m_OK);
	// Assuming m_Group is for IDC_GROUPBOX1 or a similar ID if it exists in the RC file
	// If IDC_GROUP is not defined or m_Group is not used, this line can be removed.
	// For now, I'll comment it out as IDC_GROUP is not standard and may not exist.
	// DDX_Control(pDX, IDC_GROUP, m_Group);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	// Get this process filename
	CString filePath;
	char* filePathC = new char[MAX_PATH + 1];
	GetModuleFileName(NULL, filePathC, MAX_PATH);
	filePath = filePathC;
	delete[] filePathC;
	HFILE hFile;
	FILETIME fLastWriteTime;
	SYSTEMTIME sTime;
	OFSTRUCT ofStruct;

	hFile = OpenFile(filePath, &ofStruct, OF_READ);

	GetFileTime((HANDLE)hFile,NULL,NULL,&fLastWriteTime);

	_lclose(hFile);

	FileTimeToSystemTime(&fLastWriteTime,&sTime);

	CTime time(sTime);
	CString timeString = time.Format("rep: %A, %B %d, %Y, %H:%M:%S");
	timeString.Replace("rep", A_BUILDDATE);

	m_BuildDate.SetWindowText(timeString);

	// Name of dialog
	CString dialogTitle;
	dialogTitle.Format("Construct Classic : %s", A_TITLE);
	SetWindowText(dialogTitle);

	// Product version
	CString productVersion;
	productVersion.Format("%s: Construct " CONSTRUCT_VERSION_STRING, A_PRODUCTVERSION);

#ifdef DEBUGBUILD
	productVersion += " (debug)";
#endif

	// Set texts
	SetDlgItemText(IDC_PRODUCTVERSION, productVersion);

	// SubclassChildControls(); // Removed Prof-UIS specific call

	return true;
}