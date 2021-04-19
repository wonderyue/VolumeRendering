#pragma once

#include <nanogui/nanogui.h>
#include "Enums.h"
#include "Camera.h"

class GUI
{
public:
    GUI(GLFWwindow* window, Camera* camera):
    m_gl_window(window),
    m_camera(camera),
    m_slice_window(NULL)
    {};
    ~GUI();

    void init(const std::function<void(model_type)> &sel_model, const std::function<void()> &reset);
    void setObject();
    void process_keyboard(float delta_time);
    void refresh();
    void draw();
    
    static bool keys[1024];
    int get_render_mode() {
        return render_type_2_gl_type[m_render_val];
    }
    model_type get_model_val() {
        return m_model_val;
    }
    nanogui::Color& get_obj_color_val() {
        return m_obj_col_val;
    }
    uint get_samples_per_unit() {
        return m_samples_per_unit;
    }
    float get_visible_samples() {
        return m_visible_samples;
    }
    const std::vector<float>& get_transfer_values() {
        return m_transfer_values;
    }
    
private:
    static nanogui::Screen* m_nanogui_screen;
    GLFWwindow* m_gl_window;
    Camera* m_camera;
    nanogui::FormHelper* m_inspector_window;
    nanogui::FormHelper* m_slice_window;
    model_type m_model_val = TEAPOT;
    render_type m_render_val = TRIANGLE;
    nanogui::Color m_obj_col_val = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    uint m_samples_per_unit = 8;
    float m_visible_samples = 1.0f;
    std::vector<float> m_transfer_values = {0.1f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f};
    std::function<void(model_type)> m_model_sel_callback;
    std::function<void()> m_reset_callback;
    nanogui::Slider *x_slider;
    nanogui::Slider *y_slider;
    nanogui::Slider *z_slider;
    nanogui::Slider *m_sample_slider;
    std::vector<nanogui::Slider*> m_transfer_slider;
};
