#pragma once

enum render_type {
    LINE = 0,
    TRIANGLE = 1
};
enum model_type {
    TEAPOT = 0,
    BONSAI = 1,
    HEAD = 2,
    BUCKY = 3,
};

const std::vector<int> render_type_2_gl_type = {GL_LINE, GL_FILL};
