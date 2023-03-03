#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#define PI 3.14159
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = beta = 1;
	distance = 50;
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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	this->spider = LoadTexture("spider.png");
	this->front = LoadTexture("front.jpg");
	this->back = LoadTexture("back.jpg");
	this->left = LoadTexture("left.jpg");
	this->right = LoadTexture("right.jpg");
	this->top = LoadTexture("top.jpg");
	this->bot = LoadTexture("bot.jpg");

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

	glPushMatrix();
	glTranslatef(0, 40, 0);
	DrawEnvCube(100);
	glPopMatrix();

	DrawSpider();

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
	gluPerspective(50, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &spider);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);

	wglMakeCurrent(NULL, NULL);
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT id;
	DImage* img = new DImage();
	img->Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINES);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	glBindTexture(GL_TEXTURE_2D, 0);
	delete img;
	return id;
}

void CGLRenderer::DrawAxes()
{
	glBegin(GL_LINES);
	{
		glColor3f(0, 0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(50, 0, 0);

		glColor3f(1.0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 50, 0);

		glColor3f(0, 1.0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 50);
	}
	glEnd();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPushMatrix();
	{
		DrawEnvCubeSide(a, front);

		glRotatef(-90, 0, 1, 0);
		DrawEnvCubeSide(a, right);

		glRotatef(-90, 0, 1, 0);
		DrawEnvCubeSide(a, back);

		glRotatef(-90, 0, 1, 0);
		DrawEnvCubeSide(a, left); 

		glRotatef(-90, 1, 0, 0);
		DrawEnvCubeSide(a, bot);

		glRotatef(-180, 1, 0, 0);
		DrawEnvCubeSide(a, top);
	}
	glPopMatrix();

	glDisable(GL_CULL_FACE);
}

void CGLRenderer::DrawEnvCubeSide(double a, UINT texture)
{	
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, a / 2, -a / 2);

		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, -a / 2, -a / 2);

		glTexCoord2f(1, 1);
		glVertex3f(a / 2, -a / 2, -a / 2);

		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a / 2, -a / 2);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	float ang1Step = 2 * PI / nSeg;
	float ang2Step = 2 * PI / (2 * nSeg);
	float ang1 = 0, ang2 = 0;

	glBindTexture(GL_TEXTURE_2D, spider);
	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < nSeg; i++)
	{
		for (int j = 0; j < 2 * nSeg; j++)
		{
			float x = r * cos(ang1) * cos(ang2);
			float y = r * sin(ang1);
			float z = r * cos(ang1) * sin(ang2);
			glTexCoord2f(texR * x / r + texU, texR * z / r + texV);
			glVertex3f(x, y, z);

			x = r * cos(ang1 + ang1Step) * cos(ang2);
			y = r * sin(ang1 + ang1Step);
			z = r * cos(ang1 + ang1Step) * sin(ang2);
			glTexCoord2f(texR * x / r + texU, texR * z / r + texV);
			glVertex3f(x, y, z);

			ang2 += ang2Step;
		}
		ang1 += ang1Step;
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	float angStep = 2 * PI / nSeg;
	float ang = 0;

	glBindTexture(GL_TEXTURE_2D, spider);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(texU, texV);
	glVertex3f(0, h, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		float x = r * cos(ang);
		float z = r * sin(ang);
		glTexCoord2f(texR * x / r + texU, texR * z / r + texV);
		glVertex3f(x, 0, z);

		ang += angStep;
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glPushMatrix();
	{		
		glTranslatef(0, r, 0);
		DrawSphere(r, 2 * nSeg, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
		DrawCone(r, h, nSeg, 3.0 / 4.0, 3.0 / 4.0, 1.0 / 4.0);
	}
	glPopMatrix();
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();
	{		
		DrawLegSegment(1, 10, 5);
		glTranslatef(0, 11, 0);
		glRotatef(85, 1, 0, 0);
		DrawLegSegment(1, 15, 5);
	}
	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	glPushMatrix();
	{
		glPushMatrix();
		{
			glTranslatef(-4, 0, 0);
			glScalef(1.0, 0.5, 1.0);
			DrawSphere(2, 10, 3.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glScalef(1.0, 0.5, 1.0);
			DrawSphere(3, 10, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
		}
		glPopMatrix();

		glTranslatef(6, 0, 0);

		glPushMatrix();
		{
			glScalef(1.0, 0.8, 1.0);
			DrawSphere(5, 10, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0);
		}
		glPopMatrix();

	}
	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	glPushMatrix();
	{
		DrawSpiderBody();
		glTranslatef(0, 1.5, 0);
		for (int i = 0; i < 4; i++)
		{
			glPushMatrix();
			{
				glRotatef(i * 30 - 45, 0, 1.0, 0);
				glRotatef(45, 1, 0, 0);
				DrawLeg();
			}
			glPopMatrix();
		}

		for (int i = 0; i < 4; i++)
		{
			glPushMatrix();
			{
				glRotatef(i * 30 - 45 + 180, 0, 1.0, 0);
				glRotatef(45, 1, 0, 0);
				DrawLeg();
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}
