#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>


class Transform
{
public:
    //lokalna pozycja
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f }; //w stopniach
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    //pozycja globalna w macierzy
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    //brudna flaga
    bool m_isDirty = true;


    //zwroc lokalna macierz
    glm::mat4 getLocalModelMatrix()
    {
        //macierze obrotow
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

        //polaczenie macierzy obrotow
        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        //polaczenie wszystkich macierzy w jedna
        return glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }


    //po zmianach ustaw brudna flage na false
    void computeModelMatrix()
    {
        m_modelMatrix = getLocalModelMatrix();
        m_isDirty = false;
    }

    void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        m_isDirty = false;
    }

    //zmien dana rzecz i ustaw brudna flage na true, co oznacza, ze wymagana jest aktualizacja
    void setLocalPosition(const glm::vec3& newPosition)
    {
        position = newPosition;
        m_isDirty = true;
    }

    void setLocalRotation(const glm::vec3& newRotation)
    {
        eulerRot = newRotation;
        m_isDirty = true;
    }

    void setLocalScale(const glm::vec3& newScale)
    {
        scale = newScale;
        m_isDirty = true;
    }

    //gettery na lokalne i globalne
    const glm::vec3& getGlobalPosition() const
    {
        return m_modelMatrix[3];
    }

    const glm::vec3& getLocalPosition() const
    {
        return position;
    }

    const glm::vec3& getLocalRotation() const
    {
        return eulerRot;
    }

    const glm::vec3& getLocalScale() const
    {
        return scale;
    }

    const glm::mat4& getModelMatrix() const
    {
        return m_modelMatrix;
    }

    glm::vec3 getRight() const
    {
        return m_modelMatrix[0];
    }

    glm::vec3 getUp() const
    {
        return m_modelMatrix[1];
    }

    glm::vec3 getBackward() const
    {
        return m_modelMatrix[2];
    }

    glm::vec3 getForward() const
    {
        return -m_modelMatrix[2];
    }

    //sprawdzenie stanu flagi
    bool isDirty() const
    {
        return m_isDirty;
    }

};

#endif // !TRANSFORM_H
