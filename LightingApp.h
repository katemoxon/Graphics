#pragma once

#include "FBXFile.h"
#include "FlyCamera.h"

#define GLM_SWIZZLE

#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


using glm::vec3;
using glm::vec4;
using glm::mat4;


class LightingApp
{
public:
	LightingApp();
	~LightingApp();

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);

	FBXFile* m_fbx;
	FBXFile* m_fbx2;
	unsigned int m_program;
	unsigned int m_texture;

	bool Init(const char* location);
	bool Update(float deltaTime);
	void Draw(FlyCamera *fCam);
	void Destroy();

protected:


};