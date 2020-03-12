#include "./camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

extern GLFWwindow* window;

extern bool view;
extern float fieldOfView;

Camera::Camera( vec3 position, float verticalAngle, float horizontalAngle ): position(position), verticalAngle(verticalAngle), horizontalAngle(horizontalAngle) {}

void Camera::update()
{
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float( currentTime - lastTime );

	double xpos, ypos;
	glfwGetCursorPos( window, &xpos, &ypos );
	glfwSetCursorPos( window, 1024/2, 768/2 );

	horizontalAngle += mouseSpeed * float(1024/2 - xpos );

	if( verticalAngle > -1.0 && verticalAngle < 1.0 ) { verticalAngle   += mouseSpeed * float( 768/2 - ypos ); }
	else if ( verticalAngle < -1.0  ) { verticalAngle = -0.999; }
	else if ( verticalAngle > 1.0  ) { verticalAngle = 0.999; }

	vec3 direction( cos( verticalAngle ) * sin( horizontalAngle ), sin( verticalAngle ),  cos( verticalAngle ) * cos( horizontalAngle ) );
	
	vec3 right = vec3 ( sin( horizontalAngle - 3.14f/2.0f ), 0, cos( horizontalAngle - 3.14f/2.0f ) );
	
	vec3 up = cross( right, direction );

	if (glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS) { position += direction * deltaTime * movementSpeed; }
	if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS) { position -= direction * deltaTime * movementSpeed; }
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) { position += right * deltaTime * movementSpeed; }
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) { position -= right * deltaTime * movementSpeed; }
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) { position -= up * deltaTime * movementSpeed; }
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) { position -= -up * deltaTime * movementSpeed; }

	ProjectionMatrix = perspective( radians( fieldOfView ), 4.0f / 3.0f, 0.1f, 100.0f );
	ViewMatrix = lookAt( position, position + direction, up );

	lastTime = currentTime;
}

void Camera::setPosition( vec3 position ) { this -> position = position; }

vec3 Camera::getPosition() { return position; }
mat4 Camera::getViewMatrix() { return ViewMatrix; }
mat4 Camera::getProjectionMatrix() { return ProjectionMatrix; }