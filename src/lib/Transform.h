#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>


class Transform
{
protected:
    //lokalna pozycja
    glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; //w stopniach
    glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

    //pozycja globalna w macierzy
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    //brudna flaga
    bool m_isDirty = true;

protected:

    //zwroc lokalna macierz
    glm::mat4 getLocalModelMatrix()
    {
        //macierze obrotow
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

        //polaczenie macierzy obrotow
        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        //polaczenie wszystkich macierzy w jedna
        return glm::translate(glm::mat4(1.0f), m_pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
    }
public:

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
        m_pos = newPosition;
        m_isDirty = true;
    }

    void setLocalRotation(const glm::vec3& newRotation)
    {
        m_eulerRot = newRotation;
        m_isDirty = true;
    }

    void setLocalScale(const glm::vec3& newScale)
    {
        m_scale = newScale;
        m_isDirty = true;
    }

    //gettery na lokalne i globalne
    const glm::vec3& getGlobalPosition() const
    {
        return m_modelMatrix[3];
    }

    const glm::vec3& getLocalPosition() const
    {
        return m_pos;
    }

    const glm::vec3& getLocalRotation() const
    {
        return m_eulerRot;
    }

    const glm::vec3& getLocalScale() const
    {
        return m_scale;
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
