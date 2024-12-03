// g++ lab4.cpp -lGL -lGLEW -lGLU -lsfml-window -lsfml-system -lsfml-graphics -lglut
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int FrameRateLimit = 30;

glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

GLuint loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Ошибка загрузки текстуры: " << filepath << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}


std::string loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Ошибка чтения файла шейдера: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Ошибка линковки программы: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void setupPyramid(GLuint& VAO, GLuint& VBO) {
    float vertices[] = {

        0.0f,  0.5f,  0.0f,  0.0f,  0.707f,  0.707f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, -0.0f,  0.707f,  0.707f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.707f,  0.707f,  1.0f, 0.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.707f, -0.707f,  0.5f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.707f, -0.707f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.707f, -0.707f,  0.0f, 0.0f,

        0.0f,  0.5f,  0.0f, -0.707f,  0.707f,  0.0f,   0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, -0.707f, -0.5f, -0.707f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -0.707f, -0.5f,  0.707f,   1.0f, 0.0f,

        0.0f,  0.5f,  0.0f,  0.707f,  0.707f,  0.0f,   0.5f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.707f, -0.5f,  0.707f,   1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.707f, -0.5f, -0.707f,   0.0f, 0.0f
    };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


int main() {

    sf::Window window(sf::VideoMode(800.f, 600.f), "Lab 4", sf::Style::Default, sf::ContextSettings(24));

    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка инициализации GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint texture = loadTexture("158.JPG");
    GLuint normalMap = loadTexture("158_norm.JPG");

    if (texture == 0 || normalMap == 0) {
        std::cerr << "Ошибка загрузки текстур." << std::endl;
        return -1;
    }

    GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "Ошибка создания шейдерной программы." << std::endl;
        return -1;
    }


    GLuint pyramidVAO, pyramidVBO;
    setupPyramid(pyramidVAO, pyramidVBO);

    glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glm::vec3 pyramidPosition(0.0f, 0.0f, 0.0f);


    float lightPosition_x = 3.0f;
    float lightPosition_y = 2.0f;
    float lightPosition_z = 0.0f;

    float viewPos_x = 0.0f;
    float viewPos_y = 0.0f;
    float viewPos_z = 3.0f;

    float shininess = 32.0f;

    while (window.isOpen()) {
        
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

        }

        float cameraSpeed = 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraPos += cameraSpeed * cameraFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraPos -= cameraSpeed * cameraFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            lightPosition_x -= 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            lightPosition_x += 0.1f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            shininess -= 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            shininess += 0.1f;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPosition_x, lightPosition_y, lightPosition_y);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), viewPos_x,viewPos_y,viewPos_z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f); // Белый свет
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.5f, 0.31f); // Цвет пирамиды
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);

        model = glm::translate(glm::mat4(1.0f), pyramidPosition);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);


        window.display();
    }

    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteProgram(shaderProgram);

    return 0;
}