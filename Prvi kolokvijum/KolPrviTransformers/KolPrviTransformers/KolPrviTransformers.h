
// KolPrviTransformers.h : main header file for the KolPrviTransformers application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKolPrviTransformersApp:
// See KolPrviTransformers.cpp for the implementation of this class
//

class CKolPrviTransformersApp : public CWinApp
{
public:
	CKolPrviTransformersApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKolPrviTransformersApp theApp;
