/* ----- Daftar Library yang Digunakan ----- */
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>
// ------------------------------------------

/* ----- Daftar Fungsi dan Prosedur ----- */
GLFWwindow* openGlInit();
void loadTexture(unsigned int texture, std::string path, bool alpha);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void createCubeSphere(int subdivision);
void createCubeSphereFace(int face, int subdivision, std::vector<float>* vertices);
void calculateNormalsCubesphere(int face, float angle, int axis, glm::tvec3<float> *normal);
void addTextureCoords();
unsigned int loadCubemap(std::string faces);
// ------------------------------------------

/* ------- Daftar Variable Global --------- */

/* ------------- Ukuran Layar ------------- */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
// ------------------------------------------

/* ---------- Pengaturan Camera ---------- */
float radius = 5.0f;
float mouseSensitivity = 0.1f;
float yaw = 90.0f;
float pitch = 0.0f;
float zoom = 45.0f;

// sudut pandang kamera awal
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool leftClicked = false;
bool firstMouse = true;

bool rotationEarth = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ----------------------------------------------

// ------------------ Parameters ----------------
int subdivision = 6;
int numFaces = 6;
glm::vec3 lightPos = glm::vec3(1.2f, 0.1f, 2.0f);
// ----------------------------------------------
int rowPerFace = (int)glm::pow(2, subdivision) + 1;

float cubesphereVertices[76050 + 50700];
unsigned int cubesphereIndices[152100];

// vertices sky box
float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

int main()
{
    // ----------------------------------- Initialize, konfigurasi, dan membuat window ---------------------------------------
    GLFWwindow* window = openGlInit();
    if (window == NULL) return -1;
    // -----------------------------------------------------------------------------------------------------------------------

    // -------------------------------------------------- Load Texture -------------------------------------------------------
    unsigned int textureEarth;
    glGenTextures(1, &textureEarth);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureEarth);
    loadTexture(textureEarth, std::string("earth.jpg"), false);
    // -------------------------------------------------------------------------------------------------------------------
    unsigned int textureEarthHeight;
    glGenTextures(1, &textureEarthHeight);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureEarthHeight);
    loadTexture(textureEarthHeight, std::string("heightMap.png"), true);
    // -------------------------------------------------------------------------------------------------------------------
    unsigned int textureSpecularMap;
    glGenTextures(1, &textureSpecularMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureSpecularMap);
    loadTexture(textureSpecularMap, std::string("specularMap.png"), true);
    // -------------------------------------------------------------------------------------------------------------------
    unsigned int textureCloud;
    glGenTextures(1, &textureCloud);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureCloud);
    loadTexture(textureCloud, std::string("clouds.png"), true);
    // -------------------------------------------------------------------------------------------------------------------
    // std::vector<std::string> faceCube = {"bkgl_back.jpg", "bkgl_bot.jpg", "bkgl_front.jpg", "bkgl_left.jpg", "bkgl_right.jpg", "bkgl_top.jpg"};
    unsigned int cubemapTexture = loadCubemap("stars2.jpg");
    // unsigned int cubemapTexture = loadCubemap(faceCube);
    // -------------------------------------------------------------------------------------------------------------------

    // ----------------------------------------- Build dan compile program shader ----------------------------------------
    Shader shader = Shader("shader.vs", "shader.fs", "shader.gs");
    shader.use();
    // Membuat texture shader
    shader.setInt("earth", 0);
    shader.setInt("earth_height", 1);
    shader.setInt("specularMap", 2);
    shader.setInt("clouds", 3);
    shader.setVec3("lightPos", lightPos);

    shader.setFloat("constant", 1.0f);
    shader.setFloat("linear", 0.045f);
    shader.setFloat("quadratic", 0.0075f);
    // -------------------------------------------------------------------------------------------------------------------
    Shader shaderSkybox = Shader("shaderSkybox.vs", "shaderSkybox.fs", nullptr);
    shaderSkybox.use();
    shaderSkybox.setInt("skybox", 0);
    // -------------------------------------------------------------------------------------------------------------------

    // -------------------------------- Membuat Cube Sphere ---------------------------------
    createCubeSphere(subdivision);
    // -------------------------------------------------------------------------------------------------------------------

    // ----------------------- Membuat vertex data sphere dan mengkonfigurasi atributnya ------------------------
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubesphereVertices), cubesphereVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubesphereIndices), cubesphereIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(rowPerFace * rowPerFace * numFaces * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // -------------------------------------------------------------------------------------------------------------------
    // ----------------------- Membuat vertex data sky box / cubemap dan mengkonfigurasi atributnya ----------------------
    unsigned int VBO_SKY, VAO_SKY;
    glGenVertexArrays(1, &VAO_SKY);
    glGenBuffers(1, &VBO_SKY);

    glBindVertexArray(VAO_SKY);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_SKY);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // -------------------------------------------------------------------------------------------------------------------

    // -------------------------------------- Tes Kedalaman Sphere -------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    // -------------------------------------------------------------------------------------------------------------------

    // --------------------------------- World, View, Perspective --------------------------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;
    // -------------------------------------------------------------------------------------------------------------------

    // ------------------------------------------ Render loop ------------------------------------------------------------
    int l = 0;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraTarget, up)));
        projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        shaderSkybox.use();
        shaderSkybox.setMat4("model", model);
        shaderSkybox.setMat4("view", view);
        shaderSkybox.setMat4("projection", projection);
        // ... mengatur view dan projection matrix
        glBindVertexArray(VAO_SKY);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        // ...

        // ---------------------- World, View, Perspective space adjusted for camera -------------------------------------
        shader.use();
        shader.setMat4("model", model);

        view = glm::lookAt(cameraPos, cameraTarget, up);
        shader.setMat4("view", view);

        projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", cameraPos);
        // ---------------------------------------------------------------------------------------------------------------

        // ------------------------------ Bind buffers dan Membuat Cubesphere -----------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureEarth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureEarthHeight);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureSpecularMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureCloud);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(cubesphereIndices)/4, GL_UNSIGNED_INT, 0);
        // ---------------------------------------------------------------------------------------------------------------

        if(rotationEarth)
        {
            if(l % 50 == 0)
            {
                model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.1f, 0.0f));
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        l++;
    }
    // -------------------------------------------------------------------------------------------------------------------

    // --------------------------------------- Alokasi Buffer Ulang --------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // -------------------------------------------------------------------------------------------------------------------

    glfwTerminate();
    return 0;
}

