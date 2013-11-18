// FontMaker16x16Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontMaker16x16.h"
#include "FontMaker16x16Dlg.h"

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
// CFontMaker16x16Dlg dialog

CFontMaker16x16Dlg::CFontMaker16x16Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontMaker16x16Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontMaker16x16Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strValue = _T("");
}

void CFontMaker16x16Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontMaker16x16Dlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
}

BEGIN_MESSAGE_MAP(CFontMaker16x16Dlg, CDialog)
	//{{AFX_MSG_MAP(CFontMaker16x16Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELFONT, OnButtonSelfont)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CFontMaker16x16Dlg::OnBnClickedButtonSave)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CFontMaker16x16Dlg::OnBnClickedButtonDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontMaker16x16Dlg message handlers

BOOL CFontMaker16x16Dlg::OnInitDialog()
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
	m_fFont.CreateFont( 12, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
		"굴림");

	m_nFontIdx = 0xb0a1; // 가

	m_pBitmap = new CBitmap();
	(m_pBitmap)->Detach();

	BITMAPINFO bi;
	ZeroMemory( &bi, sizeof(BITMAPINFO) ); 
	bi.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER); 
	bi.bmiHeader.biWidth          = 16; 
	bi.bmiHeader.biHeight         = 16; 
	bi.bmiHeader.biPlanes         = 1; 
	bi.bmiHeader.biBitCount       = 24; 
	bi.bmiHeader.biCompression    = BI_RGB; 
	bi.bmiHeader.biSizeImage      = 16*16*3; 
	bi.bmiHeader.biXPelsPerMeter  = 100; 
	bi.bmiHeader.biYPelsPerMeter  = 100; 
	bi.bmiHeader.biClrUsed        = 0; 
	bi.bmiHeader.biClrImportant   = 0; 

	HBITMAP hBmp;
	LPVOID pBmp = NULL; 

	hBmp = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pBmp, NULL, 0);

	m_pBitmap->Attach(hBmp);

	int aa = 0;
	tes(aa);

	unsigned short matrix[1][16]={0,};

	Draw(0xb0a1, *matrix );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CFontMaker16x16Dlg::tes(int& aa)
{

	aa = 1;
	return 0;
}

