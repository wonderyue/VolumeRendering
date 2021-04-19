#pragma once

#include "Slice.h"
#include "Texture.h"
#include "Enums.h"
#include <string>
#include <unordered_map>

class ResourceManager {
    
public:
    ResourceManager();
    ~ResourceManager();
    
    struct ModelData
    {
        std::string texture_path;
        // default camera position
        glm::vec3 dimension;
        // default camera position
        glm::vec3 position;
        // default camera rotation
        glm::vec3 rotation;
        ModelData(const std::string& t, const glm::vec3& d, const glm::vec3& p, const glm::vec3& r)
        {
            texture_path = t;
            dimension = d;
            position = p;
            rotation = r;
        }
    };
    std::vector<Slice*> get_slices()
    {
        return m_slices;
    }
    void gen_slices(const glm::vec4& normal, uint samples_per_unit);
    Texture* get_texture(model_type model_type);
    Texture* get_color_bar();
    const ModelData& get_model_data(model_type model_type);
    
    glm::vec4 cube_vertices[8] =
    {
        glm::vec4(0.0, 0.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0),
        glm::vec4(0.0, 1.0, 0.0, 1.0),
        glm::vec4(0.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 0.0, 0.0, 1.0),
        glm::vec4(1.0, 0.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 0.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0)
    };

    GLuint cube_indices[36] =
    {
        1,5,7,
        7,3,1,
        0,2,6,
        6,4,0,
        0,1,3,
        3,2,0,
        7,5,4,
        4,6,7,
        2,3,7,
        7,6,2,
        1,0,4,
        4,5,1
    };

    GLuint cube_edges[24]
    {
        1,5,
        5,7,
        7,3,
        3,1,
        0,4,
        4,6,
        6,2,
        2,0,
        0,1,
        2,3,
        4,5,
        6,7
    };
    
private:
    std::vector<Slice*> m_slices;
    std::unordered_map<std::string, Texture*> m_textures;
    void init_slice(Slice* slice);
    Texture* load_2d_texture(const std::string& path);
    Texture* get_2d_texture(const std::string& path);
    Texture* load_3d_texture(const std::string& path, int width, int height, int depth);
    Texture* get_3d_texture(const std::string& path, int width, int height, int depth);
    void clear();
};
