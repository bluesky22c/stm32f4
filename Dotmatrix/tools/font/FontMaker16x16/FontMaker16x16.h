// FontMaker16x16.h : main header file for the FONTMAKER16X16 application
//

#if !defined(AFX_FONTMAKER16X16_H__0E33C083_054D_4944_A05E_292E3B58101B__INCLUDED_)
#define AFX_FONTMAKER16X16_H__0E33C083_054D_4944_A05E_292E3B58101B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFontMaker16x16App:
// See FontMaker16x16.cpp for the implementation of this class
//

class CFontMaker16x16App : public CWinApp
{
public:
	CFontMaker16x16App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontMaker16x16App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFontMaker16x16App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMAKER16X16_H__0E33C083_054D_4944_A05E_292E3B58101B__INCLUDED_)
