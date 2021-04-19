#include "Renderer.h"

Camera* Renderer::m_camera = new Camera();
ResourceManager* Renderer::m_res_manager = new ResourceManager();
const std::vector<std::string> transferValues = {
    "transferValues[0]", "transferValues[1]", "transferValues[2]", "transferValues[3]",
    "transferValues[4]", "transferValues[5]", "transferValues[6]", "transferValues[7]"
};

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    delete m_shader;
    delete m_gui;
}

void Renderer::run()
{
	init();
	display(this->m_window);
}

void Renderer::init()
{
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_camera->init();

	this->m_window = glfwCreateWindow(m_camera->width, m_camera->height, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(this->m_window);

	glewExperimental = GL_TRUE;
	glewInit();
    m_gui = new GUI(this->m_window, m_camera);
    m_gui->init(std::bind(&Renderer::on_model_change, this, std::placeholders::_1),
                std::bind(&Renderer::scene_reset, this));
    
    m_shader = new Shader("shader/basic.vert", "shader/basic.frag");
}

void Renderer::display(GLFWwindow* window)
{
	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{
        // update delta time
        GLfloat current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

        m_gui->process_keyboard(delta_time);
		
		m_shader->use();
        
        update();
        
		setup_uniform_values(*m_shader);
        bind_texture(*m_shader);

		draw_scene(*m_shader);
        
        m_gui->refresh();
        m_gui->draw();

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return;
}

void Renderer::scene_reset()
{
    const ResourceManager::ModelData& data = m_res_manager->get_model_data(m_gui->get_model_val());
    m_camera->reset(data.position, data.rotation);
}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    int count = m_gui->get_visible_samples() * m_res_manager->get_slices().size();
    for (int i = 0; i < count; i++)
    {
        Slice* slice = m_res_manager->get_slices()[i];
        draw_object(shader, *slice);
    }
    glDisable(GL_BLEND);
}

void Renderer::draw_object(Shader& shader, Slice& slice)
{
    glPolygonMode(GL_FRONT_AND_BACK, m_gui->get_render_mode());
	glBindVertexArray(slice.vao);
    glDrawElements(GL_TRIANGLES, slice.veo_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::setup_uniform_values(Shader& shader)
{
    // Model
    glm::mat4 model = glm::mat4(1.0f);
    // Camera/View transformation
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(m_camera->cameraPos, m_camera->cameraPos + m_camera->cameraFront, m_camera->cameraUp);
    // Projection
    glm::mat4 projection;
    projection = glm::perspective(m_camera->fov, (GLfloat)m_camera->width / (GLfloat)m_camera->height, m_camera->near, m_camera->far);
    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(shader.program, "model");
    GLint viewLoc = glGetUniformLocation(shader.program, "view");
    GLint projLoc = glGetUniformLocation(shader.program, "projection");
    GLint colorLoc = glGetUniformLocation(shader.program, "color");
    GLint isWireframeLoc = glGetUniformLocation(shader.program, "isWireframe");
    GLint sampleRateLoc = glGetUniformLocation(shader.program, "sampleRate");
    GLint sampleRate0Loc = glGetUniformLocation(shader.program, "sampleRate0");
    
    // Pass the matrices to the shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(colorLoc, m_gui->get_obj_color_val().r(), m_gui->get_obj_color_val().g(), m_gui->get_obj_color_val().b(), 1.0f);
    glUniform1i(isWireframeLoc, m_gui->get_render_mode() == GL_LINE);
    glUniform1f(sampleRate0Loc, 8);
    glUniform1f(sampleRateLoc, m_gui->get_samples_per_unit());
    for (int i = 0; i < m_gui->get_transfer_values().size(); i++)
    {
        glUniform1f(glGetUniformLocation(shader.program, transferValues[i].c_str()), m_gui->get_transfer_values()[i]);
    }
}

void Renderer::bind_texture(Shader &shader)
{
    Texture* tex = m_res_manager->get_texture(m_gui->get_model_val());
    assert(tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex->texture);
    glUniform1i(glGetUniformLocation(shader.program, "tex3D"), 0);
    Texture* colorbar = m_res_manager->get_color_bar();
    assert(colorbar);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, colorbar->texture);
    glUniform1i(glGetUniformLocation(shader.program, "tex1D"), 1);
}

void Renderer::on_model_change(model_type type)
{
    m_res_manager->gen_slices(glm::vec4(m_camera->cameraFront, 1.0f), m_gui->get_samples_per_unit());
}

void Renderer::update()
{
    if (m_camera->isFrontChanged)
    {
        m_res_manager->gen_slices(glm::vec4(m_camera->cameraFront, 1.0f), m_gui->get_samples_per_unit());
    }
}

