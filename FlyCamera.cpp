#include "FlyCamera.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <assert.h>

void FlyCamera::Update(double dt)
{
	assert(m_pWindow != nullptr);

	HandleKeyboardInput(dt);
	HandleMouseInput(dt);
}

void FlyCamera::SetFlySpeed(float fSpeed)
{
	m_fFlySpeed = fSpeed;
}

void FlyCamera::HandleKeyboardInput(double dt)
{
	//Get the cameras forward/up/right
	glm::mat4 trans = GetTransform();

	glm::vec3 vRight = glm::vec3(trans[0].x, trans[0].y, trans[0].z);
	glm::vec3 vUp = glm::vec3(trans[1].x, trans[1].y, trans[1].z);
	glm::vec3 vForward = glm::vec3(trans[2].x, trans[2].y, trans[2].z);
	glm::vec3 moveDir(0.0f);

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir -= vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir += vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDir -= vUp;
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		moveDir += vUp;
	}

	//Arrow keys to control facing if you don't want to use the mouse
	glm::vec3 faceChange(0.0f);

	if (glfwGetKey(m_pWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		faceChange.y = -3.5f;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		faceChange.y = 3.5f;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		faceChange.x = -3.5f;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		faceChange.x = 3.5f;
	}
	
	CalculateRotation(dt, faceChange.x, faceChange.y);

	float fLength = glm::length(moveDir);
	if (fLength > 0.01f)
	{
		moveDir = ((float)dt * m_fFlySpeed) * glm::normalize(moveDir);
		SetPosition(GetPosition() + moveDir);
	}
}

void FlyCamera::HandleMouseInput(double dt)
{
	if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (m_bViewButtonClicked == false)
		{
			int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);

			m_dCursorX = width / 2.0;
			m_dCursorY = height / 2.0;

			glfwSetCursorPos(m_pWindow, width / 2, height / 2);

			m_bViewButtonClicked = true;
		}
		else
		{
			double mouseX, mouseY;
			glfwGetCursorPos(m_pWindow, &mouseX, &mouseY);

			double xOffset = mouseX - m_dCursorX;
			double yOffset = mouseY - m_dCursorY;

			CalculateRotation(dt, xOffset, yOffset);

		}

		int width, height;
		glfwGetFramebufferSize(m_pWindow, &width, &height);
		glfwSetCursorPos(m_pWindow, width / 2, height / 2);
	}
	else
	{
		m_bViewButtonClicked = false;
	}
}

void FlyCamera::CalculateRotation(double dt, double xOffset, double yOffset)
{
	if (xOffset == 0 && yOffset == 0) return;
	m_fRotationSpeed = 0.5f;

	if (xOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(m_fRotationSpeed * dt * -xOffset), glm::vec3(0, 1, 0));

		SetTransform(GetTransform() * rot);
	}

	if (yOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(m_fRotationSpeed * dt * -yOffset), glm::vec3(1, 0, 0));

		SetTransform(GetTransform() * rot);
	}

	//Clean up rotation

	glm::mat4 oldTrans = GetTransform();

	glm::mat4 trans;

	glm::vec3 worldUp = glm::vec3(0, 1, 0);

	//Right
	glm::vec3 oldForward = glm::vec3(oldTrans[2].x, oldTrans[2].y, oldTrans[2].z);

	trans[0] = glm::normalize(glm::vec4(glm::cross(worldUp, oldForward), 0));
	//Up
	glm::vec3 newRight = glm::vec3(trans[0].x, trans[0].y, trans[0].z);
	trans[1] = glm::normalize(glm::vec4(glm::cross(oldForward, newRight), 0));
	//Forward
	trans[2] = glm::normalize(oldTrans[2]);

	//Position
	trans[3] = oldTrans[3];

	SetTransform(trans);

}

bool FlyCamera::Culling(BoundingSphere* boundingSphere)
{
	vec4 planes[6];
	getFrustumPlanes(GetProjectionView(), planes);
	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), boundingSphere->centre) +
				  planes[i].w;
		if (d < -boundingSphere->radius)
		{
			printf("Behind, don't render it!\n");
			return false;
			break;
		}
		
	}
	printf("Front, fully visible so render it!\n");
	return true;
}

void FlyCamera::getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes)
{
	// right side
	planes[0] = vec4(transform[0][3] - transform[0][0],
					 transform[1][3] - transform[1][0],
					 transform[2][3] - transform[2][0],
					 transform[3][3] - transform[3][0]);
	// left side
	planes[1] = vec4(transform[0][3] + transform[0][0],
					 transform[1][3] + transform[1][0],
					 transform[2][3] + transform[2][0],
					 transform[3][3] + transform[3][0]);
	// top
	planes[2] = vec4(transform[0][3] - transform[0][1],
					 transform[1][3] - transform[1][1],
					 transform[2][3] - transform[2][1],
					 transform[3][3] - transform[3][1]);
	// bottom
	planes[3] = vec4(transform[0][3] + transform[0][1],
					 transform[1][3] + transform[1][1],
					 transform[2][3] + transform[2][1],
					 transform[3][3] + transform[3][1]);
	// far
	planes[4] = vec4(transform[0][3] - transform[0][2],
					 transform[1][3] - transform[1][2],
					 transform[2][3] - transform[2][2],
					 transform[3][3] - transform[3][2]);
	// near
	planes[5] = vec4(transform[0][3] + transform[0][2],
					 transform[1][3] + transform[1][2],
					 transform[2][3] + transform[2][2],
					 transform[3][3] + transform[3][2]);
	// plane normalisation, based on length of normal
	for (int i = 0; i < 6; i++) {
		float d = glm::length(vec3(planes[i]));
		planes[i] /= d;
	}

}