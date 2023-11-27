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
#include <Light.h>
#include <Transform.h>
#include <Sphere.h>
#include <Elipse.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(char const* path);
void processInput(GLFWwindow* window, bool cursorMode);
void switchCursorMode(GLFWwindow* window, bool active);
void drawOrbit(GameObject* parentObject, int radius, int noOfVertices);

bool cursorActive = true;
bool init();
void init_imgui(GLFWwindow* window);
void imgui_render();
void do_Movement();

// settings
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 800;

const GLchar* vertexPath = "res/shaders/loadModel.vert";
const GLchar* fragmentPath = "res/shaders/loadModel.frag";

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
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


    Model* sunModel = new Model("res/models/sun/sun.obj", vertexPath, fragmentPath);
    Model* mercuryModel = new Model("res/models/mercury/mercury.obj", vertexPath, fragmentPath);
    Model* venusModel = new Model("res/models/venus/venus.obj", vertexPath, fragmentPath);
    Model* earthModel = new Model("res/models/earth/earth.obj", vertexPath, fragmentPath);
    Model* jupiterModel = new Model("res/models/jupiter/jupiter.obj", vertexPath, fragmentPath);

    Model* moonModel1 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel2 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel3 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel4 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel5 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel6 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Model* moonModel7 = new Model("res/models/moon/moon.obj", vertexPath, fragmentPath);
    Mesh sphereMesh = buildSphere(1, vCount);
    Model* sphereModel = new Model(sphereMesh, vertexPath, fragmentPath);

    GameObject* moonDriver1 = new GameObject();
    GameObject* moonDriver2 = new GameObject();
    GameObject* moonDriver3 = new GameObject();
    GameObject* moonDriver4 = new GameObject();
    GameObject* moonDriver5 = new GameObject();
    GameObject* moonDriver6 = new GameObject();
    GameObject* moonDriver7 = new GameObject();

    GameObject* moon1 = new GameObject(moonModel1);
    GameObject* moon2 = new GameObject(moonModel2);
    GameObject* moon3 = new GameObject(moonModel3);
    GameObject* moon4 = new GameObject(moonModel4);
    GameObject* moon5 = new GameObject(moonModel5);
    GameObject* moon6 = new GameObject(moonModel6);
    GameObject* moon7 = new GameObject(moonModel7);

    moonModel1->setColor(glm::vec4(0.2f, 0.1f, 0.3f, 1.0f));
    moonModel2->setColor(glm::vec4(0.5f, 0.1f, 0.3f, 1.0f));
    moonModel3->setColor(glm::vec4(0.2f, 0.1f, 0.6f, 1.0f));
    moonModel4->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 1.0f));
    moonModel5->setColor(glm::vec4(0.1f, 0.6f, 0.4f, 1.0f));
    moonModel6->setColor(glm::vec4(0.0f, 0.5f, 0.3f, 1.0f));
    moonModel7->setColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    sphereModel->setColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));

    moonDriver1->addChild(moon1);
    moonDriver2->addChild(moon2);
    moonDriver3->addChild(moon3);
    moonDriver4->addChild(moon4);
    moonDriver5->addChild(moon5);
    moonDriver6->addChild(moon6);
    moonDriver7->addChild(moon7);

    GameObject* mercuryDriver = new GameObject();
    GameObject* venusDriver = new GameObject();
    GameObject* earthDriver = new GameObject();
    GameObject* sphereDriver = new GameObject();
    GameObject* jupiterDriver = new GameObject();

    GameObject* Sun = new GameObject(sunModel);
    Sun->setLocalScale(glm::vec3(0.6f, 0.6f, 0.6f));    

    GameObject* Mercury = new GameObject(mercuryModel);    
    GameObject* Venus = new GameObject(venusModel);
    GameObject* Earth = new GameObject(earthModel);
    GameObject* sphere = new GameObject(sphereModel);
    GameObject* Jupiter = new GameObject(jupiterModel);

   // GameObject* mercury = new GameObject(mercuryModel);

    Sun->addChild(jupiterDriver);
    Sun->addChild(sphereDriver);
    Sun->addChild(earthDriver);
    Sun->addChild(venusDriver);
    Sun->addChild(mercuryDriver);

    Mercury->setLocalPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    Venus->setLocalPosition(glm::vec3(15.0f, 0.0f, 0.0f));
    Earth->setLocalPosition(glm::vec3(30.0f, 0.0f, 0.0f));
    sphere->setLocalPosition(glm::vec3(35.0f, 0.0f, 0.0f));
    Jupiter->setLocalPosition(glm::vec3(50.0f, 0.0f, 0.0f));

    mercuryDriver->addChild(Mercury);
    venusDriver->addChild(Venus);
    earthDriver->addChild(Earth);
    sphereDriver->addChild(sphere);
    jupiterDriver->addChild(Jupiter);

    Jupiter->addChild(moonDriver1);
    sphere->addChild(moonDriver2);
    sphere->addChild(moonDriver3);
    Earth->addChild(moonDriver4);
    Earth->addChild(moonDriver5);
    Earth->addChild(moonDriver6);
    Venus->addChild(moonDriver7);

    //jupiter moon
    moon1->setLocalPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    moon1->setLocalScale(glm::vec3(0.2f, 0.2f, 0.2f));
    //sphere moon
    moon2->setLocalPosition(glm::vec3(4.0f, 0.0f, 0.0f));
    moon2->setLocalScale(glm::vec3(0.2f, 0.2f, 0.2f));
    //sphere moon
    moon3->setLocalPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    moon3->setLocalScale(glm::vec3(0.4f, 0.4f, 0.4f));
    //earth moon
    moon4->setLocalPosition(glm::vec3(6.0f, 0.0f, 0.0f));
    moon4->setLocalScale(glm::vec3(0.2f, 0.2f, 0.2f));
    //earth moon
    moon5->setLocalPosition(glm::vec3(4.0f, 0.0f, 0.0f));
    moon5->setLocalScale(glm::vec3(0.25f, 0.25f, 0.25f));
    //earth moon
    moon6->setLocalPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    moon6->setLocalScale(glm::vec3(0.2f, 0.2f, 0.2f));
    //venus moon
    moon7->setLocalPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    moon7->setLocalScale(glm::vec3(0.4f, 0.4f, 0.4f));


    drawOrbit(Sun, 10, 100);
    drawOrbit(Sun, 15, 100);
    drawOrbit(Sun, 30, 100);
    drawOrbit(Sun, 35, 100);
    drawOrbit(Sun, 50, 100);

    drawOrbit(Venus, 2, 70);
    drawOrbit(Earth, 2, 70);
    drawOrbit(Earth, 4, 70);
    drawOrbit(Earth, 6, 70);
    drawOrbit(sphere, 4, 70);
    drawOrbit(sphere, 5, 70);
    drawOrbit(Jupiter, 3, 70);

        // render loop
        // -----------

    GLfloat currentFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, &cursorActive);

        glClearColor(10.0f / 255.0f, 2.0f / 255.0f, 28.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        Mesh sphereMeshUpdate = buildSphere(1, vCount);
        std::vector<Mesh> meshes;
        meshes.push_back(sphereMeshUpdate);
        sphereModel->updateMeshes(meshes);
        
        Sun->setLocalRotation(glm::vec3(0.0f, currentFrame * (worldSpeed), glm::radians(45.0f)));
        mercuryDriver->setLocalRotation(glm::vec3(50.0f, currentFrame * worldSpeed * 9.5f, 0.0f));
        venusDriver->setLocalRotation(glm::vec3(-20.0f, currentFrame * worldSpeed * 8.5f, 0.0f));
        earthDriver->setLocalRotation(glm::vec3(60.0f, currentFrame * worldSpeed * 7.5f, 0.0f));
        sphereDriver->setLocalRotation(glm::vec3(10.0f, currentFrame * worldSpeed * 4.5f, 0.0f));
        jupiterDriver->setLocalRotation(glm::vec3(-30.0f, currentFrame * worldSpeed * 3.5f, 0.0f));

        moonDriver1->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * 3.5f, 0.0f));
        moonDriver2->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * 1.5f, 0.0f));
        moonDriver3->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * -1.0f, 0.0f));
        moonDriver4->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * -2.f, 0.0f));
        moonDriver5->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * 0.5f, 0.0f));
        moonDriver6->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * 5.5f, 0.0f));
        moonDriver7->setLocalRotation(glm::vec3(0.0f, currentFrame * moonSpeed * 7.5f, 0.0f));

        Sun->draw(Transform(), projection, view, true);

        if (show_tool_window)
        {
            imgui_render();
        }
        if (show_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void drawOrbit(GameObject* parentObject, int radius, int noOfVertices)
{
    Shader orbitShader("res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
    Mesh orbitMesh = buildElipse(0, 0, radius, radius, noOfVertices, orbitShader);
    Model* orbitModel = new Model(orbitMesh, "res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
    GameObject* orbitObject = new GameObject(orbitModel);
    parentObject->addChild(orbitObject);
};



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
    ImGui::Begin("Niez³e kszta³ty");    

    ImGui::SliderFloat("World rotation speed", &worldSpeed, 0, 15);
    ImGui::SliderFloat("Moon rotation speed", &moonSpeed, 0, 15);
    ImGui::SliderInt("Sphere sectors count ", &vCount, 1, 36);
    ImGui::Checkbox("Wireframe", &show_wireframe);
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


