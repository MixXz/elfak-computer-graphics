
// KolPrviShuricane.h : main header file for the KolPrviShuricane application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKolPrviShuricaneApp:
// See KolPrviShuricane.cpp for the implementation of this class
//

class CKolPrviShuricaneApp : public CWinApp
{
public:
	CKolPrviShuricaneApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKolPrviShuricaneApp theApp;
