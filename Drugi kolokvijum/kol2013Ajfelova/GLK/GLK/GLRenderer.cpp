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
	distance = 25;
	isLightOn = false;
	donji = new float[16 * 3];
	srednji = new float[16 * 3];
	gornji = new float[16 * 3];
	terasa = new float[16 * 3];
	donjiTex = new float[16 * 3];
	srednjiTex = new float[16 * 3];
	gornjiTex = new float[16 * 3];
	terasaTex = new float[16 * 3];
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
	glClearColor(0.4, 0.4, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	tex[0] = LoadTexture("Eiffel-1a.bmp");
	tex[1] = LoadTexture("Eiffel-2a.bmp");
	tex[2] = LoadTexture("Eiffel-3a.bmp");
	tex[3] = LoadTexture("Eiffel-4a.bmp");
	glEnable(GL_TEXTURE_2D);

	FillVA(donji, 4.0, 2.4, 1.5);
	FillVATex(donjiTex, 0.215, 0.0);

	FillVA(srednji, 2.40, 1.2, 1.5);
	FillVATex(srednjiTex, 0.215, 0.0);

	FillVA(gornji, 1.08, 0.0, 8.0);
	FillVATex(gornjiTex, 0.450, 0.05);

	FillVA(terasa, 2.4, 2.64, 0.25);
	FillVATex(terasaTex, 0.0, 0.0);

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
	SetupLight();

	DrawTower();

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
	gluPerspective(45, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(4, tex);
	delete[] donji;
	delete[] gornji;
	delete[] srednji;
	delete[] terasa;
	delete[] donjiTex;
	delete[] gornjiTex;
	delete[] srednjiTex;
	delete[] terasaTex;

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

	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->Width(), img->Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img->GetDIBBits());

	glBindTexture(GL_TEXTURE_2D, 0);
	delete img;
	return id;
}

void CGLRenderer::SetupLight()
{
	GLfloat lightPos[] = { 10.0, 15.0, 10.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if (isLightOn)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}

void CGLRenderer::SetEyePosition()
{
	float x = distance * cos(alpha) * cos(beta);
	float y = distance * sin(alpha);
	float z = distance * cos(alpha) * sin(beta);

	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
}

void CGLRenderer::FillVA(float* buff, float a, float b, float h)
{
	int i = 0;

	buff[i] = -b / 2; buff[i + 1] = h; buff[i + 2] = -b / 2;
	i += 3;
	buff[i] = -a / 2; buff[i + 1] = 0; buff[i + 2] = -a / 2;
	i += 3;
	buff[i] = -a / 2; buff[i + 1] = 0; buff[i + 2] = a / 2;
	i += 3;
	buff[i] = -b / 2; buff[i + 1] = h; buff[i + 2] = b / 2;
	
	i += 3;
	buff[i] = -b / 2; buff[i + 1] = h; buff[i + 2] = b / 2;
	i += 3;
	buff[i] = -a / 2; buff[i + 1] = 0; buff[i + 2] = a / 2;
	i += 3;
	buff[i] = a / 2; buff[i + 1] = 0; buff[i + 2] = a / 2;
	i += 3;
	buff[i] = b / 2; buff[i + 1] = h; buff[i + 2] = b / 2;
	
	i += 3;
	buff[i] = b / 2; buff[i + 1] = h; buff[i + 2] = b / 2;
	i += 3;
	buff[i] = a / 2; buff[i + 1] = 0; buff[i + 2] = a / 2;
	i += 3;
	buff[i] = a / 2; buff[i + 1] = 0; buff[i + 2] = -a / 2;
	i += 3;
	buff[i] = b / 2; buff[i + 1] = h; buff[i + 2] = -b / 2;
	
	i += 3;
	buff[i] = b / 2; buff[i + 1] = h; buff[i + 2] = -b / 2;
	i += 3;
	buff[i] = a / 2; buff[i + 1] = 0; buff[i + 2] = -a / 2;
	i += 3;
	buff[i] = -a / 2; buff[i + 1] = 0; buff[i + 2] = -a / 2;
	i += 3;
	buff[i] = -b / 2; buff[i + 1] = h; buff[i + 2] = -b / 2;
}

void CGLRenderer::FillVATex(float* buff, float x1, float x2)
{
	int i = 0;
	for (int j = 0; j < 4; j++) {
		buff[i] = x1; buff[i + 1] = 0.0;
		i += 2;
		buff[i] = x2; buff[i + 1] = 1;
		i += 2;
		buff[i] = 1 - x2; buff[i + 1] = 1;
		i += 2;
		buff[i] = 1 - x1; buff[i + 1] = 0.0;
		i += 2;
	}
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

void CGLRenderer::DrawTower()
{
	glPushMatrix();

	//donji
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 16; i++)
	{
		glTexCoord2f(donjiTex[i * 3], donjiTex[i * 3 + 1]);
		glVertex3f(donji[i * 3], donji[i * 3 + 1], donji[i * 3 + 2]);
	}
	glEnd();

	//terasa
	glBindTexture(GL_TEXTURE_2D, tex[3]);
	glTranslatef(0, 1.5, 0);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 16; i++)
	{
		glTexCoord2f(terasaTex[i * 3], terasaTex[i * 3 + 1]);
		glVertex3f(terasa[i * 3], terasa[i * 3 + 1], terasa[i * 3 + 2]);
	}
	glEnd();

	//srednji
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 16; i++)
	{
		glTexCoord2f(srednjiTex[i * 3], srednjiTex[i * 3 + 1]);
		glVertex3f(srednji[i * 3], srednji[i * 3 + 1], srednji[i * 3 + 2]);
	}
	glEnd();

	//terasa
	glBindTexture(GL_TEXTURE_2D, tex[3]);
	glTranslatef(0, 1.5, 0);
	glPushMatrix();
	glScalef(1.08 / 2.4, 1, 1.08 / 2.4);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 16; i++)
	{
		glTexCoord2f(terasaTex[i * 3], terasaTex[i * 3 + 1]);
		glVertex3f(terasa[i * 3], terasa[i * 3 + 1], terasa[i * 3 + 2]);
	}
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex[2]);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 16; i++)
	{
		glTexCoord2f(gornjiTex[i * 3], gornjiTex[i * 3 + 1]);
		glVertex3f(gornji[i * 3], gornji[i * 3 + 1], gornji[i * 3 + 2]);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}
