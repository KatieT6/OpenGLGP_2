#include <vector>
#include <Shader.h>

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
    /*  Funkcje  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader shader);
private:
    /*  Dane renderowania  */
    unsigned int VAO, VBO, EBO;
    /*  Funkcje    */
    void setupMesh();
};

