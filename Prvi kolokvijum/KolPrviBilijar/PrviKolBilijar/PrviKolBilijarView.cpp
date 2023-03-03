
// PrviKolBilijarView.cpp : implementation of the CPrviKolBilijarView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PrviKolBilijar.h"
#endif

#include "PrviKolBilijarDoc.h"
#include "PrviKolBilijarView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533

// CPrviKolBilijarView

IMPLEMENT_DYNCREATE(CPrviKolBilijarView, CView)

BEGIN_MESSAGE_MAP(CPrviKolBilijarView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPrviKolBilijarView construction/destruction

CPrviKolBilijarView::CPrviKolBilijarView() noexcept
{
	table = new DImage();
	table->Load(CString("felt2.jpg"));

	border = new DImage();
	border->Load(CString("wood.jpg"));

	stickAngle = 50;
	stickY = 15;
	ballX = 600;
	ballY = 400;

	collision = false;
}

CPrviKolBilijarView::~CPrviKolBilijarView()
{
	delete table;
	delete border;
}

BOOL CPrviKolBilijarView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPrviKolBilijarView drawing

void CPrviKolBilijarView::DrawStick(CDC* pDC, int w)
{
	int height = w;
	int topWidth = w / 100;
	int bottomWidth = topWidth * 2;

	CPen newPen(PS_SOLID, 1, RGB(65, 31, 2));
	CPen* oldPen = pDC->SelectObject(&newPen);

	CBrush lightBrush(RGB(254, 186, 134));
	CBrush* oldBrush = pDC->SelectObject(&lightBrush);

	POINT stickPoints[4] = { {-topWidth / 2, 0}, {topWidth / 2, 0}, {bottomWidth / 2, height}, {-bottomWidth / 2, height} };

	pDC->Polygon(stickPoints, 4);

	CBrush darkBrush(RGB(65, 31, 2));
	pDC->SelectObject(&darkBrush);

	POINT darkPartPoints[4] = { {-topWidth / 2 - 2, height - height / 3}, {topWidth / 2 + 2, height - height / 3}, {bottomWidth / 2, height}, {-bottomWidth / 2, height} };

	pDC->Polygon(darkPartPoints, 4);
	pDC->Ellipse(CRect(-bottomWidth / 2, height - bottomWidth / 2, bottomWidth / 2, height + bottomWidth / 2));

	CPen whitePen(PS_SOLID, 1.5, RGB(255, 255, 255));
	pDC->SelectObject(&whitePen);

	pDC->MoveTo(-topWidth / 2 + 2, 0);
	pDC->LineTo(-bottomWidth / 2 + 2, height);

	pDC->SelectObject(oldPen);
}

void CPrviKolBilijarView::DrawBall(CDC* pDC, int w)
{
	CPen redPen(PS_SOLID, 1, RGB(81, 14, 2));
	CPen* oldPen = pDC->SelectObject(&redPen);

	CBrush redBrush(RGB(121, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&redBrush);

	pDC->Ellipse(CRect(-w / 2, -w / 2, w / 2, w / 2));

	CBrush whiteBrush(RGB(255, 255, 255));
	pDC->SelectObject(&whiteBrush);

	float whiteWidth = w / 3;

	pDC->Ellipse(CRect(-whiteWidth / 2 + 2, -whiteWidth / 2 - 2, whiteWidth / 2 + 2, whiteWidth / 2 - 2));

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CPrviKolBilijarView::DrawTable(CDC* pDC, CRect rect)
{
	int rectW = rect.Width();
	int rectH = rect.Height();

	int imgW = table->Width();
	int imgH = table->Height();

	int n = rectW / imgW;
	int m = rectH / imgH;

	for (int i = 0; i < m + 1; i++)
		for (int j = 0; j < n + 1; j++)
			table->Draw(pDC, CRect(0, 0, imgW, imgH), CRect(imgW * j, imgH * i, imgW * j + imgW, imgH * i + imgH));
}

void CPrviKolBilijarView::DrawBorder(CDC* pDC, CRect rect, int w)
{
	border->Draw(pDC, CRect(0, 0, border->Width(), border->Height()), CRect(0, 0, rect.Width(), rect.Height()));

	CRgn rg;
	rg.CreateRectRgn(w, w, rect.Width() - w, rect.Height() - w);
	pDC->SelectClipRgn(&rg);

	DrawTable(pDC, rect);

	CRgn old;
	old.CreateRectRgn(0, 0, rect.Width(), rect.Height());
	pDC->SelectClipRgn(&old);
}

void CPrviKolBilijarView::DrawHoles(CDC* pDC, CRect rect, int size)
{
	CBrush blackBrush(RGB(1, 1, 1));
	CBrush* oldBrush = pDC->SelectObject(&blackBrush);

	XFORM form;
	pDC->GetWorldTransform(&form);

	Translate(pDC, rect.left + size, rect.top + size);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	Translate(pDC, rect.Width() / 2 - size, 0);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	Translate(pDC, rect.Width() / 2 - size, 0);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	Translate(pDC, 0, rect.Height() - size * 2);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	Translate(pDC, -rect.Width() / 2 + size, 0);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	Translate(pDC, -rect.Width() / 2 + size, 0);
	pDC->Ellipse(CRect(-size / 2, -size / 2, size / 2, size / 2));

	pDC->SetWorldTransform(&form);
	pDC->SelectObject(oldBrush);
}

void CPrviKolBilijarView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPrviKolBilijarView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = angle * TO_RADIAN;
	XFORM form = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPrviKolBilijarView::OnDraw(CDC* pDC)
{
	CPrviKolBilijarDoc* pDoc = GetDocument();
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

	int oldMode = memDC->SetGraphicsMode(GM_ADVANCED);

	XFORM form;
	memDC->GetWorldTransform(&form);

	DrawBorder(memDC, crect, 50);
	DrawHoles(memDC, crect, 50);

	Translate(memDC, ballX, ballY);
	DrawBall(memDC, 30);

	Rotate(memDC, stickAngle);
	Translate(memDC, 0, stickY);
	DrawStick(memDC, 600);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, crect.Width(), crect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CPrviKolBilijarView printing

BOOL CPrviKolBilijarView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPrviKolBilijarView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPrviKolBilijarView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CPrviKolBilijarView diagnostics

#ifdef _DEBUG
void CPrviKolBilijarView::AssertValid() const
{
	CView::AssertValid();
}

void CPrviKolBilijarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPrviKolBilijarDoc* CPrviKolBilijarView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPrviKolBilijarDoc)));
	return (CPrviKolBilijarDoc*)m_pDocument;
}
#endif //_DEBUG


// CPrviKolBilijarView message handlers


BOOL CPrviKolBilijarView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CPrviKolBilijarView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_RIGHT:
		stickAngle += 10;
		Invalidate();
		break;
	case VK_LEFT:
		stickAngle -= 10;
		Invalidate();
		break;
	case VK_UP:		
		if (stickY > 15)
		{
			stickY -= 15;
			if (stickY == 15) collision = true;
		}
		ballY += collision ? 15 : 0;
		Invalidate();
		break;
	case VK_DOWN:
		stickY += 15;
		collision = false;
		ballY = 400;
		Invalidate();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
