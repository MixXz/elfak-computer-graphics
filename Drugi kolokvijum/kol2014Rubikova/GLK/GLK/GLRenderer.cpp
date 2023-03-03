#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#define PI 3.14159
#define TO_RADIAN PI / 180
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = beta = 45 * TO_RADIAN;
	firstAng = secondAng = thirdAng = 0;
	distance = 20;
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
	glClearColor(0.7, 0.7, 0.7, 0.7);
	glEnable(GL_DEPTH_TEST);

	this->tex = LoadTexture("OpenGL.bmp");

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

	SetEyePosition();
	SetWhiteLight();

	DrawRubikCube(2, 3);

	glDisable(GL_LIGHTING);
	DrawAxis(10);

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
	gluPerspective(40, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &tex);
	wglMakeCurrent(NULL,NULL); 
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	glBindTexture(GL_TEXTURE_2D, 0);
	delete img;
	return id;
}

void CGLRenderer::SetEyePosition()
{
	float x = distance * cos(alpha) * cos(beta);
	float y = distance * sin(alpha);
	float z = distance * cos(alpha) * sin(beta);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
}

void CGLRenderer::SetWhiteLight()
{
	float amb[] = { 0.5, 0.5, 0.5, 1.0 };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	float dif[] = { 1.0, 1.0, 1.0, 1.0 };
	float amb0[] = { 0.8, 0.8, 0.8, 1.0 };
	float spec[] = { 0.5, 0.5, 0.5, 1.0 };
	float pos0[] = { 0.0, 1.0, 0.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void CGLRenderer::SetMaterial(float r, float g, float b)
{
	float ambient[] = { r / 5, g / 5, b / 5 };
	float difspec[] = { r,g,b };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difspec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, difspec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
}

void CGLRenderer::DrawAxis(float a)
{
	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(a, 0, 0);

		glColor3f(0, 1.0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, a, 0);

		glColor3f(0, 0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, a);
	}
	glEnd();
}

void CGLRenderer::DrawCubeSide(float a)
{
	glBegin(GL_QUADS);
	glBindTexture(GL_TEXTURE_2D, tex);
	glNormal3f(0, 0, 1);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(1, 1);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawCube(float a)
{
	glPushMatrix();
	{
		//prednja
		SetMaterial(1, 0, 0);
		DrawCubeSide(a);

		//desna
		glRotatef(90, 0, 1, 0);
		SetMaterial(0, 0, 1);
		DrawCubeSide(a);

		//zadnja
		glRotatef(90, 0, 1, 0);
		SetMaterial(1, 0.5, 0);
		DrawCubeSide(a);

		//leva
		glRotatef(90, 0, 1, 0);
		SetMaterial(0, 1, 0);
		DrawCubeSide(a);

		//donja
		glRotatef(90, 1, 0, 0);
		SetMaterial(1, 1, 0);
		DrawCubeSide(a);

		//gornja
		glRotatef(180, 1, 0, 0);
		SetMaterial(1, 1, 1);
		DrawCubeSide(a);
	}
	glPopMatrix();
}

void CGLRenderer::DrawRubikCube(float a, int count)
{	
	float pom = a * (count / 2) * 1.05;

	glPushMatrix();
	glTranslatef(-pom, -pom, -pom);
	for (int i = 0; i < count; i++)
	{
		float ang = 0;

		switch (i)
		{
		case 0: ang = firstAng; break;
		case 1: ang = secondAng; break;
		case 2: ang = thirdAng; break;
		default:
			break;
		}

		glPushMatrix();
		glTranslatef(0, pom, pom);		
		glRotatef(ang, 1, 0, 0);
		glTranslatef(0, -pom, -pom);

		DrawRubikCubeColumn(a, count);
		glPopMatrix();

		glTranslatef(a * 1.05, 0, 0);
	}
	glPopMatrix();
}

void CGLRenderer::DrawRubikCubeColumn(float a, int count)
{
	glPushMatrix();
	for (int j = 0; j < count; j++)
	{	
		glPushMatrix();
		for (int i = 0; i < count; i++)
		{
			DrawCube(a);
			glTranslatef(0, a * 1.05, 0);
		}
		glPopMatrix();
		glTranslatef(0, 0, a * 1.05);
	}
	glPopMatrix();
}
