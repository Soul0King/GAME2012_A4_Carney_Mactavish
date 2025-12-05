#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

#include <imgui/imgui.h>
#include <cmath>

enum ShaderType
{
    SHADER_SAMPLE_TEXTURE,
    SHADER_POSITION_COLOR,
    SHADER_TCOORD_COLOR,
    SHADER_NORMAL_COLOR,
    SHADER_TYPE_COUNT
};

enum MeshType
{
    MESH_PLANE,
    MESH_SPHERE,
    MESH_HEMISPHERE,
    MESH_HEAD,
    MESH_CT4,
    MESH_TYPE_COUNT
};

enum TextureType
{
    TEXTURE_GRADIENT_WARM,
    TEXTURE_GRADIENT_COOL,
    TEXTURE_TYPE_COUNT
};

enum A4DrawType
{
    A4_PAR_SHAPES_NORMAL_SHADER,
    A4_OBJ_FILE_TCOORDS_SHADER,
    A4_CT4_TEXTURE_SHADER,
    A4_MANUAL_MESH,
    A4_CUSTOM_DRAW,
    A4_TYPE_COUNT
};

void LoadTextures(Texture textures[TEXTURE_TYPE_COUNT])
{
    Image warm, cool;
    LoadImage(&warm, 512, 512);
    LoadImage(&cool, 512, 512);

    LoadImageGradient(&warm, Vector3Zeros, Vector3UnitX, Vector3UnitY, Vector3UnitX + Vector3UnitY);
    LoadImageGradient(&cool, Vector3UnitZ, Vector3UnitZ + Vector3UnitX, Vector3UnitY + Vector3UnitZ, Vector3Ones);

    LoadTexture(&textures[TEXTURE_GRADIENT_WARM], warm);
    LoadTexture(&textures[TEXTURE_GRADIENT_COOL], cool);
}

struct Camera
{
    float pitch = 0.0f;
    float yaw = 0.0f;
    Vector3 position = Vector3Zeros;
};

