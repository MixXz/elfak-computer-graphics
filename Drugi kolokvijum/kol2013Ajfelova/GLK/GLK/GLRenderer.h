#pragma once

class CGLRenderer
{
public:
	UINT tex[4];
	float alpha, beta, distance;
	bool isLightOn;

	float* donji, * donjiTex;
	float* srednji, * srednjiTex;
	float* gornji, * gornjiTex;
	float* terasa, * terasaTex;

	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(char* fileName);
	void SetupLight();
	void SetEyePosition();

	void FillVA(float* buff, float a, float b, float h);
	void FillVATex(float* buff, float x1, float x2);
	void DrawAxis(float a);
	void DrawTower();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
