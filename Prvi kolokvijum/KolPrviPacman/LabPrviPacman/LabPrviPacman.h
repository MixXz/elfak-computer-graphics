
// LabPrviPacman.h : main header file for the LabPrviPacman application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CLabPrviPacmanApp:
// See LabPrviPacman.cpp for the implementation of this class
//

class CLabPrviPacmanApp : public CWinApp
{
public:
	CLabPrviPacmanApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLabPrviPacmanApp theApp;
