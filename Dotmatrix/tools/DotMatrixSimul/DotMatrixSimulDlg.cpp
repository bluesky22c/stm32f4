// DotMatrixSimulDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DotMatrixSimul.h"
#include "DotMatrixSimulDlg.h"

#include "Matrix_proc.h"
#include "font_kor.h"
#include "Key_proc_handler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* Private variables ---------------------------------------------------------*/


u8 m_aryTxtIdx = 0;

extern int g_Matrixline[4];
extern int g_xMove[4];

extern u8 g_CursorFlag[4];
extern u8 g_CursorPos;
extern bool g_bCursor2Byte;

extern u8* g_FrameBuffer[4];
extern u8 g_FrameText[4][33];
extern u8 g_FrameColr[4][33];
extern DotEffect_t g_DotEffect[4];

#define SPRINTF sprintf

bool ChkFCS1(u8* str, u32 len, u8* chksum  )
{
	u32 i;
	u8  fcs   = 0;
	
	for(i = 0; i < len; i++)
	{
		fcs += *(str + i);
	}
	
	if(chksum[0] == fcs)
	{
		return TRUE;
	}
	else
	{
		//    CallTrace(TRACE_DRV_ERROR, "UartBuild_Msg: FCS Error %d %x %x \n\r ", len, chksum[0], fcs);
		
		return FALSE;
	}
}

int CharTo2Str(u8 cValue, char* buf)
{
	int ret=1;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case '0': memcpy(buf,"０",2); ret=2; break;
	case '1': memcpy(buf,"１",2); ret=2; break;
	case '2': memcpy(buf,"２",2); ret=2; break;
	case '3': memcpy(buf,"３",2); ret=2; break;
	case '4': memcpy(buf,"４",2); ret=2; break;
	case '5': memcpy(buf,"５",2); ret=2; break;
	case '6': memcpy(buf,"６",2); ret=2; break;
	case '7': memcpy(buf,"７",2); ret=2; break;
	case '8': memcpy(buf,"８",2); ret=2; break;
	case '9': memcpy(buf,"９",2); ret=2; break;
	}
	
	return ret;
}

int IntTo2Str(u8 cValue, char* buf)
{
	int ret=1;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"０",2); ret=2; break;
	case 1: memcpy(buf,"１",2); ret=2; break;
	case 2: memcpy(buf,"２",2); ret=2; break;
	case 3: memcpy(buf,"３",2); ret=2; break;
	case 4: memcpy(buf,"４",2); ret=2; break;
	case 5: memcpy(buf,"５",2); ret=2; break;
	case 6: memcpy(buf,"６",2); ret=2; break;
	case 7: memcpy(buf,"７",2); ret=2; break;
	case 8: memcpy(buf,"８",2); ret=2; break;
	case 9: memcpy(buf,"９",2); ret=2; break;
	}
	
	return ret;
}

int IntToArrow(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	case 1: memcpy(buf,"▶",2); ret=2; break;
	case 2: memcpy(buf,"◀",2); ret=2; break;
	}
	
	return ret;
}

int IntToCircle(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	case 1: memcpy(buf,"●",2); ret=2; break;
	}
	
	return ret;
}

int IntToRect(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	case 1: memcpy(buf,"■",2); ret=2; break;
	}
	
	return ret;
}

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
// CDotMatrixSimulDlg dialog

