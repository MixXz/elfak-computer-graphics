
// KolPrviShuricaneView.h : interface of the CKolPrviShuricaneView class
//
#include"DImage.h"
#pragma once


class CKolPrviShuricaneView : public CView
{
protected: // create from serialization only
	CKolPrviShuricaneView() noexcept;
	DECLARE_DYNCREATE(CKolPrviShuricaneView)

// Attributes
public:
	CKolPrviShuricaneDoc* GetDocument() const;
	DImage* bark, * sky;
	float shuricaneX, shuricaneAngle;
// Operations
public:
	void DrawBlade(CDC* pDC, int size);
	void DrawStar(CDC* pDC, int size);
	void DrawBackground(CDC* pDC, CRect rect);
	void DrawDrvo(CDC* pDC, CRect rect);
	void WriteText(CDC* pDC, CString text, CString fontName, int fontSize, float x, float y, float angle, COLORREF color, COLORREF shadowColor);
	void Translate(CDC* pDC, float dX, float dY, bool isRightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool isRightMultiply = false);
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CKolPrviShuricaneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in KolPrviShuricaneView.cpp
inline CKolPrviShuricaneDoc* CKolPrviShuricaneView::GetDocument() const
   { return reinterpret_cast<CKolPrviShuricaneDoc*>(m_pDocument); }
#endif