/* Fungsi membuat GLFW */
GLFWwindow* openGlInit() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Kelompok 3 - Simulasi Planet Bumi", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Gagal membuat GLFW Window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Gagal menginisialisasi GLAD" << std::endl;
        return NULL;
    }
    return window;
}

/* Prosedur Load Texture Pada Object */
void loadTexture(unsigned int texture, std::string path, bool alpha) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        if (alpha)
        {
            // jika texture png (memiliki transparansi)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            // jika texture bukan png
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Gagal menambahkan texture" << std::endl;
    }
    stbi_image_free(data);
}

/* Prosedur Menerima Input dari Keyboard */
void processInput(GLFWwindow* window)
{
    // exit program jika klik esc
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // rotasikan bumi jika klik R
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        rotationEarth = true;
    // berhenti rotasikan bumi jika klik S
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rotationEarth = false;
}

/* Prosedur Callback Mengganti Viewport saat Ukuran Windows Berubah */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // ------------------ Jika ukuran windows berubah maka fungsi ini akan dipanggil sebagai callback -------------------
    glViewport(0, 0, width, height);
    // ------------------------------------------------------------------------------------------------------------------
}

/* Prosedur Callback saat Mouse Digerakkan */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // ------------------------------ Jika mouse digerakkan fungsi ini akan dipanggil sebagai callback ---------------------------------
    if (leftClicked)
    {
        if (firstMouse)
        {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
        }
        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos;

        lastX = (float)xpos;
        lastY = (float)ypos;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = -sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraPos = front * radius;
    }
    // -------------------------------------------------------------------------------------------------------------------
}

/* Prosedur Menangkap Klik Mouse */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        leftClicked = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        firstMouse = true;
        leftClicked = false;
    }
}

/* Prosedur Menangkap Scroll Mouse */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // ------------------------------ Jika mouse di scroll, callback akan dipanggil ---------------------------------
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
    // -------------------------------------------------------------------------------------------------------------------
}

/* Prosedur Membuat Sphere */
void createCubeSphere(int subdivision)
{
    std::vector<float> cubesphereVerticesVector;
    for (int i = 0; i < numFaces; i++)
    {
        createCubeSphereFace(i, subdivision, &cubesphereVerticesVector);
    }
    for (int i = 0; i < cubesphereVerticesVector.size(); i++)
    {
        cubesphereVertices[i] = cubesphereVerticesVector[i];
    }
    int j = 0;
    for (int i = 0; ((float)i + rowPerFace + 1) <= cubesphereVerticesVector.size() / 3; i++)
    {
        if ((i + rowPerFace) % (rowPerFace * rowPerFace) == 0)
        {
            i += rowPerFace - 1;
            continue;
        }
        if ((i + 1) % rowPerFace == 0) continue;
        cubesphereIndices[j] = i;
        cubesphereIndices[j + 1] = i + 1;
        cubesphereIndices[j + 2] = i + rowPerFace;
        cubesphereIndices[j + 3] = i + 1;
        cubesphereIndices[j + 4] = i + rowPerFace;
        cubesphereIndices[j + 5] = i + rowPerFace + 1;
        j += 6;
    }

    addTextureCoords();
}