CDotMatrixSimulDlg::CDotMatrixSimulDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDotMatrixSimulDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDotMatrixSimulDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDotMatrixSimulDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDotMatrixSimulDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_GridControl(pDX, IDC_LEDGRID1, m_LedGrid1);
	DDX_GridControl(pDX, IDC_LEDGRID2, m_LedGrid2);
	DDX_GridControl(pDX, IDC_LEDGRID3, m_LedGrid3);
	DDX_GridControl(pDX, IDC_LEDGRID4, m_LedGrid4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDotMatrixSimulDlg, CDialog)
	//{{AFX_MSG_MAP(CDotMatrixSimulDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SET2, OnButtonSet2)
	ON_BN_CLICKED(IDC_BUTTON_SET3, OnButtonSet3)
	ON_BN_CLICKED(IDC_BUTTON_SET4, OnButtonSet4)
	ON_BN_CLICKED(IDC_BUTTON_SET5, OnButtonSet5)
	ON_BN_CLICKED(IDC_BUTTON_SET6, OnButtonSet6)
	ON_BN_CLICKED(IDC_BUTTON_PAD1, OnButtonPad1)
	ON_BN_CLICKED(IDC_BUTTON_PAD16, OnButtonPad16)
	ON_BN_CLICKED(IDC_BUTTON_PAD17, OnButtonPad17)
	ON_BN_CLICKED(IDC_BUTTON_PAD2, OnButtonPad2)
	ON_BN_CLICKED(IDC_BUTTON_PAD3, OnButtonPad3)
	ON_BN_CLICKED(IDC_BUTTON_PAD4, OnButtonPad4)
	ON_BN_CLICKED(IDC_BUTTON_PAD5, OnButtonPad5)
	ON_BN_CLICKED(IDC_BUTTON_PAD6, OnButtonPad6)
	ON_BN_CLICKED(IDC_BUTTON_PAD7, OnButtonPad7)
	ON_BN_CLICKED(IDC_BUTTON_PAD8, OnButtonPad8)
	ON_BN_CLICKED(IDC_BUTTON_PAD9, OnButtonPad9)
	ON_BN_CLICKED(IDC_BUTTON_PAD10, OnButtonPad10)
	ON_BN_CLICKED(IDC_BUTTON_PAD11, OnButtonPad11)
	ON_BN_CLICKED(IDC_BUTTON_PAD12, OnButtonPad12)
	ON_BN_CLICKED(IDC_BUTTON_PAD13, OnButtonPad13)
	ON_BN_CLICKED(IDC_BUTTON_PAD14, OnButtonPad14)
	ON_BN_CLICKED(IDC_BUTTON_PAD15, OnButtonPad15)
	ON_BN_CLICKED(IDC_BUTTON_SET7, OnButtonSet7)
	ON_BN_CLICKED(IDC_BUTTON_PAD0, OnButtonPad0)
	ON_BN_CLICKED(IDC_BUTTON_PAD18, OnButtonPad18)
	ON_BN_CLICKED(IDC_BUTTON_PAD19, OnButtonPad19)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, OnButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, OnButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DN, OnButtonDn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDotMatrixSimulDlg message handlers

BOOL CDotMatrixSimulDlg::OnInitDialog()
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
	InitGrid();

	m_nCnt[0]=1;
	m_nCnt[1]=987;

	memset(m_nAttack,1, sizeof(m_nAttack));
	MatrixProc_Init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDotMatrixSimulDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDotMatrixSimulDlg::OnPaint() 
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
HCURSOR CDotMatrixSimulDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDotMatrixSimulDlg::InitGrid()
{
    m_pLedGrid[0] = &m_LedGrid1;
    m_pLedGrid[1] = &m_LedGrid2;
    m_pLedGrid[2] = &m_LedGrid3;
    m_pLedGrid[3] = &m_LedGrid4;

	for(int m=0;m<4;m++){

		m_pLedGrid[m]->SetTextBkColor( GetSysColor(COLOR_WINDOW) );
		m_pLedGrid[m]->SetEditable(FALSE);
		m_pLedGrid[m]->EnableDragAndDrop(FALSE);
		m_pLedGrid[m]->SetRowResize(FALSE);
		m_pLedGrid[m]->EnableSelection(TRUE);
		m_pLedGrid[m]->SetColumnResize(TRUE);
		m_pLedGrid[m]->SetListMode(FALSE);
		m_pLedGrid[m]->SetSingleRowSelection(FALSE);	
		m_pLedGrid[m]->SetHeaderSort(FALSE);	 
		m_pLedGrid[m]->EnableTitleTips(FALSE);
		m_pLedGrid[m]->ExpandColumnsToFit(TRUE);
		m_pLedGrid[m]->ExpandRowsToFit(TRUE);	
		
		TRY {
			m_pLedGrid[m]->SetRowCount(16);
			m_pLedGrid[m]->SetColumnCount(DOT_MAX_X);
			m_pLedGrid[m]->SetFixedRowCount(0);
			m_pLedGrid[m]->SetFixedColumnCount(0);	
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
			return;
		}
		END_CATCH

		int i;
		for(i=0;i<m_pLedGrid[m]->GetColumnCount(); i++)
			m_pLedGrid[m]->SetColumnWidth(i,3);	

		for(i=0;i<m_pLedGrid[m]->GetRowCount(); i++)
			m_pLedGrid[m]->SetRowHeight(i,3);
		
	}
	
}

