/**
* Author: Sahil Singh
* Assignment: Rise of the AI
* Date due: 2024-11-09, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 11
#define ENEMY_COUNT 3

struct GameState {
    Entity *player;
    Entity *enemies;
    Entity *platforms;
    Entity *font;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
const float BOMB_RADIUS = 1.5f;
bool bombAvailable = true; // Track if the bomb is available to use
float bombCooldown = 3.0f; // Cooldown time in seconds
float lastBombTime = 0.0f; // Last time the bomb was used
void ActivateBomb() {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        float distance = glm::distance(state.player->position, state.enemies[i].position);
        if (state.enemies[i].isActive && distance < BOMB_RADIUS) {
            state.enemies[i].isActive = false; // "Kill" the enemy within range
        }
    }
}
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Rise Of The AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, -2.25f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -1.5f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = LoadTexture("pacman.png");
    state.player->jumpPower = 1.5f;
    state.player->height = 1.0f;
    state.player->width = 1.0f;
    
    
    
    // Initialize enemies
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID1 = LoadTexture("enemy1.png");
    GLuint enemyTextureID2 = LoadTexture("enemy2.png");
    GLuint enemyTextureID3 = LoadTexture("enemy3.png");
    
    state.enemies[0].textureID = enemyTextureID1;
    state.enemies[0].position = glm::vec3(-4, -2.25f, 0);
    state.enemies[0].speed = 0.5f;
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 1.0f;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    
    state.enemies[1].textureID = enemyTextureID2;
    state.enemies[1].position = glm::vec3(4, -2.25f, 0);
    state.enemies[1].speed = 0.5f;
    state.enemies[1].height = 0.6f;
    state.enemies[1].width = 0.6f;
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = WALKER;
    state.enemies[1].aiState = WALKING;
    
    state.enemies[2].textureID = enemyTextureID3;
    state.enemies[2].position = glm::vec3(2, -1.25, 0);
    state.enemies[2].acceleration = glm::vec3(0, -1.5f, 0);
    state.enemies[2].height = 0.7f;
    state.enemies[2].width = 0.7f;
    state.enemies[2].speed = 2.5f;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = JUMPER;
    state.enemies[2].aiState = JUMPING;
   
    
    
    
    //initialize Platform
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile007.png");
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].entityType = PLATFORMS;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5 + i, -3.25f, 0);
    }
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL, NULL, 0, 0);
    }
    
    //initialize font
    state.font = new Entity();
    state.font->position = glm::vec3(-4.25f, 2.25, 0);
    state.font->textureID = LoadTexture("font1.png");
}


void ProcessInput() {
    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;

                    case SDLK_RIGHT:
                        // Move the player right
                        break;

                    case SDLK_SPACE:
                        state.player->jump = true;
                        break;
                        
                    case SDLK_b: // Bomb action
                        if (bombAvailable) {
                            ActivateBomb();
                            bombAvailable = false;
                            lastBombTime = (float)SDL_GetTicks() / 1000.0f; // Record the bomb usage time
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    // Check bomb cooldown
    if (!bombAvailable && (ticks - lastBombTime >= bombCooldown)) {
        bombAvailable = true; // Reset bomb availability after cooldown
    }

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, state.platforms, PLATFORM_COUNT, ENEMY_COUNT);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, &state.enemies[i], state.platforms, PLATFORM_COUNT, ENEMY_COUNT);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}
void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for(int i = 0; i < text.size(); i++) {
        
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
        
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


bool isEnd = false;

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i=0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    for (int i=0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].isActive){
            break;}
        if (i == ENEMY_COUNT-1) {
             DrawText(&program, state.font->textureID, "GAME: YOU WIN", 0.5f, -0.25f, state.font->position);
             isEnd = true;
        }
       
    }
    
   for (int i=0; i < ENEMY_COUNT; i++) {
       state.enemies[i].Render(&program);}

    if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedTop) {
        DrawText(&program, state.font->textureID, "GAME: YOU LOSE", 0.5f, -0.25f, state.font->position);
        isEnd = true;
    }
    
    state.player->Render(&program);
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        if (!isEnd) Update();
        Render();
    }
    
    Shutdown();
    return 0;
}

