#pragma once
#include "DImage.h"

class CGLRenderer
{
public:
	UINT ship, back, bot, front, left, right, top;
	int FOV;
	float yAngle, xAngle, distance;
	bool isLightOn;

	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(CString fileName);
	void SetupLight();

	void DrawAxis();
	void DrawTriangle(float d1, float d2, float rep);
	void DrawShip();
	void DrawSpaceCube(double a);
	void DrawSpaceCubeSide(double a, UINT texture);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
