
// KolPrviBagerView.h : interface of the CKolPrviBagerView class
//

#pragma once
#include"DImage.h"

class CKolPrviBagerView : public CView
{
protected: // create from serialization only
	CKolPrviBagerView() noexcept;
	DECLARE_DYNCREATE(CKolPrviBagerView)

// Attributes
public:
	CKolPrviBagerDoc* GetDocument() const;
private:
	DImage* bager, * arm1, * arm2, * pozadina;
	HENHMETAFILE viljuska;
	float arm1Angle, arm2Angle, forkAngle, excavatorX;

// Operations
private:
	void DrawBackground(CDC* pDC);
	void DrawImageTransparent(CDC* pDC, DImage* pImage, float x, float y);
	void DrawBody(CDC* pDC);
	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawFork(CDC* pDC);
	void DrawExcavator(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool isRightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool isRightMultiply = false);
	void Rotate(CDC* pDC, float ang, bool isRightMultiply = false);

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
	virtual ~CKolPrviBagerView();
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

#ifndef _DEBUG  // debug version in KolPrviBagerView.cpp
inline CKolPrviBagerDoc* CKolPrviBagerView::GetDocument() const
   { return reinterpret_cast<CKolPrviBagerDoc*>(m_pDocument); }
#endif

