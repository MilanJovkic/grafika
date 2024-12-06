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
float speed = 0.0005;
float freeSpeed = 0.00007;

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
    bool isImportant;
    

    RoadSegment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, bool isRight, string name, bool isOutside, bool isImportant, bool isGreen)
        : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), x4(x4), y4(y4), isRight(isRight), name(name), congestion(0.3f), isGreen(isGreen), isOutside(isOutside), isImportant(isImportant) {
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
        if (isImportant == true) {
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

    TrafficLight(RoadSegment& road1, float x1, float y1, string color, string nextColor, bool rightTurnActive) 
        : road(road1), x(x1), y(y1), color(color), nextColor(nextColor), rightTurnActive(rightTurnActive){
        timer = 20 + rand() % 21; // Nasumično vreme između 20 i 40 sekundi
        remainingTime = timer;

        red[0] = 1.0f; red[1] = 0.0f; red[2] = 0.0f;
        yellow[0] = 1.0f; yellow[1] = 1.0f; yellow[2] = 0.0f;
        green[0] = 0.0f; green[1] = 1.0f; green[2] = 0.0f;
        rightTurn[0] = 0.0f; rightTurn[1] = 1.0f; rightTurn[2] = 0.0f;
        rightTurnNum = 30;
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
bool drawTrafficLightUp(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount);
void drawArrowLeft(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO);
void drawArrowDown(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO);
bool drawTrafficLightLeft(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount);
void drawArrowUp(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO);
bool drawTrafficLightRight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount);

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

    unsigned VAOS3;
    glGenVertexArrays(1, &VAOS3);
    unsigned VBOS3;
    glGenBuffers(1, &VBOS3);
    unsigned VAOS31;
    glGenVertexArrays(1, &VAOS31);
    unsigned VBOS31;
    glGenBuffers(1, &VBOS31);
    unsigned VAOS32;
    glGenVertexArrays(1, &VAOS32);
    unsigned VBOS32;
    glGenBuffers(1, &VBOS32);
    unsigned VAOS33;
    glGenVertexArrays(1, &VAOS33);
    unsigned VBOS33;
    glGenBuffers(1, &VBOS33);
    unsigned VAOS34;
    glGenVertexArrays(1, &VAOS34);
    unsigned VBOS34;
    glGenBuffers(1, &VBOS34);


    unsigned VAOS4;
    glGenVertexArrays(1, &VAOS4);
    unsigned VBOS4;
    glGenBuffers(1, &VBOS4);
    unsigned VAOS41;
    glGenVertexArrays(1, &VAOS41);
    unsigned VBOS41;
    glGenBuffers(1, &VBOS41);
    unsigned VAOS42;
    glGenVertexArrays(1, &VAOS42);
    unsigned VBOS42;
    glGenBuffers(1, &VBOS42);
    unsigned VAOS43;
    glGenVertexArrays(1, &VAOS43);
    unsigned VBOS43;
    glGenBuffers(1, &VBOS43);
    unsigned VAOS44;
    glGenVertexArrays(1, &VAOS44);
    unsigned VBOS44;
    glGenBuffers(1, &VBOS44);


    unsigned VAOS5;
    glGenVertexArrays(1, &VAOS5);
    unsigned VBOS5;
    glGenBuffers(1, &VBOS5);
    unsigned VAOS51;
    glGenVertexArrays(1, &VAOS51);
    unsigned VBOS51;
    glGenBuffers(1, &VBOS51);
    unsigned VAOS52;
    glGenVertexArrays(1, &VAOS52);
    unsigned VBOS52;
    glGenBuffers(1, &VBOS52);
    unsigned VAOS53;
    glGenVertexArrays(1, &VAOS53);
    unsigned VBOS53;
    glGenBuffers(1, &VBOS53);
    unsigned VAOS54;
    glGenVertexArrays(1, &VAOS54);
    unsigned VBOS54;
    glGenBuffers(1, &VBOS54);

    unsigned VAOS6;
    glGenVertexArrays(1, &VAOS6);
    unsigned VBOS6;
    glGenBuffers(1, &VBOS6);
    unsigned VAOS61;
    glGenVertexArrays(1, &VAOS61);
    unsigned VBOS61;
    glGenBuffers(1, &VBOS61);
    unsigned VAOS62;
    glGenVertexArrays(1, &VAOS62);
    unsigned VBOS62;
    glGenBuffers(1, &VBOS62);
    unsigned VAOS63;
    glGenVertexArrays(1, &VAOS63);
    unsigned VBOS63;
    glGenBuffers(1, &VBOS63);
    unsigned VAOS64;
    glGenVertexArrays(1, &VAOS64);
    unsigned VBOS64;
    glGenBuffers(1, &VBOS64);

    unsigned VAOS7;
    glGenVertexArrays(1, &VAOS7);
    unsigned VBOS7;
    glGenBuffers(1, &VBOS7);
    unsigned VAOS71;
    glGenVertexArrays(1, &VAOS71);
    unsigned VBOS71;
    glGenBuffers(1, &VBOS71);
    unsigned VAOS72;
    glGenVertexArrays(1, &VAOS72);
    unsigned VBOS72;
    glGenBuffers(1, &VBOS72);
    unsigned VAOS73;
    glGenVertexArrays(1, &VAOS73);
    unsigned VBOS73;
    glGenBuffers(1, &VBOS73);
    unsigned VAOS74;
    glGenVertexArrays(1, &VAOS74);
    unsigned VBOS74;
    glGenBuffers(1, &VBOS74);

    unsigned VAOS8;
    glGenVertexArrays(1, &VAOS8);
    unsigned VBOS8;
    glGenBuffers(1, &VBOS8);
    unsigned VAOS81;
    glGenVertexArrays(1, &VAOS81);
    unsigned VBOS81;
    glGenBuffers(1, &VBOS81);
    unsigned VAOS82;
    glGenVertexArrays(1, &VAOS82);
    unsigned VBOS82;
    glGenBuffers(1, &VBOS82);
    unsigned VAOS83;
    glGenVertexArrays(1, &VAOS83);

    unsigned VAOS9;
    glGenVertexArrays(1, &VAOS9);
    unsigned VBOS9;
    glGenBuffers(1, &VBOS9);
    unsigned VAOS91;
    glGenVertexArrays(1, &VAOS91);
    unsigned VBOS91;
    glGenBuffers(1, &VBOS91);
    unsigned VAOS92;
    glGenVertexArrays(1, &VAOS92);
    unsigned VBOS92;
    glGenBuffers(1, &VBOS92);
    unsigned VAOS93;
    glGenVertexArrays(1, &VAOS93);
    unsigned VBOS93;
    glGenBuffers(1, &VBOS93);
    unsigned VAOS94;
    glGenVertexArrays(1, &VAOS94);
    unsigned VBOS94;
    glGenBuffers(1, &VBOS94);

    unsigned VAOS10;
    glGenVertexArrays(1, &VAOS10);
    unsigned VBOS10;
    glGenBuffers(1, &VBOS10);
    unsigned VAOS101;
    glGenVertexArrays(1, &VAOS101);
    unsigned VBOS101;
    glGenBuffers(1, &VBOS101);
    unsigned VAOS102;
    glGenVertexArrays(1, &VAOS102);
    unsigned VBOS102;
    glGenBuffers(1, &VBOS102);

    unsigned VAOS125;
    glGenVertexArrays(1, &VAOS125);
    unsigned VBOS125;
    glGenBuffers(1, &VBOS125);
    unsigned VAOS121;
    glGenVertexArrays(1, &VAOS121);
    unsigned VBOS121;
    glGenBuffers(1, &VBOS121);
    unsigned VAOS122;
    glGenVertexArrays(1, &VAOS122);
    unsigned VBOS122;
    glGenBuffers(1, &VBOS122);
    unsigned VAOS123;
    glGenVertexArrays(1, &VAOS123);
    unsigned VBOS123;
    glGenBuffers(1, &VBOS123);
    unsigned VAOS124;
    glGenVertexArrays(1, &VAOS124);
    unsigned VBOS124;
    glGenBuffers(1, &VBOS124);


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

    float index[] = {
        // positions          // colors           // texture coords
         -0.8f, 0.8f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         -0.8f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         -0.35f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         -0.35f, 0.8f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    unsigned int indexIndices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };


    RoadSegment gornjiLevi(-1.0, 0.76, -0.05, 0.76, -1.0, 0.74, -0.05, 0.74, false, "kosovska", true, true, false);
    RoadSegment gornjiDesni(-0.05, 0.76, 1.0, 0.76, -0.05, 0.74, 1.0, 0.74, false, "somborska", true, true, false);
    RoadSegment gornjiGornji2(0.0, 0.76, 0.0, 1.0, 0.02, 0.76, 0.02, 1.0, false, "bulevar", true, false, false);
    RoadSegment gornjiGornji2l(-0.02, 0.76, -0.02, 1.0, 0.0, 0.76, 0.0, 1.0, false, "bulevar", true, false, false);
    RoadSegment gornji2(0.0, 0.74, 0.0, 0.0, 0.02, 0.74 , 0.02, 0.0, true, "bulevar", false, true, true);
    RoadSegment gornji2l(-0.02, 0.74, -0.02, 0.0, 0.0, 0.74, 0.0, 0.0, false, "bulevar", false, true, true);
    RoadSegment gornji1(-0.08, 0.74, -0.08, 0.0, -0.1, 0.74, -0.1, 0.0, false, "bulevar", false, true, true);
    RoadSegment gornji1l(-0.1, 0.74, -0.1, 0.0, -0.12, 0.74, -0.12, 0.0, true, "bulevar", false, true, true);
    RoadSegment gornjiGornji1(-0.08, 0.76, -0.08, 1.0, -0.1, 0.76, -0.1, 1.0, false, "bulevar", false, true, true);
    RoadSegment gornjiGornji1l(-0.1, 0.76, -0.1, 1.0, -0.12, 0.76, -0.12, 1.0, true, "bulevar", false, true, true);
    RoadSegment donjiLevi(-1.0, 0.02, -0.05, 0.02, -1.0, 0.00, -0.05, 0.0, false, "knezMihajlova", true, true, false);
    RoadSegment donjiDesni(1.0, 0.02, -0.05, 0.02, 1.0, 0.00, -0.05, 0.0, false, "knezMihajlova", true, true, false);
    RoadSegment donji2(0.0, 0.00, 0.0, -1.0, 0.02, 0.0, 0.02, -1.0, true, "bulevar", false, true, true);
    RoadSegment donji2l(-0.02, 0.00, -0.02, -1.0, 0.00, 0.0, 0.00, -1.0, false, "bulevar", false, true, true);
    RoadSegment donji1(-0.08, 0.00, -0.08, -1.0, -0.1, 0.0, -0.1, -1.0, false, "bulevar", true, false, false);
    RoadSegment donji1l(-0.1, 0.00, -0.1, -1.0, -0.12, 0.0, -0.12, -1.0, false, "bulevar", true, false, false);
    RoadSegment desniVeliki(0.3, 1.0, 0.32, 1.0, 0.3, -1.0, 0.32, -1.0, false, "backa", true, false, false);
    RoadSegment leviMali(-0.5, 0.00, -0.52, 0.00, -0.5, -1.0, -0.52, -1.0, false, "vidovdanska", true, false, false);
    RoadSegment leviVeliki(-0.75, 0.74, -0.77, 0.74, -0.75, -1.0, -0.77, -1.0, false, "zeleznicka", true, false, false);
    RoadSegment desniKosi(0.75, 0.0, 0.77, 0.0, 1.0, -0.52, 1.0, -0.57, false, "mostonga", true, false, false);
    RoadSegment desniMali(0.75, 0.0, 0.77, 0.0, 0.75, 1.0, 0.77, 1.0, false, "skolska", true, false, false);
    gornji2.addConnectedRoad(&gornjiDesni);
    gornji2.addConnectedRoad(&gornjiGornji2);
    gornji2l.addConnectedRoad(&gornjiGornji2l);
    gornji2l.addConnectedRoad(&gornjiLevi);
    gornjiGornji1.addConnectedRoad(&gornji1);
    gornjiGornji1.addConnectedRoad(&gornjiDesni);
    gornjiGornji1l.addConnectedRoad(&gornjiLevi);
    gornjiGornji1l.addConnectedRoad(&gornji1l);
    gornjiLevi.addConnectedRoad(&gornji1l);
    gornjiLevi.addConnectedRoad(&gornjiDesni);
    gornjiLevi.addConnectedRoad(&gornjiGornji2l);
    gornjiDesni.addConnectedRoad(&gornjiGornji2);
    gornjiDesni.addConnectedRoad(&gornji1);
    gornjiDesni.addConnectedRoad(&gornjiLevi);

    donjiDesni.addConnectedRoad(&gornji2);
    donjiDesni.addConnectedRoad(&donjiLevi);
    donjiDesni.addConnectedRoad(&donji1);
    donji2.addConnectedRoad(&donjiDesni);
    donji2.addConnectedRoad(&gornji2);
    donji2l.addConnectedRoad(&donjiLevi);
    donji2l.addConnectedRoad(&gornji2l);

    gornji1l.addConnectedRoad(&donjiLevi);
    gornji1l.addConnectedRoad(&donji1l);
    gornji1.addConnectedRoad(&donjiDesni);
    gornji1.addConnectedRoad(&donji1);
    donjiLevi.addConnectedRoad(&donji1l);
    donjiLevi.addConnectedRoad(&donjiDesni);
    donjiLevi.addConnectedRoad(&gornji2l);
    TrafficLight light1(gornji2, 0.05, 0.70, "green", "yellow", false);
    TrafficLight light2(gornji2l, -0.05, 0.70, "green", "yellow", false);
    TrafficLight light3(gornjiGornji1, -0.05, 0.9, "green", "yellow", false);
    TrafficLight light4(gornjiGornji1l, -0.15, 0.9, "green", "yellow", false);
    TrafficLight light5(gornjiLevi, -0.15, 0.70, "red", "redyellow", true);
    TrafficLight light6(gornjiDesni, 0.15, 0.8, "red", "redyellow", true);
    TrafficLight light7(donjiDesni, 0.15, 0.05, "red", "redyellow", true);
    TrafficLight light8(donji2, 0.05, -0.05, "green", "yellow", false);
    TrafficLight light9(donji2l, -0.05, -0.05, "green", "yellow", false);
    TrafficLight light10(gornji1l, -0.15, 0.15, "green", "yellow", false);
    TrafficLight light11(gornji1, -0.05, 0.15, "green", "yellow", false);
    TrafficLight light12(donjiLevi, -0.15, -0.05, "red", "redyellow", true);

    gornjiLevi.getColorFromCongestion();
    gornjiDesni.getColorFromCongestion();
    gornjiGornji2.getColorFromCongestion();
    gornji2.getColorFromCongestion();
    gornji2l.getColorFromCongestion();
    gornjiGornji2l.getColorFromCongestion();
    gornji1.getColorFromCongestion();
    gornji1l.getColorFromCongestion();
    gornjiGornji1.getColorFromCongestion();
    gornjiGornji1l.getColorFromCongestion();
    donjiLevi.getColorFromCongestion();
    donjiDesni.getColorFromCongestion();
    donji2.getColorFromCongestion();
    donji2l.getColorFromCongestion();
    donji1.getColorFromCongestion();
    donji1l.getColorFromCongestion();
    desniVeliki.getColorFromCongestion();
    leviMali.getColorFromCongestion();
    leviVeliki.getColorFromCongestion();
    desniKosi.getColorFromCongestion();
    desniMali.getColorFromCongestion();

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



    //tekstura za index
    unsigned int VAOI, VBOI, EBOI;
    glGenVertexArrays(1, &VAOI);
    glGenBuffers(1, &VBOI);
    glGenBuffers(1, &EBOI);

    glBindVertexArray(VAOI);

    glBindBuffer(GL_ARRAY_BUFFER, VBOI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int widthi, heighti, nrChannelsi;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("index.png", &widthi, &heighti, &nrChannelsi, 0);
    if (data2) {
        if (nrChannelsi == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthi, heighti, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        }
        else if (nrChannelsi == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthi, heighti, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data2);




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
            vector<RoadSegment*> roads = { &gornjiLevi, &gornjiDesni, &gornjiGornji2,  &gornji2, &gornji2l, &gornjiGornji2l, &gornji1, &gornji1l, &gornjiGornji1, &gornjiGornji1l , &donjiLevi, &donjiDesni, &donji2, &donji2l,  &donji1 , &donji1l, &desniVeliki, &desniMali, &leviMali, &leviVeliki, &desniKosi };
            for (auto* road : roads) {
                float minX = std::min({ road->x1, road->x2, road->x3, road->x4 });
                float maxX = std::max({ road->x1, road->x2, road->x3, road->x4 });
                float minY = std::min({ road->y1, road->y2, road->y3, road->y4 });
                float maxY = std::max({ road->y1, road->y2, road->y3, road->y4 });
                if (xPos >= minX && xPos <= maxX && yPos >= minY && yPos <= maxY) {
                    if (mouseButtonStateLeft == GLFW_PRESS) {
                        road->congestion += 0.001;
                        if (road->congestion > 1.0f) road->congestion = 1.0f;
                        road->getColorFromCongestion();
                    }
                    else if (mouseButtonStateRight == GLFW_PRESS) {
                        road->congestion -= 0.001;
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

        vector<RoadSegment*> roads = { &gornjiLevi, &gornjiDesni, &gornjiGornji2,  &gornji2, &gornji2l, &gornjiGornji2l, &gornji1, &gornji1l, &gornjiGornji1, &gornjiGornji1l, &donjiLevi, &donjiDesni, &donji2, &donji2l,  &donji1 , &donji1l , &desniVeliki, &desniMali, &leviMali, &leviVeliki, &desniKosi};
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
            unsigned char* data1 = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
            if (data1) {
                if (nrChannels == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
                }
                else if (nrChannels == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
                }
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else {
                std::cerr << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data1);
        }
        else {
            if (i > 5) {
                stbi_set_flip_vertically_on_load(true);
                unsigned char* data1 = stbi_load("bela.png", &width, &height, &nrChannels, 0);
                if (data1) {

                    if (nrChannels == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
                    }
                    else if (nrChannels == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
                    }
                    glGenerateMipmap(GL_TEXTURE_2D);
                }
                else {
                    std::cerr << "Failed to load texture" << std::endl;
                }
                stbi_image_free(data1);
            }
        }
        i += 1;

       
        gornjiLevi.changeCongestion();
        gornjiDesni.changeCongestion();
        gornjiGornji2.changeCongestion();
        gornji2.changeCongestion();
        gornji2l.changeCongestion();
        gornjiGornji2l.changeCongestion();
        gornji1.changeCongestion();
        gornji1l.changeCongestion();
        gornjiGornji1.changeCongestion();
        gornjiGornji1l.changeCongestion();
        donjiLevi.changeCongestion();
        donjiDesni.changeCongestion();
        donji2.changeCongestion();
        donji2l.changeCongestion();
        donji1.changeCongestion();
        donji1l.changeCongestion();
        desniVeliki.changeCongestion();
        leviMali.changeCongestion();
        leviVeliki.changeCongestion();
        desniKosi.changeCongestion();
        desniMali.changeCongestion();
        float vertices[] = {
        desniVeliki.x1, desniVeliki.y1, desniVeliki.r,desniVeliki.g, desniVeliki.b,
        desniVeliki.x2, desniVeliki.y2,desniVeliki.r, desniVeliki.g, desniVeliki.b,
         desniVeliki.x3, desniVeliki.y3, desniVeliki.r,desniVeliki.g, desniVeliki.b,
        desniVeliki.x4, desniVeliki.y4,desniVeliki.r, desniVeliki.g, desniVeliki.b,

         desniMali.x1, desniMali.y1, desniMali.r,desniMali.g, desniMali.b,
        desniMali.x2, desniMali.y2,desniMali.r, desniMali.g, desniMali.b,
         desniMali.x3, desniMali.y3, desniMali.r,desniMali.g, desniMali.b,
        desniMali.x4, desniMali.y4,desniMali.r, desniMali.g, desniMali.b,

         desniKosi.x1, desniKosi.y1, desniKosi.r,desniKosi.g, desniKosi.b,
        desniKosi.x2, desniKosi.y2,desniKosi.r, desniKosi.g, desniKosi.b,
         desniKosi.x3, desniKosi.y3, desniKosi.r,desniKosi.g, desniKosi.b,
        desniKosi.x4, desniKosi.y4,desniKosi.r, desniKosi.g, desniKosi.b,

        leviVeliki.x1, leviVeliki.y1, leviVeliki.r,leviVeliki.g, leviVeliki.b,
        leviVeliki.x2, leviVeliki.y2,leviVeliki.r, leviVeliki.g, leviVeliki.b,
         leviVeliki.x3, leviVeliki.y3, leviVeliki.r,leviVeliki.g, leviVeliki.b,
        leviVeliki.x4, leviVeliki.y4,leviVeliki.r, leviVeliki.g, leviVeliki.b,

        leviMali.x1, leviMali.y1, leviMali.r,leviMali.g, leviMali.b,
        leviMali.x2, leviMali.y2,leviMali.r, leviMali.g, leviMali.b,
         leviMali.x3, leviMali.y3, leviMali.r,leviMali.g, leviMali.b,
        leviMali.x4, leviMali.y4,leviMali.r, leviMali.g, leviMali.b,

        gornjiLevi.x1, gornjiLevi.y1, gornjiLevi.r,gornjiLevi.g, gornjiLevi.b,
        gornjiLevi.x2, gornjiLevi.y2,gornjiLevi.r, gornjiLevi.g, gornjiLevi.b,
         gornjiLevi.x3, gornjiLevi.y3, gornjiLevi.r,gornjiLevi.g, gornjiLevi.b,
        gornjiLevi.x4, gornjiLevi.y4,gornjiLevi.r, gornjiLevi.g, gornjiLevi.b,

        gornjiDesni.x1, gornjiDesni.y1, gornjiDesni.r,gornjiDesni.g, gornjiDesni.b,
        gornjiDesni.x2, gornjiDesni.y2,gornjiDesni.r, gornjiDesni.g, gornjiDesni.b,
         gornjiDesni.x3, gornjiDesni.y3, gornjiDesni.r,gornjiDesni.g, gornjiDesni.b,
        gornjiDesni.x4, gornjiDesni.y4,gornjiDesni.r, gornjiDesni.g, gornjiDesni.b,

        gornjiGornji2.x1, gornjiGornji2.y1, gornjiGornji2.r,gornjiGornji2.g, gornjiGornji2.b,
        gornjiGornji2.x2, gornjiGornji2.y2,gornjiGornji2.r, gornjiGornji2.g, gornjiGornji2.b,
         gornjiGornji2.x3, gornjiGornji2.y3, gornjiGornji2.r,gornjiGornji2.g, gornjiGornji2.b,
        gornjiGornji2.x4, gornjiGornji2.y4,gornjiGornji2.r, gornjiGornji2.g, gornjiGornji2.b,

        gornji2.x1, gornji2.y1, gornji2.r,gornji2.g, gornji2.b,
        gornji2.x2, gornji2.y2,gornji2.r, gornji2.g, gornji2.b,
         gornji2.x3, gornji2.y3, gornji2.r,gornji2.g, gornji2.b,
        gornji2.x4, gornji2.y4,gornji2.r, gornji2.g, gornji2.b,

         gornji2l.x1, gornji2l.y1, gornji2l.r,gornji2l.g, gornji2l.b,
        gornji2l.x2, gornji2l.y2,gornji2l.r, gornji2l.g, gornji2l.b,
         gornji2l.x3, gornji2l.y3, gornji2l.r,gornji2l.g, gornji2l.b,
        gornji2l.x4, gornji2l.y4,gornji2l.r, gornji2l.g, gornji2l.b,

        gornjiGornji2l.x1, gornjiGornji2l.y1, gornjiGornji2l.r,gornjiGornji2l.g, gornjiGornji2l.b,
        gornjiGornji2l.x2, gornjiGornji2l.y2,gornjiGornji2l.r, gornjiGornji2l.g, gornjiGornji2l.b,
         gornjiGornji2l.x3, gornjiGornji2l.y3, gornjiGornji2l.r,gornjiGornji2l.g, gornjiGornji2l.b,
        gornjiGornji2l.x4, gornjiGornji2l.y4,gornjiGornji2l.r, gornjiGornji2l.g, gornjiGornji2l.b,

        gornji1.x1, gornji1.y1, gornji1.r,gornji1.g, gornji1.b,
        gornji1.x2, gornji1.y2,gornji1.r, gornji1.g, gornji1.b,
         gornji1.x3, gornji1.y3, gornji1.r,gornji1.g, gornji1.b,
        gornji1.x4, gornji1.y4,gornji1.r, gornji1.g, gornji1.b,

        gornji1l.x1, gornji1l.y1, gornji1l.r,gornji1l.g, gornji1l.b,
        gornji1l.x2, gornji1l.y2,gornji1l.r, gornji1l.g, gornji1l.b,
         gornji1l.x3, gornji1l.y3, gornji1l.r,gornji1l.g, gornji1l.b,
        gornji1l.x4, gornji1l.y4,gornji1l.r, gornji1l.g, gornji1l.b,

        gornjiGornji1.x1, gornjiGornji1.y1, gornjiGornji1.r,gornjiGornji1.g, gornjiGornji1.b,
        gornjiGornji1.x2, gornjiGornji1.y2,gornjiGornji1.r, gornjiGornji1.g, gornjiGornji1.b,
         gornjiGornji1.x3, gornjiGornji1.y3, gornjiGornji1.r,gornjiGornji1.g, gornjiGornji1.b,
        gornjiGornji1.x4, gornjiGornji1.y4,gornjiGornji1.r, gornjiGornji1.g, gornjiGornji1.b,

        gornjiGornji1l.x1, gornjiGornji1l.y1, gornjiGornji1l.r,gornjiGornji1l.g, gornjiGornji1l.b,
        gornjiGornji1l.x2, gornjiGornji1l.y2,gornjiGornji1l.r, gornjiGornji1l.g, gornjiGornji1l.b,
         gornjiGornji1l.x3, gornjiGornji1l.y3, gornjiGornji1l.r,gornjiGornji1l.g, gornjiGornji1l.b,
        gornjiGornji1l.x4, gornjiGornji1l.y4,gornjiGornji1l.r, gornjiGornji1l.g, gornjiGornji1l.b,

        donjiLevi.x1, donjiLevi.y1, donjiLevi.r,donjiLevi.g, donjiLevi.b,
        donjiLevi.x2, donjiLevi.y2,donjiLevi.r, donjiLevi.g, donjiLevi.b,
         donjiLevi.x3, donjiLevi.y3, donjiLevi.r,donjiLevi.g, donjiLevi.b,
        donjiLevi.x4, donjiLevi.y4,donjiLevi.r, donjiLevi.g, donjiLevi.b,

        donjiDesni.x1, donjiDesni.y1, donjiDesni.r,donjiDesni.g, donjiDesni.b,
        donjiDesni.x2, donjiDesni.y2,donjiDesni.r, donjiDesni.g, donjiDesni.b,
         donjiDesni.x3, donjiDesni.y3, donjiDesni.r,donjiDesni.g, donjiDesni.b,
        donjiDesni.x4, donjiDesni.y4,donjiDesni.r, donjiDesni.g, donjiDesni.b,

        donji2.x1, donji2.y1, donji2.r,donji2.g, donji2.b,
        donji2.x2, donji2.y2,donji2.r, donji2.g, donji2.b,
         donji2.x3, donji2.y3, donji2.r,donji2.g, donji2.b,
        donji2.x4, donji2.y4,donji2.r, donji2.g, donji2.b,

        donji2l.x1, donji2l.y1, donji2l.r,donji2l.g, donji2l.b,
        donji2l.x2, donji2l.y2,donji2l.r, donji2l.g, donji2l.b,
         donji2l.x3, donji2l.y3, donji2l.r,donji2l.g, donji2l.b,
        donji2l.x4, donji2l.y4,donji2l.r, donji2l.g, donji2l.b,

        donji1.x1, donji1.y1, donji1.r,donji1.g, donji1.b,
        donji1.x2, donji1.y2,donji1.r, donji1.g, donji1.b,
         donji1.x3, donji1.y3, donji1.r,donji1.g, donji1.b,
        donji1.x4, donji1.y4,donji1.r, donji1.g, donji1.b,

        donji1l.x1, donji1l.y1, donji1l.r,donji1l.g, donji1l.b,
        donji1l.x2, donji1l.y2,donji1l.r, donji1l.g, donji1l.b,
         donji1l.x3, donji1l.y3, donji1l.r,donji1l.g, donji1l.b,
        donji1l.x4, donji1l.y4,donji1l.r, donji1l.g, donji1l.b,
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
        glDrawArrays(GL_TRIANGLE_STRIP, 40, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 44, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 48, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 52, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 56, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 60, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 64, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 68, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 72, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 76, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 80, 4);

        glUseProgram(basicShader);
        glViewport(0, 0, wWidth, wHeight);
        updateTrafficLight(light1, deltaTime);
        updateTrafficLight(light2, deltaTime);
        updateTrafficLight(light3, deltaTime);
        updateTrafficLight(light4, deltaTime);
        updateTrafficLight(light5, deltaTime);
        updateTrafficLight(light6, deltaTime);
        updateTrafficLight(light7, deltaTime);
        updateTrafficLight(light8, deltaTime);
        updateTrafficLight(light9, deltaTime);
        updateTrafficLight(light10, deltaTime);
        updateTrafficLight(light11, deltaTime);
        updateTrafficLight(light12, deltaTime);
        bool isTwo = false;
        isTwo = drawTrafficLight(light1, VAO, VBO, VAO2, VBO2, VAO3, VBO3, VAO4, VBO4, VAO5, VBO5, vertexCount);
        isTwo = drawLeftTrafficLight(light2, VAOS1, VBOS1, VAOS12, VBOS12, VAOS13, VBOS13, vertexCount);
        isTwo = drawLeftTrafficLightUp(light3, VAOS2, VBOS2, VAOS21, VBOS21, VAOS22, VBOS22, vertexCount);
        isTwo = drawTrafficLightUp(light4, VAOS3, VBOS3, VAOS31, VBOS31, VAOS32, VBOS32, VAOS33, VBOS33, VAOS34, VBOS34, vertexCount);
        isTwo = drawTrafficLightLeft(light5, VAOS4, VBOS4, VAOS41, VBOS41, VAOS42, VBOS42, VAOS43, VBOS43, VAOS44, VBOS44, vertexCount);
        isTwo = drawTrafficLightRight(light6, VAOS5, VBOS5, VAOS51, VBOS51, VAOS52, VBOS52, VAOS53, VBOS53, VAOS54, VBOS54, vertexCount);
        isTwo = drawTrafficLightRight(light7, VAOS6, VBOS6, VAOS61, VBOS61, VAOS62, VBOS62, VAOS63, VBOS63, VAOS64, VBOS64, vertexCount);
        isTwo = drawTrafficLight(light8, VAOS7, VBOS7, VAOS71, VBOS71, VAOS72, VBOS72, VAOS73, VBOS73, VAOS74, VBOS74, vertexCount);
        isTwo = drawLeftTrafficLight(light9, VAOS8, VBOS8, VAOS81, VBOS81, VAOS82, VBOS82,vertexCount);
        isTwo = drawTrafficLightUp(light10, VAOS9, VBOS9, VAOS91, VBOS91, VAOS92, VBOS92, VAOS93, VBOS93, VAOS94, VBOS94, vertexCount);
        isTwo = drawLeftTrafficLightUp(light11, VAOS10, VBOS10, VAOS101, VBOS101, VAOS102, VBOS102, vertexCount);
        isTwo = drawTrafficLightLeft(light12, VAOS125, VBOS125, VAOS121, VBOS121, VAOS122, VBOS122, VAOS123, VBOS123, VAOS124, VBOS124, vertexCount);
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
        glBindVertexArray(VAOS3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS31);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS32);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS33);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS34);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS4);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS41);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS42);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS43);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS44);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS5);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS51);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS52);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS53);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS54);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS6);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS61);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS62);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS63);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS64);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS7);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS71);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS72);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS73);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS74);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS8);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS81);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS82);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS9);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS91);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS92);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS93);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS94);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glBindVertexArray(VAOS10);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS101);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS102);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS125);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS121);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS122);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS123);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(VAOS124);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

        glUseProgram(textureShader);
        glBindVertexArray(VAOG);
        glUniform1i(glGetUniformLocation(textureShader, "texture2"), 0);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, textureGRB); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(textureShader);
        glBindVertexArray(VAOI);
        glUniform1i(glGetUniformLocation(textureShader, "texture3"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureInx);
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


    unsigned int vaos[] = {
        VAO, VAO2, VAO3, VAO4, VAO5, VAO1, VAOS1, VAOS12, VAOS13, VAOS2, VAOS21, VAOS22,
        VAOS3, VAOS31, VAOS32, VAOS33, VAOS34, VAOS4, VAOS41, VAOS42, VAOS43, VAOS44,
        VAOS5, VAOS51, VAOS52, VAOS53, VAOS54, VAOS6, VAOS61, VAOS62, VAOS63, VAOS64,
        VAOS7, VAOS71, VAOS72, VAOS73, VAOS74, VAOS8, VAOS81, VAOS82, VAOS83, VAOS9,
        VAOS91, VAOS92, VAOS93, VAOS94, VAOS10, VAOS101, VAOS102, VAOS125, VAOS121,
        VAOS122, VAOS123, VAOS124, VAOU, VAOG, VAOI
    };
    glDeleteVertexArrays(sizeof(vaos) / sizeof(unsigned int), vaos);
    unsigned int vbos[] = {
       VBO, VBO2, VBO3, VBO4, VBO5, VBO1, VBOS1, VBOS12, VBOS13, VBOS2, VBOS21, VBOS22,
       VBOS3, VBOS31, VBOS32, VBOS33, VBOS34, VBOS4, VBOS41, VBOS42, VBOS43, VBOS44,
       VBOS5, VBOS51, VBOS52, VBOS53, VBOS54, VBOS6, VBOS61, VBOS62, VBOS63, VBOS64,
       VBOS7, VBOS71, VBOS72, VBOS73, VBOS74, VBOS8, VBOS81, VBOS82, VBOS9, VBOS91,
       VBOS92, VBOS93, VBOS94, VBOS10, VBOS101, VBOS102, VBOS125, VBOS121, VBOS122,
       VBOS123, VBOS124, VBOU, VBOG, VBOI
    };
    glDeleteBuffers(sizeof(vbos) / sizeof(unsigned int), vbos);

    unsigned int ebos[] = { EBOU, EBOG, EBOI };
    glDeleteBuffers(sizeof(ebos) / sizeof(unsigned int), ebos);

    glDeleteProgram(basicShader);
    glDeleteProgram(textureShader);

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



bool drawTrafficLightLeft(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.1f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x - 0.05f, light.y , 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.1f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);


    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x - 0.1f, light.y , 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.05f, light.y , 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);

    }
    if (light.color == "redyellow") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.1f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    return isTwo;
}