void CFontMaker16x16Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CFontMaker16x16Dlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if(m_pBitmap){
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	m_fFont.DeleteObject();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFontMaker16x16Dlg::OnPaint() 
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
		CPaintDC dc(this);

		if(m_pBitmap && m_pBitmap->m_hObject!=NULL){
			HDC memDC;                     //메모리 DC선언
			HBITMAP OldBitmap;           //그림을 가지고 있을 bitmap
			memDC = CreateCompatibleDC(dc); //화면 dc화 호환시킴
			OldBitmap = (HBITMAP)SelectObject(memDC, m_pBitmap->m_hObject);
			BITMAP info;
			m_pBitmap->GetBitmap(&info);

			StretchBlt(dc,150,150,64,64, memDC, 0, 0, info.bmWidth, info.bmHeight,SRCCOPY);

			SelectObject(memDC,(CBitmap *)NULL); //반환				
			DeleteDC(memDC);
		}

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFontMaker16x16Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFontMaker16x16Dlg::OnButtonSelfont() 
{
	// TODO: Add your control notification handler code here
	CFontDialog fontDlg(&m_logFont);

	if(fontDlg.DoModal() == IDOK)
	{
		fontDlg.GetCurrentFont(&m_logFont);
		m_fFont.DeleteObject();
		m_fFont.CreateFontIndirect(&m_logFont);

		unsigned short buf[1][16];
		Draw(0xb0a1, *buf);
	}	
}

void CFontMaker16x16Dlg::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CFileDialog fdlg( false, "*.h", NULL, OFN_HIDEREADONLY , "font Files(*.h|*.h|", NULL );

	if(fdlg.DoModal() != IDOK )
	return;
	
	CString strFileName = fdlg.GetFileName();
	CFile file;;

	if(!file.Open(strFileName,CFile::modeCreate|CFile::modeWrite)){
	    AfxMessageBox("File Open Error");
		return;
	}

	CString str, Text;
	str.Format("//--------------------------------------------------------------\r\n"); file.Write(str,str.GetLength() );
	str.Format("// ksc-5601 2355자"):
	str.Format("// 완성형 한글 데이터 16x16\r\n"); file.Write(str,str.GetLength() );
	str.Format("// by bluesky22c 2011.11.18\r\n"); file.Write(str,str.GetLength() );
    str.Format("// 초성(19자)");
	str.Format("// ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ");
	str.Format("// 중성(21자)");
	str.Format("//ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ");
	str.Format("// 종성(18자)");
	str.Format("// 없음ㄱㄴㄷㄹㄺㄻㅁㅂㅄㅅㅆㅇㅈㅊㅌㅍㅎ");

	str.Format("//--------------------------------------------------------------\r\n"); file.Write(str,str.GetLength() );
	str.Format("\r\n"); file.Write(str,str.GetLength() );
	str.Format("const unsigned char KS_fontTable[]={\r\n"); file.Write(str,str.GetLength() );

	// 2byte number
str.Format(
	" // 0 [０] 0xa3b0 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x0f,0xf0,  // ....QQQQQQQQ....\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x0f,0xf0,  // ....QQQQQQQQ....\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	" 	// 1 [１] 0xa3b1 ------------------------\r\n" 
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x07,0xc0,  // .....QQQQQ......\r\n"
	"   0x1f,0xc0,  // ...QQQQQQQ......\r\n"
	"   0x3b,0xc0,  // ..QQQ.QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x03,0xc0,  // ......QQQQ......\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
"\r\n"
);	file.Write(str,str.GetLength() );

str.Format(
	"	// 2 [２] 0xa3b2 ------------------------\r\n" 
	"	0x00,0x00,	// ................\r\n"
	"	0x00,0x00,	// ................\r\n"
	"	0x0f,0xe0,	// ....QQQQQQQ.....\r\n"
	"	0x1f,0xf0,	// ...QQQQQQQQQ....\r\n"
	"	0x38,0x3c,	// ..QQQ.....QQQQ..\r\n"
	"	0x30,0x1c,	// ..QQ.......QQQ..\r\n"
	"	0x30,0x1c,	// ..QQ.......QQQ..\r\n"
	"	0x00,0x38,	// ..........QQQ...\r\n"
	"	0x00,0x70,	// .........QQQ....\r\n"
	"	0x01,0xc0,	// .......QQQ......\r\n"
	"	0x03,0x80,	// ......QQQ.......\r\n"
	"	0x07,0x00,	// .....QQQ........\r\n"
	"	0x1f,0xfc,	// ...QQQQQQQQQQQ..\r\n"
	"	0x3f,0xfc,	// ..QQQQQQQQQQQQ..\r\n"
	"	0x00,0x00,	// ................\r\n"
	"	0x00,0x00,	// ................\r\n"
"\r\n"
);	file.Write(str,str.GetLength() );


str.Format(
	"	// 3 [３] 0xa3b3 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x0f,0xe0,  // ....QQQQQQQ.....\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x38,0x3c,  // ..QQQ.....QQQQ..\r\n"
	"   0x30,0x1c,  // ..QQ.......QQQ..\r\n"
	"   0x00,0x38,  // ..........QQQ...\r\n"
	"   0x0f,0xf8,  // ....QQQQQQQQQ...\r\n"
	"   0x00,0x1c,  // ...........QQQ..\r\n"
	"   0x00,0x0c,  // ............QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x3f,0xf8,  // ..QQQQQQQQQQQ...\r\n"
	"   0x0f,0xe0,  // ....QQQQQQQ.....\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	"	// 4 [４] 0xa3b4 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x60,  // .........QQ.....\r\n"
	"   0x00,0xe0,  // ........QQQ.....\r\n"
	"   0x01,0xe0,  // .......QQQQ.....\r\n"
	"   0x03,0xe0,  // ......QQQQQ.....\r\n"
	"   0x06,0xe0,  // .....QQ.QQQ.....\r\n"
	"   0x0c,0xe0,  // ....QQ..QQQ.....\r\n"
	"   0x18,0xe0,  // ...QQ...QQQ.....\r\n"
	"   0x30,0xe0,  // ..QQ....QQQ.....\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x00,0xe0,  // ........QQQ.....\r\n"
	"   0x00,0xe0,  // ........QQQ.....\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	"	// 5 [５] 0xa3b5 ------------------------\r\n"
	"	0x00,0x00,	// ................\r\n"
	"	0x00,0x00,	// ................\r\n"
	"	0x3f,0xfc,	// ..QQQQQQQQQQQQ..\r\n"
	"	0x3f,0xfc,	// ..QQQQQQQQQQQQ..\r\n"
	"	0x30,0x00,	// ..QQ............\r\n"
	"	0x30,0x00,	// ..QQ............\r\n"
	"	0x30,0x00,	// ..QQ............\r\n"
	"	0x3f,0xe0,	// ..QQQQQQQQQ.....\r\n"
	"	0x3f,0xf8,	// ..QQQQQQQQQQQ...\r\n"
	"	0x00,0x1c,	// ...........QQQ..\r\n"
	"	0x00,0x0c,	// ............QQ..\r\n"
	"	0x30,0x1c,	// ..QQ.......QQQ..\r\n"
	"	0x3f,0xf8,	// ..QQQQQQQQQQQ...\r\n"
	"	0x1f,0xe0,	// ...QQQQQQQQ.....\r\n"
	"	0x00,0x00,	// ................\r\n"
	"	0x00,0x00,	// ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	"	// 6 [６] 0xa3b6 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x0f,0xe0,  // ....QQQQQQQ.....\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x30,0x18,  // ..QQ.......QQ...\r\n"
	"   0x30,0x00,  // ..QQ............\r\n"
	"   0x3f,0xf0,  // ..QQQQQQQQQQ....\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x30,0x1c,  // ..QQ.......QQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x07,0xe0,  // .....QQQQQ......\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	"	// 7 [７] 0xa3b7 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x00,0x1c,  // ...........QQQ..\r\n"
	"   0x00,0x38,  // ..........QQQ...\r\n"
	"   0x00,0x70,  // .........QQQ....\r\n"
	"   0x00,0xe0,  // ........QQQ.....\r\n"
	"   0x01,0xc0,  // .......QQQ......\r\n"
	"   0x03,0x80,  // ......QQQ.......\r\n"
	"   0x07,0x00,  // .....QQQ........\r\n"
	"   0x07,0x00,  // .....QQQ........\r\n"
	"   0x07,0x00,  // .....QQQ........\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

str.Format(
	"	// 8 [８] 0xa3b8 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x0f,0xf0,  // ....QQQQQQQQ....\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x38,0x3c,  // ..QQQ.....QQQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x3f,0xf8,  // ..QQQQQQQQQQQ...\r\n"
	"   0x1f,0xf0,  // ...QQQQQQQQQ....\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );

	str.Format(
	"	// 9 [９] 0xa3b9 ------------------------\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x0f,0xf0,  // ....QQQQQQQQ....\r\n"
	"   0x1f,0xf8,  // ...QQQQQQQQQQ...\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x30,0x0c,  // ..QQ........QQ..\r\n"
	"   0x38,0x1c,  // ..QQQ......QQQ..\r\n"
	"   0x3f,0xfc,  // ..QQQQQQQQQQQQ..\r\n"
	"   0x1f,0xfc,  // ...QQQQQQQQQQQ..\r\n"
	"   0x00,0x0c,  // ............QQ..\r\n"
	"   0x00,0x0c,  // ............QQ..\r\n"
	"   0x00,0x1c,  // ...........QQQ..\r\n"
	"   0x3f,0xf8,  // ..QQQQQQQQQQQ...\r\n"
	"   0x1f,0xf0,  // ...QQQQQQQQQ....\r\n"
	"   0x00,0x00,  // ................\r\n"
	"   0x00,0x00,  // ................\r\n"
	"\r\n"
	);	file.Write(str,str.GetLength() );
	//-------------


	unsigned short buf[1][16];
	byte high,low;
	int nIdx=0;

	for(m_nFontIdx=0xb0a1; m_nFontIdx< 0xc8ff ;m_nFontIdx++){
		high = (m_nFontIdx>>8)&0xff;
		low  =  m_nFontIdx    &0xff;

		if(low<0xa1)
			continue;

		if(low>0xfe)
			continue;

		Text = Draw(m_nFontIdx, *buf);


		str.Format("// %d [%s] 0x%04x -----------------------------\r\n", nIdx, Text, m_nFontIdx); file.Write(str,str.GetLength() );
		nIdx++;

		for(int y=0;y<16;y++){
			str.Format("     0x%02x,0x%02x,  ", byte(buf[0][y]>>8), byte(buf[0][y]) ); file.Write(str,str.GetLength() );
			str.Format("// "); file.Write(str,str.GetLength() );
			for(int x=15;x>=0;x--){
				if( (buf[0][y])&(1<<x)  ){ str.Format("Q"); file.Write(str,str.GetLength() ); }
				else                     { str.Format("."); file.Write(str,str.GetLength() ); }
			}
			str.Format("\r\n"); file.Write(str,str.GetLength() );
		}
		str.Format("\r\n"); file.Write(str,str.GetLength() );
	}

	str.Format("      0xff\r\n"); file.Write(str,str.GetLength() );
	str.Format("};\r\n"); file.Write(str,str.GetLength() );

	file.Close();
}

CString CFontMaker16x16Dlg::Draw(int nFontIdx, unsigned short* matrix)
{
	//----	
	CClientDC dc( NULL );         // paramater 가 NULL 이면 바탕화면 DC 얻어온다.

	BITMAP info;
	m_pBitmap->GetBitmap(&info);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap *pOldBmp;//, *pBitmap;

	pOldBmp = memDC.SelectObject(m_pBitmap);

	CFont* pOldFont=(CFont*)memDC.SelectObject(&m_fFont);		

	unsigned char tmp,buf[3]; buf[2]=0;
	memcpy(buf, &nFontIdx, 2);
	tmp=buf[0];buf[0]=buf[1];buf[1]=tmp;

	CString Text; Text.Format("%s", buf);
	CRect rc(0,2,16,16);	
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(255,0,0));

	CBrush brush;
	CPen pen;
	brush.CreateSolidBrush( RGB(0,0,0) );
	pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));

	memDC.FillRect(CRect(0,0,16, 16), &brush );
	memDC.SelectObject(&pen);

	memDC.DrawText(Text, rc,DT_CENTER|DT_VCENTER );

	unsigned short value = 0;

	for(int y=0;y<16;y++){ 
		value=0;
		for(int x=0;x<16;x++){
			if(memDC.GetPixel(x,y)!=0)
			value |= (1 << (15-x) );
		}

		*(matrix+y) = value;
	}

	memDC.SelectObject(pOldFont);
	memDC.DeleteDC();
	brush.DeleteObject();
	pen.DeleteObject();

	Invalidate();

	return Text;
}


void CFontMaker16x16Dlg::OnBnClickedButtonDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	char tmp[3];
//	sprintf(tmp, "%s", m_strValue);

	unsigned short buf[1][16];
	Draw( atoi(m_strValue), *buf);
}


