#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std;
float speed = 0.0001;
float freeSpeed = 0.00002;

struct RoadSegment {
    float x1, y1, x2, y2, x3, y3, x4, y4;
    float congestion;
    bool isRight;
    string name;
    float r;
    float g;
    float b;
    vector<RoadSegment*> connectedRoads;
    bool isGreen;
    bool isOutside;
    

    RoadSegment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, bool isRight, string name, bool isOutside)
        : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), x4(x4), y4(y4), isRight(isRight), name(name), congestion(0.3f), isGreen(false), isOutside(isOutside) {
    }

    void addConnectedRoad(RoadSegment* road) {
        connectedRoads.push_back(road);
    }

    void getColorFromCongestion() {
        r = congestion;      
        g = 1.0f - congestion; 
        b = 0.0f;           
    }

    void lightIsGreen() {
       // speed = -speed;
        isGreen = true;
        if (congestion < 0) {
            congestion = 0;
       };
        if (congestion > 1) {
            congestion = 1;
        };

        getColorFromCongestion();
    }

    void lightIsRed() {
      // speed = -speed;
        isGreen = false;
        if (congestion < 0) {
            congestion = 0;
        };
        if (congestion > 1) {
            congestion = 1;
        };
        getColorFromCongestion();
    }

    void changeCongestion() {
        if (isOutside == true) {
            congestion -= freeSpeed;
            if (congestion < 0) {
                congestion = 0;
            };
            if (congestion > 1) {
                congestion = 1;
            };
            getColorFromCongestion();
        }
        else {
            getColorFromCongestion();
            if (isGreen == true) {
                congestion -= speed;
                if (congestion < 0) {
                    congestion = 0;
                };
                if (congestion > 1) {
                    congestion = 1;
                };
                int numConnectedRoads = connectedRoads.size(); 
                for (RoadSegment* road : connectedRoads) {
                    if (road != nullptr) { 
                        road->congestion += speed / numConnectedRoads;
                        if (road->congestion < 0) {
                            road->congestion = 0;
                        };
                        if (road->congestion > 1) {
                            road->congestion = 1;
                        };
                        road->getColorFromCongestion();
                    }
                }
            }
            else {
                congestion += speed;
                if (isRight == true) {
                    congestion -= speed / 3;
                    RoadSegment* road = connectedRoads[0];
                    road->congestion += speed / 3;
                    if (road->congestion < 0) {
                        road->congestion = 0;
                    };
                    if (road->congestion > 1) {
                        road->congestion = 1;
                    };
                    road->getColorFromCongestion();
                }
                if (congestion < 0) {
                    congestion = 0;
                };
                if (congestion > 1) {
                    congestion = 1;
                };
            }

         
        }
    }
};


struct TrafficLight {
    float x, y;
    float red[3];
    float yellow[3];
    float green[3];
    float rightTurn[3];
    float timer;
    float remainingTime;
    bool rightTurnActive;
    int rightTurnNum;
    string color;
    string nextColor;
    RoadSegment& road;

    TrafficLight(RoadSegment& road1, float x1, float y1) 
        : road(road1), x(x1), y(y1){
        timer = 20 + rand() % 21; // Nasumično vreme između 20 i 40 sekundi
        remainingTime = timer;

        red[0] = 1.0f; red[1] = 0.0f; red[2] = 0.0f;
        yellow[0] = 1.0f; yellow[1] = 1.0f; yellow[2] = 0.0f;
        green[0] = 0.0f; green[1] = 1.0f; green[2] = 0.0f;
        rightTurn[0] = 0.0f; rightTurn[1] = 1.0f; rightTurn[2] = 0.0f;

        rightTurnActive = true; 
        color = "red";
        rightTurnNum = 30;
        nextColor = "redyellow";
    }
};



unsigned int compileShader(GLenum type, const char* source); 
unsigned int createShader(const char* vsSource, const char* fsSource); 
void initTrafficLight(TrafficLight& light, float x, float y);
void updateTrafficLight(TrafficLight& light, float deltaTime);
void drawCircle(float offsetX, float offsetY, float r, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO, int& vertexCount);
bool drawTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount);
bool drawLeftTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, int& vertexCount);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drawArrow(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO);
bool drawLeftTrafficLightUp(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, int& vertexCount);

int main(void)
{
    srand(time(0));
    if (!glfwInit()) 
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
    unsigned int wWidth = 1000;
    unsigned int wHeight = 1000;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); 
        return 2;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) 
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    glfwSetScrollCallback(window, scroll_callback);



    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    unsigned VAO;
    glGenVertexArrays(1, &VAO);
    unsigned VBO;
    glGenBuffers(1, &VBO);
    unsigned VAO2;
    glGenVertexArrays(1, &VAO2);
    unsigned VBO2;
    glGenBuffers(1, &VBO2);
    unsigned VAO3;
    glGenVertexArrays(1, &VAO3);
    unsigned VBO3;
    glGenBuffers(1, &VBO3);
    unsigned VAO4;
    glGenVertexArrays(1, &VAO4);
    unsigned VBO4;
    glGenBuffers(1, &VBO4);
    unsigned VAO5;
    glGenVertexArrays(1, &VAO5);
    unsigned VBO5;
    glGenBuffers(1, &VBO5);
    int vertexCount = 0;
    unsigned VAO1;
    glGenVertexArrays(1, &VAO1);
    unsigned VBO1;
    glGenBuffers(1, &VBO1);

    unsigned VAOS1;
    glGenVertexArrays(1, &VAOS1);
    unsigned VBOS1;
    glGenBuffers(1, &VBOS1);
    unsigned VAOS12;
    glGenVertexArrays(1, &VAOS12);
    unsigned VBOS12;
    glGenBuffers(1, &VBOS12);
    unsigned VAOS13;
    glGenVertexArrays(1, &VAOS13);
    unsigned VBOS13;
    glGenBuffers(1, &VBOS13);

    unsigned VAOS2;
    glGenVertexArrays(1, &VAOS2);
    unsigned VBOS2;
    glGenBuffers(1, &VBOS2);
    unsigned VAOS21;
    glGenVertexArrays(1, &VAOS21);
    unsigned VBOS21;
    glGenBuffers(1, &VBOS21);
    unsigned VAOS22;
    glGenVertexArrays(1, &VAOS22);
    unsigned VBOS22;
    glGenBuffers(1, &VBOS22);

    double  mouseX, mouseY;
    int mouseButtonStateLeft, mouseButtonStateRight;

    float streats[] = {
        // positions          // colors           // texture coords
         1.0, -0.75f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         1.0, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.50f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         0.50f, -0.75f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    float grb[] = {
        // positions          // colors           // texture coords
         -1.0, 0.8, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         -1.0, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         -0.8f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         -0.8f, 0.8, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int grbIndices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };


    RoadSegment road1(-1.0, 0.76, -0.05, 0.76, -1.0, 0.74, -0.05, 0.74, false, "kkk1", true);
    RoadSegment road2(-0.05, 0.76, 1.0, 0.76, -0.05, 0.74, 1.0, 0.74, false, "kkk", true);
    RoadSegment road3(0.0, 0.76, 0.0, 1.0, 0.02, 0.76, 0.02, 1.0, false, "sss", true);
    RoadSegment road4(0.0, 0.76, 0.0, 0.0, 0.02, 0.76 , 0.02, 0.0, true, "sss", false);
    RoadSegment road5(-0.02, 0.76, -0.02, 0.0, 0.0, 0.76, 0.0, 0.0, false, "sss", false);
    RoadSegment road3l(-0.02, 0.76, -0.02, 1.0, 0.0, 0.76, 0.0, 1.0, false, "sss", true);
    RoadSegment road6(-0.08, 0.76, -0.08, 0.0, -0.1, 0.76, -0.1, 0.0, false, "kkk1", true);
    RoadSegment road7(-0.1, 0.76, -0.1, 0.0, -0.12, 0.76, -0.12, 0.0, false, "sss", true);
    RoadSegment road8(-0.08, 0.76, -0.08, 1.0, -0.1, 0.76, -0.1, 1.0, false, "kkk1", false);
    RoadSegment road9(-0.1, 0.76, -0.1, 1.0, -0.12, 0.76, -0.12, 1.0, false, "sss", true);
    road4.addConnectedRoad(&road2);
    road4.addConnectedRoad(&road3);
    road5.addConnectedRoad(&road3l);
    road5.addConnectedRoad(&road1);
    road8.addConnectedRoad(&road6);
    road8.addConnectedRoad(&road2);
    TrafficLight light1(road4, 0.08, 0.70);
    TrafficLight light2(road5, -0.05, 0.70);
    TrafficLight light3(road8, -0.05, 0.9);

    road1.getColorFromCongestion();
    road2.getColorFromCongestion();
    road3.getColorFromCongestion();
    road4.getColorFromCongestion();
    road5.getColorFromCongestion();
    road3l.getColorFromCongestion();
    road6.getColorFromCongestion();
    road7.getColorFromCongestion();
    road8.getColorFromCongestion();
    road9.getColorFromCongestion();



    //tekstura za ulice
    unsigned int VAOU, VBOU, EBOU;
    glGenVertexArrays(1, &VAOU);
    glGenBuffers(1, &VBOU);
    glGenBuffers(1, &EBOU);

    glBindVertexArray(VAOU);

    glBindBuffer(GL_ARRAY_BUFFER, VBOU);
    glBufferData(GL_ARRAY_BUFFER, sizeof(streats), streats, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOU);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    



    //tektura za grb
    unsigned int VAOG, VBOG, EBOG;
    glGenVertexArrays(1, &VAOG);
    glGenBuffers(1, &VBOG);
    glGenBuffers(1, &EBOG);

    glBindVertexArray(VAOG);

    glBindBuffer(GL_ARRAY_BUFFER, VBOG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grb), grb, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grbIndices), grbIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int textureGRB;
    glGenTextures(1, &textureGRB);
    glBindTexture(GL_TEXTURE_2D, textureGRB);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int widthg, heightg, nrChannelsg;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("COA_Odzaci.png", &widthg, &heightg, &nrChannelsg, 0);
    if (data) {
        if (nrChannelsg == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthg, heightg, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (nrChannelsg == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthg, heightg, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);




    glClearColor(1.0, 1.0, 1.0, 1.0);
    float lastTime = 0.0f;
    int i = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glfwGetCursorPos(window, &mouseX, &mouseY);

        float xPos = (2.0f * mouseX) / wWidth - 1.0f;
        float yPos = 1.0f - (2.0f * mouseY) / wHeight;
        std::string hoveredRoadName = "";

        mouseButtonStateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        mouseButtonStateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

        if (mouseButtonStateLeft == GLFW_PRESS || mouseButtonStateRight == GLFW_PRESS) {
            vector<RoadSegment*> roads = { &road1, &road2, &road3,  &road4, &road5, &road3l, &road6, &road7, &road8, &road9 };
            for (auto* road : roads) {
                float minX = std::min({ road->x1, road->x2, road->x3, road->x4 });
                float maxX = std::max({ road->x1, road->x2, road->x3, road->x4 });
                float minY = std::min({ road->y1, road->y2, road->y3, road->y4 });
                float maxY = std::max({ road->y1, road->y2, road->y3, road->y4 });
                if (xPos >= minX && xPos <= maxX && yPos >= minY && yPos <= maxY) {
                    if (mouseButtonStateLeft == GLFW_PRESS) {
                        road->congestion += 0.0003;
                        if (road->congestion > 1.0f) road->congestion = 1.0f;
                        road->getColorFromCongestion();
                    }
                    else if (mouseButtonStateRight == GLFW_PRESS) {
                        road->congestion -= 0.0003;
                        if (road->congestion < 0.0f) road->congestion = 0.0f;
                        road->getColorFromCongestion();
                    }
                }

            }
        }

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        glfwPollEvents();
        //Brisanje ekrana
        glClear(GL_COLOR_BUFFER_BIT);

        vector<RoadSegment*> roads = { &road1, &road2, &road3,  &road4, &road5, &road3l, &road6, &road7, &road8, &road9};
        for (auto* road : roads) {
            float minX = std::min({ road->x1, road->x2, road->x3, road->x4 });
            float maxX = std::max({ road->x1, road->x2, road->x3, road->x4 });
            float minY = std::min({ road->y1, road->y2, road->y3, road->y4 });
            float maxY = std::max({ road->y1, road->y2, road->y3, road->y4 });

            if (xPos >= minX && xPos <= maxX && yPos >= minY && yPos <= maxY) {
                hoveredRoadName = road->name;
                break;
            }
        }
        if (hoveredRoadName != "") {
            stbi_set_flip_vertically_on_load(true);
            string name = hoveredRoadName + ".png";
            unsigned char* data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                if (nrChannels == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                else if (nrChannels == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                }
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else {
                std::cerr << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
        }
        else {
            if (i != 0) {
                stbi_set_flip_vertically_on_load(true);
                unsigned char* data = stbi_load("sss.png", &width, &height, &nrChannels, 0);
                if (data) {

                    if (nrChannels == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    }
                    else if (nrChannels == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    }
                    glGenerateMipmap(GL_TEXTURE_2D);
                }
                else {
                    std::cerr << "Failed to load texture" << std::endl;
                }
                stbi_image_free(data);
            }
        }
        i += 1;

       
        road1.changeCongestion();
        road2.changeCongestion();
        road3.changeCongestion();
        road4.changeCongestion();
        road5.changeCongestion();
        road3l.changeCongestion();
        road6.changeCongestion();
        road7.changeCongestion();
        road8.changeCongestion();
        road9.changeCongestion();
        float vertices[] = {
        road1.x1, road1.y1, road1.r,road1.g, road1.b,
        road1.x2, road1.y2,road1.r, road1.g, road1.b,
         road1.x3, road1.y3, road1.r,road1.g, road1.b,
        road1.x4, road1.y4,road1.r, road1.g, road1.b,

        road2.x1, road2.y1, road2.r,road2.g, road2.b,
        road2.x2, road2.y2,road2.r, road2.g, road2.b,
         road2.x3, road2.y3, road2.r,road2.g, road2.b,
        road2.x4, road2.y4,road2.r, road2.g, road2.b,

        road3.x1, road3.y1, road3.r,road3.g, road3.b,
        road3.x2, road3.y2,road3.r, road3.g, road3.b,
         road3.x3, road3.y3, road3.r,road3.g, road3.b,
        road3.x4, road3.y4,road3.r, road3.g, road3.b,

        road4.x1, road4.y1, road4.r,road4.g, road4.b,
        road4.x2, road4.y2,road4.r, road4.g, road4.b,
         road4.x3, road4.y3, road4.r,road4.g, road4.b,
        road4.x4, road4.y4,road4.r, road4.g, road4.b,

         road5.x1, road5.y1, road5.r,road5.g, road5.b,
        road5.x2, road5.y2,road5.r, road5.g, road5.b,
         road5.x3, road5.y3, road5.r,road5.g, road5.b,
        road5.x4, road5.y4,road5.r, road5.g, road5.b,

        road3l.x1, road3l.y1, road3l.r,road3l.g, road3l.b,
        road3l.x2, road3l.y2,road3l.r, road3l.g, road3l.b,
         road3l.x3, road3l.y3, road3l.r,road3l.g, road3l.b,
        road3l.x4, road3l.y4,road3l.r, road3l.g, road3l.b,

        road6.x1, road6.y1, road6.r,road6.g, road6.b,
        road6.x2, road6.y2,road6.r, road6.g, road6.b,
         road6.x3, road6.y3, road6.r,road6.g, road6.b,
        road6.x4, road6.y4,road6.r, road6.g, road6.b,

        road7.x1, road7.y1, road7.r,road7.g, road7.b,
        road7.x2, road7.y2,road7.r, road7.g, road7.b,
         road7.x3, road7.y3, road7.r,road7.g, road7.b,
        road7.x4, road7.y4,road7.r, road7.g, road7.b,

        road8.x1, road8.y1, road8.r,road8.g, road8.b,
        road8.x2, road8.y2,road8.r, road8.g, road8.b,
         road8.x3, road8.y3, road8.r,road8.g, road8.b,
        road8.x4, road8.y4,road8.r, road8.g, road8.b,

        road9.x1, road9.y1, road9.r,road9.g, road9.b,
        road9.x2, road9.y2,road9.r, road9.g, road9.b,
         road9.x3, road9.y3, road9.r,road9.g, road9.b,
        road9.x4, road9.y4,road9.r, road9.g, road9.b,
        };

        glBindVertexArray(VAO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glViewport(0, 0, wWidth, wHeight);
        glUseProgram(basicShader);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 28, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 32, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 36, 4);


        glUseProgram(basicShader);
        glViewport(0, 0, wWidth, wHeight);
        updateTrafficLight(light1, deltaTime);
        updateTrafficLight(light2, deltaTime);
        updateTrafficLight(light3, deltaTime);
        bool isTwo = false;
        isTwo = drawTrafficLight(light1, VAO, VBO, VAO2, VBO2, VAO3, VBO3, VAO4, VBO4, VAO5, VBO5, vertexCount);
        isTwo = drawLeftTrafficLight(light2, VAOS1, VBOS1, VAOS12, VBOS12, VAOS13, VBOS13, vertexCount);
        isTwo = drawLeftTrafficLightUp(light3, VAOS2, VBOS2, VAOS21, VBOS21, VAOS22, VBOS22, vertexCount);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAO4);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAO5);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS12);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS13);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS21);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS22);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);


        glUseProgram(textureShader);
        glBindVertexArray(VAOG);
        glUniform1i(glGetUniformLocation(textureShader, "texture2"), 0);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, textureGRB); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(textureShader);
        glBindVertexArray(VAOU);
        glUniform1i(glGetUniformLocation(textureShader, "texture1"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);





        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}




unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
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
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
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
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}






void updateTrafficLight(TrafficLight& light, float deltaTime) {
    light.remainingTime -= deltaTime;

    if (light.remainingTime <= 0) {
        // Prebacivanje boje semafora
        if (light.color == "red" && light.nextColor=="redyellow") { // Ako je trenutno crveno
            light.yellow[0] = 1.0f;
            light.remainingTime = 3.0f; // Trajanje žutog svetla
            light.color = "redyellow"; light.nextColor = "green";
            light.rightTurnActive = true;
            return;
        }
        else if (light.color == "redyellow" && light.nextColor == "green") { // Ako je trenutno žuto
            light.yellow[0] = 0.0f; light.green[1] = 1.0f; // Prelazak u zeleno
            light.remainingTime = light.timer / 2; // Polovinu vremena za zeleno
            light.color = "green"; light.nextColor = "yellow";
            light.road.lightIsGreen();
            light.rightTurnActive = false;
            return;
        }
        else if (light.color == "green" && light.nextColor == "yellow") { // Ako je trenutno zeleno
            light.green[1] = 0.0f; light.yellow[0] = 1.0f; // Prelazak u žuto
            light.remainingTime = 3.0f; // Trajanje žutog svetla
            light.color = "yellow"; light.nextColor = "red";
            light.rightTurnActive = true;
            return;
        }
        else if (light.color == "yellow" && light.nextColor == "red") { // Ako je trenutno zeleno
            light.red[0] = 1.0f; light.yellow[0] = 0.0f; // Prelazak u žuto
            light.remainingTime = light.timer; // Trajanje žutog svetla
            light.color = "red"; light.nextColor = "redyellow";
            light.road.lightIsRed();
            light.rightTurnActive = true;
            return;
        }

        //light.timer = 20 + rand() % 21; // Novo nasumično vreme za semafor
       // light.remainingTime = light.timer; // Restartovanje timer-a
    }
}

