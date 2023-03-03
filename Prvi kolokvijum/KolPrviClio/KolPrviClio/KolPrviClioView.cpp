
// KolPrviClioView.cpp : implementation of the CKolPrviClioView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KolPrviClio.h"
#endif

#include "KolPrviClioDoc.h"
#include "KolPrviClioView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533
#define TO_DEGREE 57.296
#define PI 3.141592653589793238

// CKolPrviClioView

IMPLEMENT_DYNCREATE(CKolPrviClioView, CView)

BEGIN_MESSAGE_MAP(CKolPrviClioView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CKolPrviClioView construction/destruction

CKolPrviClioView::CKolPrviClioView() noexcept
{
	groundAngle = 10;
	carX = 0;

	wheel = new DImage();
	wheel->Load(CString("Wheel.png"));

	clio = GetEnhMetaFile(CString("clio.emf"));
}

CKolPrviClioView::~CKolPrviClioView()
{
	delete wheel;
	DeleteEnhMetaFile(clio);
}

BOOL CKolPrviClioView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKolPrviClioView drawing

void CKolPrviClioView::DrawGround(CDC* pDC, float angle)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	CPen newPen(PS_SOLID, 3, RGB(128, 32, 32));
	CPen* oldPen = pDC->SelectObject(&newPen);

	CBrush newBrush(HS_FDIAGONAL, RGB(128, 32, 32));
	CBrush* oldBrush = pDC->SelectObject(&newBrush);

	POINT pts[3] = { {clientRect.left, clientRect.bottom},
					 {clientRect.right, clientRect.bottom},
					 {clientRect.right, clientRect.bottom - clientRect.Width() * tan(angle * TO_RADIAN)} };

	COLORREF oldBkColor = pDC->SetBkColor(RGB(220, 255, 192));
	pDC->Polygon(pts, 3);

	pDC->SetBkColor(oldBkColor);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CKolPrviClioView::DrawBackground(CDC* pDC, COLORREF color)
{
	CRect rect;
	GetClientRect(&rect);

	CBrush newBrush(color);
	CBrush* oldBrush = pDC->SelectObject(&newBrush);

	pDC->Rectangle(0, 0, rect.Width(), rect.Height());
	pDC->SelectObject(oldBrush);
}

void CKolPrviClioView::DrawCar(CDC* pDC, int x, int y, int w, int h)
{
	PlayEnhMetaFile(pDC->m_hDC, clio, CRect(x + w/2, y - h/2, x - w/2, y + h/2));
}

void CKolPrviClioView::DrawWheel(CDC* pDC, int x, int y, int r, float angle)
{
	XFORM xFormOld;
	pDC->GetWorldTransform(&xFormOld);

	Translate(pDC, x, y);
	Rotate(pDC, angle);
	wheel->DrawTransparent(pDC, CRect(52, 15, 52 + 376, 15 + 376), CRect(-r, -r, r, r), RGB(255, 255, 255));

	pDC->SetWorldTransform(&xFormOld);
}

void CKolPrviClioView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM xForm = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&xForm, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviClioView::Scale(CDC* pDC, float sX, float sY, bool isRightMultiply)
{
	XFORM xForm = { sX, 0, 0, sY, 0,0 };
	pDC->ModifyWorldTransform(&xForm, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviClioView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = angle * TO_RADIAN;
	XFORM xForm = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };
	pDC->ModifyWorldTransform(&xForm, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviClioView::OnDraw(CDC* pDC)
{
	CKolPrviClioDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect crect;
	GetClientRect(&crect);

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, crect.Width(), crect.Height());
	memDC->SelectObject(&bmp);

	DrawBackground(memDC, RGB(200, 220, 255));

	XFORM form;
	int oldMode = memDC->SetGraphicsMode(GM_ADVANCED);

	DrawGround(memDC, groundAngle);

	int carWidth = 450;
	int carHeight = carWidth / 2.5; 
	int x = carX, y = 0, r = 38;
	float rotAngle = TO_DEGREE * carX / (2 * PI);

	pDC->GetWorldTransform(&form);

	Translate(memDC, carWidth / 2, -carHeight / 2 - r / 2, true);
	Rotate(memDC, -groundAngle, true);
	Translate(memDC, 0, crect.Height(), true);

	DrawCar(memDC, x, y, carWidth, carHeight);

	DrawWheel(memDC, x - 155, y + 70, r, rotAngle);
	DrawWheel(memDC, x + 135, y + 70, r, rotAngle);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, crect.Width(), crect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CKolPrviClioView printing

BOOL CKolPrviClioView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolPrviClioView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolPrviClioView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKolPrviClioView diagnostics

#ifdef _DEBUG
void CKolPrviClioView::AssertValid() const
{
	CView::AssertValid();
}

void CKolPrviClioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolPrviClioDoc* CKolPrviClioView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolPrviClioDoc)));
	return (CKolPrviClioDoc*)m_pDocument;
}
#endif //_DEBUG


// CKolPrviClioView message handlers


void CKolPrviClioView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_UP:
		carX = 0;
		groundAngle += 10;
		groundAngle = groundAngle > 80 ? 80 : groundAngle;
		Invalidate();
		break;
	case VK_DOWN:
		carX = 0;
		groundAngle -= 10;
		groundAngle = groundAngle < -10 ? -10 : groundAngle;
		Invalidate();
		break;
	case VK_RIGHT:
		carX += 10;
		Invalidate();
		break;
	case VK_LEFT:
		carX -= 10;
		Invalidate();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolPrviClioView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
