#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <GameObject.h>
#include <Lights.h>
#include <Transform.h>
#include <Sphere.h>
#include <Elipse.h>
#include <SkyBox.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"


#pragma region helper functions
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(char const* path);
void processInput(GLFWwindow* window, bool cursorMode);
void switchCursorMode(GLFWwindow* window, bool active);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drawOrbit(GameObject* parentObject, int radius, int noOfVertices);

void MoveDomek(int index);
void MoveDach(int index);

bool init();
void init_imgui(GLFWwindow* window);
void imgui_render();
void do_Movement();
bool cursorActive = true;
#pragma endregion

// settings
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 800;
int rows = 10, cols = 10;
int amount = rows * cols;

glm::mat4* houseModelMatrices;
glm::mat4* roofModelMatrices;

GameObject ROOT("ROOT");

#pragma region zmienne do transformacji itp
bool moveDomek = false;
bool moveDach = false;

int indexDomek = 0;
float transformX = 0;
float transformY = 0;
float transformZ = 0;

float rotateX = 0;
float rotateY = 0;
float rotateZ = 0;

float scaleX = 1;
float scaleY = 1;
float scaleZ = 1;

int indexDach = 0;

float transformXDach = 0;
float transformYDach = 0;
float transformZDach = 0;

float rotateXDach = 0;
float rotateYDach = 0;
float rotateZDach = 0;

float scaleXDach = 1;
float scaleYDach = 1;
float scaleZDach = 1;
#pragma endregion


//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool show_tool_window = true;
bool show_wireframe = false;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float worldSpeed = 8.0f;
float moonSpeed = 8.0f;
int vCount = 4;

ImVec4 color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

const     char* glsl_version = "#version 460";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 6;

GLFWwindow* window = nullptr;
unsigned int texture1, texture2;


