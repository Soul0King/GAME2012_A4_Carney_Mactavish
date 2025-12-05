#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Window.h"
#include <cassert>
#include <iostream>
#include <memory>

struct App
{
    GLFWwindow* window = nullptr;
    int keys_prev[KEY_COUNT]{};
    int keys_curr[KEY_COUNT]{};
    float frame_time_begin = 0.0f;
    float frame_time_end = 0.0f;
    float frame_time_delta = 0.0f;

    double mouse_prev_x = 0.0;
    double mouse_prev_y = 0.0;
    double mouse_delta_x = 0.0;
    double mouse_delta_y = 0.0;
    bool first_mouse = true;
} g_app;

void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT) return;
    g_app.keys_curr[key] = action;
}

void APIENTRY DebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

static void MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_app.first_mouse)
    {
        g_app.mouse_prev_x = xpos;
        g_app.mouse_prev_y = ypos;
        g_app.first_mouse = false;
    }

    g_app.mouse_delta_x = xpos - g_app.mouse_prev_x;
    g_app.mouse_delta_y = ypos - g_app.mouse_prev_y;

    g_app.mouse_prev_x = xpos;
    g_app.mouse_prev_y = ypos;
}

void CreateWindow(int width, int height, const char* title)
{
    assert(glfwInit() == GLFW_TRUE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    g_app.window = glfwCreateWindow(width, height, title, NULL, NULL);
    assert(g_app.window != nullptr);

    glfwMakeContextCurrent(g_app.window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    glfwSetKeyCallback(g_app.window, KeyboardCallback);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, nullptr);
#endif

    glfwSetInputMode(g_app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(g_app.window, MousePosCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(g_app.window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void SetWindowShouldClose(bool close)
{
    glfwSetWindowShouldClose(g_app.window, close ? GLFW_TRUE : GLFW_FALSE);
}

bool WindowShouldClose()
{
    return glfwWindowShouldClose(g_app.window);
}

float FrameTime()
{
    return g_app.frame_time_delta;
}

float Time()
{
    return glfwGetTime();
}

void Loop()
{
    memcpy(g_app.keys_prev, g_app.keys_curr, sizeof(int) * KEY_COUNT);

    glfwSwapBuffers(g_app.window);
    glfwPollEvents();
}

void BeginFrame()
{
    g_app.frame_time_begin = glfwGetTime();
}

void EndFrame()
{
    g_app.frame_time_end = glfwGetTime();
    g_app.frame_time_delta = g_app.frame_time_end - g_app.frame_time_begin;
}

void BeginGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EndGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool IsKeyDown(int key)
{
    return g_app.keys_curr[key] == GLFW_PRESS;
}

bool IsKeyUp(int key)
{
    return g_app.keys_curr[key] == GLFW_RELEASE;
}

bool IsKeyPressed(int key)
{
    return
        g_app.keys_prev[key] == GLFW_PRESS &&
        g_app.keys_curr[key] == GLFW_RELEASE;
}

void DestroyWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

int WindowWidth()
{
    int width, height;
    glfwGetWindowSize(g_app.window, &width, &height);
    return width;
}

int WindowHeight()
{
    int width, height;
    glfwGetWindowSize(g_app.window, &width, &height);
    return height;
}

MouseDelta GetMouseDelta()
{
    MouseDelta out;
    out.x = (float)g_app.mouse_delta_x;
    out.y = (float)g_app.mouse_delta_y;
    g_app.mouse_delta_x = 0.0;
    g_app.mouse_delta_y = 0.0;
    return out;
}