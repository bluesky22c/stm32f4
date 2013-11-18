// FontToBinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontToBin.h"
#include "FontToBinDlg.h"

#include "영문_바탕_9_보통.c"
#include "한글_바탕_12_보통.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontToBinDlg dialog

CFontToBinDlg::CFontToBinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontToBinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontToBinDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFontToBinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontToBinDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontToBinDlg, CDialog)
	//{{AFX_MSG_MAP(CFontToBinDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontToBinDlg message handlers

BOOL CFontToBinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFontToBinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFontToBinDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFontToBinDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFontToBinDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	DeleteFile("KS.FNT");
	
	CFile file;
	if(!file.Open("KS.FNT", CFile::modeCreate | CFile::modeWrite))
		return;
	
	int size = sizeof(KS_fontTable);
	
	for(int i=0;i<size;i++){
		file.Write(KS_fontTable+i, 1);
	}	
	
	file.Close();
	
	AfxMessageBox("Create KS.FNT");
	
}

void CFontToBinDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here

	DeleteFile("ASCII.FNT");

	CFile file;
	if(!file.Open("ASCII.FNT", CFile::modeCreate | CFile::modeWrite))
	return;

	int size = sizeof(ASCII_fontTable);

	for(int i=0;i<size;i++){
		file.Write(ASCII_fontTable+i, 1);
	}	

	file.Close();

	AfxMessageBox("Create ASCII.FNT");
}


void CFontToBinDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	CString strSrc, strDec, strTemp, strline;

	GetDlgItem(IDC_EDIT1)->GetWindowText(strSrc);
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);

	unsigned char c;
	int cnvcnt = 0;
	int bitPos = 8;
	byte byData= 0;

	pEdit->SetWindowText("");
	strDec = "   ";

	for(int i=0;i<strSrc.GetLength();i++){
		c = strSrc.GetAt(i);

		if(c=='/')pEdit->ReplaceSel("\r\n");
		if(c!='.' && c!='Q')continue;
		if(c=='Q') byData |= (1 << (bitPos-1)) ;
		strline+= c;

		bitPos--;

		if(bitPos==0){
			bitPos=8;

			strTemp.Format("0x%02x,",byData);
			strDec += strTemp;
			
			byData = 0;

			cnvcnt++;
//			if(cnvcnt==2){ // 16x16
			if(cnvcnt==1){  // 8x16
				cnvcnt=0;
				
				int string_len = pEdit->GetWindowTextLength();
				if(string_len>0)pEdit->SetSel(string_len, string_len);
				strTemp = strDec;
//		strDec.Format("\"%s  // %s\\r\\n\"", strTemp, strline);
				strDec.Format("%s  // %s", strTemp, strline);
				pEdit->ReplaceSel(strDec);
				pEdit->ReplaceSel("\r\n");

				strDec = "   "; 
				strline="";
			}
		}
	}
}
