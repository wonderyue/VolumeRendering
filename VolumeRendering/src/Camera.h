#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROTATE_X_UP,
	ROTATE_X_DOWN,
	ROTATE_Y_UP,
	ROTATE_Y_DOWN,
	ROTATE_Z_UP,
	ROTATE_Z_DOWN,
};

class Camera {
public:
	unsigned int width;
	unsigned int height;
    GLfloat cameraSpeed = 0.05f;
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    GLfloat angleSpeed = 1.0f;
    glm::vec3 cameraAngle   = glm::vec3(0.0f, -90.0f, 90.0f);
    GLfloat fov = 45.0f;
    GLfloat near = 0.1f;
    GLfloat far = 100.0f;
    bool isFrontChanged = true;

	Camera(
		unsigned int width_ = 1080,
		unsigned int height_ = 640
	)
	{
		this->width = width_;
		this->height = height_;
	}

	void init() {
	};

	void reset(glm::vec3 position, glm::vec3 rotation) {
        cameraPos = position;
        cameraAngle = rotation;
	}
    
    void update_direction()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(cameraAngle.y)) * cos(glm::radians(cameraAngle.x));
        front.y = sin(glm::radians(cameraAngle.x));
        front.z = sin(glm::radians(cameraAngle.y)) * cos(glm::radians(cameraAngle.x));
        front = glm::normalize(front);
        isFrontChanged = !glm::epsilonEqual(front, cameraFront, glm::vec3(0.001f))[0];
        cameraFront = front;
        glm::vec3 up;
        up.x = -cos(glm::radians(cameraAngle.y)) * sin(glm::radians(cameraAngle.x)) * sin(glm::radians(cameraAngle.z)) - sin(glm::radians(cameraAngle.y)) * cos(glm::radians(cameraAngle.z));
        up.y = cos(glm::radians(cameraAngle.x)) * sin(glm::radians(cameraAngle.z));
        up.z = -sin(glm::radians(cameraAngle.y)) * sin(glm::radians(cameraAngle.x)) * sin(glm::radians(cameraAngle.z)) + cos(glm::radians(cameraAngle.y)) * cos(glm::radians(cameraAngle.z));
        cameraUp = glm::normalize(up);
    }

	void process_keyboard(Camera_Movement direction, GLfloat velocity)
	{
		if (direction == FORWARD) {
            cameraPos += cameraSpeed * cameraFront;
		}
		if (direction == BACKWARD) {
            cameraPos -= cameraSpeed * cameraFront;
		}
		if (direction == LEFT) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (direction == RIGHT) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (direction == UP) {
            cameraPos += cameraSpeed * cameraUp;
		}
		if (direction == DOWN) {
            cameraPos -= cameraSpeed * cameraUp;
		}
		if (direction == ROTATE_X_UP) {
            cameraAngle.x += angleSpeed;
		}
		if (direction == ROTATE_X_DOWN) {
            cameraAngle.x -= angleSpeed;
		}
		if (direction == ROTATE_Y_UP) {
            cameraAngle.y += angleSpeed;
		}
		if (direction == ROTATE_Y_DOWN) {
            cameraAngle.y -= angleSpeed;
		}
		if (direction == ROTATE_Z_UP) {
            cameraAngle.z += angleSpeed;
		}
		if (direction == ROTATE_Z_DOWN) {
            cameraAngle.z -= angleSpeed;
		}
	}
};
