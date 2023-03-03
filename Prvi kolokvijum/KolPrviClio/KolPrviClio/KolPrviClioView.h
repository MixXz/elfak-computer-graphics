
// KolPrviClioView.h : interface of the CKolPrviClioView class
//

#pragma once
#include"DImage.h"

class CKolPrviClioView : public CView
{
protected: // create from serialization only
	CKolPrviClioView() noexcept;
	DECLARE_DYNCREATE(CKolPrviClioView)

// Attributes
public:
	CKolPrviClioDoc* GetDocument() const;
private:
	float groundAngle, carX;
	DImage* wheel;
	HENHMETAFILE clio;

// Operations
public:
	void DrawGround(CDC* pDC, float angle);
	void DrawBackground(CDC* pDC, COLORREF color);
	void DrawCar(CDC* pDC, int x, int y, int w, int h);
	void DrawWheel(CDC* pDC, int x, int y, int r, float angle);
	void Translate(CDC* pDC, float dX, float dY, bool isRightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool isRightMultiply = false);
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
	virtual ~CKolPrviClioView();
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

#ifndef _DEBUG  // debug version in KolPrviClioView.cpp
inline CKolPrviClioDoc* CKolPrviClioView::GetDocument() const
   { return reinterpret_cast<CKolPrviClioDoc*>(m_pDocument); }
#endif

