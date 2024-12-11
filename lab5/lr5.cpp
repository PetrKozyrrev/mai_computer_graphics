#include <SFML/Graphics.hpp>
#include "RayTracing.h"

// Размер окна
const int WIDTH = 800;
const int HEIGHT = 600;

// Камера
Vector3 cameraPosition(0, 0, 0);
Vector3 cameraDirection(0, 0, 1);

// Основная функция
int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Ray Tracing");

    Scene scene;

    // Буфер для изображения
    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);

    // Камера на 2D-плоскости, генерируем лучи для каждого пикселя
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Нормализуем координаты пикселя на [-1, 1] (в пространстве камеры)
            float px = (2.0f * (x + 0.5f) / WIDTH - 1.0f) * 1.0f; // X координата
            float py = -(2.0f * (y + 0.5f) / HEIGHT - 1.0f) * 1.0f; // Y координата

            // Направление луча
            Vector3 direction(px, py, 1.0f);
            Ray ray(cameraPosition, direction);

            // Трассировка луча
            Vector3 color = scene.trace(ray);

            // Переводим цвет в диапазон [0, 255] и сохраняем в изображении
            image.setPixel(x, y, sf::Color(
                std::min(int(color.x * 255), 255),
                std::min(int(color.y * 255), 255),
                std::min(int(color.z * 255), 255)
            ));
        }
    }

    // Отображаем изображение
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
