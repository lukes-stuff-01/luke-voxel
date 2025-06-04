#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD, 
	LEFT, 
	RIGHT

};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{ 
public:
	glm::vec3 position; // whoda thunk it, more SHIT CODING PRACTICES but it's convenient this way so fuck you
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		position = Position;
		worldUp = Up;
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float Yaw = YAW, float Pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void ProcessKeyboard(CameraMovement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD) {
			position += front * velocity;
		}
		if (direction == BACKWARD) {
			position -= front * velocity;
		}
		if (direction == LEFT) {
			position -= right * velocity;
		}
		if (direction == RIGHT) {
			position += right * velocity;
		}
	}

	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) {
		xOffset *= MouseSensitivity * (Zoom / 45.0f);
		yOffset *= MouseSensitivity * (Zoom/45.0f);
		
		yaw += xOffset;
		pitch += yOffset;
		if (constrainPitch)
		{
			if (pitch > 89.0f) {
				pitch = 89.0f;
			}
			if (pitch < -89.0f) {
				pitch = -89.0f;
			}
		}
		updateCameraVectors();//send this bitch after movin the mf camera around
	}
	void ProcessMouseScroll(float yOffset) {
		Zoom -= (float)yOffset;
		if (Zoom < 1.0f) {
			Zoom = 1.0;
		}
		if (Zoom > 55.0f) {
			Zoom = 55.0f;
		}
	}

private:

	void updateCameraVectors() {
		glm::vec3 Front;
		Front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		Front.y = sin(glm::radians(pitch));
		Front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(Front);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};

#endif