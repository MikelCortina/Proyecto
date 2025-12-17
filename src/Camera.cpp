#include "Camera.h"	






Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::Width = width;
	Camera::Height = height;
	position = position;
}

void Camera::Matrix(float FOVdeg, float nearPLane, float farPlane, Shader& shader, const char* uniform)
{
	// Initialize matrices
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	// Change view matrix to a lookat matrix
	view = glm::lookAt(position, position + orientation, up);
	// Create projection matrix
	projection = glm::perspective(glm::radians(FOVdeg), (float)(Width / Height), nearPLane, farPlane);
	// Send matrices to shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}