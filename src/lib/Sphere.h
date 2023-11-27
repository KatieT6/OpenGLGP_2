#include <Mesh.h>


Mesh buildSphere(float radius, int verticesCount)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = verticesCount;
    const unsigned int Y_SEGMENTS = verticesCount;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
            float ySegment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
            float xPos = radius * std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
            float yPos = radius * std::cos(ySegment * glm::pi<float>());
            float zPos = radius * std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

            // Assuming your Vertex struct has appropriate members (Position, Normal, TexCoords)
            Vertex vertex;
            vertex.Position = glm::vec3(xPos, yPos, zPos);
            vertex.Normal = glm::normalize(vertex.Position);
            vertex.TexCoords = glm::vec2(xSegment, ySegment);

            vertices.push_back(vertex);
        }
    }

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x)
        {
            unsigned int index = y * (X_SEGMENTS + 1) + x;
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + X_SEGMENTS + 1);
            indices.push_back(index + 1);
            indices.push_back(index + X_SEGMENTS + 2);
            indices.push_back(index + X_SEGMENTS + 1);
        }
    }

    return Mesh(vertices, indices, std::vector<Texture>());  // Assuming 'textures' is a member variable of Mesh
}