void CDotMatrixSimulDlg::SetDotColorPort(CGridCtrl* pGrid, int line, int x, u8 DataH, u8 DataL)
{
	if( DataH==1)
		pGrid->SetItemBkColour(line*2,x,RGB(255,0,0));
	
	else if( DataH==2)
		pGrid->SetItemBkColour(line*2,x,RGB(0,255,0));
	
	else if( DataH==3)
		pGrid->SetItemBkColour(line*2,x,RGB(255,255,0));
	
	else
		pGrid->SetItemBkColour(line*2,x,RGB(0,0,0));


	if( DataL==1)
		pGrid->SetItemBkColour(line*2+1,x,RGB(255,0,0));
	
	else if( DataL==2)
		pGrid->SetItemBkColour(line*2+1,x,RGB(0,255,0));
	
	else if( DataL==3)
		pGrid->SetItemBkColour(line*2+1,x,RGB(255,255,0));
	
	else
		pGrid->SetItemBkColour(line*2+1,x,RGB(0,0,0));
}

void CDotMatrixSimulDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here
	u8 DataH, DataL;
	u16 xbuf=0;
	
	// DotMatrixDisplay	
	for(int m=0;m<4;m++){
		for(int line=0;line<DOT_MAX_Y/2;line++){

			g_Matrixline[m] = line;

			//----
			for(int x=0;x<DOT_MAX_X;x++){

			    xbuf = x+g_xMove[m];

				if(xbuf>=DOT_MAX_X)
					xbuf-=DOT_MAX_X;
			
				DataH = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+0)*DOT_MAX_X+xbuf);
				DataL = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+1)*DOT_MAX_X+xbuf);

				SetDotColorPort(m_pLedGrid[m], g_Matrixline[m], x, DataH, DataL);
			}
            //----

			g_Matrixline[m]++;
			if(g_Matrixline[m]>7)
		    g_Matrixline[m]=0;
		}

		// cursor
		if(g_CursorFlag[m] == 1)
		{ 
			int xStart= g_CursorPos*8;

			if(g_bCursor2Byte)
			  for(int x=0;x<16;x++)m_pLedGrid[m]->SetItemBkColour(15,xStart+x,RGB(255,0,0));
			else
			  for(int x=0;x< 8;x++)m_pLedGrid[m]->SetItemBkColour(15,xStart+x,RGB(255,0,0));
		}

		m_pLedGrid[m]->Refresh();
	}
}

void CDotMatrixSimulDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	for(int m=0;m<4;m++)
	free(g_FrameBuffer[m]);
}

