#ifndef RAYTRACING_H
#define RAYTRACING_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <limits>
#include <random>

const float EPSILON = 1e-4f;
const int MAX_DEPTH = 5;
const float PI = 3.14159265358979323846f;

// Класс для представления 3D-вектора
class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 normalize() const {
        float len = sqrt(x * x + y * y + z * z);
        return *this * (1.0f / len);
    }

    float dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
};

// Класс луча
class Ray {
public:
    Vector3 origin, direction;

    Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction.normalize()) {}

    Vector3 at(float t) const {
        return origin + direction * t;
    }
};

// Класс материала для объектов
class Material {
public:
    Vector3 color;
    float reflectivity;
    float roughness;    // Чем выше, тем шероховатее поверхность
    int samples;        // Количество случайных выборок лучей для освещения

    Material(const Vector3& color, float reflectivity = 0.0f, float roughness = 0.0f, int samples = 1)
        : color(color), reflectivity(reflectivity), roughness(roughness), samples(samples) {}
};

// Класс для представления сфер
class Sphere {
public:
    Vector3 center;
    float radius;
    Material material;

    Sphere(const Vector3& center, float radius, const Material& material)
        : center(center), radius(radius), material(material) {}

    bool intersect(const Ray& ray, float& t) const {
        Vector3 oc = ray.origin - center;
        float b = oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - c;

        if (discriminant > 0) {
            t = -b - sqrt(discriminant);
            return t > 0;
        }
        return false;
    }

    Vector3 getNormal(const Vector3& point) const {
        return (point - center).normalize();
    }
};

// Класс для представления плоскости (например, пола)
class Plane {
public:
    Vector3 point;     // Точка на плоскости
    Vector3 normal;    // Нормаль плоскости
    Material material; // Материал плоскости

    Plane(const Vector3& point, const Vector3& normal, const Material& material)
        : point(point), normal(normal), material(material) {}

    bool intersect(const Ray& ray, float& t) const {
        float denom = normal.dot(ray.direction);
        if (fabs(denom) > EPSILON) {
            Vector3 p0l0 = point - ray.origin;
            t = p0l0.dot(normal) / denom;
            return (t >= 0);
        }
        return false;
    }

    Vector3 getNormal(const Vector3& point) const {
        return normal;
    }
};

// Класс для представления источников света
class Light {
public:
    Vector3 position;
    Vector3 color;
    float intensity;

    Light(const Vector3& position, const Vector3& color, float intensity)
        : position(position), color(color), intensity(intensity) {}
};

// Функция для генерации случайных направлений (на основе сферы)
    Vector3 randomInUnitSphere() {
        static std::default_random_engine generator;
        static std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

        Vector3 p;
        do {
            p = Vector3(distribution(generator), distribution(generator), distribution(generator));
        } while (p.dot(p) >= 1.0f);  // Делаем так, чтобы точка была внутри единичной сферы
        return p;
    }

// Основной класс сцены
class Scene {
public:
    std::vector<Sphere> spheres;
    std::vector<Plane> planes;
    std::vector<Light> lights;

    Scene() {

        // Пример материала с шероховатостью и случайными выборками
        Material morematteMaterial(Vector3(0.2f, 0.7f, 0.3f), 0.1f, 0.9f, 20);  // 20 выборок для лучей
        Material matteMaterial(Vector3(0.7f, 0.7f, 0.7f), 0.1f, 0.3f, 10);  // 10 выборок для лучей
        Material shinyMaterial(Vector3(1.0f, 0.5f, 0.2f), 0.1f, 0.0f, 50);  // 50 выборок для более детализированного освещения 

        // Добавляем 3 сферы
        spheres.push_back(Sphere(Vector3(0, -1, 3), 1.0f, matteMaterial));
        spheres.push_back(Sphere(Vector3(2, 0, 4), 1.0f, shinyMaterial));
        spheres.push_back(Sphere(Vector3(-2, 0, 4), 1.0f, morematteMaterial));

        // Добавляем плоскость (пол)
        planes.push_back(Plane(Vector3(0, -2, 0), Vector3(0, 1, 0), Material(Vector3(0.1f, 0.1f, 0.1f))));

        // Источник света
        lights.push_back(Light(Vector3(5, 5, 0), Vector3(1, 1, 1), 1.0f));
    }

