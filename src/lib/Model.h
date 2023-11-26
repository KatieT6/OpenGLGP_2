#include <vector>
#include <Shader.h>
#include <iostream>
#include <string>

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>


unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
    /*  Funkcje   */
    Model(const char* path)
    {
        loadModel(path);
    }
    void Draw(Shader& shader);
private:
    /*  Dane modelu  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    /*  Funkcje   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

