#pragma once

#include "BaseCamera.h"
#include "BoundingSphere.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>

class FlyCamera : public BaseCamera
{
public:
	FlyCamera() : FlyCamera(10.0f, 5.0f)
	{
		m_bViewButtonClicked = false;
	}
	FlyCamera(float flySpeed, float rotSpeed) : BaseCamera(), m_fFlySpeed(flySpeed), m_fRotationSpeed(rotSpeed), m_pWindow(nullptr)
	{

	}
	~FlyCamera() {};

	virtual void Update(double dt);

	void SetFlySpeed(float fSpeed);
	float GetFlySpeed() const { return m_fFlySpeed; }

	float GetRotationSpeed() const { return m_fRotationSpeed; }
	void SetRotationSpeed(float val) { m_fRotationSpeed = val; }

	//TEMP - Pass the window to be used to query the keyboard
	//This will get changed to an Input Manager later
	GLFWwindow* m_pWindow;

	void SetInputWindow(GLFWwindow* pWindow) { m_pWindow = pWindow; }
	
	bool Culling(BoundingSphere* boundingSphere);
	void getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes);

protected:
	void HandleKeyboardInput(double dt);
	void HandleMouseInput(double dt);

	void CalculateRotation(double dt, double xOffset, double yOffset);

	
	float m_fFlySpeed;
	float m_fRotationSpeed;
	bool m_bViewButtonClicked;
	double m_dCursorX, m_dCursorY;

};