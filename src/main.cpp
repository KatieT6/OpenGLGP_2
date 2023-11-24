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

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
bool init();
void init_imgui(GLFWwindow* window);
void imgui_render();
void do_Movement();
void drawSierpinskiPyramid(Shader& shader, int depth, int const maxDepth, glm::mat4 model);


// settings
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

bool firstMouse = true;
bool show_tool_window = true;

float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

int depth = 1;
int maxDepth = 7;
float angleX = 0.0f;
float angleY = 0.0f;
ImVec4 color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
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

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("res/shaders/basic.vert", "res/shaders/basic.frag");

    GLfloat vertices[] = {
        -0.5f, 0.0f, -0.289f,0.0f, 1.0f,
        0.5f, 0.0f, -0.289f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.577f, 0.5f, 0.0f,

        -0.5f, 0.0f, -0.289f, 0.0f, 1.0f,
        0.5f, 0.0f, -0.289f, 1.0f, 1.0f,
        0.0f, 0.816f, 0.0f, 0.5f, 0.5f,

        0.5f, 0.0f, -0.289f,1.0f, 1.0f,
        0.0f, 0.0f, 0.577f, 0.5f, 0.0f,
        0.0f, 0.816f, 0.0f, 0.5f, 0.5f,

        0.0f, 0.0f, 0.577f,0.5f, 0.0f,
        -0.5f, 0.0f, -0.289f,0.0f, 1.0f,
        0.0f, 0.816f, 0.0f, 0.5f, 0.5f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("res/textures/meme.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.setInt("texture1", 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

    GLfloat currentFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        if (show_tool_window)
        {
			imgui_render();
        }


        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
      
        glBindVertexArray(VAO);

        ourShader.use();


        //macierz modelu
        glm::mat4 model = glm::mat4(1.0f);
        //macierz widoku
        glm::mat4 view = glm::mat4(1.0f);

        
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
         
        model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", color.x, color.y, color.z, color.w);

        drawSierpinskiPyramid(ourShader, 1, depth, model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void do_Movement()
{
    GLfloat cameraSpeed = 2.5f * deltaTime;
};



void processInput(GLFWwindow* window)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move forward (zoom in)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    // Move backward (zoom out)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
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
    static float f = 0.0f;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Sierpinsky Pyramid");
	ImGui::Text("Recursion depth");
	ImGui::SliderInt("Recursion depth", &depth, 1, maxDepth);

    ImGui::ColorEdit3("Color of Fractal", (float*)&color);
	ImGui::Text("Rotation angle X");
    
	ImGui::SliderFloat("Rotation angle X", &angleX, 0.0f, 360.0f);
	ImGui::Text("Rotation angle Y");
	ImGui::SliderFloat("Rotation angle Y", &angleY, 0.0f, 360.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void drawSierpinskiPyramid(Shader& shader, int depth, int const max, glm::mat4 model)
{
    if (depth == max)
    {
        model = glm::rotate(model, 45.0f, glm::vec3(0, 1, 0));
        float const scale = 1.0f / glm::pow(2.0f, static_cast<float>(depth) - 1.0f);
        shader.setMat4("model", glm::scale(model, glm::vec3(scale, scale, scale)));

        glDrawArrays(GL_TRIANGLES, 0, 12);
        return;
    }

    int const next_depth = depth + 1;
    float const translation = 1.0f / glm::pow(2.0f, next_depth);

    drawSierpinskiPyramid(shader, next_depth, max, glm::translate(model, glm::vec3(0.0f, translation * 0.816f, 0.0)));
    drawSierpinskiPyramid(shader, next_depth, max, glm::translate(model, glm::vec3(-translation, -translation * 0.816f, translation * 0.6f)));
    drawSierpinskiPyramid(shader, next_depth, max, glm::translate(model, glm::vec3(translation, -translation * 0.816f, translation * 0.6f)));
    drawSierpinskiPyramid(shader, next_depth, max, glm::translate(model, glm::vec3(0.0f, -translation * 0.816f, -translation * 1.15f)));
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}