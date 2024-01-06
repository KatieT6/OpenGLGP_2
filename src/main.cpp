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

const GLchar* vertexPath = "res/shaders/instance.vert";
const GLchar* fragmentPath = "res/shaders/instance.frag";

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

    Shader instanceShader(vertexPath, fragmentPath);

    Model* domek = new Model("res/models/domek/domek.obj", vertexPath, fragmentPath);
    Model* dach = new Model("res/models/dach/dach.obj", vertexPath, fragmentPath);
    Model* plane = new Model("res/models/grass/plane.obj", vertexPath, fragmentPath);


    GameObject* dachDriver = new GameObject();
    GameObject* domekDriver = new GameObject();


    GameObject* Root = new GameObject(plane);
    GameObject* Domek = new GameObject(domek);
    GameObject* Dach = new GameObject(dach);    

    Root->setLocalScale(glm::vec3(1.0f, 1.0f, 1.0f));


    Dach->setLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    Root->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    dachDriver->addChild(Dach);
    domekDriver->addChild(Domek);

    Domek->addChild(dachDriver);
    Root->addChild(domekDriver);

    // Instanced matrices setup
    int rows = 200, cols = 200;
    int amount = rows * cols;

    std::vector<Transform*> domekTransforms;
    std::vector<Transform*> dachTransforms;

    auto planeTransform = Root->getLocalTransform();

    //glm::mat4 tmp = glm::translate(glm::mat4(1.0f), { -400, 0, -400 });
    //for (auto i = 0; i < cols; i++) {
    //    glm::mat4 model(1.0f);
    //    for (auto j = 0; j < rows; j++) {
    //        auto houseTransform = new Transform();
    //        auto roofTransform = new Transform();

    //        tmp = glm::translate(tmp, glm::vec3(4.0f, 0.0f, 0.0f));
    //        model = glm::translate(tmp, glm::vec3(0.0f, 1.5f, 0.0f));

    //        houseTransform->getLocalModelMatrix(model);
    //        houseTransform->setParent(neighbourhoodTransform);

    //        houseTransforms.emplace_back(houseTransform);

    //        roofTransform->setLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    //        roofTransform->setParent(houseTransforms.back());

    //        roofTransforms.emplace_back(roofTransform);
    //    }
    //    tmp = glm::translate(tmp, glm::vec3(-1.0f * static_cast<float>(rows) * 4.0f, 0.0f, 4.0f));
    //}

    //// Create instanced entities
    //auto house = new InstancedEntity(houseModel, &lightShader, houseTransforms);
    //auto roof = new InstancedEntity(roofModel, &lightShader, roofTransforms);

    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    glm::mat4 tmp = glm::translate(glm::mat4(1.0f), { -400, 0, -400 });
    for (unsigned int i = 0; i < cols; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int j = 0; j < rows; j++)
        {
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            tmp = glm::translate(tmp, glm::vec3(4.0f, 0.0f, 0.0f));
            model = glm::translate(tmp, glm::vec3(0.0f,-1.0f, 0.0f));

            // 4. now add to list of matrices
            modelMatrices[i * rows + j] = model;
        }
        tmp = glm::translate(tmp, glm::vec3(-1.0f * static_cast<float>(rows) * 4.0f, 0.0f, 4.0f));
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < domek->meshes.size(); i++)
    {
        unsigned int VAO = domek->meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }


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

        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);

        // draw meteorites
        instanceShader.use();
        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, domek->textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < domek->meshes.size(); i++)
        {
            glBindVertexArray(domek->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(domek->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        
        //Root->setLocalRotation(glm::vec3(0.0f, 0 /*currentFrame * (worldSpeed)*/, glm::radians(45.0f)));

        //Root->draw(Transform(), projection, view, true);

        //if (show_tool_window)
        //{
        //    imgui_render();
        //}
        //if (show_wireframe)
        //{
        //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //}
        //else
        //{
        //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //}

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


