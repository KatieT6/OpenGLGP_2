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


unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
    /*  Dane modelu  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    Shader shader;
    glm::vec4 color;
    /*  Funkcje   */
    Model(const char* path, const glm::mat4* instanceMatrices, const unsigned int instanceCount)
    {
        color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        loadModel(path);
    }

    Model(Mesh mesh, const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		shader = Shader(vertexPath, fragmentPath);
        color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		meshes.push_back(mesh);
	}

    void updateMeshes(std::vector<Mesh> meshes)
	{
		this->meshes = meshes;
	}

    void setColor(glm::vec4 color)
	{
		this->color = color;
	}

    void Draw(Transform parent, Transform* local, glm::mat4 projection, glm::mat4 view, bool dirty);
    void Draw(glm::mat4 projection, glm::mat4 view, glm::mat4 local);
    void DrawInstanced(glm::mat4 projection, glm::mat4 view, glm::mat4 local, Shader& shader, unsigned int instanceCount);
private:
    /*  Funkcje   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4* instanceMatrices, unsigned int instanceCount);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H

