#pragma once

class CGLRenderer
{
public:
	UINT marble, fabric, metal;
	float alpha, beta, distance;

	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(CString fileName);

	void DrawAxes();
	void DrawCube(double size);
	void DrawTube(double r1, double r2, double h, int step);
	void DrawAbazur(double r1, double r2, double h, int step);
	void DrawBody(double r1, double r2, double r3, double h, int step);
	void DrawLamp();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
