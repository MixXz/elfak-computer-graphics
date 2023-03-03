
// KolPrviTransformersView.cpp : implementation of the CKolPrviTransformersView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KolPrviTransformers.h"
#endif

#include "KolPrviTransformersDoc.h"
#include "KolPrviTransformersView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533

// CKolPrviTransformersView

IMPLEMENT_DYNCREATE(CKolPrviTransformersView, CView)

BEGIN_MESSAGE_MAP(CKolPrviTransformersView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CKolPrviTransformersView construction/destruction

CKolPrviTransformersView::CKolPrviTransformersView() noexcept
{
	body = new DImage();
	body->Load(CString("body1.png"));

	arm1 = new DImage();
	arm1->Load(CString("arm1.png"));

	arm2 = new DImage();
	arm2->Load(CString("arm2.png"));

	leg1 = new DImage();
	leg1->Load(CString("leg1.png"));

	leg2 = new DImage();
	leg2->Load(CString("leg2.png"));

	background = new DImage();
	background->Load(CString("background.jpg"));

	bodyAng = arm1Ang = arm2Ang = leg1Ang = leg2Ang = 0;
}

CKolPrviTransformersView::~CKolPrviTransformersView()
{
	delete body;
	delete arm1;
	delete arm2;
	delete leg1;
	delete leg2;
	delete background;
}

BOOL CKolPrviTransformersView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKolPrviTransformersView drawing

void CKolPrviTransformersView::DrawBackground(CDC* pDC, CRect rc)
{
	background->Draw(pDC, CRect(0, 0, background->Width(), background->Height()), rc);
}

void CKolPrviTransformersView::DrawImgTransparent(CDC* pDC, DImage* pImage, float x, float y)
{
	BYTE* bits = pImage->GetDIBBits();
	COLORREF transparentColor = RGB(bits[2], bits[1], bits[0]);

	pImage->DrawTransparent(pDC, CRect(0, 0, pImage->Width(), pImage->Height()), CRect(-x, -y, pImage->Width() - x, pImage->Height() - y), transparentColor);
}

void CKolPrviTransformersView::DrawArm1(CDC* pDC)
{
	Rotate(pDC, arm1Ang);
	DrawImgTransparent(pDC, arm1, 34, 31);
}

void CKolPrviTransformersView::DrawArm2(CDC* pDC)
{
	Rotate(pDC, arm2Ang);
	DrawImgTransparent(pDC, arm2, 23, 61);
}

void CKolPrviTransformersView::DrawLeg1(CDC* pDC)
{
	Rotate(pDC, leg1Ang);
	DrawImgTransparent(pDC, leg1, 30, 125);
}

void CKolPrviTransformersView::DrawLeg2(CDC* pDC)
{
	Rotate(pDC, leg2Ang);
	DrawImgTransparent(pDC, leg2, 35, 60);
}

void CKolPrviTransformersView::DrawBody1(CDC* pDC)
{
	Rotate(pDC, bodyAng);
	DrawImgTransparent(pDC, body, 26, 133);
}

void CKolPrviTransformersView::DrawTransformer(CDC* pDC)
{
	XFORM form;
	DrawLeg2(pDC);

	DrawLeg1(pDC);

	Translate(pDC, 206, 2);
	DrawBody1(pDC);	

	Translate(pDC, 185, -47);
	DrawArm1(pDC); 

	Translate(pDC, 175, 70);
	DrawArm2(pDC);
}

void CKolPrviTransformersView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviTransformersView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = angle * TO_RADIAN;
	XFORM form = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviTransformersView::OnDraw(CDC* pDC)
{
	CKolPrviTransformersDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;	

	CRect crect;
	GetClientRect(&crect);

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, crect.Width(), crect.Height());
	CBitmap* oldBmp = memDC->SelectObject(&bmp);

	int oldMode = memDC->SetGraphicsMode(GM_ADVANCED);

	XFORM form;
	memDC->GetWorldTransform(&form);

	DrawBackground(memDC, crect);

	Translate(memDC, crect.left + body->Width(), crect.bottom - body->Height());
	DrawTransformer(memDC);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, crect.Width(), crect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->SelectObject(oldBmp);
	memDC->DeleteDC();

	delete memDC;
}


// CKolPrviTransformersView printing

BOOL CKolPrviTransformersView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolPrviTransformersView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolPrviTransformersView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKolPrviTransformersView diagnostics

#ifdef _DEBUG
void CKolPrviTransformersView::AssertValid() const
{
	CView::AssertValid();
}

void CKolPrviTransformersView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolPrviTransformersDoc* CKolPrviTransformersView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolPrviTransformersDoc)));
	return (CKolPrviTransformersDoc*)m_pDocument;
}
#endif //_DEBUG


// CKolPrviTransformersView message handlers


void CKolPrviTransformersView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	//Whole
	case VK_LEFT:
		leg2Ang = leg2Ang + 10 > 120 ? 120 : leg2Ang + 10;
		leg1Ang = leg1Ang - 20 < -220 ? -220 : leg1Ang - 20;
		arm1Ang = arm1Ang - 18 < -270 ? -270 : arm1Ang - 18;
		arm2Ang += 30;
		bodyAng = 10;
		Invalidate();
		break;
	case VK_RIGHT:
		leg2Ang = leg2Ang - 10 < 0 ? 0 : leg2Ang - 10;
		leg1Ang = leg1Ang + 20 > 0 ? 0 : leg1Ang + 20;
		arm1Ang = arm1Ang + 18 > 0 ? 0 : arm1Ang + 18;
		arm2Ang -= 30;
		bodyAng = 0;
		Invalidate();
		break;
	//Body
	case 'W':
		bodyAng += 10;		
		Invalidate();
		break;
	case 'S':
		bodyAng -= 10;
		Invalidate();
		break;
	//Arm1
	case 'Q':
		arm1Ang += 10;
		Invalidate();
		break;
	case 'A':
		arm1Ang -= 10;
		Invalidate();
		break;
	//Arm2
	case 'T':
		arm2Ang += 10;
		Invalidate();
		break;
	case 'G':
		arm2Ang -= 10;
		Invalidate();
		break;
	//Leg1
	case 'E':
		leg1Ang += 10;
		Invalidate();
		break;
	case 'D':
		leg1Ang -= 10;
		Invalidate();
		break;
	//Leg2
	case 'R':
		leg2Ang += 10;
		Invalidate();
		break;
	case 'F':
		leg2Ang -= 10;
		Invalidate();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolPrviTransformersView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
