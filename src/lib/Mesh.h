#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Shader.h>
#include <string>
#include <glm/ext/scalar_constants.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
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
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void 
    (Shader shader);
    void DrawInstanced(Shader shader, int count);
private:
    /*  Dane renderowania  */
    unsigned int VBO, EBO;
    unsigned int instanceMatricesBuffer = 0;
    /*  Funkcje    */
    void setupMesh();
    //void setupMeshInstanced(unsigned int instanceCount);
};

#endif // MESH_H
