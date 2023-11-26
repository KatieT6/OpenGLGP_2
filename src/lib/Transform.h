#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>


class Transform
{
protected:
    /*SPACE INFORMATION*/
    //Local space information
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 modelMatrix = glm::mat4(1.0f);


	Transform() = default;

protected:
    glm::mat4 getLocalModelMatrix()
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
        return glm::translate(glm::mat4(1.0f), position) *
            roationMatrix *
            glm::scale(glm::mat4(1.0f), scale);
    }

};
