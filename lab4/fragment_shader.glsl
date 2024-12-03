#version 330 core

in vec3 FragPos; // Позиция фрагмента (из vertex shader)
in vec3 Normal;  // Нормаль фрагмента (из vertex shader)
in vec2 TexCoord; // Текстурные координаты (из vertex shader)

out vec4 FragColor;

uniform vec3 lightPos;     // Позиция источника света
uniform vec3 viewPos;      // Позиция камеры
uniform vec3 lightColor;   // Цвет света
uniform vec3 objectColor;  // Цвет объекта
uniform sampler2D texture1; // Текстура объекта

// Материальные параметры
uniform float shininess; // Гладкость материала

void main()
{
    // Нормализуем векторы
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Диффузное освещение (освещенность зависит от угла между нормалью и направлением на источник света)
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Спекулярное освещение (зависит от угла между отражением света и направлением на камеру)
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Цвет объекта с учетом текстуры
    vec4 texColor = texture(texture1, TexCoord);
    
    // Компоненты освещения
    vec3 ambient = 0.1 * lightColor; // Небольшая амбиентная составляющая
    vec3 diffuse = diff * lightColor; // Диффузная составляющая
    vec3 specular = spec * lightColor; // Спекулярная составляющая
    
    // Итоговый цвет
    vec3 result = (ambient + diffuse + specular) * texColor.rgb * objectColor;
    
    FragColor = vec4(result, 1.0);
}
