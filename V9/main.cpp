// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

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
    //glDisable(GL_CULL_FACE); // privremeno isklju?en culling radi provere svih stranica

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
         0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,

        -0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
         0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
        -0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,

        // Zadnja strana (gledano iz -Z, CCW)
         0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
         0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
        -0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,

         0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
        -0.02f, 0.5f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
        -0.02f, 0.0f, 0.0f,  0.8f, 0.8f, 0.8f, 1.0f,
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

    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");

    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    // Orbital camera parameters
    float camRadius = 5.0f;
    float camAngle = 0.0f; // in radians
    glm::vec3 camTarget(0.0f, 1.0f, 0.0f); // central windmill top
    glm::vec3 camUp(0.0f, 1.0f, 0.0f);
    float camSpeed = 0.02f; // angle/radius speed

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

        // Camera controls: A/D rotate, W/S zoom (orbital)
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camAngle -= camSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camAngle += camSpeed;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camRadius -= 0.05f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camRadius += 0.05f;
        if (camRadius < 1.5f) camRadius = 1.5f;
        if (camRadius > 20.0f) camRadius = 20.0f;
        glm::vec3 camPos = glm::vec3(
            camRadius * sin(camAngle),
            0.0f,
            camRadius * cos(camAngle)
        );
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(unifiedShader);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));

        for (int i = 0; i < 3; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), windmillPositions[i]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            for (int k = 0; k < 4; ++k) {
                glm::mat4 bladeModel = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.22f)); // blize kocki
                bladeModel = glm::rotate(bladeModel, glm::radians(bladeAngles[i] + k * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bladeModel));
                glBindVertexArray(bladeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 12);
            }
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &bladeVBO);
    glDeleteVertexArrays(1, &bladeVAO);
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
