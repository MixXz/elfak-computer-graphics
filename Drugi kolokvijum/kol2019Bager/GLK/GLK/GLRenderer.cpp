#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#define TO_DEG 57.295827908797774375395898255342
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	xAngle = 0.75;
	yAngle = 0.15;	
	distance = 25;

	bodyAng = arm1Ang = arm2Ang = forkAng = 0;
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

	this->excavator = LoadTexture("excavator.png");
	this->front = LoadTexture("front.jpg");
	this->back = LoadTexture("back.jpg");
	this->top = LoadTexture("top.jpg");
	this->bot = LoadTexture("bot.jpg");
	this->left = LoadTexture("left.jpg");
	this->right = LoadTexture("right.jpg");

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

	DrawExcavator();

	DrawEnvCube(100);

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
	gluPerspective(55, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);
	glDeleteTextures(1, &excavator);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &back);

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

void CGLRenderer::DrawAxis(int a)
{
	glBegin(GL_LINES);
	{
		glColor3f(0, 0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(a, 0, 0);

		glColor3f(1.0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, a, 0);

		glColor3f(0, 1.0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, a);
	}
	glEnd();
}

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < n; i++)
	{
		glTexCoord2f(texCoords[i].x, texCoords[i].y);
		glVertex3f(points[i].x, points[i].y, 0);
	}
	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glPushMatrix();

	glTranslatef(0, 0, -zh / 2);
	DrawPolygon(points, texCoords, n);
	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUAD_STRIP);
	glColor3f(r, g, b);
	for (int i = 0; i <= n; i++)
	{
		glVertex3f(points[i % n].x, points[i % n].y, -zh / 2);
		glVertex3f(points[i % n].x, points[i % n].y, zh / 2);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawBase()
{
	//Prostorne i teksturne koordinate temena treba odrediti na osnovu sl. 3, pri cemu se
	//moze smatrati da je jedan podeok na slici 0.5 prostornih jedinica.

	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 8;
	POINTF points[] = {
		{0, 1},
		{1, 0},
		{15, 0},
		{16, 1},
		{16, 3},
		{15, 4},
		{1, 4},
		{0, 3}
	};
	POINTF tex[8];
	for (int i = 0; i < n; i++)
	{
		tex[i].x = (points[i].x) / 16.0;
		tex[i].y = (16.0 - points[i].y) / 16.0;
		points[i].x /= 2;
		points[i].y /= 2;
	}

	DrawExtrudedPolygon(points, tex, n, 5, 0.3, 0.3, 0.3);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Lakse za razumevanje
	//glBindTexture(GL_TEXTURE_2D, excavator);

	//POINTF points[] = {
	//	{0, 0.5},
	//	{0.5, 0},
	//	{7.5, 0},
	//	{8.0, 0.5},
	//	{8.0, 1.5},
	//	{7.5, 2},
	//	{0.5, 2},
	//	{0, 1.5}
	//};

	//POINTF tex[] = {
	//	{0, 15.0 / 16.0},
	//	{1 / 16.0, 16 / 16.0},
	//	{15 / 16.0, 16.0 / 16.0},
	//	{16 / 16.0, 15 / 16.0},
	//	{16 / 16.0, 13 / 16.0},
	//	{15 / 16.0, 12 / 16.0},
	//	{1 / 16.0, 12 / 16.0},
	//	{0, 13 / 16.0}
	//};

	//DrawExtrudedPolygon(points, tex, 8, 5, 0.3, 0.3, 0.3);

	//glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawBody()
{
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 5;
	POINTF points[] = {
		{8, 0},
		{8, 4},
		{4, 8},
		{0, 8},
		{0,0}
	};

	POINTF tex[5];
	for (int i = 0; i < n; i++)
	{
		tex[i].x = (8 + points[i].x) / 16.0;
		tex[i].y = (8.0 - points[i].y) / 16.0;
		points[i].x /= 2;
		points[i].y /= 2;
	}
	DrawExtrudedPolygon(points, tex, n, 4, 0.96, 0.5, 0.12);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawArm(double zh)
{
	glBindTexture(GL_TEXTURE_2D, excavator);

	int n = 8;
	POINTF points[] = {
		{1, 0},
		{4, 0},
		{16, 1},
		{16, 3},
		{4, 4},
		{1, 4},
		{0, 3},
		{0, 1}
	};

	POINTF tex[8];
	for (int i = 0; i < n; i++)
	{
		tex[i].x = (points[i].x) / 16.0;
		tex[i].y = (12.0 - points[i].y) / 16.0;
		points[i].x /= 2;
		points[i].y /= 2;
	}

	DrawExtrudedPolygon(points, tex, n, zh, 0.96, 0.5, 0.12);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawFork()
{
	glBindTexture(GL_TEXTURE_2D, excavator);

	const int n = 6;
	POINTF points[] = {
		{0, 5},
		{0, 0},
		{8, 0},
		{8, 5},
		{7, 6},
		{1, 6}
	};

	POINTF tex[n];
	for (int i = 0; i < n; i++) {
		tex[i].x = (points[i].x) / 16.0;
		tex[i].y = (6.0 - points[i].y) / 16.0;
		points[i].x /= 2;
		points[i].y /= 2;
	}

	DrawExtrudedPolygon(points, tex, n, 1, 0.7, 0.7, 0.7);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLRenderer::DrawExcavator()
{
	glPushMatrix();

	glTranslatef(-4, 0, 0);
	DrawBase();

	glTranslatef(4, 2, 0);
	glRotatef(bodyAng * TO_DEG, 0, 1, 0);
	glTranslatef(-2, 0, 0);
	DrawBody();

	glTranslatef(3, 1, -2.5);
	glRotatef(90 + arm1Ang * TO_DEG, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawArm(1);

	glTranslatef(7, 1, 0);
	glRotatef(-90 + arm2Ang * TO_DEG, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawArm(1.5);

	glTranslatef(7, 1, 0);
	glRotatef(-90 + forkAng * TO_DEG, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawFork();

	glPopMatrix();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glPushMatrix();
	glTranslatef(0, a / 2, 0);
	DrawEnvCubeSide(a, front);

	glRotatef(90, 0, 1, 0);
	DrawEnvCubeSide(a, left);

	glRotatef(90, 0, 1, 0);
	DrawEnvCubeSide(a, back);

	glRotatef(90, 0, 1, 0);
	DrawEnvCubeSide(a, right);

	glRotatef(90, 1, 0, 0);
	DrawEnvCubeSide(a, top);

	glRotatef(-180, 1, 0, 0);
	DrawEnvCubeSide(a, bot);

	glPopMatrix();
}

void CGLRenderer::DrawEnvCubeSide(double a, UINT texture)
{
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
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
