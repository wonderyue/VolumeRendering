#pragma once

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <nanogui/nanogui.h>

#include "Shader.h"
#include "Camera.h"
#include "Slice.h"
#include "GUI.h"
#include "ResourceManager.h"

class Renderer
{
public:
	GLFWwindow* m_window;

	static Camera* m_camera;
    static ResourceManager* m_res_manager;

	glm::vec4 background_color = glm::vec4(0.0,0.0,0.0,0.0);

	GLfloat delta_time = 0.0;
	GLfloat last_frame = 0.0;

public:
	Renderer();
	~Renderer();

	void run();

private:
    GUI* m_gui;
    Shader* m_shader;
    
	void init();
    
	void display(GLFWwindow* window);
    void update();
	
	void on_model_change(model_type mode_type);
    
	void draw_scene(Shader& shader);

	void draw_object(Shader& shader, Slice& object);

	void setup_uniform_values(Shader& shader);
    void bind_texture(Shader& shader);
	void scene_reset();
};

