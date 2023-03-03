
// LabPrviPacmanView.h : interface of the CLabPrviPacmanView class
//

#pragma once
#include"DImage.h"

class CLabPrviPacmanView : public CView
{
protected: // create from serialization only
	CLabPrviPacmanView() noexcept;
	DECLARE_DYNCREATE(CLabPrviPacmanView)

// Attributes
public:
	CLabPrviPacmanDoc* GetDocument() const;
	DImage* bck;
	float angle, pacmanX, pacmanY;
	bool mouth;
	bool mirrorX, mirrorY;

// Operations
public:
	void DrawBackground(CDC* pDC, CRect window);
	void DrawPacman(CDC* pDC, CRect rect, float angle);
	void DrawGhost(CDC* pDC, CRect rect);
	void DrawGhostEye(CDC* pDC, int r);
	void WriteText(CDC* pDC, char* text, int x, int y, int size, COLORREF color);
	void CheckCollision(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool isRightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool isRightMultiply = false);
	void Mirror(CDC* pDC, bool mX, bool mY, bool isRightMultiply = false);


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
	virtual ~CLabPrviPacmanView();
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

#ifndef _DEBUG  // debug version in LabPrviPacmanView.cpp
inline CLabPrviPacmanDoc* CLabPrviPacmanView::GetDocument() const
   { return reinterpret_cast<CLabPrviPacmanDoc*>(m_pDocument); }
#endif

