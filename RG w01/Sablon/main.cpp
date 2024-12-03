// Autor: Nedeljko Tesanovic
// Opis: Zestoko iskomentarisan program koji crta sareni trougao u OpenGL-u

#define _CRT_SECURE_NO_WARNINGS
 //Biblioteke za stvari iz C++-a (unos, ispis, fajlovi, itd - potrebne za kompajler sejdera) 
#include <iostream>
#include <fstream>
#include <sstream>

//Biblioteke OpenGL-a
#include <GL/glew.h>   //Omogucava laksu upotrebu OpenGL naredbi
#include <GLFW/glfw3.h>//Olaksava pravljenje i otvaranje prozora (konteksta) sa OpenGL sadrzajem
#include <vector>
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
        // Inicijalizacija sa vrednostima
    }

    void addConnectedRoad(RoadSegment* road) {
        connectedRoads.push_back(road); // Dodavanje puta u povezane puteve
    }

    void getColorFromCongestion() {
        r = congestion;      // Crvena raste s gužvom
        g = 1.0f - congestion; // Zelena opada s gužvom
        b = 0.0f;            // Plava je stalno 0
    }

    void lightIsGreen() {
        speed = -speed;
        isGreen = true;
       // congestion -= 0.6;
        if (congestion < 0) {
            congestion = 0;
       };
        if (congestion > 1) {
            congestion = 1;
        };
        getColorFromCongestion();
    }

    void lightIsRed() {
     //congestion += 0.6;
        speed = -speed;
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
            congestion += speed;
            getColorFromCongestion();
            if (isGreen == true) {
                int numConnectedRoads = connectedRoads.size(); // Get the number of connected roads
                for (RoadSegment* road : connectedRoads) {
                    if (road != nullptr) { // Proveri da li je pokazivač validan
                        road->congestion -= speed / numConnectedRoads;
                        road->getColorFromCongestion();
                    }
                }
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
    string color;
    string nextColor;
    RoadSegment& road;

    TrafficLight(RoadSegment& road1, float x1, float y1) 
        : road(road1), x(x1), y(y1){
        timer = 20 + rand() % 21; // Nasumično vreme između 20 i 40 sekundi
        remainingTime = timer;

        // Definisanje boja
        red[0] = 1.0f; red[1] = 0.0f; red[2] = 0.0f;
        yellow[0] = 1.0f; yellow[1] = 1.0f; yellow[2] = 0.0f;
        green[0] = 0.0f; green[1] = 1.0f; green[2] = 0.0f;
        rightTurn[0] = 0.3f; rightTurn[1] = 0.3f; rightTurn[2] = 0.3f;

        rightTurnActive = false; // Početno isključeno
        color = "red";
        nextColor = "yellow";
    }
};



unsigned int compileShader(GLenum type, const char* source); //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
unsigned int createShader(const char* vsSource, const char* fsSource); //Pravi objedinjeni sejder program koji se sastoji od Verteks sejdera ciji je kod na putanji vsSource i Fragment sejdera na putanji fsSource
void initTrafficLight(TrafficLight& light, float x, float y);
void updateTrafficLight(TrafficLight& light, float deltaTime);
void drawCircle(float offsetX, float offsetY, float r, float red, float green, float blue, unsigned int& VAO, unsigned int& VBO, int& vertexCount);
void drawTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, int& vertexCount);




