#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // ID program object  
    unsigned int ID;

    // konstruktor czyta plik shadera z dysku i tworzy go  
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    Shader() {}
    // aktywuj shader  
    void use();
    // funkcje operuj¹ce na uniformach  
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) ;
    void setVec2(const std::string& name, float x, float y) ;
    void setVec3(const std::string& name, const glm::vec3& value) ;
    void setVec3(const std::string& name, float x, float y, float z) ;
    void setVec4(const std::string& name, const glm::vec4& value) ;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) ;
    void setMat3(const std::string& name, const glm::mat3& mat) ;
    void setMat4(const std::string& name, const glm::mat4& mat) ;
        
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif