
// PrviKolBilijarView.h : interface of the CPrviKolBilijarView class
//

#pragma once
#include "DImage.h"

class CPrviKolBilijarView : public CView
{
protected: // create from serialization only
	CPrviKolBilijarView() noexcept;
	DECLARE_DYNCREATE(CPrviKolBilijarView)

// Attributes
public:
	CPrviKolBilijarDoc* GetDocument() const;
private:
	DImage* table, * border;
	float stickAngle, stickY, ballX, ballY;
	bool collision;
// Operations
private:
	void DrawStick(CDC* pDC, int w);
	void DrawBall(CDC* pDC, int w);
	void DrawTable(CDC* pDC, CRect rect);
	void DrawBorder(CDC* pDC, CRect rect, int w);
	void DrawHoles(CDC* pDC, CRect rect, int size);
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
	virtual ~CPrviKolBilijarView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in PrviKolBilijarView.cpp
inline CPrviKolBilijarDoc* CPrviKolBilijarView::GetDocument() const
   { return reinterpret_cast<CPrviKolBilijarDoc*>(m_pDocument); }
#endif