//--------------------------------------------
// 다목적 SendPanelDataToBig2()
// 팀명색상지원(8bytes)
// 48 bytes
//--------------------------------------------
void CDotMatrixSimulDlg::OnButtonSet2() 
{
	// TODO: Add your control notification handler code here

//	m_nCnt[0]++;
     
	m_nAttack[0][0]=!m_nAttack[0][0];

//---- Create Virtual Data
	u8 cl = 0;
	u8 buf[48];
	u16 len = 48, i;
	u8 sum;

	memset(buf, 0, sizeof(buf));

	// Test Data
	buf[0] = 0x02;
    buf[1] = (u8)(len >> 8);
    buf[2] = (u8)(len);
    buf[3] = 0x30;
   
    buf[4] = '1';
    buf[5] = '0';
    buf[6] = ':';
    buf[7] = '2';
    buf[8] = '5';

    buf[9]  = (2<<6 | 0<<4);
	buf[10] = 'T';
	buf[11] = 'E';
	buf[12] = 'A';
	buf[13] = 'M';
	buf[14] = 'L';
	buf[15] = 'E';
	buf[16] = 'F';
	buf[17] = 'T';

    buf[18] = (3<<6 | 0<<4);
	buf[19] = 'T';
	buf[20] = 'E';
	buf[21] = 'A';
	buf[22] = 'M';
	buf[23] = 'R';
	buf[24] = 'I';
	buf[25] = 'G';
	buf[26] = 'H';
    // SET
	buf[27] = 'S';
	buf[28] = 'T';

	// 왼쪽점수
	buf[29] = (u8)(m_nCnt[0]>>8);
	buf[30] = (u8)(m_nCnt[0]);
	// 오른쪽점수
	buf[31] = (u8)(m_nCnt[1]>>8);
	buf[32] = (u8)(m_nCnt[1]);
    // 왼쪽화살표
	buf[33] = 1;
    // 오른쪽화살표
	buf[34] = 1;

    // 왼쪽동그라미
	buf[35] = m_nAttack[0][0];
	buf[36] = m_nAttack[0][1];
	buf[37] = m_nAttack[0][2];

    // 오른쪽동그라미
	buf[38] = m_nAttack[1][0];
	buf[39] = m_nAttack[1][1];
	buf[40] = m_nAttack[1][2];

	// 왼쪽팀파울
	buf[41] = 5;
	// 선수번호
	buf[42] = 6;
	// 선수파울
	buf[43] = 7;
	// 오른쪽팀파울
	buf[44] = 8;

	sum = 0;
	for( i=0; i<len-2; i++ ) sum += buf[i];

	buf[45] = m_nAttack[0][0]; // buzzer
	buf[46] = sum;
	buf[47] = 0x03;
//----------------------------
//    for( i=0;i<48;i++)
//		UartBuild_Msg(buf[i]);

	u8 temp[56];

	memcpy(temp+5,buf,sizeof(buf));
    for( i=0;i<sizeof(temp);i++)
	UartBuild_Msg(temp[i]);

//----------------------------	
// HostRecv_48Team4
//----------------------------
	u8 txt[2][16*2+1]; 
	u8 clr[2][16*2+1];

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED

	// time  hh:mm
	CharTo2Str(buf[4], (char*)(txt[0]+4*2) );
	CharTo2Str(buf[5], (char*)(txt[0]+5*2) );
	*(txt[0]+6*2) =buf[6];
	CharTo2Str(buf[7], (char*)(txt[0]+6*2+1) );
	CharTo2Str(buf[8], (char*)(txt[0]+7*2+1) );
	
	// left team Name
	memset(clr[0],*(buf+9),8);
	memcpy(txt[0],buf+10,8);
	
	// right team Name
	memset(clr[0]+8*2+1,*(buf+18),8);
	memcpy(txt[0]+8*2+1,buf+19,8);	
		
	// set
	memcpy(txt[1]+10*2,buf+27,2);		

	char tmp[4];
    // left count
	SPRINTF(tmp,"%03d", (buf[29]<<8) + (u8)(buf[30]) );
	CharTo2Str(tmp[0], (char*)(txt[0]+13*2+1) );
	CharTo2Str(tmp[1], (char*)(txt[0]+14*2+1) );
	CharTo2Str(tmp[2], (char*)(txt[0]+15*2+1) );

    // right count
	SPRINTF(tmp,"%03d", (buf[31]<<8) + (u8)(buf[32]) );
	CharTo2Str(tmp[0], (char*)(txt[1]+0*2) );
	CharTo2Str(tmp[1], (char*)(txt[1]+1*2) );
	CharTo2Str(tmp[2], (char*)(txt[1]+2*2) );

	// left arrow
	IntToArrow(buf[33], (char*)(txt[1]+7*2) );
	// right arrow
	if(buf[34]==1)
	IntToArrow(2, (char*)(txt[0]+12*2+1) );
	else
	IntToArrow(0, (char*)(txt[0]+12*2+1) );

	// circle
	IntToCircle(buf[35], (char*)(txt[1]+3*2) );
	IntToCircle(buf[36], (char*)(txt[1]+4*2) );
	IntToCircle(buf[37], (char*)(txt[1]+5*2) );
	IntToCircle(buf[38], (char*)(txt[1]+12*2) );
	IntToCircle(buf[39], (char*)(txt[1]+13*2) );
	IntToCircle(buf[40], (char*)(txt[1]+14*2) );

	// left team foul
	IntTo2Str(buf[41], (char*)(txt[1]+6*2) );
	// player no 
	SPRINTF(tmp,"%02d", buf[42]);
	CharTo2Str(tmp[0], (char*)(txt[1]+8*2) );
	CharTo2Str(tmp[1], (char*)(txt[1]+9*2) );
	// player foul 
	IntTo2Str(buf[43], (char*)(txt[1]+11*2) );
	// right team foul 
	IntTo2Str(buf[44], (char*)(txt[1]+15*2) );

	//g_Buzzer = buf[45];

	MatrixConvKssm(txt[0], 33);
	MatrixConvKssm(txt[1], 33);

	MatrixSetFrame(0, txt[0], clr[0],33);				
	MatrixSetFrame(1, txt[1], clr[1],32);	
	
//----------------------------	

	OnButtonSet();
}

