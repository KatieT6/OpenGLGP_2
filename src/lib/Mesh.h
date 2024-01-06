#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Shader.h>
#include <string>
#include <glm/ext/scalar_constants.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    /*  dane klasy Mesh  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;
    /*  Funkcje  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, const glm::mat4* instanceMatrices, unsigned int instanceCount);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader shader);
    void DrawInstanced(Shader shader, int count);
private:
    /*  Dane renderowania  */
    unsigned int VBO, EBO;
    unsigned int instanceMatricesBuffer = 0;
    /*  Funkcje    */
    void setupMesh(const glm::mat4* instanceMatrices, unsigned int instanceCount);
    void setupMeshInstanced(const glm::mat4* instanceMatrices, unsigned int instanceCount);
};

#endif // MESH_H
