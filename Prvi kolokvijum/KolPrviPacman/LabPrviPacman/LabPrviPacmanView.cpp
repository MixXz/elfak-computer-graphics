
// LabPrviPacmanView.cpp : implementation of the CLabPrviPacmanView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LabPrviPacman.h"
#endif

#include "LabPrviPacmanDoc.h"
#include "LabPrviPacmanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RADIAN 0.0174533

// CLabPrviPacmanView

IMPLEMENT_DYNCREATE(CLabPrviPacmanView, CView)

BEGIN_MESSAGE_MAP(CLabPrviPacmanView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLabPrviPacmanView construction/destruction

CLabPrviPacmanView::CLabPrviPacmanView() noexcept
{
	bck = new DImage();
	bck->Load(CString("blue.png"));

	angle = 0;
	pacmanX = 400;
	pacmanY = 400;
	mouth = true;
	mirrorX = true;
	mirrorY = true;
}

CLabPrviPacmanView::~CLabPrviPacmanView()
{
	delete bck;
}

BOOL CLabPrviPacmanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLabPrviPacmanView drawing

void CLabPrviPacmanView::DrawBackground(CDC* pDC, CRect window)
{
	bck->Draw(pDC, CRect(0, 0, bck->Width(), bck->Height()), window);
}

void CLabPrviPacmanView::DrawPacman(CDC* pDC, CRect rect, float angle)
{
	CBrush yellowBrush(RGB(255, 255, 0));
	CBrush* oldBrush = pDC->SelectObject(&yellowBrush);

	float rectWidth = rect.Width();
	float rectHeight = rect.Height();

	CPoint p1(rectWidth, rectHeight / 2 - rectWidth * tan(2 * angle * TO_RADIAN));
	CPoint p2(rectWidth, rectHeight / 2 + rectWidth * tan(2 * angle * TO_RADIAN));

	pDC->Pie(rect, p1, p2);
	 
	CBrush blackBrush(RGB(0, 0, 0));
	pDC->SelectObject(&blackBrush);

	Translate(pDC, rectWidth - 25, rectHeight / 3 - 10);
	pDC->Ellipse(CRect(-5, -5, 5, 5));

	pDC->SelectObject(oldBrush);
}

void CLabPrviPacmanView::DrawGhost(CDC* pDC, CRect rect)
{
	CRgn old;
	GetClipRgn(pDC->m_hDC, old);

	//Clip rgn za odsecanje viska lukova na suknjici duha
	CRgn clip;
	clip.CreateRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1);
	pDC->SelectClipRgn(&clip);

	int width = rect.right; 
	int widthHalf = rect.right - rect.Width() / 2;

	int height = rect.bottom; 
	int heightHalf = rect.bottom - rect.Height() / 2; 

	//Pen za iscrtavanje kontura duha sa bojom razlicitom od RGB(0,0,0) 
	//zato sto floodFill za 0,0,0 mesa pozadinu sa crnom bojom i konture duha
	//pa ne zna sta da popuni a sta ne, pa duh ostane pola neobojen ako je iza njega crna pozadina sa RGB(0,0,0).
	CPen pen(PS_SOLID, 1, RGB(2, 2, 2));
	CPen* oldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rect.left, height);
	pDC->LineTo(rect.left, heightHalf);

	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->Arc(rect, { rect.left, heightHalf }, { width, heightHalf });
	pDC->SetArcDirection(AD_COUNTERCLOCKWISE);

	pDC->MoveTo(width, heightHalf);
	pDC->LineTo(width, height);

	int arcW = (rect.right - rect.left) / 6;
	int arcH = (rect.bottom - rect.top) / 6;

	XFORM form;
	pDC->GetWorldTransform(&form);

	Translate(pDC, rect.right - rect.Width() - arcW / 2, rect.bottom - arcH);
	for (int i = 0; i < 7; i++)
	{
		CRect arcRect(i * arcW, 0, (i + 1) * arcW, arcH);
		pDC->SetArcDirection(i % 2 == 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);

		pDC->Arc(arcRect, { i * arcW, arcH / 2 }, { (i + 1) * arcW, arcH / 2 });
	}	

	pDC->SetWorldTransform(&form);

	Translate(pDC, widthHalf, heightHalf);
	DrawGhostEye(pDC, 12);

	Translate(pDC, 28, 0);
	DrawGhostEye(pDC, 12);

	pDC->SetWorldTransform(&form);

	CBrush red(RGB(255, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&red);

	pDC->FloodFill(rect.left + 5, rect.bottom - 5, RGB(2,2,2));

	pDC->SelectClipRgn(&old);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void CLabPrviPacmanView::DrawGhostEye(CDC* pDC, int r)
{	
	XFORM form;
	pDC->GetWorldTransform(&form);

	CBrush white(RGB(255, 255, 255));
	CBrush blue(RGB(0, 0, 255));

	CBrush* oldBrush = pDC->SelectObject(&white);
	pDC->Ellipse(CRect(-r, -r, r, r));

	Translate(pDC, 2, 0);
	pDC->SelectObject(&blue);
	pDC->Ellipse(CRect(-r / 2, -r / 2, r / 2, r / 2));

	pDC->SetWorldTransform(&form);
	pDC->SelectObject(oldBrush);
}

void CLabPrviPacmanView::WriteText(CDC* pDC, char* text, int x, int y, int size, COLORREF color)
{
	COLORREF prev = pDC->SetTextColor(color);
	CString name("Arial");
	LOGFONT l;
	l.lfCharSet = ANSI_CHARSET;
	l.lfHeight = size;
	l.lfWeight = FW_BOLD;
	l.lfOrientation = 0;
	l.lfEscapement = 0;
	wcscpy_s(l.lfFaceName, 32, L"Times New Roman");

	CFont f;
	f.CreateFont(size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, name);

	CFont* old = pDC->SelectObject(&f);

	int o = pDC->SetBkMode(TRANSPARENT);

	Translate(pDC, x, y);
	pDC->TextOut(0, 0, CString(text));
	Translate(pDC, -x, -y);

	pDC->SetBkMode(o);

	pDC->SelectObject(old);
	pDC->SetTextColor(prev);
}

void CLabPrviPacmanView::CheckCollision(CDC* pDC)
{	
	CRect crect;
	GetClientRect(&crect);

	float ghostRadius = (196 - 100) / 2;
	float pacmanRadius = 50;

	float ghostCenterX = 196 - ghostRadius;
	float ghostCenterY = 216 - ghostRadius;

	float dictX = ghostCenterX - pacmanX;
	float dictY = ghostCenterY - pacmanY;

	float distance = sqrt(dictX * dictX + dictY * dictY);
	
	if (distance <= pacmanRadius + ghostRadius)
		WriteText(pDC, "The End", crect.Width() / 2 - 120, crect.Height() / 2 - 60, 72, RGB(255, 255, 255));
}

void CLabPrviPacmanView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLabPrviPacmanView::Rotate(CDC* pDC, float angle, bool isRightMultiply)
{
	float ang = angle * TO_RADIAN;
	XFORM xForm = { cos(ang), sin(ang), -sin(ang), cos(ang), 0, 0 };
	pDC->ModifyWorldTransform(&xForm, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLabPrviPacmanView::Mirror(CDC* pDC, bool mX, bool mY, bool isRightMultiply)
{
	XFORM xForm = { mX ? 1 : -1, 0, 0, mY ? 1 : -1, 0, 0 };
	pDC->ModifyWorldTransform(&xForm, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLabPrviPacmanView::OnDraw(CDC* pDC)
{
	CLabPrviPacmanDoc* pDoc = GetDocument();
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

	DrawBackground(memDC, crect);
	DrawGhost(memDC, CRect(100, 120, 196, 216));

	Translate(memDC, pacmanX, pacmanY);
	Rotate(memDC, angle);
	Mirror(memDC, mirrorX, mirrorY);
	Translate(memDC, -50, -50);
	DrawPacman(memDC, CRect(0, 0, 100, 100), mouth ? 8 : 0);

	memDC->SetWorldTransform(&form);

	CheckCollision(memDC);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, crect.Width(), crect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CLabPrviPacmanView printing

BOOL CLabPrviPacmanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLabPrviPacmanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLabPrviPacmanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLabPrviPacmanView diagnostics

#ifdef _DEBUG
void CLabPrviPacmanView::AssertValid() const
{
	CView::AssertValid();
}

void CLabPrviPacmanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLabPrviPacmanDoc* CLabPrviPacmanView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLabPrviPacmanDoc)));
	return (CLabPrviPacmanDoc*)m_pDocument;
}
#endif //_DEBUG


// CLabPrviPacmanView message handlers


BOOL CLabPrviPacmanView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CLabPrviPacmanView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_RIGHT:
		pacmanX += 10;
		mouth = !mouth;
		mirrorX = mirrorY = true;
		angle = 0;
		Invalidate();
		break;
	case VK_LEFT:
		pacmanX -= 10;
		mouth = !mouth;
		mirrorX = false;
		angle = 0;
		Invalidate();
		break;
	case VK_UP:
		pacmanY -= 10;
		mirrorX = true;
		mouth = !mouth;
		angle = -90;
		Invalidate();
		break;
	case VK_DOWN:
		pacmanY += 10;
		mirrorX = true;
		mouth = !mouth;
		angle = 90;
		Invalidate();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
