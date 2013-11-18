// FontToBin.h : main header file for the FONTTOBIN application
//

#if !defined(AFX_FONTTOBIN_H__799AD364_F597_4E1F_8AAA_5A346D33B2DE__INCLUDED_)
#define AFX_FONTTOBIN_H__799AD364_F597_4E1F_8AAA_5A346D33B2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFontToBinApp:
// See FontToBin.cpp for the implementation of this class
//

class CFontToBinApp : public CWinApp
{
public:
	CFontToBinApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontToBinApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFontToBinApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTTOBIN_H__799AD364_F597_4E1F_8AAA_5A346D33B2DE__INCLUDED_)
