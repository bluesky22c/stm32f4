// DotMatrixSimul.h : main header file for the DOTMATRIXSIMUL application
//

#if !defined(AFX_DOTMATRIXSIMUL_H__16F46ADB_40B7_43B0_B65E_A154A6851BF0__INCLUDED_)
#define AFX_DOTMATRIXSIMUL_H__16F46ADB_40B7_43B0_B65E_A154A6851BF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDotMatrixSimulApp:
// See DotMatrixSimul.cpp for the implementation of this class
//

class CDotMatrixSimulApp : public CWinApp
{
public:
	CDotMatrixSimulApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDotMatrixSimulApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDotMatrixSimulApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOTMATRIXSIMUL_H__16F46ADB_40B7_43B0_B65E_A154A6851BF0__INCLUDED_)