int main()
{
    CreateWindow(800, 800, "Graphics 1 - Assignment");

    Mesh meshes[MESH_TYPE_COUNT];

    LoadMeshPlane(&meshes[MESH_PLANE]);
    LoadMeshSphere(&meshes[MESH_SPHERE]);
    LoadMeshHemisphere(&meshes[MESH_HEMISPHERE]);
    LoadMeshObj(&meshes[MESH_HEAD], "./assets/meshes/head.obj");
    LoadMeshObj(&meshes[MESH_CT4], "./assets/meshes/ct4.obj");

    GLuint position_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/position_color.vert");
    GLuint tcoord_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/tcoord_color.vert");
    GLuint normal_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/normal_color.vert");
    GLuint vertex_color_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");

    GLuint a4_texture_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a4_texture.vert");
    GLuint a4_texture_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a4_texture.frag");

    GLuint shaders[SHADER_TYPE_COUNT];
    shaders[SHADER_SAMPLE_TEXTURE] = CreateProgram(a4_texture_vert, a4_texture_frag);
    shaders[SHADER_POSITION_COLOR] = CreateProgram(position_color_vert, vertex_color_frag);
    shaders[SHADER_TCOORD_COLOR] = CreateProgram(tcoord_color_vert, vertex_color_frag);
    shaders[SHADER_NORMAL_COLOR] = CreateProgram(normal_color_vert, vertex_color_frag);

    Texture textures[TEXTURE_TYPE_COUNT];
    LoadTextures(textures);

    Mesh manualMesh;
    {
        float verts[] =
        {

            -1, -1, 0,           1, 0, 0, 1,
             1, -1, 0,           0, 1, 0, 1,
             0,  1, 0,           0, 0, 1, 1
        };

        unsigned int idx[] = { 0, 1, 2 };

        LoadMeshManual(&manualMesh, verts, 3, idx, 3);
    }

    Camera cam;
    cam.position = { 0.0f, 0.0f, 5.0f };

    int draw_index = A4_PAR_SHAPES_NORMAL_SHADER;
    int mesh_index = MESH_PLANE;
    int texture_index = TEXTURE_GRADIENT_COOL;

    while (!WindowShouldClose())
    {
        BeginFrame();
        float dt = FrameTime();

        if (IsKeyPressed(KEY_ESCAPE)) SetWindowShouldClose(true);
        if (IsKeyPressed(KEY_TAB)) mesh_index = (mesh_index + 1) % MESH_TYPE_COUNT;
        if (IsKeyPressed(KEY_T))   texture_index = (texture_index + 1) % TEXTURE_TYPE_COUNT;
        if (IsKeyPressed(KEY_Y))   draw_index = (draw_index + 1) % A4_TYPE_COUNT;

        MouseDelta md = GetMouseDelta();

        float sensitivity = 0.0025f;
        cam.yaw -= md.x * sensitivity;
        cam.pitch -= md.y * sensitivity;

        float limit = 89.0f * DEG2RAD;
        if (cam.pitch > limit) cam.pitch = limit;
        if (cam.pitch < -limit) cam.pitch = -limit;

        Matrix camRot = MatrixRotateX(cam.pitch) * MatrixRotateY(cam.yaw);

        Vector3 forward = { camRot.m8, camRot.m9, camRot.m10 };
        Vector3 right = { camRot.m0, camRot.m1, camRot.m2 };
        Vector3 up = { camRot.m4, camRot.m5, camRot.m6 };

        if (IsKeyDown(KEY_W)) cam.position -= forward * dt * 10.0f;
        if (IsKeyDown(KEY_S)) cam.position += forward * dt * 10.0f;
        if (IsKeyDown(KEY_D)) cam.position += right * dt * 10.0f;
        if (IsKeyDown(KEY_A)) cam.position -= right * dt * 10.0f;
        if (IsKeyDown(KEY_SPACE)) cam.position += up * dt * 10.0f;
        if (IsKeyDown(KEY_LEFT_SHIFT)) cam.position -= up * dt * 10.0f;

        Matrix proj = MatrixPerspective(75.0f * DEG2RAD,
            WindowWidth() / (float)WindowHeight(),
            0.01f, 100.0f);

        Matrix view = MatrixInvert(camRot * MatrixTranslate(cam.position.x, cam.position.y, cam.position.z));
        Matrix world = MatrixIdentity();
        Matrix mvp = world * view * proj;

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (draw_index)
        {
        case A4_PAR_SHAPES_NORMAL_SHADER:
        {
            BeginShader(shaders[SHADER_NORMAL_COLOR]);
            SendMat4(mvp, "u_mvp");
            DrawMesh(meshes[mesh_index]);
            EndShader();
        } break;

        case A4_OBJ_FILE_TCOORDS_SHADER:
        {
            BeginShader(shaders[SHADER_TCOORD_COLOR]);
            SendMat4(mvp, "u_mvp");
            DrawMesh(meshes[MESH_HEAD]);
            EndShader();
        } break;

        case A4_CT4_TEXTURE_SHADER:
        {
            BeginShader(shaders[SHADER_SAMPLE_TEXTURE]);
            BeginTexture(textures[texture_index]);
            SendMat4(mvp, "u_mvp");
            DrawMesh(meshes[MESH_CT4]);
            EndTexture();
            EndShader();
        } break;

        case A4_MANUAL_MESH:
        {
            BeginShader(shaders[SHADER_POSITION_COLOR]);
            SendMat4(mvp, "u_mvp");
            DrawMesh(manualMesh);
            EndShader();
        } break;

        case A4_CUSTOM_DRAW:
        {
            Matrix spin = MatrixRotateY(Time());
            BeginShader(shaders[SHADER_SAMPLE_TEXTURE]);
            BeginTexture(textures[TEXTURE_GRADIENT_WARM]);
            SendMat4(spin * view * proj, "u_mvp");
            DrawMesh(meshes[MESH_HEMISPHERE]);
            EndTexture();
            EndShader();
        } break;
        }

        BeginGui();
        EndGui();

        Loop();
        EndFrame();
    }

    for (int i = 0; i < MESH_TYPE_COUNT; i++) UnloadMesh(&meshes[i]);
    UnloadMesh(&manualMesh);

    for (int i = 0; i < TEXTURE_TYPE_COUNT; i++) UnloadTexture(&textures[i]);
    for (int i = 0; i < SHADER_TYPE_COUNT; i++) DestroyProgram(&shaders[i]);

    DestroyWindow();
    return 0;
}