bool drawTrafficLightRight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x - 0.1f, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x , light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x - 0.05f, light.y, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.1f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);


    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x , light.y, 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x - 0.1f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);

    }
    if (light.color == "redyellow") {
        drawCircle(light.x - 0.1f, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
    }

    return isTwo;
}



bool drawTrafficLightUp(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, unsigned int& VAO1, unsigned int& VBO1, unsigned int& VAO2, unsigned int& VBO2, unsigned int& VAO3, unsigned int& VBO3, unsigned int& VAO4, unsigned int& VBO4, int& vertexCount) {
    // Prikazivanje crvenog svetla
    bool isTwo = false;
    if (light.color == "red") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y , 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        drawArrowLeft(light.x - 0.05f, light.y, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        if (light.rightTurnNum <= 0) {
            light.rightTurnNum = 30;
            if (light.rightTurn[0] >= 0.2) {
                light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
            }
            else {
                light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
            }
        }
        drawArrowLeft(light.x - 0.05f, light.y, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
        light.rightTurnNum -= 1;

    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x, light.y , 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.1f, 0.02f, 0.3, 0.3, 0.3, VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
        drawCircle(light.x - 0.05f, light.y , 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO3, VBO3, vertexCount);
        drawArrowLeft(light.x - 0.05f, light.y, 0.02f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
    }
    if (light.color == "redyellow") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO1, VBO1, vertexCount);
        drawCircle(light.x, light.y , 0.02f, 0.3, 0.3, 0.3, VAO2, VBO2, vertexCount);
        drawCircle(light.x - 0.05f, light.y, 0.02f, 0.3, 0.3, 0.3, VAO3, VBO3, vertexCount);
        if (light.rightTurnNum <= 0) {
            light.rightTurnNum = 30;
            if (light.rightTurn[0] >= 0.2) {
                light.rightTurn[0] = 0.0f; light.rightTurn[1] = 1.0f; light.rightTurn[2] = 0.0f;
            }
            else {
                light.rightTurn[0] = 0.3f; light.rightTurn[1] = 0.3f; light.rightTurn[2] = 0.3f;
            }
        }
        drawArrowLeft(light.x - 0.05f, light.y, 0.015f, 0.02f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO4, VBO4);
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


void drawArrowLeft(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO) {
    // Create vertices for the arrow
    std::vector<float> arrowVertices;

    // Shaft of the arrow
    float halfWidth = width / 2.0f;
    arrowVertices.insert(arrowVertices.end(), {
        offsetX, offsetY + halfWidth, red, green, blue,                 // Top-right
        offsetX - length + headLength, offsetY + halfWidth, red, green, blue, // Top-left
        offsetX, offsetY - halfWidth, red, green, blue,                 // Bottom-right

        offsetX - length + headLength, offsetY + halfWidth, red, green, blue, // Top-left
        offsetX - length + headLength, offsetY - halfWidth, red, green, blue, // Bottom-left
        offsetX, offsetY - halfWidth, red, green, blue                  // Bottom-right
        });

    // Head of the arrow
    arrowVertices.insert(arrowVertices.end(), {
        offsetX - length + headLength, offsetY + halfWidth * 2, red, green, blue, // Top point
        offsetX - length, offsetY, red, green, blue,                              // Tip of arrow
        offsetX - length + headLength, offsetY - halfWidth * 2, red, green, blue  // Bottom point
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


void drawArrowDown(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO) {
    // Create vertices for the arrow
    std::vector<float> arrowVertices;

    // Shaft of the arrow
    float halfWidth = width / 2.0f;
    arrowVertices.insert(arrowVertices.end(), {
        offsetX - halfWidth, offsetY, red, green, blue,                 // Top-left
        offsetX + halfWidth, offsetY, red, green, blue,                 // Top-right
        offsetX - halfWidth, offsetY - length + headLength, red, green, blue, // Bottom-left

        offsetX + halfWidth, offsetY, red, green, blue,                 // Top-right
        offsetX + halfWidth, offsetY - length + headLength, red, green, blue, // Bottom-right
        offsetX - halfWidth, offsetY - length + headLength, red, green, blue  // Bottom-left
        });

    // Head of the arrow
    arrowVertices.insert(arrowVertices.end(), {
        offsetX - halfWidth * 2, offsetY - length + headLength, red, green, blue, // Left point
        offsetX, offsetY - length, red, green, blue,                              // Tip of arrow
        offsetX + halfWidth * 2, offsetY - length + headLength, red, green, blue  // Right point
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


void drawArrowUp(float offsetX, float offsetY, float length, float width, float headLength, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO) {
    // Create vertices for the arrow
    std::vector<float> arrowVertices;

    // Shaft of the arrow
    float halfWidth = width / 2.0f;
    arrowVertices.insert(arrowVertices.end(), {
        offsetX - halfWidth, offsetY, red, green, blue,                 // Bottom-left
        offsetX + halfWidth, offsetY, red, green, blue,                 // Bottom-right
        offsetX - halfWidth, offsetY + length - headLength, red, green, blue, // Top-left

        offsetX + halfWidth, offsetY, red, green, blue,                 // Bottom-right
        offsetX + halfWidth, offsetY + length - headLength, red, green, blue, // Top-right
        offsetX - halfWidth, offsetY + length - headLength, red, green, blue  // Top-left
        });

    // Head of the arrow
    arrowVertices.insert(arrowVertices.end(), {
        offsetX - halfWidth * 2, offsetY + length - headLength, red, green, blue, // Left point
        offsetX, offsetY + length, red, green, blue,                              // Tip of arrow
        offsetX + halfWidth * 2, offsetY + length - headLength, red, green, blue  // Right point
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
        if (speed < 0.0001) speed = 0.0001;  // Minimum speed
        if (speed > 0.002) speed = 0.002;      // Maximum speed
    }
    else {
        speed -= yoffset * 0.00005;  // Adjust this multiplier for desired sensitivity

        // Limit the speed to be within a certain range
        if (speed > -0.0001) speed = -0.0001;  // Minimum speed
        if (speed < -0.002) speed = -0.002;
    }
    freeSpeed += yoffset * 0.00001;  // Adjust this multiplier for desired sensitivity

    // Limit the speed to be within a certain range
    if (freeSpeed < 0.00001) freeSpeed = 0.00001;  // Minimum speed
    if (freeSpeed > 0.00007) freeSpeed = 0.0001;
}

