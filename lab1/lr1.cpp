// g++ -o lr1 lr1.cpp -lGL -lGLEW -lglfw -lm
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

float radius = 100.0f;
float pulseSpeed = 0.05f;
bool increasing = true;
bool is_update = false;
bool is_inscrease = false;
bool is_descrease = false;


void drawCircle(float x_center, float y_center, float radius) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        glBegin(GL_POINTS);
        glVertex2f(x_center + x, y_center + y);
        glVertex2f(x_center + y, y_center + x);
        glVertex2f(x_center - x, y_center + y);
        glVertex2f(x_center - y, y_center + x);
        glVertex2f(x_center + x, y_center - y);
        glVertex2f(x_center + y, y_center - x);
        glVertex2f(x_center - x, y_center - y);
        glVertex2f(x_center - y, y_center - x);
        glEnd();

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void display(float x_center,float y_center,float radius) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCircle(x_center, y_center, radius);
    glFlush();
}

void updateRadius() {
    if (increasing) {
        radius += pulseSpeed;
        if (radius > 110.0f) increasing = false;
    } else {
        radius -= pulseSpeed;
        if (radius < 90.0f) increasing = true;
    }
}

void updateIncrease(){
    if(radius < 400.0f) radius += pulseSpeed;
}

void updateDecrease(){
    if(radius > 10.0f) radius -= pulseSpeed;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        is_inscrease = true;
        is_descrease = false;
        is_update = false;
    }
        
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        is_inscrease = false;
        is_descrease = true;
        is_update = false;
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS){
        is_inscrease = false;
        is_descrease = false;
        is_update = false;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS){
        is_inscrease = false;
        is_descrease = false;
        is_update = true;
    }
}

int main(int argc, char* argv[]) {

    float x_center = WIDTH/2;
    float y_center = HEIGHT/2;

    if(argc == 4){
        x_center = atof(argv[1]);
        y_center = atof(argv[2]);
        radius = atof(argv[3]);
    }

    if(argc == 3){
        x_center = atof(argv[1]);
        y_center = atof(argv[2]);
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Брезенхэм - Окружность", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glPointSize(3.0f);

    glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WIDTH, 0.0, HEIGHT, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glfwSetKeyCallback(window, keyCallback);

    while (!glfwWindowShouldClose(window)) {
        if (is_update) updateRadius();
        if (is_inscrease) updateIncrease();
        if (is_descrease) updateDecrease();
        display(x_center,y_center,radius);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

