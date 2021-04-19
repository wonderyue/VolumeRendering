#pragma once

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <utility>
#include <cmath>

class Slice
{
public:
    glm::vec3 max_bound = glm::vec3(INT_MIN);
    glm::vec3 min_bound = glm::vec3(INT_MAX);
    
    //vao vector
    std::vector<glm::vec3> vao_vertices;
    //veo vector
    std::vector<unsigned int> veo_indices;

    GLuint vao, vbo, ebo;

public:
    Slice(const std::vector<glm::vec3>& vertices, const glm::vec3& normal) {
        assert(vertices.size() >= 3);
        init(vertices, normal);
    };

    ~Slice() {};
    
    void init(const std::vector<glm::vec3>& vertices, const glm::vec3& normal)
    {
        vao_vertices.clear();
        veo_indices.clear();
        glm::vec3 center = glm::vec3();
        for (const auto& v : vertices)
        {
            center += v;
        }
        center /= vertices.size();
        glm::vec3 standard_edge = vertices[0] - center;
        std::vector<std::pair<float, glm::vec3>> angles;
        angles.push_back(std::make_pair(.0f, vertices[0]));
        for (int i = 1; i < vertices.size(); i++)
        {
            glm::vec3 edge = vertices[i] - center;
            float angle = acos(glm::dot(standard_edge, edge));
            //If cross(standard_edge, edge) direction opposites the normal, it means that the angle is negtive
            if (glm::dot(glm::cross(standard_edge, edge), normal) < 0)
            {
                angle = 2*M_PI - angle;
            }
            angles.push_back(std::make_pair(angle, vertices[i]));
        }
        std::sort(angles.begin(), angles.end(), [](const auto& a, const auto& b) -> bool
        {
            return a.first > b.first;
        });
        for (int i = 0; i < angles.size(); i++)
        {
            vao_vertices.push_back(angles[i].second);
            veo_indices.push_back(i);
            veo_indices.push_back((i+1)%angles.size());
            veo_indices.push_back(angles.size()); // center
        }
        vao_vertices.push_back(center);
    };
};
