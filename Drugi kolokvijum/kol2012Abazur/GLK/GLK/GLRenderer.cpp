#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = beta = 1.0;
	distance = 5;
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	this->marble = LoadTexture(CString("Marble.bmp"));
	this->fabric = LoadTexture(CString("Fabric1.bmp"));
	this->metal = LoadTexture(CString("Metal1.bmp"));

	GLfloat lightPos[] = { 6.0, 4.0, 6.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_TEXTURE_2D);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float x = distance * cos(alpha) * cos(beta);
	float y = distance * sin(alpha);
	float z = distance * cos(alpha) * sin(beta);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

	DrawLamp();

	DrawAxes();

	glFlush();
	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &marble);
	glDeleteTextures(1, &fabric);
	glDeleteTextures(1, &metal);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(CString fileName)
{
	UINT id;
	DImage* img = new DImage();
	img->Load(fileName);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	glBindTexture(GL_TEXTURE_2D, 0);
	delete img;
	return id;
}

void CGLRenderer::DrawAxes()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(10.0, 0, 0);

		glColor3f(0, 1.0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10.0, 0);

		glColor3f(0, 0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10.0);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void CGLRenderer::DrawCube(double size)
{
	const float a = size / 2;

	glBindTexture(GL_TEXTURE_2D, marble);
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a, a, -a);

		glTexCoord2f(0, 1);
		glVertex3f(-a, -a, -a);
		
		glTexCoord2f(1, 1);
		glVertex3f(a, -a, -a);

		glTexCoord2f(1, 0);
		glVertex3f(a, a, -a);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a, a, -a);

		glTexCoord2f(0, 1);
		glVertex3f(-a, -a, -a);

		glTexCoord2f(1, 1);
		glVertex3f(-a, -a, a);

		glTexCoord2f(1, 0);
		glVertex3f(-a, a, a);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a, a, a);

		glTexCoord2f(0, 1);
		glVertex3f(-a, -a, a);

		glTexCoord2f(1, 1);
		glVertex3f(a, -a, a);

		glTexCoord2f(1, 0);
		glVertex3f(a, a, a);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	{
		glTexCoord2f(0, 0);
		glVertex3f(a, a, a);

		glTexCoord2f(0, 1);
		glVertex3f(a, -a, a);

		glTexCoord2f(1, 1);
		glVertex3f(a, -a, -a);

		glTexCoord2f(1, 0);
		glVertex3f(a, a, -a);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a, -a, -a);

		glTexCoord2f(0, 1);
		glVertex3f(a, -a, -a);

		glTexCoord2f(1, 1);
		glVertex3f(a, -a, a);

		glTexCoord2f(1, 0);
		glVertex3f(-a, -a, a);
	}
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a, a, -a);

		glTexCoord2f(0, 1);
		glVertex3f(a, a, -a);

		glTexCoord2f(1, 1);
		glVertex3f(a, a, a);

		glTexCoord2f(1, 0);
		glVertex3f(-a, a, a);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int step)
{
	float angleStep = 2.0 * 3.14159 / (float)step;
	float ang = 0.0;

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.7, 0.7, 0.7);
	for (int i = 0; i <= step; i++)
	{
		float x = cos(ang);
		float z = sin(ang);
		glNormal3f(x, 0, z);
		glTexCoord2f((double)i / step, 0);
		glVertex3f(x * r1, 0, z * r1);

		glNormal3f(x, h, z);
		glTexCoord2f((double)i / step, 1);
		glVertex3f(x * r2, h, z * r2);

		ang += angleStep;
	}
	glEnd();
}

void CGLRenderer::DrawAbazur(double r1, double r2, double h, int step)
{
	glBindTexture(GL_TEXTURE_2D, fabric);

	DrawTube(r1, r2, h, step);		

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawBody(double r1, double r2, double r3, double h, int step)
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, metal);

	DrawTube(r1, r2, h/4, step);

	glTranslatef(0, h / 4, 0);
	DrawTube(r2, r3, h / 4, step);

	glTranslatef(0, h / 4, 0);
	DrawTube(r3, r3, h / 4, step);

	glTranslatef(0, h / 4, 0);
	DrawTube(r3, r2, h / 4, step);

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void CGLRenderer::DrawLamp()
{
	glPushMatrix();

	DrawCube(1.0);

	glTranslatef(0, 0.5, 0);
	DrawBody(0.2, 0.1, 0.05, 1.0, 16);

	glTranslatef(0, 1.0, 0);
	DrawAbazur(0.5, 0.25, 0.2, 16);

	glPopMatrix();
}