    Vector3 trace(const Ray& ray, int depth = 0) const {
        if (depth > MAX_DEPTH) return Vector3(0.2f, 0.2f, 0.2f); // Фон

        float t_min = std::numeric_limits<float>::max();
        const Sphere* hitSphere = nullptr;
        const Plane* hitPlane = nullptr;

        // Находим ближайшее пересечение с объектом
        for (const auto& sphere : spheres) {
            float t;
            if (sphere.intersect(ray, t) && t < t_min) {
                t_min = t;
                hitSphere = &sphere;
            }
        }

        // Находим пересечение с плоскостью
        for (const auto& plane : planes) {
            float t;
            if (plane.intersect(ray, t) && t < t_min) {
                t_min = t;
                hitPlane = &plane;
            }
        }

        if (hitSphere) {
            Vector3 hitPoint = ray.at(t_min);
            Vector3 normal = hitSphere->getNormal(hitPoint);
            Vector3 color = hitSphere->material.color;

            // Суммируем освещенность от нескольких случайных выборок
            Vector3 lighting(0.0f, 0.0f, 0.0f);

            // Генерация случайных выборок лучей
            for (int i = 0; i < hitSphere->material.samples; ++i) {
                // Случайное отклонение направления луча, с учетом шероховатости
                Vector3 randomDir = normal + randomInUnitSphere() * hitSphere->material.roughness;
                Ray scatteredRay(hitPoint + normal * EPSILON, randomDir.normalize());

                // Освещение от случайно отклонённого луча
                for (const auto& light : lights) {
                    Vector3 lightDir = (light.position - hitPoint).normalize();
                    float diff = std::max(normal.dot(lightDir), 0.0f);
                    lighting = lighting + light.color * diff * light.intensity;
                }
            }

            // Усредняем освещенность по количеству выборок
            lighting = lighting * (1.0f / hitSphere->material.samples);
            color = color * (1 - hitSphere->material.reflectivity) + lighting;

            // Отражения
            if (hitSphere->material.reflectivity > 0.0f) {
                Vector3 reflectDir = ray.direction - normal * 2 * ray.direction.dot(normal);
                Ray reflectRay(hitPoint + normal * EPSILON, reflectDir);
                color = color * (1 - hitSphere->material.reflectivity) +
                        trace(reflectRay, depth + 1) * hitSphere->material.reflectivity;
            }

            return color;
        }

        if (hitPlane) {
            Vector3 hitPoint = ray.at(t_min);
            Vector3 normal = hitPlane->getNormal(hitPoint);
            Vector3 color = hitPlane->material.color;

            // Суммируем освещенность от нескольких случайных выборок
            Vector3 lighting(0.0f, 0.0f, 0.0f);

            for (int i = 0; i < hitPlane->material.samples; ++i) {
                // Случайное отклонение направления луча
                Vector3 randomDir = normal + randomInUnitSphere() * hitPlane->material.roughness;
                Ray scatteredRay(hitPoint + normal * EPSILON, randomDir.normalize());

                // Освещение от случайно отклонённого луча
                for (const auto& light : lights) {
                    Vector3 lightDir = (light.position - hitPoint).normalize();
                    float diff = std::max(normal.dot(lightDir), 0.0f);
                    lighting = lighting + light.color * diff * light.intensity;
                }
            }

            lighting = lighting * (1.0f / hitPlane->material.samples);
            color = color * (1 - hitPlane->material.reflectivity) + lighting;

            return color;
        }

        return Vector3(0.2f, 0.2f, 0.2f); // Фон
    }
};

#endif
