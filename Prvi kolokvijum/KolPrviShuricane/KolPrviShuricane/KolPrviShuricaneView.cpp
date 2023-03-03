
// KolPrviShuricaneView.cpp : implementation of the CKolPrviShuricaneView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KolPrviShuricane.h"
#endif

#include "KolPrviShuricaneDoc.h"
#include "KolPrviShuricaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533 

// CKolPrviShuricaneView

IMPLEMENT_DYNCREATE(CKolPrviShuricaneView, CView)

BEGIN_MESSAGE_MAP(CKolPrviShuricaneView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CKolPrviShuricaneView construction/destruction

CKolPrviShuricaneView::CKolPrviShuricaneView() noexcept
{
	bark = new DImage();
	bark->Load(CString("bark.jpg"));

	sky = new DImage();
	sky->Load(CString("sky.jpg"));

	shuricaneX = shuricaneAngle = 0;
}

CKolPrviShuricaneView::~CKolPrviShuricaneView()
{
	delete bark;
	delete sky;
}

BOOL CKolPrviShuricaneView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKolPrviShuricaneView drawing

void CKolPrviShuricaneView::DrawBlade(CDC* pDC, int size)
{
	int h = 4 * size;
	int w = 2 * size;

	XFORM form;
	pDC->GetWorldTransform(&form);

	CPen greyPen(PS_SOLID, 1, RGB(169, 169, 169));
	CPen* oldPen = pDC->SelectObject(&greyPen);

	CBrush darkGreyBrush(RGB(169, 169, 169));
	CBrush* oldBrush = pDC->SelectObject(&darkGreyBrush);

	Translate(pDC, 0, -h);

	CPoint points[3]{ {0, 0}, {0, h}, {w / 2, h - w / 2} };
	pDC->Polygon(points, 3);

	CBrush lightGreyBrush(RGB(211, 211, 211));
	pDC->SelectObject(&lightGreyBrush);

	points[2] = { -w / 2, h - w / 2 };
	pDC->Polygon(points, 3);

	int fontSize = 0.7 * size;
	WriteText(pDC, CString("Shuricane"), CString("Monotype Corsiva"), fontSize, -fontSize / 2 - 4, h - w / 2, -90, RGB(0, 0, 0), RGB(255, 255, 255));

	pDC->SetWorldTransform(&form);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CKolPrviShuricaneView::DrawStar(CDC* pDC, int size)
{
	for (int i = 0; i < 4; i++)
	{
		DrawBlade(pDC, size);
		Rotate(pDC, 90);
	}
}

void CKolPrviShuricaneView::DrawBackground(CDC* pDC, CRect rect)
{
	sky->Draw(pDC, CRect(0, 0, sky->Width(), sky->Height()), rect);
}

void CKolPrviShuricaneView::DrawDrvo(CDC* pDC, CRect rect)
{
	bark->Draw(pDC, CRect(0, 0, bark->Width(), bark->Height()), CRect(rect.Width() - bark->Width(), 0, rect.Width(), rect.Height()));
}

void CKolPrviShuricaneView::WriteText(CDC* pDC, CString text, CString fontName, int fontSize, float x, float y, float angle, COLORREF color, COLORREF shadowColor)
{
	CFont newFont;
	newFont.CreateFontW(fontSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, fontName);

	CFont* oldFont = pDC->SelectObject(&newFont);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);

	XFORM form;
	pDC->GetWorldTransform(&form);

	COLORREF oldTextColor = pDC->SetTextColor(shadowColor);

	Translate(pDC, x, y);
	Rotate(pDC, angle);
	pDC->TextOutW(0, 0, text);

	Translate(pDC, 1, 1);
	pDC->SetTextColor(color);
	pDC->TextOutW(0, 0, text);

	pDC->SetWorldTransform(&form);

	pDC->SetBkMode(oldBkMode);
	pDC->SelectObject(oldFont);
	pDC->SetTextColor(oldTextColor);
}

void CKolPrviShuricaneView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviShuricaneView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = TO_RADIAN * angle;
	XFORM form = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviShuricaneView::OnDraw(CDC* pDC)
{
	CKolPrviShuricaneDoc* pDoc = GetDocument();
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

	DrawBackground(memDC, client);

	Translate(memDC, 100 + shuricaneX, 200);
	Rotate(memDC, shuricaneAngle);
	DrawStar(memDC, 50);

	memDC->SetWorldTransform(&form);

	DrawDrvo(memDC, client);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, client.Width(), client.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CKolPrviShuricaneView printing

BOOL CKolPrviShuricaneView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolPrviShuricaneView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolPrviShuricaneView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKolPrviShuricaneView diagnostics

#ifdef _DEBUG
void CKolPrviShuricaneView::AssertValid() const
{
	CView::AssertValid();
}

void CKolPrviShuricaneView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolPrviShuricaneDoc* CKolPrviShuricaneView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolPrviShuricaneDoc)));
	return (CKolPrviShuricaneDoc*)m_pDocument;
}
#endif //_DEBUG


// CKolPrviShuricaneView message handlers


void CKolPrviShuricaneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'W')
	{
		CRect client;
		GetClientRect(&client);

		int left = client.Width() - bark->Width() - 250;

		shuricaneX += shuricaneX < left ? 20 : 0;
		shuricaneAngle += shuricaneX <= left ? 10 : 0;
		Invalidate();
	}
	if (nChar == 'S')
	{
		shuricaneX = shuricaneAngle = 0;
		Invalidate();
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolPrviShuricaneView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
