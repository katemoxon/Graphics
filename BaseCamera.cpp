#include "BaseCamera.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

BaseCamera::BaseCamera()
	: BaseCamera(glm::mat4(1))
{

}

BaseCamera::BaseCamera(glm::vec3 position)
	: BaseCamera()
{
	m_worldTransform[3] = glm::vec4(position, 1);
	m_bPerspectiveSet = false;
}

BaseCamera::BaseCamera(glm::mat4 transform)
	: m_worldTransform(transform)
{
	UpdateProjectionViewTransform();
}

void BaseCamera::SetTransform(glm::mat4 transform)
{
	m_worldTransform = transform;
	UpdateProjectionViewTransform();
}

const glm::mat4 BaseCamera::GetTransform() const
{
	return m_worldTransform;
}

void BaseCamera::SetPosition(glm::vec3 position)
{
	m_worldTransform[3] = glm::vec4(position, 1);
	UpdateProjectionViewTransform();
}

void BaseCamera::SetLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
	m_viewTransform = glm::lookAt(from, to, up);
	m_worldTransform = glm::inverse(m_viewTransform);
	UpdateProjectionViewTransform();
}

void BaseCamera::LookAt(const glm::vec3& lookAt, const glm::vec3& worldUp)
{
	m_worldTransform = glm::inverse(glm::lookAt(GetPosition(), lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::LookAt(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& worldUp)
{
	m_worldTransform = glm::inverse(glm::lookAt(position, lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::SetupPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
	m_bPerspectiveSet = true;

	UpdateProjectionViewTransform();
}

void BaseCamera::UpdateProjectionViewTransform()
{
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}