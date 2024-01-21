#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <Shader.h>
#include <iostream>
#include <string>
#include <Transform.h>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
using namespace std;


unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
    /*  Dane modelu  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    bool gammaCorrection;
    /*  Funkcje   */
    Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    Model(Mesh mesh, const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		meshes.push_back(mesh);
	}

    void updateMeshes(std::vector<Mesh> meshes)
	{
		this->meshes = meshes;
	}

    void Draw(Shader shader);

    //void DrawInstanced(Shader& shader, unsigned int instanceCount);
private:
    /*  Funkcje   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H

