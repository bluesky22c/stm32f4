// DotMatrixSimulDlg.h : header file
//

#if !defined(AFX_DOTMATRIXSIMULDLG_H__3F9E9BAF_395E_4D54_A59B_0BB7BA95D02F__INCLUDED_)
#define AFX_DOTMATRIXSIMULDLG_H__3F9E9BAF_395E_4D54_A59B_0BB7BA95D02F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GRIDCTRL\GridCtrl.h"	

#define TIMER_MATRIX_SET_EFFECT 0x01
#define TIMER_MATRIX_SET_EFFECT_TIME 1000
/////////////////////////////////////////////////////////////////////////////
// CDotMatrixSimulDlg dialog

class CDotMatrixSimulDlg : public CDialog
{
// Construction
public:
	void SetDotColorPort(CGridCtrl* pGrid, int line, int x, u8 DataH, u8 DataL);
	int m_nAttack[2][3];
	int m_nCnt[2];
	void UartBuild_Msg(u8 rch);
	void InitGrid();
	CDotMatrixSimulDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDotMatrixSimulDlg)
	enum { IDD = IDD_DOTMATRIXSIMUL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDotMatrixSimulDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	CGridCtrl m_LedGrid1;
	CGridCtrl m_LedGrid2;
	CGridCtrl m_LedGrid3;
	CGridCtrl m_LedGrid4;

	CGridCtrl* m_pLedGrid[4];

	// Generated message map functions
	//{{AFX_MSG(CDotMatrixSimulDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonSet();
	afx_msg void OnDestroy();
	afx_msg void OnButtonSet2();
	afx_msg void OnButtonSet3();
	afx_msg void OnButtonSet4();
	afx_msg void OnButtonSet5();
	afx_msg void OnButtonSet6();
	afx_msg void OnButtonPad1();
	afx_msg void OnButtonPad16();
	afx_msg void OnButtonPad17();
	afx_msg void OnButtonPad2();
	afx_msg void OnButtonPad3();
	afx_msg void OnButtonPad4();
	afx_msg void OnButtonPad5();
	afx_msg void OnButtonPad6();
	afx_msg void OnButtonPad7();
	afx_msg void OnButtonPad8();
	afx_msg void OnButtonPad9();
	afx_msg void OnButtonPad10();
	afx_msg void OnButtonPad11();
	afx_msg void OnButtonPad12();
	afx_msg void OnButtonPad13();
	afx_msg void OnButtonPad14();
	afx_msg void OnButtonPad15();
	afx_msg void OnButtonSet7();
	afx_msg void OnButtonPad0();
	afx_msg void OnButtonPad18();
	afx_msg void OnButtonPad19();
	afx_msg void OnButtonLeft();
	afx_msg void OnButtonRight();
	afx_msg void OnButtonColor();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOTMATRIXSIMULDLG_H__3F9E9BAF_395E_4D54_A59B_0BB7BA95D02F__INCLUDED_)
