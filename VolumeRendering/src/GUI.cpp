#include "GUI.h"

nanogui::Screen* GUI::m_nanogui_screen = nullptr;
bool GUI::keys[1024];


GUI::~GUI()
{
}

void GUI::init(const std::function<void(model_type)> &sel_model, const std::function<void()> &reset)
{
    m_nanogui_screen = new nanogui::Screen();
    m_nanogui_screen->initialize(m_gl_window, true);
    m_model_sel_callback = sel_model;
    m_reset_callback = reset;
    glViewport(0, 0, m_camera->width, m_camera->height);

    // Create nanogui gui
    m_inspector_window = new nanogui::FormHelper(m_nanogui_screen);
    m_inspector_window->setFixedSize(Eigen::Vector2i(150,20));
    
    //Inspector
    m_inspector_window->addWindow(Eigen::Vector2i(0, 0), "Inspector");
    
    m_inspector_window->addGroup("Position");
    m_inspector_window->addVariable("x", m_camera->cameraPos.x, true)->setSpinnable(true);
    m_inspector_window->addVariable("y", m_camera->cameraPos.y, true)->setSpinnable(true);
    m_inspector_window->addVariable("z", m_camera->cameraPos.z, true)->setSpinnable(true);
    
    m_inspector_window->addGroup("Rotation");
    x_slider = new nanogui::Slider(m_inspector_window->addVariable("x", m_camera->cameraAngle.x, false));
    x_slider->setFixedSize(Eigen::Vector2i(120,20));
    x_slider->setRange(std::pair<float, float>(-180,180));
    x_slider->setValue(m_camera->cameraAngle.x);
    x_slider->setCallback([&](float value) {
        m_camera->cameraAngle.x = value;
    });
    y_slider = new nanogui::Slider(m_inspector_window->addVariable("y", m_camera->cameraAngle.y, false));
    y_slider->setFixedSize(Eigen::Vector2i(120,20));
    y_slider->setRange(std::pair<float, float>(-180,180));
    y_slider->setValue(m_camera->cameraAngle.y);
    y_slider->setCallback([&](float value) {
        m_camera->cameraAngle.y = value;
    });
    z_slider = new nanogui::Slider(m_inspector_window->addVariable("z", m_camera->cameraAngle.z, false));
    z_slider->setFixedSize(Eigen::Vector2i(120,20));
    z_slider->setRange(std::pair<float, float>(-180,180));
    z_slider->setValue(m_camera->cameraAngle.z);
    z_slider->setCallback([&](float value) {
        m_camera->cameraAngle.z = value;
    });
    
    m_inspector_window->addGroup("Configration");
    nanogui::ComboBox* modelComboBox = m_inspector_window->addVariable("Model", m_model_val, true);
    m_inspector_window->addVariable("near", m_camera->near, true)->setSpinnable(true);
    m_inspector_window->addVariable("far", m_camera->far, true)->setSpinnable(true);
    modelComboBox->setItems({ "TEAPOT", "BONSAI", "HEAD", "BUCKY" });
    modelComboBox->setCallback([&](int value) {
        m_model_val = (model_type)value;
        m_model_sel_callback(m_model_val);
        m_reset_callback();
    });
    m_model_sel_callback(m_model_val);
    m_reset_callback();
    m_inspector_window->addVariable("Mode", m_render_val)->setItems({ "Line", "Trangle" });
    
    m_inspector_window->addVariable("Object Color", m_obj_col_val, true);
    
    m_inspector_window->addButton("Reset", [&]() {
        m_reset_callback();
    })->setTooltip("Reset camera position and rotation.");
    
    //Slice
    m_slice_window = new nanogui::FormHelper(m_nanogui_screen);
    m_slice_window->setFixedSize(Eigen::Vector2i(150,20));
    m_slice_window->addWindow(Eigen::Vector2i(m_camera->width-300, 0), "Slices");
    m_slice_window->addVariable("Samples per unit", m_samples_per_unit, true)->setCallback([&](int value) {
        m_samples_per_unit = value;
        m_model_sel_callback(m_model_val);
    });
    
    m_sample_slider = new nanogui::Slider(m_slice_window->addVariable("visible samples", m_visible_samples, false));
    m_sample_slider->setFixedSize(Eigen::Vector2i(120,20));
    m_sample_slider->setRange(std::pair<float, float>(0.0f,1.0f));
    m_sample_slider->setValue(m_visible_samples);
    m_sample_slider->setCallback([&](float value) {
        m_visible_samples = value;
        m_model_sel_callback(m_model_val);
    });
    
    m_slice_window->addGroup("Transfer Sliders");
    for (int i = 0; i < 8; i++)
    {
        nanogui::Slider* slider = new nanogui::Slider(m_slice_window->addVariable(std::to_string(i), m_transfer_values[i], false));
        slider->setFixedSize(Eigen::Vector2i(120,20));
        slider->setRange(std::pair<float, float>(0.0f,1.0f));
        slider->setValue(m_transfer_values[i]);
        slider->setCallback([&, i](float value) {
            m_transfer_values[i] = value;
        });
        m_transfer_slider.push_back(slider);
    }
    //Show UI
    m_nanogui_screen->setVisible(true);
    m_nanogui_screen->performLayout();

    glfwSetCursorPosCallback(m_gl_window,
        [](GLFWwindow* window, double x, double y) {
            m_nanogui_screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(m_gl_window,
        [](GLFWwindow*, int button, int action, int modifiers) {
            m_nanogui_screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(m_gl_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            if (key >= 0 && key < 1024)
            {
                if (action == GLFW_PRESS)
                    keys[key] = true;
                else if (action == GLFW_RELEASE)
                    keys[key] = false;
            }
        }
    );

    glfwSetCharCallback(m_gl_window,
        [](GLFWwindow*, unsigned int codepoint) {
            m_nanogui_screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(m_gl_window,
        [](GLFWwindow*, int count, const char** filenames) {
            m_nanogui_screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(m_gl_window,
        [](GLFWwindow*, double x, double y) {
            m_nanogui_screen->scrollCallbackEvent(x, y);
            //m_camera->ProcessMouseScroll(y);
        }
    );

    glfwSetFramebufferSizeCallback(m_gl_window,
        [](GLFWwindow*, int width, int height) {
            m_nanogui_screen->resizeCallbackEvent(width, height);
        }
    );
}

void GUI::process_keyboard(float delta_time)
{
    m_camera->update_direction();
    if (keys[GLFW_KEY_W])
        m_camera->process_keyboard(FORWARD, delta_time);
    if (keys[GLFW_KEY_S])
        m_camera->process_keyboard(BACKWARD, delta_time);
    if (keys[GLFW_KEY_A])
        m_camera->process_keyboard(LEFT, delta_time);
    if (keys[GLFW_KEY_D])
        m_camera->process_keyboard(RIGHT, delta_time);
    if (keys[GLFW_KEY_Q])
        m_camera->process_keyboard(UP, delta_time);
    if (keys[GLFW_KEY_E])
        m_camera->process_keyboard(DOWN, delta_time);
    if (keys[GLFW_KEY_I])
        m_camera->process_keyboard(ROTATE_X_UP, delta_time);
    if (keys[GLFW_KEY_K])
        m_camera->process_keyboard(ROTATE_X_DOWN, delta_time);
    if (keys[GLFW_KEY_J])
        m_camera->process_keyboard(ROTATE_Y_UP, delta_time);
    if (keys[GLFW_KEY_L])
        m_camera->process_keyboard(ROTATE_Y_DOWN, delta_time);
    if (keys[GLFW_KEY_U])
        m_camera->process_keyboard(ROTATE_Z_UP, delta_time);
    if (keys[GLFW_KEY_O])
        m_camera->process_keyboard(ROTATE_Z_DOWN, delta_time);
}

void GUI::refresh()
{
    m_inspector_window->refresh();
    m_slice_window->refresh();
    x_slider->setValue(m_camera->cameraAngle.x);
    y_slider->setValue(m_camera->cameraAngle.y);
    z_slider->setValue(m_camera->cameraAngle.z);
}

void GUI::draw()
{
    m_nanogui_screen->drawWidgets();
}
