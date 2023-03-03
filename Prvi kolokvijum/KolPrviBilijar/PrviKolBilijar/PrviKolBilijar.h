
// PrviKolBilijar.h : main header file for the PrviKolBilijar application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPrviKolBilijarApp:
// See PrviKolBilijar.cpp for the implementation of this class
//

class CPrviKolBilijarApp : public CWinApp
{
public:
	CPrviKolBilijarApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPrviKolBilijarApp theApp;
