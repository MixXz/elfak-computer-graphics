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
	alpha = 1.75;
	beta = 0.25;
	vetrenjacaAng = 0;
	distance = 35;
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
	this->brick = LoadTexture("brick.png");
	this->env = LoadTexture("env.png");
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

	float x = distance * cos(beta) * cos(alpha);
	float y = distance * sin(beta);
	float z = distance * sin(alpha) * cos(beta);

	gluLookAt(x, y, z, 0, 10, 0, 0, 1, 0);

	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(x, y, z);
	DrawCube(1);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	SetupLight();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	DrawTube(3.5, 2.5, 10, 32);

	glTranslatef(0, 10, 0);
	DrawCone(3.8, 2, 32);
	
	glTranslatef(0, 0, 3.8);
	glPushMatrix();
	{
		glRotatef(vetrenjacaAng, 0, 0, 1);
		DrawPaddle(8, 1.5);

		glRotatef(90, 0, 0, 1);
		DrawPaddle(8, 1.5);

		glRotatef(90, 0, 0, 1);
		DrawPaddle(8, 1.5);

		glRotatef(90, 0, 0, 1);
		DrawPaddle(8, 1.5);
	}
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glTranslatef(0, -10, -3.8);
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
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &brick);
	glDeleteTextures(1, &env);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texId;
	DImage* img = new DImage();
	img->Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	glBindTexture(GL_TEXTURE_2D, 0);

	delete img;
	return texId;
}

void CGLRenderer::SetupLight()
{
	GLfloat lightPos[] = { 1.0, 1.0, 0.0, 0.0 };
	GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambient);
}

void CGLRenderer::DrawCube(double a)
{
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, env);
	glPushMatrix();

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 2.0 / 3.0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 2.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 1.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(0, 1.0 / 3.0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glRotatef(-90, 0, 1, 0);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(2.0 / 4.0, 1);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 2.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 2.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 1.0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glRotatef(-90, 0, 1, 0);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(3.0 / 4.0, 1.0 / 3.0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 1.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 2.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(3.0 / 4.0, 2.0 / 3.0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glRotatef(-90, 0, 1, 0);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1.0 / 4.0, 0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 1.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 1.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glPopMatrix();
	glPushMatrix();

	glRotatef(90, 1, 0, 0);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1.0 / 4.0, 2.0 / 3.0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 2.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(2.0 / 4.0, 1.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(1.0 / 4.0, 1.0 / 3.0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glRotatef(180, 1, 0, 0);
	glRotatef(180, 0, 0, 1);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1.0, 1.0 / 3.0);
		glVertex3f(-a / 2, a / 2, a / 2);

		glTexCoord2f(3.0 / 4.0, 1.0 / 3.0);
		glVertex3f(-a / 2, -a / 2, a / 2);

		glTexCoord2f(3.0 / 4.0, 2.0 / 3.0);
		glVertex3f(a / 2, -a / 2, a / 2);

		glTexCoord2f(1.0, 2.0 / 3.0);
		glVertex3f(a / 2, a / 2, a / 2);
	}
	glEnd();

	glPopMatrix();

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{
	double angleStep = 2 * PI / n;
	double angle = 0;
	double y = sin(atan2(r1 - r2, h));	//Ne znam kako i zasto.

	glBindTexture(GL_TEXTURE_2D, brick);
	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0, 1.0, 1.0);

	for (int i = 0; i <= n; i++)
	{
		float x = r1 * cos(angle);
		float z = r1 * sin(angle);

		glNormal3f(x / r1, y / r1, z / r1);
		glTexCoord2f((double)i / n, 0);
		glVertex3f(x, 0 , z);

		x = r2 * cos(angle);
		z = r2 * sin(angle);

		glTexCoord2f((double)i / n, 1);
		glVertex3f(x, h, z);

		angle += angleStep;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double angleStep = 2 * PI / n;
	double angle = 0;

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < n; i++)
	{
		float x = r * cos(angle);
		float z = r * sin(angle);

		glVertex3f(x, 0, z);

		angle += angleStep;
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);
	for (int i = 0; i <= n; i++)
	{
		float x = r * cos(angle);
		float z = r * sin(angle);
		glNormal3f(x / r, h / r, z / r);
		glVertex3f(x, 0, z);

		angle += angleStep;
	}
	glEnd();
}

void CGLRenderer::DrawPaddle(double length, double width)
{
	glPushMatrix();
	glScalef(1, -1, 1);
	glBegin(GL_QUADS);
	{
		glNormal3f(0, 0, 1);

		glVertex3f(0, 0, 0);
		glVertex3f(length / 8, 0, 0);
		glVertex3f(length / 8, width / 8, 0);
		glVertex3f(0, width / 8, 0);

		glVertex3f(length / 8, 0, 0);
		glVertex3f(2 * (length / 8), 0, 0);
		glVertex3f(2 * (length / 8), width / 8, 0);
		glVertex3f(length / 8, width / 8, 0);

		glVertex3f(2 * (length / 8), 0, 0);
		glVertex3f(length, 0, 0);
		glVertex3f(length, width, 0);
		glVertex3f(2 * (length / 8), width, 0);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	{
		glNormal3f(0, 0, 1);
		glVertex3f(length / 8, width/8, 0);
		glVertex3f(2 * (length / 8), width / 8, 0);
		glVertex3f(2 * (length / 8), width, 0);
	}
	glEnd();
	glPopMatrix();
}

void CGLRenderer::DrawAxis(double a)
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
