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
int rows = 50, cols = 50;
int amount = rows * cols;

glm::mat4* houseModelMatrices;
glm::mat4* roofModelMatrices;

GameObject ROOT("ROOT");


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

    Shader instanceShader("res/shaders/instance.vert", "res/shaders/instance.frag");
    Shader ourShader("res/shaders/loadModel.vert", "res/shaders/loadModel.frag");

    Model domek("res/models/domek/domek.obj");
    Model dach("res/models/dach/dach.obj");
    Model podloga("res/models/grass/plane.obj");


    /*GameObject* Root = new GameObject(root, &instanceShader);
    GameObject* Domek = new GameObject(domek, &instanceShader);
    GameObject* Dach = new GameObject(dach, &instanceShader);
    Root->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));*/

    ROOT.transform.setLocalPosition({ 0, 0, 0 });
    const float scale = 1;
    ROOT.transform.setLocalScale({ scale, scale, scale });

    //podloga
    std::string nazwaPodlogi = "podloga";
    ROOT.addChild(nazwaPodlogi);
    GameObject* Podloga = ROOT.getChildByName(nazwaPodlogi);
    Podloga->transform.setLocalPosition(glm::vec3({ 0, -1, 0 }));
    Podloga->transform.setLocalScale(glm::vec3({ 1000, 0.1, 1000 }));
    Podloga->updateSelfAndChild();

    // Instanced matrices setup


    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    houseModelMatrices = new glm::mat4[amount];
    roofModelMatrices = new glm::mat4[amount];

    float offset = 10.0f;
    int index = 0;

    glm::mat4 tmp = glm::translate(glm::mat4(1.0f), { -400, 0, -400 });
    glm::mat4 up = glm::mat4(1.0f);
    for (unsigned int i = 0; i < cols; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int j = 0; j < rows; j++)
        {
            //domek
            std::string nazwaDomku = "Domek nr " + std::to_string(index);
            ROOT.addChild(nazwaDomku);
            GameObject* Domek = ROOT.getChildByName(nazwaDomku);

            // Ustaw transformacjê dla ka¿dego domku
            float x = i * offset;
            float y = 0;
            float z = j * offset;
            Domek->transform.setLocalPosition(glm::vec3(x, y, z));
            Domek->transform.setLocalScale(glm::vec3(1));
            Domek->forceUpdateSelfAndChild();
            houseModelMatrices[index] = Domek->transform.getModelMatrix();

            //dach
            std::string nazwaDachu = "Dach nr " + std::to_string(index);
            Domek->addChild(dach, nazwaDachu);
            GameObject* Dach = Domek->getChildByName(nazwaDachu);

            // Ustaw transformacjê dla ka¿dego dachu
            x = 0;
            y = 0;
            z = 0;
            Dach->transform.setLocalPosition(glm::vec3(x, y, z));
            Dach->transform.setLocalScale(glm::vec3(1));
            Dach->forceUpdateSelfAndChild();
            roofModelMatrices[index] = Dach->transform.getModelMatrix();

            index++;
        }
        tmp = glm::translate(tmp, glm::vec3(-1.0f * static_cast<float>(rows) * 4.0f, 0.0f, 4.0f));
    }


     
    // configure instanced array
    // -------------------------
    unsigned int houseBuffer;
    glGenBuffers(1, &houseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, houseBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &houseModelMatrices[0], GL_STATIC_DRAW);

    unsigned int roofBuffer;
    glGenBuffers(1, &roofBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &roofModelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    //for (unsigned int i = 0; i < Domek->model_->meshes.size(); i++)
    //{
    //    unsigned int VAO = Domek->model_->meshes[i].VAO;
    //    glBindVertexArray(VAO);
    //    // set attribute pointers for matrix (4 times vec4)
    //    glEnableVertexAttribArray(3);
    //    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    //    glEnableVertexAttribArray(4);
    //    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    //    glEnableVertexAttribArray(5);
    //    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    //    glEnableVertexAttribArray(6);
    //    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    //    glVertexAttribDivisor(3, 1);
    //    glVertexAttribDivisor(4, 1);
    //    glVertexAttribDivisor(5, 1);
    //    glVertexAttribDivisor(6, 1);

    //    glBindVertexArray(0);
    //}


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

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ROOT.updateSelfAndChild();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, podloga.textures_loaded[0].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        ourShader.setInt("texture_diffuse1", 0);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", Podloga->transform.getModelMatrix());


        for (unsigned int i = 0; i < podloga.meshes.size(); i++) {
            unsigned int VAO = podloga.meshes[i].VAO;
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(podloga.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        instanceShader.use();
        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, domek.textures_loaded[0].id);
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

            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(domek.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        //samo renderowanie wszystkich dachow
        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dach.textures_loaded[0].id);
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

            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(dach.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);

            glBindVertexArray(0);
        }


        if (show_tool_window)
        {
            imgui_render();
        }
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

//void drawOrbit(GameObject* parentObject, int radius, int noOfVertices)
//{
//    Shader orbitShader("res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
//    Mesh orbitMesh = buildElipse(0, 0, radius, radius, noOfVertices, orbitShader);
//    Model* orbitModel = new Model(orbitMesh, "res/shaders/loadModel.vert", "res/shaders/loadModel.frag");
//    GameObject* orbitObject = new GameObject(orbitModel);
//    parentObject->addChild(orbitObject);
//};



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


