// Dependecias Necesarias
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <vector>
#include <cassert>

// Headers de las clases Necesarias
#include "../headers/uniforms.h"
#include "../headers/shaders.h"
#include "../headers/fragment.h"
#include "../headers/triangle.h"
#include "../headers/camera.h"
#include "../headers/ObjLoader.h"
#include "../headers/noise.h"
#include "../headers/model.h"
#include "../headers/color.h"
#include "../headers/print.h"
#include "../headers/framebuffer.h"

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Color currentColor;

std::vector<Model> models;
Uniforms uniforms;

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Error: Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Error: Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    setupNoise();

    return true;
}

void setColor(const Color &color)
{
    currentColor = color;
}

void render()
{
    for (const auto &model : models)
    {
        // 1. Vertex Shader
        uniforms.model = model.modelMatrix;
        std::vector<Vertex> transformedVertices(model.vertices.size() / 3);
        for (size_t i = 0; i < model.vertices.size() / 3; ++i)
        {
            Vertex vertex = {model.vertices[3 * i], model.vertices[3 * i + 1], model.vertices[3 * i + 2]};
            transformedVertices[i] = vertexShader(vertex, uniforms);
        }

        // 2. Primitive Assembly
        std::vector<std::vector<Vertex>> assembledVertices(transformedVertices.size() / 3);
        for (size_t i = 0; i < transformedVertices.size() / 3; ++i)
        {
            Vertex edge1 = transformedVertices[3 * i];
            Vertex edge2 = transformedVertices[3 * i + 1];
            Vertex edge3 = transformedVertices[3 * i + 2];
            assembledVertices[i] = {edge1, edge2, edge3};
        }

        // 3. Rasterization
        std::vector<Fragment> fragments;

        for (size_t i = 0; i < assembledVertices.size(); ++i)
        {
            std::vector<Fragment> rasterizedTriangle = triangle(
                assembledVertices[i][0],
                assembledVertices[i][1],
                assembledVertices[i][2]);
            fragments.insert(fragments.end(), rasterizedTriangle.begin(), rasterizedTriangle.end());
        }

        // 4. Fragment Shader
        for (size_t i = 0; i < fragments.size(); ++i)
        {
            Fragment (*fragmentShader)(Fragment &) = nullptr;

            switch (model.currentShader)
            {
            case ROCKY:
                fragmentShader = rockyPlanetShader;
                break;
            case GAS:
                fragmentShader = gasGiantShader;
                break;
            case SUN:
                fragmentShader = sunShader;
                break;
            case EARTH:
                fragmentShader = earthShader;
                break;
            case MARS:
                fragmentShader = marsShader;
                break;
            case NEPTUNE:
                fragmentShader = neptuneShader;
                break;
            case STAR:
                fragmentShader = starShader;
                break;
            default:
                std::cerr << "Error: Shader no reconocido." << std::endl;
                break;
            }
            const Fragment &fragment = fragmentShader(fragments[i]);

            point(fragment);
        }
    }
}

void renderStars(int ox, int oy)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            FastNoiseLite noiseGenerator;
            noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

            float scale = 1000.0f;
            float noiseValue = noiseGenerator.GetNoise((x + (ox * 100.0f)) * scale, (y + oy * 100.0f) * scale);

            // If the noise value is above a threshold, draw a star
            if (noiseValue > 0.97f)
            {
                framebuffer[y * SCREEN_WIDTH + x] = star;
            }
            else
            {
                framebuffer[y * SCREEN_WIDTH + x] = blank;
            }
        }
    }
}

std::vector<glm::vec3> createVBO(std::string path)
{

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> texCoords;
    std::vector<Face> faces;
    std::vector<glm::vec3> vertexBufferObject; // This will contain both vertices and normals

    loadOBJ(path.c_str(), vertices, normals, texCoords, faces);

    for (const auto &face : faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            // Get the vertex position
            glm::vec3 vertexPosition = vertices[face.vertexIndices[i]];

            // Get the normal for the current vertex
            glm::vec3 vertexNormal = normals[face.normalIndices[i]];

            // Get the texture for the current vertex
            glm::vec3 vertexTexture = texCoords[face.texIndices[i]];

            // Add the vertex position and normal to the vertex array
            vertexBufferObject.push_back(vertexPosition);
            vertexBufferObject.push_back(vertexNormal);
            vertexBufferObject.push_back(vertexTexture);
        }
    }

    return vertexBufferObject;
}

