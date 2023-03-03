#pragma once

class CGLRenderer
{
public:
	UINT tsc[6];
	UINT m[6];
	UINT s[6];

	float xAngle, yAngle, distance;
	float moonAngle;

	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(char* fileName);
	void SetupLight();

	void DrawAxis();
	void DrawPatch(double R, int n);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);
	void DrawSphere(double R, int tes, UINT texture[]);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