bool drawTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
        drawCircle(light.x + 0.05f, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        drawArrow(light.x + 0.05f, light.y - 0.1f, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        drawCircle(light.x + 0.05f, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        if (light.rightTurnNum <= 0) {
            light.rightTurnNum = 30;
            if (light.rightTurn[0] >= 0.2) {
                light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
            }
            else {
                light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
            }
        }
        drawArrow(light.x + 0.05f, light.y - 0.1f, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
        light.rightTurnNum -= 1;

    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
        drawCircle(light.x + 0.05f, light.y - 0.1f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO3, VBO3, vertexCount);
        drawArrow(light.x + 0.05f, light.y - 0.1f, 0.02f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
    }
    if (light.color == "redyellow") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        drawCircle(light.x + 0.05f, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        if (light.rightTurnNum <= 0) {
            light.rightTurnNum = 30;
            if (light.rightTurn[0] >= 0.2) {
                light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
            }
            else {
                light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
            }
        }
        drawArrow(light.x + 0.05f, light.y - 0.1f, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
        light.rightTurnNum -= 1;
    }

    return isTwo;
}


bool drawLeftTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }
    if (light.color == "redyellow") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    return isTwo;
}

bool drawLeftTrafficLightUp(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x, light.y, 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }
    if (light.color == "redyellow") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y , 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    return isTwo;
}

void drawArrow(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO) {
    // Create vertices for the arrow
    std::vector<float> arrowVertices;

    // Shaft of the arrow
    float halfWidth = width / 2.0f;
    arrowVertices.insert(arrowVertices.end(), {
        offsetX, offsetY + halfWidth, red, green, blue,               // Top-left
        offsetX + length - headLength, offsetY + halfWidth, red, green, blue, // Top-right
        offsetX, offsetY - halfWidth, red, green, blue,               // Bottom-left

        offsetX + length - headLength, offsetY + halfWidth, red, green, blue, // Top-right
        offsetX + length - headLength, offsetY - halfWidth, red, green, blue, // Bottom-right
        offsetX, offsetY - halfWidth, red, green, blue                // Bottom-left
        });

    // Head of the arrow
    arrowVertices.insert(arrowVertices.end(), {
        offsetX + length - headLength, offsetY + halfWidth * 2, red, green, blue, // Top point
        offsetX + length, offsetY, red, green, blue,                            // Tip of arrow
        offsetX + length - headLength, offsetY - halfWidth * 2, red, green, blue // Bottom point
        });

    // Generate and bind VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, arrowVertices.size() * sizeof(float), arrowVertices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes (position and color)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}



void drawCircle(float offsetX, float offsetY, float r, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO, int& vertexCount) {
    // Number of segments (the more segments, the smoother the circle)
    int segments = 360;

    // Create the circle's vertices
    std::vector<float> circleVertices;

    // Center of the circle
    circleVertices.push_back(offsetX);  // X
    circleVertices.push_back(offsetY);  // Y
    circleVertices.push_back(red);      // R
    circleVertices.push_back(green);    // G
    circleVertices.push_back(blue);     // B

    // Circle perimeter vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.141592 * i / segments;
        float x = offsetX + r * cos(angle);
        float y = offsetY + r * sin(angle);

        circleVertices.push_back(x);      // X
        circleVertices.push_back(y);      // Y
        circleVertices.push_back(red);    // R
        circleVertices.push_back(green);  // G
        circleVertices.push_back(blue);   // B
    }

    // Generate and bind VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes (position and color)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Return the vertex count (the number of vertices in the circle)
    vertexCount = circleVertices.size() / 5;  // 5 values per vertex (X, Y, R, G, B)
}





void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Adjust speed based on vertical scroll
    if (speed >= 0) {
        speed += yoffset * 0.00005;  // Adjust this multiplier for desired sensitivity

        // Limit the speed to be within a certain range
        if (speed < 0.00005) speed = 0.00005;  // Minimum speed
        if (speed > 0.002) speed = 0.002;      // Maximum speed
    }
    else {
        speed -= yoffset * 0.00005;  // Adjust this multiplier for desired sensitivity

        // Limit the speed to be within a certain range
        if (speed > -0.00005) speed = -0.00005;  // Minimum speed
        if (speed < -0.002) speed = -0.002;
    }
    freeSpeed += yoffset * 0.00001;  // Adjust this multiplier for desired sensitivity

    // Limit the speed to be within a certain range
    if (freeSpeed < 0.00001) freeSpeed = 0.00001;  // Minimum speed
    if (freeSpeed > 0.00004) freeSpeed = 0.00004;
}