glm::mat4 createViewportMatrix(size_t screenWidth, size_t screenHeight)
{
    glm::mat4 viewport = glm::mat4(1.0f);

    // Scale
    viewport = glm::scale(viewport, glm::vec3(screenWidth / 2.0f, screenHeight / 2.0f, 0.5f));

    // Translate
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}

int main(int argc, char *argv[])
{

    ShaderType currentShader = ROCKY;

    if (!init())
    {
        return 1;
    }

    std::vector<glm::vec3> vertexBufferObject = createVBO("../models/sphere.obj");
    std::vector<glm::vec3> vBoSpaceship = createVBO("../models/nave.obj");

    glm::mat4 model = glm::mat4(1);
    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);

    glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
    // float a = 45.0f;
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), translationVector);

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleFactor);

    // Initialize a Camera object
    Camera camera;
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Projection matrix
    float fovInDegrees = 45.0f;
    float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT); // Assuming a screen resolution of 800x600
    float nearClip = 0.1f;
    float farClip = 100.0f;
    uniforms.projection = glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);

    // Viewport matrix
    uniforms.viewport = createViewportMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);
    Uint32 frameStart, frameTime;
    std::string title = "FPS: ";
    float speed = 0.5f;

    bool running = true;
    // Model 1
    Model model1;
    model1.modelMatrix = glm::mat4(1);
    model1.vertices = vertexBufferObject;
    model1.rotationSpeed = 1.0f;
    model1.currentShader = SUN;
    models.push_back(model1); // Add model1 to models vector

    // Model 2:
    Model model2;
    model2.modelMatrix = glm::mat4(1);
    model2.vertices = vertexBufferObject;
    model2.degreesRotation = 45.0f; // grados de mi rotación
    model2.radius = 1.5f;           // radio de alejamiennto a sol
    model2.currentShader = ROCKY;
    model2.rotationSpeed = 20.0f;
    model2.translationSpeed = 0.001f; // velocidad de traslación
    models.push_back(model2);         // Add model2 to models vector

    // Model 3:
    Model model3;
    model3.modelMatrix = glm::mat4(1);
    model3.vertices = vertexBufferObject;
    model3.degreesRotation = 10.0f;   // grados de mi rotación
    model3.radius = 1.2f;             // radio de alejamiennto a sol
    model3.translationSpeed = 0.005f; // velocidad de traslación
    model3.currentShader = GAS;
    model3.rotationSpeed = 2.0f;
    models.push_back(model3); // Add model3 to models vector

    // Model 4:
    Model model4;
    model4.modelMatrix = glm::mat4(1);
    model4.vertices = vertexBufferObject;
    model4.currentShader = EARTH;
    model4.degreesRotation = 15.0f;   // grados de mi rotación
    model4.radius = 0.9f;             // radio de alejamiennto a sol
    model4.translationSpeed = 0.009f; // velocidad de traslación
    model4.rotationSpeed = 2.0f;
    models.push_back(model4); // Add model3 to models vector

    // Model 5:
    Model model5;
    model5.modelMatrix = glm::mat4(1);
    model5.vertices = vertexBufferObject;
    model5.currentShader = STAR;
    model5.rotationSpeed = 8.0f;
    model5.degreesRotation = 25.0f;   // grados de mi rotación
    model5.radius = 1.4f;             // radio de alejamiennto a sol
    model5.translationSpeed = 0.009f; // velocidad de traslación
    models.push_back(model5);         // Add model3 to models vector

    Model model6;
    model6.modelMatrix = glm::mat4(1);
    model6.vertices = vertexBufferObject;
    model6.currentShader = MARS;
    model6.rotationSpeed = 3.0f;
    model6.degreesRotation = 20.0f;   // grados de mi rotación
    model6.radius = 1.6f;             // radio de alejamiennto a sol
    model6.translationSpeed = 0.009f; // velocidad de traslación
    models.push_back(model6);         // Add model3 to models vector

    Model model7;
    model7.modelMatrix = glm::mat4(1);
    model7.vertices = vertexBufferObject;
    model7.currentShader = NEPTUNE;
    model7.rotationSpeed = 3.0f;
    model7.degrees = 45.0f;
    model7.degreesRotation = 0.0f;    // grados de mi rotación
    model7.radius = 0.9f;             // radio de alejamiennto a sol
    model7.translationSpeed = 0.009f; // velocidad de traslación
    models.push_back(model7);         // Add model3 to models vector

    Model nave;
    nave.modelMatrix = glm::mat4(100);
    nave.vertices = vBoSpaceship;
    nave.currentShader = ROCKY;
    models.push_back(nave); // Add model3 to models vector

    while (running)
    {
        frameStart = SDL_GetTicks();

        // Calculate the model matrix
        uniforms.model = translation * scale;

        // Create the view matrix using the Camera object
        uniforms.view = glm::lookAt(
            camera.cameraPosition, // The position of the camera
            camera.targetPosition, // The point the camera is looking at
            camera.upVector        // The up vector defining the camera's orientation
        );

        // actualización de los modelos
        models.at(0).modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(model1.degrees += model1.rotationSpeed), rotationAxis);
        models.at(1).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(1).radius * glm::cos(models.at(1).degreesRotation),
                                                            0.0f,
                                                            models.at(1).radius * glm::sin(models.at(1).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model2.degrees += model2.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        models.at(1).degreesRotation += model2.translationSpeed;

        models.at(2).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(2).radius * glm::cos(models.at(2).degreesRotation),
                                                            0.0f,
                                                            models.at(2).radius * glm::sin(models.at(2).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model3.degrees += model3.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        models.at(2).degreesRotation += model3.translationSpeed;

        models.at(3).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(3).radius * glm::cos(models.at(3).degreesRotation),
                                                            0.0f,
                                                            models.at(3).radius * glm::sin(models.at(3).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model4.degrees += model4.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        models.at(3).degreesRotation += model4.translationSpeed;

        models.at(4).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(4).radius * glm::cos(models.at(4).degreesRotation),
                                                            0.0f,
                                                            models.at(4).radius * glm::sin(models.at(4).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model5.degrees += model5.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
        models.at(4).degreesRotation += model5.translationSpeed;

        models.at(5).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(5).radius * glm::cos(models.at(5).degreesRotation),
                                                            0.0f,
                                                            models.at(5).radius * glm::sin(models.at(5).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model6.degrees += model6.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
        models.at(5).degreesRotation += model6.translationSpeed;

        models.at(6).modelMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(models.at(6).radius * glm::cos(models.at(6).degreesRotation),
                                                            0.0f,
                                                            models.at(6).radius * glm::sin(models.at(6).degreesRotation))) *
                                   glm::rotate(glm::mat4(1.0f), glm::radians(model7.degrees += model7.rotationSpeed), rotationAxis) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        models.at(6).degreesRotation += model7.translationSpeed;

        models.at(7).modelMatrix = glm::translate(glm::mat4(1), glm::vec3(camera.cameraPosition.x, camera.cameraPosition.y, 1.5f)) * glm::scale(glm::mat4(1), glm::vec3(0.01f, 0.01f, 0.01f));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    camera.cameraPosition.x -= speed;
                    camera.targetPosition.x -= speed;
                    break;
                case SDLK_RIGHT:
                    camera.cameraPosition.x += speed;
                    camera.targetPosition.x += speed;
                    break;
                case SDLK_UP:
                    camera.cameraPosition.y += speed;
                    camera.targetPosition.y += speed;
                    break;
                case SDLK_DOWN:
                    camera.cameraPosition.y -= speed;
                    camera.targetPosition.y -= speed;
                    break;
                case SDLK_w:
                    camera.cameraPosition.z -= speed;
                    camera.targetPosition.z -= speed;
                    break;
                case SDLK_s:
                    camera.cameraPosition.z += speed;
                    camera.targetPosition.z += speed;
                    break;
                case SDLK_a:
                    // Si 'a' es para mover la cámara lateralmente
                    camera.cameraPosition.x -= speed;
                    break;
                case SDLK_d:
                    // Si 'd' es para mover la cámara lateralmente
                    camera.cameraPosition.x += speed;
                    break;
                }
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0) // rueda se mueve hacia arriba
                {
                    // Zoom in
                    camera.cameraPosition.z -= speed * 1; // Ajusta el multiplicador según la sensibilidad deseada
                }
                else if (event.wheel.y < 0) // rueda se mueve hacia abajo
                {
                    // Zoom out
                    camera.cameraPosition.z += speed * 1; // Ajusta el multiplicador según la sensibilidad deseada
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // x y y de a donde esta viendo la camara
        renderStars(camera.cameraPosition.x, camera.cameraPosition.y);

        render();

        renderBuffer(renderer);

        frameTime = SDL_GetTicks() - frameStart;

        // Calculate frames per second and update window title
        if (frameTime > 0)
        {
            std::ostringstream titleStream;
            titleStream << "FPS: " << 1000.0 / frameTime; // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
