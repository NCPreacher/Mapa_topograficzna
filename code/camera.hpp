#include <glm/glm.hpp>

class Camera
{
	const float mouseSpeed = 0.00005;
	const float movementSpeed = 1.0;

	float verticalAngle = 0.0f;
	float horizontalAngle = 1.5f;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::vec3 position;

	public:

		Camera() {}
		Camera( glm::vec3 position, float verticalAngle, float horizontalAngle );

		void update();
		void setPosition( glm::vec3 position );

		glm::vec3 getPosition();
		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();
};