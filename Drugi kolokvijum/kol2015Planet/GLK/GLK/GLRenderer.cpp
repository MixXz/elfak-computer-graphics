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
	xAngle = yAngle = 1.0;
	moonAngle = 0;
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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	char tscName[] = "TSC0.jpg";
	char sName[] = "S0.jpg";
	char mName[] = "M0.jpg";
	for (int i = 0; i < 6; i++)
	{
		tscName[3] = '0' + i;
		tsc[i] = LoadTexture(tscName);

		sName[1] = '0' + i;
		s[i] = LoadTexture(sName);

		mName[1] = '0' + i;
		m[i] = LoadTexture(mName);
	}
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

	float x = distance * cos(yAngle) * cos(xAngle);
	float y = distance * sin(yAngle);
	float z = distance * sin(xAngle) * cos(yAngle);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);

	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(x, y, z);
	DrawSpace(1, 20);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	SetupLight();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	DrawEarth(3.0, 20);

	glPushMatrix();
	glTranslatef(50, 0, 0);
	DrawMoon(1, 10);
	glPopMatrix();

	glDisable(GL_LIGHTING);

	DrawAxis();

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
	glDeleteTextures(6, tsc);
	glDeleteTextures(6, s);
	glDeleteTextures(6, m);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;

	DImage* img = new DImage();
	img->Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	delete img;
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void CGLRenderer::SetupLight()
{
	GLfloat lightPos[] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambient);
}

void CGLRenderer::DrawAxis()
{
	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		glColor3f(0, 1.0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		glColor3f(0, 0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
	}
	glEnd();
}

void CGLRenderer::DrawPatch(double R, int n)
{
	double step = 2.0 / (double)n;
	double y = 1.0;

	for (int i = 0; i < n; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);

		double x = -1;
		for (int j = 0; j < n + 1; j++)
		{
			double phi = atan(x);
			double theta = atan(y * cos(phi));

			double xd = R * cos(theta) * sin(phi);
			double yd = R * sin(theta);
			double zd = R * cos(theta) * cos(phi);

			glNormal3f(xd / R, yd / R, zd / R);
			glTexCoord2f((x + 1.0) / 2.0, (-y + 1.0) / 2.0);
			glVertex3f(xd, yd, zd);

			phi = atan(x);
			theta = atan((y - step) * cos(phi));

			xd = R * cos(theta) * sin(phi);
			yd = R * sin(theta);
			zd = R * cos(theta) * cos(phi);

			glNormal3d(xd / R, yd / R, zd / R);
			glTexCoord2d((x + 1.0) / 2.0, (-y + step + 1.0) / 2.0);
			glVertex3d(xd, yd, zd);

			x += step;
		}
		glEnd();
		y -= step;
	}
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	DrawSphere(R, tes, tsc);
}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glPushMatrix();
	glRotatef(moonAngle, 0, 1, 0);
	DrawSphere(R, tes, m);
	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	DrawSphere(R, tes, s);
}

void CGLRenderer::DrawSphere(double R, int tes, UINT texture[])
{	
	glPushMatrix();
	for (int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		DrawPatch(R, tes);
		glRotatef(90, 0, 1, 0);
	}
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	DrawPatch(R, tes);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
}
