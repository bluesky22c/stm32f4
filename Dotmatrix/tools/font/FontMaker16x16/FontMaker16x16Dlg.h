// FontMaker16x16Dlg.h : header file
//

#if !defined(AFX_FONTMAKER16X16DLG_H__E827A23F_C3CD_4C6F_BA8F_00243821E4C7__INCLUDED_)
#define AFX_FONTMAKER16X16DLG_H__E827A23F_C3CD_4C6F_BA8F_00243821E4C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFontMaker16x16Dlg dialog

class CFontMaker16x16Dlg : public CDialog
{
// Construction
public:
	CFontMaker16x16Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFontMaker16x16Dlg)
	enum { IDD = IDD_FONTMAKER16X16_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontMaker16x16Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	LOGFONT m_logFont;
	CFont m_fFont;
	unsigned short m_nFontIdx;
	CBitmap* m_pBitmap;		// Ç¥½Ã¿ë

	// Generated message map functions
	//{{AFX_MSG(CFontMaker16x16Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSelfont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnDestroy();
	CString Draw(int nFontIdx, unsigned short* matrix);
	CString m_strValue;
	afx_msg void OnBnClickedButtonDraw();
	int tes(int& aa);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMAKER16X16DLG_H__E827A23F_C3CD_4C6F_BA8F_00243821E4C7__INCLUDED_)
