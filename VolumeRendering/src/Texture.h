#pragma once

// NOTE: do not define STB_IMAGE_IMPLEMENTATION
//          it would conflict with nanovg.c (libnanogui.a)
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <iostream>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Texture
{
public:
    GLuint texture;
    int width, height, depth;

public:
    Texture(const std::string& tex_path)
    {
        m_tex_path = tex_path;
        load_2d_texture(m_tex_path);
    };
    
    
    Texture(const std::string& tex_path, int width, int height, int depth)
    {
        m_tex_path = tex_path;
        this->width = width;
        this->height = height;
        this->depth = depth;
        load_3d_texture(m_tex_path);
    };

    ~Texture() {};

    void load_2d_texture(const std::string& tex_path) {
        //texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_1D, texture);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //load img
        unsigned char* image = stbi_load(tex_path.c_str(), &width, &height, 0, 0);
        assert(image);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_1D);
        //free&unbind
        glBindTexture(GL_TEXTURE_1D, 0);
        stbi_image_free(image);
    }
    
    void load_3d_texture(const std::string& tex_path) {
        //texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //load img
        GLubyte* image = load_3d_raw_data(tex_path.c_str(), width, height, depth);
        assert(image);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_3D);
        //free&unbind
        glBindTexture(GL_TEXTURE_3D, 0);
        free(image);
    }
    
    GLubyte* load_3d_raw_data(std::string texture_path, int width, int height, int depth) {
        size_t size = width * height * depth;

        FILE *fp;
        GLubyte *data = new GLubyte[size]; // 8bit
        if (!(fp = fopen(texture_path.c_str(), "rb"))) {
            std::cout << "Error: opening .raw file failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fread(data, sizeof(char), size, fp) != size) {
            std::cout << "Error: read .raw file failed" << std::endl;
            exit(1);
        }
        fclose(fp);
        return data;
    }
    
private:
    std::string m_tex_path;
};