int main(void)
{
    srand(time(0));

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Pokretanje GLFW biblioteke
    // Nju koristimo za stvaranje okvira prozora
    if (!glfwInit()) // !0 == 1; glfwInit inicijalizuje GLFW i vrati 1 ako je inicijalizovana uspjesno, a 0 ako nije
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    //Odredjivanje OpenGL verzije i profila (3.3, programabilni pajplajn)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Stvaranje prozora
    GLFWwindow* window; //Mjesto u memoriji za prozor
    unsigned int wWidth = 1000;
    unsigned int wHeight = 1000;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); // Napravi novi prozor
    // glfwCreateWindow( sirina, visina, naslov, monitor na koji ovaj prozor ide preko citavog ekrana (u tom slucaju umjesto NULL ide glfwGetPrimaryMonitor() ), i prozori sa kojima ce dijeliti resurse )
    if (window == NULL) //Ako prozor nije napravljen
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); //Gasi GLFW
        return 2; //Vrati kod za gresku
    }
    // Postavljanje novopecenog prozora kao aktivni (sa kojim cemo da radimo)
    glfwMakeContextCurrent(window);

    // Inicijalizacija GLEW biblioteke
    if (glewInit() != GLEW_OK) //Slicno kao glfwInit. GLEW_OK je predefinisani izlazni kod za uspjesnu inicijalizaciju sadrzan unutar biblioteke
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    unsigned VAO;
    glGenVertexArrays(1, &VAO);
    unsigned VBO;
    glGenBuffers(1, &VBO);
    int vertexCount = 0;
    unsigned VAO1;
    glGenVertexArrays(1, &VAO1);
    unsigned VBO1;
    glGenBuffers(1, &VBO1);


    RoadSegment road1(-1.0, 0.76, 0.0, 0.76, -1.0, 0.74, 0.0, 0.74, true, "Main Street", false);
    RoadSegment road2(0.0, 0.76, 1.0, 0.76, 0.0, 0.74, 1.0, 0.74, true, "Main Street", true);
    RoadSegment road3(0.0, 0.76, 0.0, 1.0, 0.02, 0.76, 0.02, 1.0, true, "Main Street", true);
    road1.addConnectedRoad(&road2);
    road1.addConnectedRoad(&road3);
    TrafficLight light1(road1, 0.0, 0.75);
    // Pozivamo funkciju za prigušenost
    road1.getColorFromCongestion();
    road2.getColorFromCongestion();
    road3.getColorFromCongestion();
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
    };

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    // Inside the main loop:
    double  mouseX, mouseY;
    int mouseButtonStateLeft, mouseButtonStateRight;



    glClearColor(0.5, 0.5, 0.5, 1.0); //Podesavanje boje pozadine: RGBA (R - Crvena, G - Zelena, B - Plava, A = neprovidno; Opseg od 0 do 1, gdje je 0 crno a 1 svijetlo)
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window)) //Beskonacna petlja iz koje izlazimo tek kada prozor treba da se zatvori
    {
        //Unos od korisnika bez callback funckcije. GLFW_PRESS = Dugme je trenutno pritisnuto. GLFW_RELEASE = Dugme trenutno nije pritisnuto
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Convert mouse position from screen coordinates to OpenGL coordinates
        float xPos = (2.0f * mouseX) / wWidth - 1.0f;
        float yPos = 1.0f - (2.0f * mouseY) / wHeight;

        // Check if the left or right mouse button is pressed
        mouseButtonStateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        mouseButtonStateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

        // Only proceed if either mouse button is pressed
        if (mouseButtonStateLeft == GLFW_PRESS || mouseButtonStateRight == GLFW_PRESS) {
            vector<RoadSegment*> roads = { &road1, &road2, &road3 };
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
        lastTime = currentTime;// Pronađite vreme proteklo između frejmova
        glfwPollEvents(); // Obrađivanje događaja
        //Brisanje ekrana
        glClear(GL_COLOR_BUFFER_BIT);
       
        road1.changeCongestion();
        road2.changeCongestion();
        road3.changeCongestion();
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
        };

        glBindVertexArray(VAO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glViewport(0, 0, wWidth, wHeight); // Da crtamo na lijevoj polovini ekrana
        glUseProgram(basicShader);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);   // Horizontalna 1
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);


        glUseProgram(basicShader);
        glViewport(0, 0, wWidth, wHeight);
       // for (int i = 0; i < 3; i++) {
            updateTrafficLight(light1, deltaTime);
            drawTrafficLight(light1, VAO, VBO, vertexCount);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
      //  }


        //Zamjena vidljivog bafera sa pozadinskim
        glfwSwapBuffers(window);

        //Hvatanje dogadjaja koji se ticu okvira prozora (promjena velicine, pomjeranje itd)
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    //Sve OK - batali program
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
        if (light.color == "red" && light.nextColor=="yellow") { // Ako je trenutno crveno
            light.red[0] = 0.0f; light.yellow[0] = 1.0f;
            light.remainingTime = 3.0f; // Trajanje žutog svetla
            light.color = "yellow"; light.nextColor = "green";
            return;
        }
        else if (light.color == "yellow" && light.nextColor == "green") { // Ako je trenutno žuto
            light.yellow[0] = 0.0f; light.green[1] = 1.0f; // Prelazak u zeleno
            light.remainingTime = light.timer / 2; // Polovinu vremena za zeleno
            light.color = "green"; light.nextColor = "yellow";
            light.road.lightIsGreen();
            return;
        }
        else if (light.color == "green" && light.nextColor == "yellow") { // Ako je trenutno zeleno
            light.green[1] = 0.0f; light.yellow[0] = 1.0f; // Prelazak u žuto
            light.remainingTime = 3.0f; // Trajanje žutog svetla
            light.color = "yellow"; light.nextColor = "red";
            return;
        }
        else if (light.color == "yellow" && light.nextColor == "red") { // Ako je trenutno zeleno
            light.red[0] = 1.0f; light.yellow[0] = 0.0f; // Prelazak u žuto
            light.remainingTime = light.timer; // Trajanje žutog svetla
            light.color = "red"; light.nextColor = "yellow";
            light.road.lightIsRed();
            return;
        }

        //light.timer = 20 + rand() % 21; // Novo nasumično vreme za semafor
       // light.remainingTime = light.timer; // Restartovanje timer-a
    }
}

void drawTrafficLight(TrafficLight& light, unsigned int& VAO, unsigned int& VBO, int& vertexCount) {
    // Prikazivanje crvenog svetla
    if (light.color == "red") {
        drawCircle(light.x, light.y, 0.02f, light.red[0], light.red[1], light.red[2], VAO, VBO, vertexCount);
    }

    // Prikazivanje žutog svetla
    if (light.color == "yellow") {
        drawCircle(light.x, light.y - 0.05f, 0.02f, light.yellow[0], light.yellow[1], light.yellow[2], VAO, VBO, vertexCount);
    }

    // Prikazivanje zelenog svetla
    if (light.color == "green") {
        drawCircle(light.x, light.y - 0.1f, 0.02f, light.green[0], light.green[1], light.green[2], VAO, VBO, vertexCount);
    }

    // Prikazivanje indikatora za desno skretanje ako je aktivan
    if (light.rightTurnActive) {
        drawCircle(light.x + 0.03f, light.y - 0.1f, 0.02f, light.rightTurn[0], light.rightTurn[1], light.rightTurn[2], VAO, VBO, vertexCount);
    }
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