//--------------------------------------------
// 다목적 SendCharPanelData()
// 46 bytes
//--------------------------------------------
void CDotMatrixSimulDlg::OnButtonSet3() 
{
	// TODO: Add your control notification handler code here

//---- Create Virtual Data
	u8 cl = 0;
	u8 buf[46];
	u16 len = 46, i;
	u8 sum;

	// Test Data
	buf[0] = 0x02;
    buf[1] = (u8)(len >> 8);
    buf[2] = (u8)(len);
    buf[3] = 0x30;
   
//	SPRINTF( (char*)(buf+4), "←선수교체선수교체→");
	SPRINTF( (char*)(buf+4), "０선수교체선수교체→");
    
	for( i=0;i<20;i++){
		buf[i+24] = (1 << 6) | (0 << 4);
	}

	sum = 0;
	for( i=0; i<len-2; i++ ) sum += buf[i];

	buf[44] = sum;
	buf[45] = 0x03;
//----------------------------
	
//----------------------------	
// HostRecv_46Char20
//----------------------------
	u8 txt[1][16*2]; 
	u8 clr[1][16*2];

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED

	memcpy(txt[0]+12, buf+ 4, 20);
	memcpy(clr[0]+12, buf+24, 20);

	MatrixConvKssm(txt[0], 32);
	MatrixSetFrame(2, txt[0], clr[0],32);		
//----------------------------	

	OnButtonSet();
}

#define PACKET_MAX_LEN            1700
#define STX 0x02
#define ETX 0x03

u8       g_uartBufStr[PACKET_MAX_LEN];

void CDotMatrixSimulDlg::UartBuild_Msg(u8 rch)
{
	static u32  buf_index = 0;
	static u32  fcs_index = 0;
	static u32  stx_index = 0;
	static u32  etx_index = 0;
	static bool etx_flag  = FALSE;
    bool bChk = FALSE;
	u32 len = 48;
  
	static u8   fcs_str[2];	
	u8*         rcv_str   = NULL;
	
	if(buf_index>=PACKET_MAX_LEN)
		buf_index=0;
	
	switch(rch)
	{
	case STX:
		g_uartBufStr[buf_index] = rch;
		buf_index++;
		
		etx_flag  = FALSE;  /* etx flag */
		fcs_index = 0;      /* frame check sum index */
		break;
		
	case ETX:
		if(buf_index >= len-1 ){
			etx_index = buf_index;
			stx_index = buf_index-(len-1);
		}

		fcs_index = buf_index-1;      /* frame check sum index */
		fcs_str[0] = g_uartBufStr[fcs_index];
		
		g_uartBufStr[buf_index] = rch;
		buf_index++;	

    if( g_uartBufStr[stx_index+3]==0x30 && len==48 )  // for total panel case exist Buzzer flag
    bChk = ChkFCS1(g_uartBufStr+stx_index, len-2, fcs_str);
    else
	bChk = ChkFCS1(g_uartBufStr+stx_index, len-1, fcs_str);

    if(bChk)
	{	
		//	rcv_str = Malloc(&g_SystemHeap,buf_index);
		//	memcpy( rcv_str, g_uartBufStr, buf_index);
			
		//	HostComProc_QPut(rcv_str);
			TRACE("Packet Complet\n");

			etx_flag  = TRUE;
			buf_index = 0;	  /* buffer index */
		}
		else
		{
			//  CallTrace(TRACE_DRV_ERROR, "UartBuild_Msg: FCS Error \n\r ");
		}
		fcs_index = 0;
		
		break;
		
	default:
		if( etx_flag == FALSE )
		{
			g_uartBufStr[buf_index] = rch;
			buf_index++;
		}
		
		break;
	}
	
}