/* Prosedur Membuat Permukaan Sphere */
void createCubeSphereFace(int face, int subdivision, std::vector<float>* vertices)
{
    glm::tvec3<float> n1(0, 0, 0);
    glm::tvec3<float> n2(0, 0, 0);
    glm::tvec3<float> v;
    float a1;
    float a2;

    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // memutar bidang lintang dari 45 hingga -45 derajat sepanjang sumbu Z (atas ke bawah)
    for (int i = 0; i < pointsPerRow; ++i)
    {
        a2 = glm::radians((45.0f - 90.0f * i / (pointsPerRow - 1)));

        calculateNormalsCubesphere(face, a2, 2, &n2);

        // memutar bidang membujur dari -45 ke 45 sepanjang sumbu Y (kiri-ke-kanan)
        for (int j = 0; j < pointsPerRow; ++j)
        {
            a1 = glm::radians((-45.0f + 90.0f * j / (pointsPerRow - 1)));

            calculateNormalsCubesphere(face, a1, 1, &n1);

            // cari vektor arah garis berpotongan, n1 x n2
            v = normalize(glm::cross(n1, n2));

            (*vertices).push_back(v[0]);
            (*vertices).push_back(v[1]);
            (*vertices).push_back(v[2]);
        }
    }
}

/* Prosedur Menghitung Sisi sisi Sphere */
void calculateNormalsCubesphere(int face, float angle, int axis, glm::tvec3<float> *normal)
{
    switch (face)
    {
    case 0: // Kanan
        if (axis == 1)
        {
            (*normal)[0] = -sin(angle);
            (*normal)[1] = 0;
            (*normal)[2] = -cos(angle);
        }
        else if (axis == 2)
        {
            (*normal)[0] = -sin(angle);
            (*normal)[1] = cos(angle);
            (*normal)[2] = 0;
        }
        break;
    case 1: // Belakang
        if (axis == 1) {
            (*normal)[0] = -cos(angle);
            (*normal)[1] = 0;
            (*normal)[2] = sin(angle);
        }
        else if (axis == 2) {
            (*normal)[0] = 0;
            (*normal)[1] = cos(angle);
            (*normal)[2] = sin(angle);
        }
        break;
    case 2: // Kiri
        if (axis == 1)
        {
            (*normal)[0] = sin(angle);
            (*normal)[1] = 0;
            (*normal)[2] = cos(angle);
        }
        else if (axis == 2)
        {
            (*normal)[0] = sin(angle);
            (*normal)[1] = cos(angle);
            (*normal)[2] = 0;
        }
        break;
    case 3: // Depan
        if (axis == 1)
        {
            (*normal)[0] = cos(angle);
            (*normal)[1] = 0;
            (*normal)[2] = -sin(angle);
        }
        else if (axis == 2)
        {
            (*normal)[0] = 0;
            (*normal)[1] = cos(angle);
            (*normal)[2] = -sin(angle);
        }
        break;
    case 4: // Bawah
        if (axis == 1)
        {
            (*normal)[0] = -cos(angle);
            (*normal)[1] = sin(angle);
            (*normal)[2] = 0;
        }
        else if (axis == 2)
        {
            (*normal)[0] = 0;
            (*normal)[1] = sin(angle);
            (*normal)[2] = -cos(angle);
        }
        break;
    case 5: // Atas
        if (axis == 1)
        {
            (*normal)[0] = cos(angle);
            (*normal)[1] = -sin(angle);
            (*normal)[2] = 0;
        }
        else if (axis == 2)
        {
            (*normal)[0] = 0;
            (*normal)[1] = -sin(angle);
            (*normal)[2] = -cos(angle);
        }
        break;
    }
}

/* Prosedur Menambahkan Coordinate Texture */
void addTextureCoords()
{
    int totalVerCoords = rowPerFace * rowPerFace * numFaces * 3;
    for (int i = 0; i < rowPerFace * rowPerFace * numFaces * 3;)
    {
        float cx = 0, cy = 0, cz = 0, r = 1;

        float theta, phi;

        theta = atan2(-(cubesphereVertices[i + 2] - cz), cubesphereVertices[i] - cx);
        phi = acos(-(cubesphereVertices[i + 1] - cy) / r);

        float u = (theta + glm::pi<float>()) / (2 * glm::pi<float>());
        float v = phi / glm::pi<float>();

        if (u == 1)
        {
            u = 0;
        }
        cubesphereVertices[totalVerCoords++] = u;
        cubesphereVertices[totalVerCoords++] = v;
        i += 3;
    }
}

/* Fungsi Membuat Cubemap */
unsigned int loadCubemap(std::string faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    // membuat 6 sisi cubemap
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(faces.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap " << faces << " gagal dibuat." << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
