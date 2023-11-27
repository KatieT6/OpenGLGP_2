#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>


class Transform
{
public:
    /*SPACE INFORMATION*/
    //Local space information
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    Transform() {}


    void getLocalModelMatrix(Transform& other)
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.x),
            glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.y),
            glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.z),
            glm::vec3(0.0f, 0.0f, 1.0f));

        // Y * X * Z
        const glm::mat4 roationMatrix = transformY * transformX * transformZ;

        // translation * rotation * scale (also know as TRS matrix)
        modelMatrix = glm::translate(glm::mat4(1.0f), position) *
            roationMatrix *
            glm::scale(glm::mat4(1.0f), scale);
        modelMatrix = other.modelMatrix * modelMatrix;
    }

};

#endif // !TRANSFORM_H
