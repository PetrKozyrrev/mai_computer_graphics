#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Функция для инициализации OpenGL
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

// Функция для отображения 3D-куба
void drawCube() {
    
    glBegin(GL_QUADS);

    // Цвет и вершины куба
    glColor3f(1, 0, 0); // Красная грань
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glColor3f(0, 1, 0); // Зелёная грань
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glColor3f(0, 0, 1); // Синяя грань
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glColor3f(1, 1, 0); // Жёлтая грань
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    glColor3f(0, 1, 1); // Голубая грань
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glColor3f(1, 0, 1); // Фиолетовая грань
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();
}

// Функция для отрисовки текста на экране
void drawMatrixText(sf::RenderWindow& window, const glm::mat4& matrix, float x, float y) {
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Ошибка загрузки шрифта\n";
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);

    // Вывожу матрицу на экран
    for (int i = 0; i < 4; ++i) {
        std::string row = "";
        for (int j = 0; j < 4; ++j) {
            row += std::to_string(matrix[i][j]) + " ";
        }
        text.setString(row);
        text.setPosition(x, y + i * 15);
        window.pushGLStates();
        window.draw(text);
        window.popGLStates();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "3D Transformations with OpenGL", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);
    initOpenGL();

    glm::mat4 model = glm::mat4(1.0f); // Модельная матрица
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Матрица камеры
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); // Проекционная матрица

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Управление для изменения трансформаций куба
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f)); // Перемещение вперед
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.1f)); // Перемещение назад
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            model = glm::translate(model, glm::vec3(-0.1f, 0.0f, 0.0f)); // Перемещение влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f)); // Перемещение влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            model = glm::translate(model, glm::vec3(0.0f, 0.1f, 0.0f)); // Перемещение вверх
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f)); // Перемещение вниз

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Поворот влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Поворот вправо
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Поворот вверх
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Поворот вниз
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f)); // Увеличение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f)); // Уменьшение

        // Камера

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            view = glm::translate(view, glm::vec3(-0.1f, 0.0f, 0.0f)); // Перемещение камеры влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            view = glm::translate(view, glm::vec3(0.1f, 0.0f, 0.0f)); // Перемещение камеры влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
            view = glm::translate(view, glm::vec3(0.0f, 0.1f, 0.0f)); // Перемещение камеры вверх
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            view = glm::translate(view, glm::vec3(0.0f, -0.1f, 0.0f)); // Перемещение камеры вниз
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f)); // Перемещение камеры вперед
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f)); // Перемещение камеры назад

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Передача матриц в OpenGL
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection));
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(view * model));

        drawCube(); // Отрисовка куба

        // Отображение текстовой информации о матрицах
        drawMatrixText(window, model, 10.0f, 10.0f); // Отображение модельной матрицы

        window.display();
    }

    return 0;
}
