#include "FlyCamera.h"
#include <stb_image.h>

#pragma once
class Textures
{
public:
	Textures();
	~Textures();

	unsigned int m_program;
	unsigned int m_texture;
	unsigned int m_texture2;
	unsigned int m_texture3;

	bool Init();
	void Destroy();
protected:

	unsigned int m_VAO;	//vertex array object
	unsigned int m_VBO; //vertex buffer object
	unsigned int m_IBO;	//index buffer object

};

