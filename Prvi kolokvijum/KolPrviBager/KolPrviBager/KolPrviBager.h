
// KolPrviBager.h : main header file for the KolPrviBager application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKolPrviBagerApp:
// See KolPrviBager.cpp for the implementation of this class
//

class CKolPrviBagerApp : public CWinApp
{
public:
	CKolPrviBagerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKolPrviBagerApp theApp;