//--------------------------------------------
// GPS
// $GPGGA,120524.291,3712.1665,N,12703.2576,E,0,00,50.0,123.22,M,0.00,M,,*50
//--------------------------------------------
void CDotMatrixSimulDlg::OnButtonSet4() 
{
	u8 buf[11];
	sprintf((char*)buf, "120524.291");

	//--------------------------------
	// hh:mm
	u8 text[16*2+1];
	u8 colr[16*2+1];
	
	u8 temp[5]; temp[2]=0;
	u8 HH, mm;
	
	memset(text, 0x20, sizeof(text) );
	memset(colr, (1<<6|0<<4), sizeof(colr) );

	MatrixSetFrame(1, text,colr,33);
	
	memcpy(temp,buf,2); 
	HH = atoi((char*)temp)+9; // korea +9
	memcpy(temp,buf+2,2); 
	mm = atoi((char*)temp);
	
	SPRINTF((char*)temp,"%02d:%02d",HH,mm);
	CharTo2Str(temp[0], (char*)(text+4*2) );
	CharTo2Str(temp[1], (char*)(text+5*2) );	
	text[6*2]=temp[2];
	CharTo2Str(temp[3], (char*)(text+6*2+1) );	
	CharTo2Str(temp[4], (char*)(text+7*2+1) );	
		
	MatrixSetFrame(0, text,colr,33);
	//--------------------------------

	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonSet5() 
{
	g_DotEffect[0].inType=1;

	MatrixSetEffect(0); OnButtonSet();

//	SetTimer(TIMER_MATRIX_SET_EFFECT, TIMER_MATRIX_SET_EFFECT_TIME, NULL);
}

void CDotMatrixSimulDlg::OnTimer(UINT nIDEvent) 
{	
	switch(nIDEvent){
	case TIMER_MATRIX_SET_EFFECT:
		MatrixSetEffect(0); OnButtonSet();
//		MatrixSetEffect(1); m_LedGrid[1].Refresh();
//		MatrixSetEffect(2); m_LedGrid[2].Refresh();
//		MatrixSetEffect(3); m_LedGrid[3].Refresh();
		break;
	}
	
	CDotMatrixSimulDlg::OnTimer(nIDEvent);
}

void CDotMatrixSimulDlg::OnButtonSet6() 
{
    unsigned char data;
    int x,y;

	int nIndex=0;

	nIndex =GetDlgItemInt(IDC_EDIT_INDEX,FALSE,FALSE);

/*
	// English
	unsigned char E_font[] ={0x00,0x00,0x40,0x40,0x40,0xC0,0x80,0x00,0x00,0x06,0x0F,0x09,0x09,0x0F,0x0F,0x00};

	for(x=0;x<8;x++){
		data = E_font[x];

		for(y=0;y<8;y++){     
          if(data & 0x01) m_pLedGrid[0]->SetItemBkColour(y,x,RGB(255,0,0) );

		  data = data >> 1;
		}
	}

	for(x=0;x<8;x++){
		data = E_font[x+8];
		
		for(y=0;y<8;y++){     
			if(data & 0x01) m_pLedGrid[0]->SetItemBkColour(y+8,x,RGB(255,0,0) );
			
			data = data >> 1;
		}
	}

    m_pLedGrid[0]->Refresh();
	return;
*/
	
   // KSSM
	int cnt=0;
   unsigned char H_font[32];
   memcpy(H_font, K_font1[nIndex], 32); 	

	for(x=0;x<16;x++){
		data = H_font[x];
		
		for(y=0;y<8;y++){     
			if(data & 0x01) m_pLedGrid[0]->SetItemBkColour(y,x,RGB(255,0,0) );
			else            m_pLedGrid[0]->SetItemBkColour(y,x,RGB(255,255,255) );

			data = data >> 1;
		}
	}
	
	for(x=0;x<16;x++){
		data = H_font[x+16];
		
		for(y=0;y<8;y++){     
			if(data & 0x01) m_pLedGrid[0]->SetItemBkColour(y+8,x,RGB(255,0,0) );
			else            m_pLedGrid[0]->SetItemBkColour(y+8,x,RGB(255,255,255) );
			
			data = data >> 1;
		}
	}
	
    m_pLedGrid[0]->Refresh();
	return;
}

// 초성표시
void CDotMatrixSimulDlg::OnButtonSet7() 
{
	// TODO: Add your control notification handler code here
    u16 code;

	u8 cho=0;
	u8 joong=0;
	u8 jong=0;
	
	cho   = GetDlgItemInt(IDC_EDIT_CHO,FALSE,FALSE);
	joong = GetDlgItemInt(IDC_EDIT_JOONG,FALSE,FALSE);
	jong  = GetDlgItemInt(IDC_EDIT_JONG,FALSE,FALSE);
	
	code = GetKSSMCode(cho,joong,jong);

	u8 text[16*2+1];
	u8 colr[16*2+1];
	
	memset(text, 0x20, sizeof(text) );
	memset(colr, (1<<6|0<<4), sizeof(colr) );
	
	text[0] = (u8)(code>>8);
	text[1] = (u8)code;

	MatrixSetFrame(1, text,colr,32);
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad16() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_DEL);  // 지움
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad17() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_SPACE);	// 공백
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad0() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM0);	// 	
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad1() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM1);	// 
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad2() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM2);	// 	
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad3() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM3);	// 	
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad4() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM4);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad5() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM5);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad6() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM6);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad7() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM7);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad8() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM8);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad9() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_NUM9);	// 		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad10() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_STAR);	// 	*		
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad12() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_HESI);	// 	#			
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonPad11() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_STATE_HAN);	// 				
	OnButtonSet();
}

