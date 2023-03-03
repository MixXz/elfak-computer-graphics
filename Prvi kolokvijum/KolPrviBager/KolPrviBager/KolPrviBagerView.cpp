
// KolPrviBagerView.cpp : implementation of the CKolPrviBagerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "KolPrviBager.h"
#endif

#include "KolPrviBagerDoc.h"
#include "KolPrviBagerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 0.0174533;


// CKolPrviBagerView

IMPLEMENT_DYNCREATE(CKolPrviBagerView, CView)

BEGIN_MESSAGE_MAP(CKolPrviBagerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CKolPrviBagerView construction/destruction

CKolPrviBagerView::CKolPrviBagerView() noexcept
{
	bager = new DImage();
	bager->Load(CString("bager.png"));

	arm1 = new DImage();
	arm1->Load(CString("arm1.png"));

	arm2 = new DImage();
	arm2->Load(CString("arm2.png"));

	pozadina = new DImage();
	pozadina->Load(CString("pozadina.png"));

	viljuska = GetEnhMetaFile(CString("viljuska.emf"));

	arm1Angle = arm2Angle = forkAngle = -90;
	excavatorX = 0;
}

CKolPrviBagerView::~CKolPrviBagerView()
{
	delete bager;
	delete arm1;
	delete arm2;
	delete pozadina;
	DeleteEnhMetaFile(viljuska);
}

BOOL CKolPrviBagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKolPrviBagerView drawing

void CKolPrviBagerView::DrawBackground(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pozadina->Draw(pDC, CRect(0, 0, pozadina->Width(), pozadina->Height()), CRect(0, 0, rect.Width(), rect.Height()));
}

void CKolPrviBagerView::DrawImageTransparent(CDC* pDC, DImage* pImage, float x, float y)
{
	CRect rect;
	GetClientRect(&rect);

	BYTE* bites = pImage->GetDIBBits();
	COLORREF firstPixelColor = RGB(bites[2], bites[1], bites[0]);

	pImage->DrawTransparent(pDC, CRect(0, 0, pImage->Width(), pImage->Height()), CRect(-x, -y, pImage->Width() - x, pImage->Height() - y), firstPixelColor);
}

void CKolPrviBagerView::DrawBody(CDC* pDC)
{
	DrawImageTransparent(pDC, bager, bager->Width() / 2, bager->Height() / 2);
	Translate(pDC, -120, 20);
}

void CKolPrviBagerView::DrawArm1(CDC* pDC)
{
	Rotate(pDC, arm1Angle);
	DrawImageTransparent(pDC, arm1, 58, 61);	
}

void CKolPrviBagerView::DrawArm2(CDC* pDC)
{
	Rotate(pDC, arm2Angle);
	DrawImageTransparent(pDC, arm2, 36, 40);
}

void CKolPrviBagerView::DrawFork(CDC* pDC)
{
	Rotate(pDC, forkAngle);
	Scale(pDC, 2.5, 2.5);
	PlayEnhMetaFile(pDC->m_hDC, viljuska, CRect(-13, -18, 70 - 13, 52.5 - 18));
	Scale(pDC, 1 / 2.5, 1 / 2.5);
}

void CKolPrviBagerView::DrawExcavator(CDC* pDC)
{
	DrawBody(pDC);
	DrawArm1(pDC);
	Translate(pDC, 250, 0);
	DrawArm2(pDC);
	Translate(pDC, 235, 0);
	DrawFork(pDC);
}	

void CKolPrviBagerView::Translate(CDC* pDC, float dX, float dY, bool isRightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviBagerView::Scale(CDC* pDC, float sX, float sY, bool isRightMultiply)
{
	XFORM form = { sX, 0, 0, sY, 0, 0 };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviBagerView::Rotate(CDC* pDC, float ang, bool isRightMultiply)
{
	float angle = ang * TO_RAD;
	XFORM form = { cos(angle), sin(angle), -sin(angle), cos(angle) };
	pDC->ModifyWorldTransform(&form, isRightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKolPrviBagerView::OnDraw(CDC* pDC)
{
	CKolPrviBagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(&rect);

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memDC->SelectObject(&bmp);

	int oldMode = memDC->SetGraphicsMode(GM_ADVANCED);

	XFORM form;
	memDC->GetWorldTransform(&form);

	DrawBackground(memDC);
	Translate(memDC, rect.right - bager->Width() + excavatorX, rect.bottom - bager->Height() / 2);
	DrawExcavator(memDC);

	memDC->SetWorldTransform(&form);
	memDC->SetGraphicsMode(oldMode);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);

	memDC->DeleteDC();
	delete memDC;
}


// CKolPrviBagerView printing

BOOL CKolPrviBagerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolPrviBagerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolPrviBagerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKolPrviBagerView diagnostics

#ifdef _DEBUG
void CKolPrviBagerView::AssertValid() const
{
	CView::AssertValid();
}

void CKolPrviBagerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolPrviBagerDoc* CKolPrviBagerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolPrviBagerDoc)));
	return (CKolPrviBagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CKolPrviBagerView message handlers


void CKolPrviBagerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
		excavatorX -= 10;
		Invalidate();
		break;
	case VK_RIGHT:
		excavatorX += 10;
		Invalidate();
		break;
	case '1':
		arm1Angle += 10;
		Invalidate();
		break;
	case '2':
		arm1Angle -= 10;
		Invalidate();
		break;
	case '3':
		arm2Angle += 10;
		Invalidate();
		break;
	case '4':
		arm2Angle -= 10;
		Invalidate();
		break;
	case '5':
		forkAngle += 10;
		Invalidate();
		break;
	case '6':
		forkAngle -= 10;
		Invalidate();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolPrviBagerView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
