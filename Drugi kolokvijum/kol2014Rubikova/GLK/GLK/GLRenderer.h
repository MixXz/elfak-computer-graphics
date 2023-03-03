#pragma once

class CGLRenderer
{
public:
	UINT tex;
	float alpha, beta, distance;
	float firstAng, secondAng, thirdAng;

	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(char* fileName);
	void SetEyePosition();
	void SetWhiteLight();
	void SetMaterial(float r, float g, float b);

	void DrawAxis(float a);
	void DrawCubeSide(float a);
	void DrawCube(float a);
	void DrawRubikCube(float a, int count);
	void DrawRubikCubeColumn(float a, int count);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