void CDotMatrixSimulDlg::OnButtonPad13() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_STATE_ENG);	// 				
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonPad14() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_STATE_eng);	// 				
	OnButtonSet();		
}

void CDotMatrixSimulDlg::OnButtonPad15() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_STATE_NUM);	// 				
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonPad18() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_POWER);	// 				
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonPad19() 
{
	// TODO: Add your control notification handler code here
	u8 text[16*2+1];
    u8 colr[16*2+1];

	memset(colr, 2<<6, sizeof(colr));

	SPRINTF((char*)text, "                 No [%02d] ", m_aryTxtIdx);

	for(u8 i=24;i<32;i++){
		text[i] = g_FrameText[0][i];
		colr[i] = g_FrameColr[0][i];
    }

	MatrixSetFrame(0, text,colr,32);

//	KeyProc_Key_Handler(KEY_MENU);	// 				
	OnButtonSet();		
}

void CDotMatrixSimulDlg::OnButtonLeft() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_LEFT);	// 				
	OnButtonSet();		
}

void CDotMatrixSimulDlg::OnButtonRight() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_RIGHT);	// 				
	OnButtonSet();		
}

void CDotMatrixSimulDlg::OnButtonUp() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_UP);	// 				
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonDn() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_DOWN);	// 				
	OnButtonSet();	
}

void CDotMatrixSimulDlg::OnButtonColor() 
{
	// TODO: Add your control notification handler code here
	KeyProc_Key_Handler(KEY_COLOR);	// 				
	OnButtonSet();		
}

