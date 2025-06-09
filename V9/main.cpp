// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "stb_easy_font.h"
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

int main(void)
{
    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 500;
    unsigned int wHeight = 500;
    const char wTitle[] = "Nedeljko Tesanovic, RN123/2020 - Vetrenjace";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowTitle(window, wTitle);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'((\n";
        return 3;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int stride = (3 + 4) * sizeof(float);

    // --- Windmill geometry ---
    float cubeVertices[] = {
        // positions         // colors
        // Front face
        -0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        // Back face
         0.2f,  0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f, -0.2f,0.7f, 0.5f, 0.2f, 1.0f    ,    
        // Left face
        -0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        // Right face (CCW)
         0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        // Top face (CCW)
        -0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f,  0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        // Bottom face
        -0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
         0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.7f, 0.5f, 0.2f, 1.0f,
    };
    float bladeVertices[] = {
        // Prednja strana (gledano iz +Z, CCW)
    -0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
     0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
     0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,

    -0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
     0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
    -0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
        // Jedina vidljiva strana (gledano iz -Z, CCW)
     0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
    -0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
    -0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,

     0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
    -0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
     0.02f, 0.4f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
    };
    unsigned int cubeVAO, cubeVBO, bladeVAO, bladeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glGenVertexArrays(1, &bladeVAO);
    glGenBuffers(1, &bladeVBO);
    glBindVertexArray(bladeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bladeVertices), bladeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // --- VAO/VBO/EBO i tekstura za index kvadrat (ime, prezime, indeks) ---
    float indexQuad[] = {
        // positions         // colors         // tex coords
        -0.95f,  0.95f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left
        -0.95f,  0.80f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.55f,  0.80f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
        -0.55f,  0.95f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f  // top right
    };
    unsigned int indexIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    unsigned int VAOI, VBOI, EBOI;
    glGenVertexArrays(1, &VAOI);
    glGenBuffers(1, &VBOI);
    glGenBuffers(1, &EBOI);
    glBindVertexArray(VAOI);
    glBindBuffer(GL_ARRAY_BUFFER, VBOI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indexQuad), indexQuad, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOI);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexIndices), indexIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int textureInx;
    glGenTextures(1, &textureInx);
    glBindTexture(GL_TEXTURE_2D, textureInx);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int widthi, heighti, nrChannelsi;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("index.png", &widthi, &heighti, &nrChannelsi, 0);
    if (data2) {
        if (nrChannelsi == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthi, heighti, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        } else if (nrChannelsi == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthi, heighti, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture index.png" << std::endl;
    }
    stbi_image_free(data2);
    unsigned int indexShader = createShader("index.vert", "index.frag");

    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");

    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    // Kamera: slobodno kretanje po XZ (WSAD), gleda ka vrhu centralne vetrenja?e
    glm::vec3 camPos(0.0f, 0.0f, 5.0f); // startna pozicija
    glm::vec3 camTarget(0.0f, 1.0f, 0.0f); // vrh centralne vetrenja?e
    glm::vec3 camUp(0.0f, 1.0f, 0.0f);
    float camSpeed = 0.08f;

    float bladeAngles[3] = {0.0f, 0.0f, 0.0f};
    float bladeSpeeds[3] = {1.0f, 1.5f, 2.0f};
    float bladeSpeedFactor = 1.0f;
    glm::vec3 windmillPositions[3] = {
        glm::vec3(-1.5f, 0.0f, 0.0f),
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 1.5f, 0.0f, 0.0f)
    };

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    const double FPS_LIMIT = 1.0 / 60.0;

    glUseProgram(unifiedShader);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));

    glClearColor(0.5, 0.5, 0.5, 1.0);

    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
        if (elapsed < FPS_LIMIT) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FPS_LIMIT - elapsed));
            continue;
        }
        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 1. Aktiviraj unifiedShader za celu scenu
        glUseProgram(unifiedShader);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));

        // 2. WSAD/QE logika i view matrica
        glm::vec3 dir = glm::normalize(glm::vec3(camTarget.x - camPos.x, 0.0f, camTarget.z - camPos.z));
        glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += dir * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= dir * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= right * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += right * camSpeed;
        glm::mat4 view = glm::lookAt(camPos, camTarget, camUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) bladeSpeedFactor += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) bladeSpeedFactor -= 0.01f;
        if (bladeSpeedFactor < 0.1f) bladeSpeedFactor = 0.1f;
        if (bladeSpeedFactor > 5.0f) bladeSpeedFactor = 5.0f;

        for (int i = 0; i < 3; ++i) {
            bladeAngles[i] += bladeSpeeds[i] * bladeSpeedFactor;
            if (bladeAngles[i] > 360.0f) bladeAngles[i] -= 360.0f;
        }

        for (int i = 0; i < 3; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), windmillPositions[i]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            for (int k = 0; k < 4; ++k) {
                glm::mat4 bladeModel = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.22f));
                bladeModel = glm::rotate(bladeModel, glm::radians(bladeAngles[i] + k * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bladeModel));
                glBindVertexArray(bladeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 12);
            }
        }
        glBindVertexArray(0);

        // 3. Prikaz teksture sa imenom, prezimenom i indeksom u gornjem levom uglu
        glUseProgram(indexShader);
        glBindVertexArray(VAOI);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureInx);
        glUniform1i(glGetUniformLocation(indexShader, "ourTexture"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &bladeVBO);
    glDeleteVertexArrays(1, &bladeVAO);
    glDeleteVertexArrays(1, &VAOI);
    glDeleteBuffers(1, &VBOI);
    glDeleteBuffers(1, &EBOI);
    glDeleteProgram(unifiedShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
