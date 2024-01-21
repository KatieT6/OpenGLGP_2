#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled;
};


float dirLightX = -0.5f;
float dirLightY = -1.0f;
float dirLightZ = -0.5f;

float spotlight1X = 1;
float spotlight1Y = 0;
float spotlight1Z = 0;

float spotlight2X = 0;
float spotlight2Y = 0;
float spotlight2Z = 1;

float posSpot1X = 5;
float posSpot1Y = 0;
float posSpot1Z = 0;

float posSpot2X = 0;
float posSpot2Y = 0;
float posSpot2Z = 5;

glm::vec3 ambientD(0.5f);
glm::vec3 diffuseD(0.4f);
glm::vec3 specularD(0.5f);

glm::vec3 ambientS1(0.05f);
glm::vec3 diffuseS1(0.8f);
glm::vec3 specularS1(1.0f);

glm::vec3 ambientS2(0.05f);
glm::vec3 diffuseS2(0.8f);
glm::vec3 specularS2(1.0f);

glm::vec3 ambientP(1);
glm::vec3 diffuseP(1);
glm::vec3 specularP(1);









bool dirON = true;
bool pointON = true;
bool spot1ON = true;
bool spot2ON = true;
