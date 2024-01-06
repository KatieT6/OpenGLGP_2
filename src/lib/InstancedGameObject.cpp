#include "InstancedGameObject.h"

void InstancedGameObject::prepareInstanceMatrixBuffer()
{
	glGenBuffers(1, &instanceMatrixBuffer);

	std::vector<glm::mat4> instanceMatrices;
	for (const auto& transform : instanceTransforms)
		instanceMatrices.emplace_back(transform->modelMatrix);
     
	glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<int>(instanceMatrices.size()) * sizeof(glm::mat4), instanceMatrices.data(), GL_DYNAMIC_DRAW);


	for (const auto& mesh : model_ -> meshes)
	{
        unsigned int VAO = mesh.VAO;
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        
        glBindVertexArray(0);

	}
}

void InstancedGameObject::updateInstanceMatrixBuffer()
{
}

InstancedGameObject::InstancedGameObject()
{
    prepareInstanceMatrixBuffer();
}

InstancedGameObject::InstancedGameObject(Model* model, Shader* shader, std::vector<Transform*> transforms) :
    GameObject(model, shader), instanceTransforms(std::move(transforms)) {
        prepareInstanceMatrixBuffer();
}

void InstancedGameObject::draw(Transform parent, glm::mat4 projection, glm::mat4 view, bool dirty)
{
    updateInstanceMatrixBuffer();

    if (model_ != nullptr) {
        shader_->use();
        model_->DrawInstanced(instanceTransforms.size());
    }

}