int main()
{
    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");

    init_imgui(window);

    show_tool_window = true;

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader instanceShader("res/shaders/instance.vert", "res/shaders/instance.frag");
    Shader reflectiveShader("res/shaders/reflective_shader.vert", "res/shaders/reflective_shader.frag");
   // Shader refractiveShader("res/shaders/refractive_shader.vert", "res/shaders/refractive.frag");

    Model domek("res/models/domek/domek.obj");
    Model dach("res/models/dach/dach.obj");
    Model podloga("res/models/grass/plane.obj");
    Model slonce("res/models/sun/sun.obj"); //point light
    Model cube1("res/models/light/cube.obj"); //spotlight1
    Model cube2("res/models/light/cube.obj"); //spotlight2
    Model cone("res/models/light/dir.obj"); //dirlight
    Model kadlub("res/models/plane/kadlub.obj"); //kadlub"
    Model skrzydloglowne("res/models/plane/skrzydlo-glowne.obj"); //skrzydla glowne
    Model skrzydlokierunkowe("res/models/plane/skrzydlo-kierunkowe.obj"); //skrzydla kierunkowe

    SkyBox skybox;

    ROOT.transform.setLocalPosition({ 0, 0, 0 });
    const float scale = 1;
    ROOT.transform.setLocalScale({ scale, scale, scale });

    //podloga
    std::string nazwaPodlogi = "podloga";
    ROOT.addChild(nazwaPodlogi);
    GameObject* Podloga = ROOT.getChildByName(nazwaPodlogi);
    Podloga->transform.setLocalPosition(glm::vec3({ 0, 0, 0 }));
    Podloga->transform.setLocalScale(glm::vec3({ 400, 1, 400 }));
    Podloga->update();

#pragma region samolot
    std::string nazwaKadluba = "kadlub";
    ROOT.addChild(nazwaKadluba);
    GameObject* Kadlub = ROOT.getChildByName(nazwaKadluba);

    std::string nazwaSkrzydelGlownych = "skrzydloglowne";
    ROOT.addChild(skrzydloglowne, nazwaSkrzydelGlownych);
    GameObject* SkrzydlaGlowne = ROOT.getChildByName(nazwaSkrzydelGlownych);

    std::string nazwaSkrzydelKierunkowych = "skrzydlokierunkowe";
    ROOT.addChild(skrzydlokierunkowe, nazwaSkrzydelKierunkowych);
    GameObject* SkrzydlaKierunkowe = ROOT.getChildByName(nazwaSkrzydelKierunkowych);


#pragma endregion samolot

#pragma region swiatelka
    //Reflektor1
    string namespotlight = "spotlight1";
    ROOT.addChild(namespotlight);
    GameObject* Spotlight1 = ROOT.getChildByName(namespotlight);
    Spotlight1->transform.setLocalPosition(glm::vec3({ posSpot1X, posSpot1Y, posSpot1Z}));
    Spotlight1->transform.setLocalScale(glm::vec3({ 0.5, 0.5, 0.5 }));
    Spotlight1->update();

    //Reflektor2
    string namespotlight2 = "spotlight2";
    ROOT.addChild(namespotlight2);
    GameObject* Spotlight2 = ROOT.getChildByName(namespotlight2);
    Spotlight2->transform.setLocalPosition(glm::vec3({ posSpot2X,posSpot2Y,posSpot2Z }));
    Spotlight2->transform.setLocalScale(glm::vec3({ 0.5, 0.5, 0.5 }));
    Spotlight2->update();

    //Kierunkowe
    string namedirlight = "dirlight";
    ROOT.addChild(namedirlight);
    GameObject* Dirlight = ROOT.getChildByName(namedirlight);
    Dirlight->transform.setLocalPosition(glm::vec3({ 0,5,0 }));
    Dirlight->transform.setLocalScale(glm::vec3({ 1, 1, 1 }));
    Dirlight->update();

    //Punktowe
    string namepointlight = "pointLight";
    ROOT.addChild(namepointlight);
    GameObject* Pointlight = ROOT.getChildByName(namepointlight);

#pragma endregion swiatelka

    // Instanced matrices setup


    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    houseModelMatrices = new glm::mat4[amount];
    roofModelMatrices = new glm::mat4[amount];

    float offset = 10.0f;
    int index = 0;

    for (unsigned int i = 0; i < cols; i++)
    {
        for (unsigned int j = 0; j < rows; j++)
        {
            //domek
            std::string nazwaDomku = "Domek nr " + std::to_string(index);
            ROOT.addChild(nazwaDomku);
            GameObject* Domek = ROOT.getChildByName(nazwaDomku);
            std::string nazwaDachu = "Dach nr " + std::to_string(index);
            Domek->addChild(dach, nazwaDachu);
            GameObject* Dach = Domek->getChildByName(nazwaDachu);
            float x = i * offset;
            float y = 0;
            float z = j * offset;
            Domek->transform.setLocalPosition(glm::vec3(x, y, z));
            Domek->transform.setLocalScale(glm::vec3(1));
            Domek->forceUpdate();
            houseModelMatrices[index] = Domek->transform.getModelMatrix();

            //dach
            

            x = 0;
            y = 2;
            z = 0;
            Dach->transform.setLocalPosition(glm::vec3(x, y, z));
            Dach->transform.setLocalScale(glm::vec3(1));
            Dach->forceUpdate();
            Domek->update();
            roofModelMatrices[index] = Dach->transform.getModelMatrix();

            index++;
        }
    }


     
    #pragma region  configure instanced array
    // vertex Buffer Object
    unsigned int houseBuffer;
    glGenBuffers(1, &houseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, houseBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &houseModelMatrices[0], GL_STATIC_DRAW);



    for (unsigned int i = 0; i < domek.meshes.size(); i++)
    {
        unsigned int VAO = domek.meshes[i].VAO;
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, houseBuffer);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    unsigned int roofBuffer;
    glGenBuffers(1, &roofBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &roofModelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < dach.meshes.size(); i++)
    {
        unsigned int VAO = dach.meshes[i].VAO;
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
#pragma endregion

        // render loop
        // -----------

    GLfloat currentFrame = 0.0f;

    reflectiveShader.setInt("texture1", 0);
    reflectiveShader.setInt("skybox", 1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, &cursorActive);

        glClearColor(10.0f / 255.0f, 2.0f / 255.0f, 28.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        instanceShader.use();
        instanceShader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));


        DirLight dirLight = {
                glm::vec3(dirLightX, dirLightY, dirLightZ),
                ambientD,
                diffuseD,
                specularD,
                dirON
        };
        instanceShader.setVec3("dirLight.direction", dirLight.direction);
        instanceShader.setVec3("dirLight.ambient", dirLight.ambient);
        instanceShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        instanceShader.setVec3("dirLight.specular", dirLight.specular);
        instanceShader.setBool("dirLight.enabled", dirLight.enabled);

        SpotLight spotLights[2] = {
            //reflektor1
            {
                    glm::vec3(posSpot1X, posSpot1Y, posSpot1Z),  // pozycja
                    glm::vec3(spotlight1X, spotlight1Y, spotlight1Z), // kierunek
                    glm::cos(glm::radians(12.5f)),  // cutOff (wewnêtrzny k¹t)
                    glm::cos(glm::radians(17.5f)),  // outerCutOff (zewnêtrzny k¹t)
                    ambientS1, // ambient
                    diffuseS1,    // diffuse
                    specularS1,     // specular
                    spot1ON
            },
            //reflektor2
            {
                    glm::vec3(posSpot2X, posSpot2Y, posSpot2Z), // pozycja
                    glm::vec3(spotlight2X, spotlight2Y, spotlight2Z),  // kierunek
                    glm::cos(glm::radians(15.0f)), // cutOff
                    glm::cos(glm::radians(20.0f)), // outerCutOff
                    ambientS2,// ambient
                    diffuseS2,   // diffuse
                    specularS2,    // specular
                    spot2ON
            }
        };

        for (int i = 0; i < 2; i++) {
            std::string number = std::to_string(i);
            glUniform3fv(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].position").c_str()), 1, &spotLights[i].position[0]);
            glUniform3fv(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].direction").c_str()), 1, &spotLights[i].direction[0]);
            glUniform1f(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].cutOff").c_str()), spotLights[i].cutOff);
            glUniform1f(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].outerCutOff").c_str()), spotLights[i].outerCutOff);

            glUniform3fv(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].ambient").c_str()), 1, &spotLights[i].ambient[0]);
            glUniform3fv(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].diffuse").c_str()), 1, &spotLights[i].diffuse[0]);
            glUniform3fv(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].specular").c_str()), 1, &spotLights[i].specular[0]);
            glUniform1i(glGetUniformLocation(instanceShader.ID, ("spotLights[" + number + "].enabled").c_str()), spotLights[i].enabled);

        }

        //punktowe
        float t = glfwGetTime(); // czas, który up³yn¹³ od uruchomienia programu
        float r = 10.0f;         // promieñ okrêgu
        float cx = 5.0f;        // X œrodka okrêgu
        float cz = 5.0f;        // Z œrodka okrêgu

        //magiczna matematyka ruchu po okregu
        float x = cx + r * cos(t);
        float z = cz + r * sin(t);

        float a, b, c;

        if (pointON) {
            a = 0;
            b = 0;
            c = 0.032f;
        }
        else {
            a = 1;
            b = 1;
            c = 1;
        }

        PointLight pointLights[1] = {
                glm::vec3(x, 5.0f, z), // Pozycja
                a, b, c,         // Sta³a, liniowa i kwadratowa sk³adowa t³umienia
                ambientP, // Ambient
                diffuseP,    // Diffuse
                specularP,     // Specular
                pointON
        };
        for (int i = 0; i < 1; i++) {
			std::string number = std::to_string(i);
        glUniform3fv(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].position").c_str()), 1, &pointLights[i].position[0]);
        glUniform1f(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].constant").c_str()), pointLights[i].constant);
        glUniform1f(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].linear").c_str()), pointLights[i].linear);
        glUniform1f(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].quadratic").c_str()), pointLights[i].quadratic);
        glUniform3fv(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].ambient").c_str()), 1, &pointLights[i].ambient[0]);
        glUniform3fv(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].diffuse").c_str()), 1, &pointLights[i].diffuse[0]);
        glUniform3fv(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].specular").c_str()), 1, &pointLights[i].specular[0]);
        glUniform1i(glGetUniformLocation(instanceShader.ID, ("pointLights[" + number + "].enabled").c_str()), pointLights[i].enabled);
        }


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);

        ROOT.update();

        if (moveDomek)
        {
            MoveDomek(indexDomek);
            // Aktualizuj buffer z model matrices
            glBindBuffer(GL_ARRAY_BUFFER, houseBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, amount * sizeof(glm::mat4), &houseModelMatrices[0]);
            glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, amount * sizeof(glm::mat4), &roofModelMatrices[0]);

            printf("Domek nr %d\n", indexDomek);

            moveDomek = false;
        }

        if (moveDach)
		{
			MoveDach(indexDach);
			// Aktualizuj buffer z model matrices
			glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, amount * sizeof(glm::mat4), &roofModelMatrices[0]);

			printf("Dach nr %d\n", indexDach);

			moveDach = false;
		}

        instanceShader.setBool("isInstanced", false);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, podloga.textures_loaded[0].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        instanceShader.setInt("texture_diffuse1", 0);

        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setMat4("model", Podloga->transform.getModelMatrix());

        for (unsigned int i = 0; i < podloga.meshes.size(); i++) {
            unsigned int VAO = podloga.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(podloga.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);


        reflectiveShader.use();
        reflectiveShader.setMat4("projection", projection);
        reflectiveShader.setMat4("view", view);

        Kadlub->transform.setLocalScale({ 5,5,5 });
        Kadlub->transform.setLocalPosition({ 0,2,0 });

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, kadlub.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);
        Kadlub->transform.setLocalPosition({ posSpot1X, posSpot1Y, posSpot1Z });
        reflectiveShader.setMat4("projection", projection);
        reflectiveShader.setMat4("view", view);
        reflectiveShader.setMat4("model", Kadlub->transform.getModelMatrix());


        for (unsigned int i = 0; i < kadlub.meshes.size(); i++) {
            unsigned int VAO = kadlub.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(kadlub.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        reflectiveShader.use();
        reflectiveShader.setMat4("projection", projection);
        reflectiveShader.setMat4("view", view);

        SkrzydlaGlowne->transform.setLocalScale({ 5,5,5 });
        SkrzydlaGlowne->transform.setLocalPosition({ 0,2,0 });

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skrzydloglowne.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);
        SkrzydlaGlowne->transform.setLocalPosition({ posSpot1X, posSpot1Y, posSpot1Z });
        reflectiveShader.setMat4("projection", projection);
        reflectiveShader.setMat4("view", view);
        reflectiveShader.setMat4("model", SkrzydlaGlowne->transform.getModelMatrix());


        for (unsigned int i = 0; i < skrzydloglowne.meshes.size(); i++) {
            unsigned int VAO = skrzydloglowne.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(skrzydloglowne.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);


#pragma region swiatla
        //reflektor1

        instanceShader.use();
        instanceShader.setBool("isInstanced", false);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube1.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);
        Spotlight1->transform.setLocalPosition({ posSpot1X, posSpot1Y, posSpot1Z });
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setMat4("model", Spotlight1->transform.getModelMatrix());


        for (unsigned int i = 0; i < cube1.meshes.size(); i++) {
            unsigned int VAO = cube1.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(cube1.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        //reflektor2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube2.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);
        Spotlight2->transform.setLocalPosition({ posSpot2X, posSpot2Y, posSpot2Z });

        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setMat4("model", Spotlight2->transform.getModelMatrix());


        for (unsigned int i = 0; i < cube2.meshes.size(); i++) {
            unsigned int VAO = cube2.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(cube2.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        //kierunkowe
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube2.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);
        Dirlight->transform.setLocalRotation({ dirLightX, dirLightY, dirLightZ });

        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setMat4("model", Dirlight->transform.getModelMatrix());


        for (unsigned int i = 0; i < cone.meshes.size(); i++) {
            unsigned int VAO = cone.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(cone.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        //punktowe
        Pointlight->transform.setLocalScale({ 0.1,0.1,0.1 });
        Pointlight->transform.setLocalPosition({ x, 5.0f, z });
        Pointlight->update();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, slonce.textures_loaded[0].id);
        instanceShader.setInt("texture_diffuse1", 0);

        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setMat4("model", Pointlight->transform.getModelMatrix());


        for (unsigned int i = 0; i < slonce.meshes.size(); i++) {
            unsigned int VAO = slonce.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(slonce.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);


        instanceShader.setBool("isInstanced", true);

        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, domek.textures_loaded[0].id);
        for (unsigned int i = 0; i < domek.meshes.size(); i++)
        {
            unsigned int VAO = domek.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(domek.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

#pragma endregion

#pragma region rysowanie-domkow

        //samo renderowanie wszystkich dachow
        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dach.textures_loaded[0].id);
        for (unsigned int i = 0; i < dach.meshes.size(); i++)
        {
            unsigned int VAO = dach.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(dach.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);

            glBindVertexArray(0);
        }
#pragma endregion

        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        skybox.DrawSkyBox(view, projection);

        //switch cursore mode on tab
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		{
			cursorActive = !cursorActive;
			switchCursorMode(window, cursorActive);
		}

        if (show_tool_window)
        {
            imgui_render();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

void do_Movement()
{
    GLfloat cameraSpeed = 2.5f * deltaTime;
};

//void drawOrbit(GameObject* parentObject, int radius, int noOfVertices)
//{
//    Shader orbitShader("res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
//    Mesh orbitMesh = buildElipse(0, 0, radius, radius, noOfVertices, orbitShader);
//    Model* orbitModel = new Model(orbitMesh, "res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
//    GameObject* orbitObject = new GameObject(orbitModel);
//    parentObject->addChild(orbitObject);
//};
void MoveDomek(int index) {
    for (unsigned int i = 0; i < amount; i++)
	{
        GameObject* Domek = ROOT.getChildByName("Domek nr " + std::to_string(i));
		if (i == index)
		{
            Domek->transform.setLocalPosition({ transformX, transformY, transformZ });
            Domek->transform.setLocalRotation({ rotateX, rotateY, rotateZ });
            Domek->transform.setLocalScale({ scaleX, scaleY, scaleZ });
            Domek->update();
            printf("Domek nr %d\n", i);
        }

        houseModelMatrices[i] = Domek->transform.getModelMatrix();
        roofModelMatrices[i] = Domek->getChildByName("Dach nr " + to_string(i))->transform.getModelMatrix();
	}
}

void MoveDach(int index) {
		    GameObject* Domek = ROOT.getChildByName("Domek nr " + std::to_string(index));
        std:string name = "Dach nr " + std::to_string(index);
            GameObject* Dach = Domek->getChildByName(name);
			Dach->transform.setLocalPosition({ transformXDach, transformYDach, transformZDach });
			Dach->transform.setLocalRotation({ rotateXDach, rotateYDach, rotateZDach });
			Dach->transform.setLocalScale({ scaleXDach, scaleYDach, scaleZDach });
			Dach->update();
			printf("Dach nr %d\n", index);

		roofModelMatrices[index] = Dach->transform.getModelMatrix();
}



void processInput(GLFWwindow* window, bool cursorMode)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


bool init()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        spdlog::error("Failed to initalize GLFW!");
        return false;
    }

    // GL 4.6 + GLSL 460
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with graphics context
   // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sierpinsky Pyramid", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    return true;
}

void init_imgui(GLFWwindow* window)
{
    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


}

void imgui_render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::PushItemWidth(80);
    ImGui::Begin("Niez³e domki");    

    //zmiana pozycji domków
ImGui::Text("Transformacja domkow");
	ImGui::SliderInt("Index", &indexDomek, 0, amount - 1);
    ImGui::InputFloat("X ", &transformX);
    ImGui::InputFloat("Y ", &transformY);
    ImGui::InputFloat("Z ", &transformZ);

	ImGui::Text("Rotacja domkow");
    ImGui::InputFloat("Rotation X", &rotateX);
    ImGui::InputFloat("Rotation Y", &rotateY);
    ImGui::InputFloat("Rotation Z", &rotateZ);

	ImGui::Text("Skalowanie domkow");
    ImGui::InputFloat("Scale X", &scaleX);
    ImGui::InputFloat("Scale Y", &scaleY);
    ImGui::InputFloat("Scale Z", &scaleZ);

    if (ImGui::Button("Zatwierdz domki"))
	    {
		    moveDomek = true;
        }

    ImGui::Text("Transformacja Dachów");
    ImGui::SliderInt("Inde xDach", &indexDach, 0, amount - 1);
	ImGui::InputFloat("X", &transformXDach);
	ImGui::InputFloat("Y", &transformYDach);
	ImGui::InputFloat("Z", &transformZDach);

	ImGui::Text("Rotacja Dachów");
    ImGui::InputFloat("Rotation X", &rotateXDach);
    ImGui::InputFloat("Rotation Y", &rotateYDach);
    ImGui::InputFloat("Rotation Z", &rotateZDach);

	ImGui::Text("Skalowanie Dachów");
    ImGui::InputFloat("Scale X", &scaleXDach);
    ImGui::InputFloat("Scale Y", &scaleYDach);
    ImGui::InputFloat("Scale Z", &scaleZDach);

if (ImGui::Button("Zatwierdz dachy"))
	{
        moveDach = true;
        printf("Dach nr %d\n", indexDach);
	}

    ImGui::Text("Zmiana swiatelek");
    ImGui::Text("Directional");
	ImGui::SliderFloat("DirLightX", &dirLightX, -1, 1);
	ImGui::SliderFloat("DirLightY", &dirLightY, -1, 1);
	ImGui::SliderFloat("DirLightZ", &dirLightZ, -1, 1);
    //ustawianie koloru

    ImGui::ColorEdit3("ambientD", (float*)&ambientD);
	ImGui::ColorEdit3("diffuseD", (float*)&diffuseD);
    ImGui::ColorEdit3("specularD", (float*)&specularD);

	ImGui::Text("SpotLight1");
	ImGui::SliderFloat("SpotLight1 X direction", &spotlight1X, -10, 10);
	ImGui::SliderFloat("SpotLight1 Y direction", &spotlight1Y, -10, 10);
	ImGui::SliderFloat("SpotLight1 Z direction", &spotlight1Z, -10, 10);
    //posiotion 
ImGui::SliderFloat("SpotLight1 X position", &posSpot1X, -10, 10);
	ImGui::SliderFloat("SpotLight1 Y position", &posSpot1Y, -10, 10);
	ImGui::SliderFloat("SpotLight1 Z position", &posSpot1Z, -10, 10);

	ImGui::ColorEdit3("ambientS1", (float*)&ambientS1);
	ImGui::ColorEdit3("diffuseS1", (float*)&diffuseS1);
	ImGui::ColorEdit3("specularS1", (float*)&specularS1);

	ImGui::Text("SpotLight2");
	ImGui::SliderFloat("SpotLight2 X direction", &spotlight2X, -10, 10);
	ImGui::SliderFloat("SpotLight2 Y direction", &spotlight2Y, -10, 10);
	ImGui::SliderFloat("SpotLight2 Z direction", &spotlight2Z, -10, 10);
	//posiotion 
	ImGui::SliderFloat("SpotLight2 X position", &posSpot2X, -10, 10);
	ImGui::SliderFloat("SpotLight2 Y position", &posSpot2Y, -10, 10);
	ImGui::SliderFloat("SpotLight2 Z position", &posSpot2Z, -10, 10);

	ImGui::ColorEdit3("ambientS2", (float*)&ambientS2);
	ImGui::ColorEdit3("diffuseS2", (float*)&diffuseS2);
	ImGui::ColorEdit3("specularS2", (float*)&specularS2);

	ImGui::Checkbox("DirLightON", &dirON);
	ImGui::Checkbox("SpotLight1ON", &spot1ON);
	ImGui::Checkbox("SpotLight2ON", &spot2ON);
	ImGui::Checkbox("PointLightON", &pointON);

   /* ImGui::SliderFloat("World rotation speed", &worldSpeed, 0, 15);
    ImGui::SliderFloat("Moon rotation speed", &moonSpeed, 0, 15);
    ImGui::SliderInt("Sphere sectors count ", &vCount, 1, 36);*/
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void switchCursorMode(GLFWwindow* window, bool active)
{
    if (active) {
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
    else {
        glfwSetCursorPosCallback(window, NULL);
        glfwSetScrollCallback(window, NULL);
    }
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


