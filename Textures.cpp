#include "Textures.h"



Textures::Textures()
{
}


Textures::~Textures()
{
}


bool Textures::Init()
{
	unsigned char* data;
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;


	//plain texture
	data = stbi_load("./data/textures/ground.jpg", &imageWidth,
	       &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(m_texture, "tex0"), 0);


	//spear texture
	data = stbi_load("./data/models/soulspear/soulspear_diffuse.tga",
		   &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture2);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(m_texture2, "tex0"), 1);


	//dragon texture
	data = stbi_load("./data/models/characters/Marksman/Marksman_D.tga",
		   &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture3);
	glBindTexture(GL_TEXTURE_2D, m_texture3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(m_texture3, "tex0"), 2);

	return true;
}

void Textures::Destroy()
{
}