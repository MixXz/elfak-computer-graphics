
// KolPrviTransporterView.h : interface of the CKolPrviTransporterView class
//
#include"DImage.h"
#pragma once


class CKolPrviTransporterView : public CView
{
protected: // create from serialization only
	CKolPrviTransporterView() noexcept;
	DECLARE_DYNCREATE(CKolPrviTransporterView)

// Attributes
public:
	CKolPrviTransporterDoc* GetDocument() const;
private:
	DImage* back2, * body, * leg1, * leg2, * leg3;
	float bckX, bckY;
	float tScale, tX, * tAngle;
	int frontLegsIndex, rearLegsIndex;

// Operations
private:
	void DrawScene(CDC* pDC, CRect rect);
	void DrawBackground(CDC* pDC, CRect rect);
	void DrawBody(CDC* pDC);
	void DrawLeg(CDC* pDC, double alpha, double dx, double dy);
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
	virtual ~CKolPrviTransporterView();
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

#ifndef _DEBUG  // debug version in KolPrviTransporterView.cpp
inline CKolPrviTransporterDoc* CKolPrviTransporterView::GetDocument() const
   { return reinterpret_cast<CKolPrviTransporterDoc*>(m_pDocument); }
#endif

