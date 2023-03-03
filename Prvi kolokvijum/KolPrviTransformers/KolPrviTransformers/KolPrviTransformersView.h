
// KolPrviTransformersView.h : interface of the CKolPrviTransformersView class
//

#pragma once
#include"DImage.h"

class CKolPrviTransformersView : public CView
{
protected: // create from serialization only
	CKolPrviTransformersView() noexcept;
	DECLARE_DYNCREATE(CKolPrviTransformersView)

// Attributes
public:
	CKolPrviTransformersDoc* GetDocument() const;
private:
	DImage* body, * arm1, * arm2, * leg1, * leg2, * background;
	float bodyAng, arm1Ang, arm2Ang, leg1Ang, leg2Ang;

// Operations
private:
	void DrawBackground(CDC* pDC, CRect rc);
	void DrawImgTransparent(CDC* pDC, DImage* pImage, float x, float y);
	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawLeg1(CDC* pDC);
	void DrawLeg2(CDC* pDC);
	void DrawBody1(CDC* pDC);
	void DrawTransformer(CDC* pDC);
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
	virtual ~CKolPrviTransformersView();
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

#ifndef _DEBUG  // debug version in KolPrviTransformersView.cpp
inline CKolPrviTransformersDoc* CKolPrviTransformersView::GetDocument() const
   { return reinterpret_cast<CKolPrviTransformersDoc*>(m_pDocument); }
#endif

