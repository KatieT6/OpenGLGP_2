#include <Mesh.h>

Mesh buildElipse(float c_x,  float c_z, float r_x, float r_z, int segmentsNum, Shader& shader) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

	float theta = 2 * 3.1415926 / float(segmentsNum);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = 1;//we start at angle = 0 
    float z = 0; 

    glBegin(GL_LINE_LOOP); 
    for(int i = 0; i < segmentsNum; ++i)
    { 
        Vertex vertex = {};
        vertex.Position = glm::vec3(x * r_x + c_x, 0.0f, z * r_z + c_z);

        //apply the rotation matrix
        t = x;
        x = c * x - s * z;
        z = s * t + c * z;

        vertices.emplace_back(vertex);
        indices.emplace_back(i);
    } 
    glEnd(); 

    return Mesh(vertices, indices, textures);
}