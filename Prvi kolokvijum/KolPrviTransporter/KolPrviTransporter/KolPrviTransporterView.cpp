
// KolPrviTransporterView.cpp : implementation of the CKolPrviTransporterView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KolPrviTransporter.h"
#endif

#include "KolPrviTransporterDoc.h"
#include "KolPrviTransporterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533


// CKolPrviTransporterView

IMPLEMENT_DYNCREATE(CKolPrviTransporterView, CView)

BEGIN_MESSAGE_MAP(CKolPrviTransporterView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CKolPrviTransporterView construction/destruction

CKolPrviTransporterView::CKolPrviTransporterView() noexcept
{
	back2 = new DImage();
	back2->Load(CString("Back2.jpg"));

	body = new DImage();
	body->Load(CString("Body.png"));

	leg1 = new DImage();
	leg1->Load(CString("Leg1.png"));

	leg2 = new DImage();
	leg2->Load(CString("Leg2.png"));

	leg3 = new DImage();
	leg3->Load(CString("Leg3.png"));

	bckX = bckY = 0;
	tScale = 1;
	tX = 0;
	frontLegsIndex = 0;
	rearLegsIndex = 2;
	tAngle = new float[5] {20, 10, 0, -10, -20};
}

CKolPrviTransporterView::~CKolPrviTransporterView()
{
	delete back2;
	delete body;
	delete leg1;
	delete leg2;
	delete leg3;
	delete[] tAngle;
}

BOOL CKolPrviTransporterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKolPrviTransporterView drawing

void CKolPrviTransporterView::DrawScene(CDC* pDC, CRect rect)
{
	DrawBackground(pDC, rect);

	Scale(pDC, tScale, tScale);
	Translate(pDC, tX + 100, 100);
	DrawBody(pDC);

	//zadnje u odnosnu na telo po X su 52, Aks se zeznuo malko s racunicu.
	DrawLeg(pDC, tAngle[rearLegsIndex], 52, 168);
	DrawLeg(pDC, -tAngle[rearLegsIndex], 52, 168);

	//prednje
	DrawLeg(pDC, tAngle[frontLegsIndex], 270, 168);
	DrawLeg(pDC, -tAngle[frontLegsIndex], 270, 168);
}

void CKolPrviTransporterView::DrawBackground(CDC* pDC, CRect rect)
{
	XFORM form;
	pDC->GetWorldTransform(&form);

	Translate(pDC, bckX, bckY);
	back2->Draw(pDC, CRect(0, 0, back2->Width(), back2->Height()), CRect(0, 0, back2->Width(), back2->Height()));

	pDC->SetWorldTransform(&form);
}

void CKolPrviTransporterView::DrawBody(CDC* pDC)
{
	body->DrawTransparent(pDC, CRect(0, 0, body->Width(), body->Height()), CRect(0, 0, body->Width(), body->Height()), RGB(255, 255, 255));
}

void CKolPrviTransporterView::DrawLeg(CDC* pDC, double alpha, double dx, double dy)
{
	XFORM form;
	pDC->GetWorldTransform(&form);

	Translate(pDC, dx, dy);

	Translate(pDC, 28, 29);
	Rotate(pDC, alpha);
	Translate(pDC, -28, -29);
	leg1->DrawTransparent(pDC, CRect(0, 0, leg1->Width(), leg1->Height()), CRect(0, 0, leg1->Width(), leg1->Height()), RGB(255,255,255));
	
	Translate(pDC, 0, leg1->Height() - 50);
	Translate(pDC, 28, 29);
	Rotate(pDC, -alpha);
	Translate(pDC, -28, -29);
	leg2->DrawTransparent(pDC, CRect(0, 0, leg2->Width(), leg2->Height()), CRect(0, 0, leg2->Width(), leg2->Height()), RGB(255, 255, 255));

	Translate(pDC, -35, leg2->Height() - 20);
	leg3->DrawTransparent(pDC, CRect(0, 0, leg3->Width(), leg3->Height()), CRect(0, 0, leg3->Width(), leg3->Height()), RGB(255, 255, 255));

	pDC->SetWorldTransform(&form);
}

void CKolPrviTransporterView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviTransporterView::Scale(CDC* pDC, float sX, float sY, bool isRightMultiply)
{
	XFORM form = { sX, 0, 0, sY, 0, 0 };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviTransporterView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = TO_RADIAN * angle;
	XFORM form = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };

	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviTransporterView::OnDraw(CDC* pDC)
{
	CKolPrviTransporterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect client;
	GetClientRect(&client);

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, client.Width(), client.Height());
	memDC->SelectObject(&bmp);

	XFORM form;
	int oldMode = memDC->SetGraphicsMode(GM_ADVANCED);
	memDC->GetWorldTransform(&form);

	DrawScene(memDC, CRect(0, 0, client.Width(), client.Height()));

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, client.Width(), client.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CKolPrviTransporterView printing

BOOL CKolPrviTransporterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolPrviTransporterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolPrviTransporterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKolPrviTransporterView diagnostics

#ifdef _DEBUG
void CKolPrviTransporterView::AssertValid() const
{
	CView::AssertValid();
}

void CKolPrviTransporterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolPrviTransporterDoc* CKolPrviTransporterView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolPrviTransporterDoc)));
	return (CKolPrviTransporterDoc*)m_pDocument;
}
#endif //_DEBUG


// CKolPrviTransporterView message handlers


BOOL CKolPrviTransporterView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CKolPrviTransporterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRect client;
	GetClientRect(&client);

	float offsetX = back2->Width() - client.Width() - 200;
	float offsetY = back2->Height() - client.Height() - 200;

	if (nChar == 'A' && bckX != 0)
	{
		bckX += 200;
		Invalidate();

	}
	if (nChar == 'D' && abs(bckX) < offsetX)
	{
		bckX -= 200;
		Invalidate();
	}
	if (nChar == 'W' && bckY != 0)
	{
		bckY += 200;
		Invalidate();
	}
	if (nChar == 'S' && abs(bckY) < offsetY)
	{
		bckY -= 200;
		Invalidate();
	}
	if (nChar == VK_UP)
	{
		tScale += 0.1;
		Invalidate();
	}
	if (nChar == VK_DOWN)
	{
		tScale -= 0.1;
		Invalidate();
	}
	if (nChar == VK_RIGHT)
	{
		frontLegsIndex = frontLegsIndex++ == 4 ? 0 : frontLegsIndex++;
		rearLegsIndex = rearLegsIndex++ == 4 ? 0 : rearLegsIndex++;
		tX += 20;
		Invalidate();
	}
	if (nChar == VK_LEFT)
	{
		frontLegsIndex = frontLegsIndex-- == 0 ? 4 : frontLegsIndex--;
		rearLegsIndex = rearLegsIndex-- == 0 ? 4 : rearLegsIndex--;
		tX -= 20;
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
