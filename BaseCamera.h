#pragma once
#include <glm/glm.hpp>

class BaseCamera
{
public:
	BaseCamera();
	BaseCamera(glm::vec3 position);
	BaseCamera(glm::mat4 transform);
	virtual ~BaseCamera() {};

	virtual void Update(double dt) = 0;

	void SetTransform(glm::mat4 transform);
	const glm::mat4 GetTransform() const;

	void SetPosition(glm::vec3 position);
	glm::vec3 GetPosition() const { return glm::vec3(m_worldTransform[3].x, m_worldTransform[3].y, m_worldTransform[3].z); }

	void SetLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);

	void LookAt(const glm::vec3& lookAt, const glm::vec3& up);
	void LookAt(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up);

	void SetupPerspective(float fieldOfView, float aspectRatio, float near = 0.1f, float far = 1000.0f);

	const glm::mat4& GetProjection() const { return m_projectionTransform; }
	const glm::mat4& GetView() const { return m_viewTransform; }
	const glm::mat4& GetProjectionView() const { return m_projectionViewTransform; }

	bool GetPerspectiveSet() const { return m_bPerspectiveSet; }
protected:
	void UpdateProjectionViewTransform();


	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;

private:
	glm::mat4 m_worldTransform;
	bool m_bPerspectiveSet;
};