#include "ResourceManager.h"
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <limits>


const std::vector<ResourceManager::ModelData> model_type_2_data = {
    ResourceManager::ModelData("texture/BostonTeapot_256_256_178.raw", glm::vec3(256, 256,  178), glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(0.0f, -90.0f,  90.0f)),
    ResourceManager::ModelData("texture/Bonsai_512_512_154.raw", glm::vec3(512, 512,  154), glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(0.0f, -90.0f,  90.0f)),
    ResourceManager::ModelData("texture/Head_256_256_225.raw", glm::vec3(256, 256,  225), glm::vec3(-1.0f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f,  0.0f)),
    ResourceManager::ModelData("texture/Bucky_32_32_32.raw", glm::vec3(32, 32,  32), glm::vec3(0.5f, 0.5f, 2.0f), glm::vec3(0.0f, -90.0f,  90.0f)),
};

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    clear();
}

void ResourceManager::clear()
{
    for (auto slice : m_slices) {
        delete slice;
    }
    m_slices.clear();
}

void ResourceManager::gen_slices(const glm::vec4& normal, uint samples_per_unit)
{
    clear();
    float min_d = std::numeric_limits<float>::max();
    float max_d = std::numeric_limits<float>::min();
    //range of d
    for (int i = 0; i < 8; i++)
    {
        float d = -glm::dot(normal, cube_vertices[i]);
        min_d = fmin(min_d, d);
        max_d = fmax(max_d, d);
    }
    int samples = (max_d-min_d)*samples_per_unit;
    for (int j = 0; j < samples; j++) {
        float d = min_d + (max_d-min_d) / samples * j;
        //intersections with edges
        std::vector<glm::vec3> intersections;
        for (int i = 0; i < 12; i++)
        {
            glm::vec4 a = cube_vertices[cube_edges[i*2]];
            glm::vec4 b = cube_vertices[cube_edges[i*2+1]];
            //p = (1-t)*a+t*b => t = (a·normal+d) / (a-b)·normal
            float t = (glm::dot(normal, a) + d) / glm::dot(normal, a - b);
            if (t > 0 && t < 1) {
                glm::vec3 p = (1-t)*a+t*b;
                intersections.push_back(p);
            }
        }
        if (intersections.size() >= 3)
        {
            Slice* slice = new Slice(intersections, normal);
            init_slice(slice);
            m_slices.push_back(slice);
        }
    }
}

void ResourceManager::init_slice(Slice* slice)
{
    glGenVertexArrays(1, &slice->vao);
    glGenBuffers(1, &slice->vbo);
    glGenBuffers(1, &slice->ebo);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(slice->vao);
    // Bind vertices to Array Buffer
    glBindBuffer(GL_ARRAY_BUFFER, slice->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * slice->vao_vertices.size(), &slice->vao_vertices[0], GL_STATIC_DRAW);
    // Bind indices to Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slice->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * slice->veo_indices.size(), &slice->veo_indices[0], GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Unbind VAO
    glBindVertexArray(0);
}

Texture* ResourceManager::load_2d_texture(const std::string& path)
{
    Texture* tex = new Texture(path);
    return tex;
}

Texture* ResourceManager::get_2d_texture(const std::string& path)
{
    if (path.empty())
        return NULL;
    if (m_textures.find(path) == m_textures.end()) {
        m_textures[path] = load_2d_texture(path);
    }
    return m_textures[path];
}

Texture* ResourceManager::load_3d_texture(const std::string& path, int width, int height, int depth)
{
    Texture* tex = new Texture(path, width, height, depth);
    return tex;
}

Texture* ResourceManager::get_3d_texture(const std::string& path, int width, int height, int depth)
{
    if (path.empty())
        return NULL;
    if (m_textures.find(path) == m_textures.end()) {
        m_textures[path] = load_3d_texture(path, width, height, depth);
    }
    return m_textures[path];
}

Texture* ResourceManager::get_texture(model_type type)
{
    const ResourceManager::ModelData& data = model_type_2_data[type];
    return get_3d_texture(data.texture_path, data.dimension.x, data.dimension.y, data.dimension.z);
}

Texture* ResourceManager::get_color_bar()
{
    return get_2d_texture("texture/colorbar.png");
}


const ResourceManager::ModelData& ResourceManager::get_model_data(model_type type)
{
    return model_type_2_data[type];
}
