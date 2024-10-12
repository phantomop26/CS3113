/**
* Author: Sahil kumar Singh
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PADDLE_SPEED = 0.9f;
const float BALL_SPEED = 0.25f;
const int PADDLE_WIDTH = 50;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE = 20;

float paddle1_y = (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
float paddle2_y = (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
float ball_x = WINDOW_WIDTH / 2;
float ball_y = WINDOW_HEIGHT / 2;
float ball_dx = BALL_SPEED;
float ball_dy = BALL_SPEED;
bool is_single_player = false;
bool game_over = false;
int score1 = 0;
int score2 = 0;

SDL_Texture* paddle1_texture = nullptr;
SDL_Texture* paddle2_texture = nullptr;
SDL_Texture* ball_texture = nullptr;
SDL_Texture* background_texture = nullptr;
SDL_Texture* result1_texture = nullptr;
SDL_Texture* result2_texture = nullptr;

void handle_input(const Uint8* keys) {
    if (!is_single_player) {
        if (keys[SDL_SCANCODE_W] && paddle1_y > 0) {
            paddle1_y -= PADDLE_SPEED * 10;
        }
        if (keys[SDL_SCANCODE_S] && paddle1_y + PADDLE_HEIGHT < WINDOW_HEIGHT) {
            paddle1_y += PADDLE_SPEED * 10;
        }
    }
    if (keys[SDL_SCANCODE_UP] && paddle2_y > 0) {
        paddle2_y -= PADDLE_SPEED * 10;
    }
    if (keys[SDL_SCANCODE_DOWN] && paddle2_y + PADDLE_HEIGHT < WINDOW_HEIGHT) {
        paddle2_y += PADDLE_SPEED * 10;
    }

    if (keys[SDL_SCANCODE_T]) {
        is_single_player = !is_single_player;
        if (is_single_player) {
            paddle1_y = (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
        }
    }
}
void move_ball() {
    ball_x += ball_dx * 10;
    ball_y += ball_dy * 10;

    if (ball_y <= 0 || ball_y + BALL_SIZE >= WINDOW_HEIGHT) {
        ball_dy = -ball_dy;
    }
    if (ball_x <= 20 && ball_y >= paddle1_y && ball_y <= paddle1_y + PADDLE_HEIGHT) {
        ball_dx = -ball_dx;
    }
    if (ball_x + BALL_SIZE >= WINDOW_WIDTH - 20 && ball_y >= paddle2_y && ball_y <= paddle2_y + PADDLE_HEIGHT) {
        ball_dx = -ball_dx;
    }
    if (ball_x <= 0) {
        game_over = true;
        score2++;
    }
    if (ball_x + BALL_SIZE >= WINDOW_WIDTH) {
        game_over = true;
        score1++;
    }
}
void ai_move_paddle() {
    if (ball_y < paddle1_y) {
        paddle1_y -= PADDLE_SPEED * 5;
    } else if (ball_y > paddle1_y + PADDLE_HEIGHT) {
        paddle1_y += PADDLE_SPEED * 5;
    }
    if (paddle1_y < 0) {
        paddle1_y = 0;
    }
    if (paddle1_y + PADDLE_HEIGHT > WINDOW_HEIGHT) {
        paddle1_y = WINDOW_HEIGHT - PADDLE_HEIGHT;
    }
}
void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect background_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);

    if (!game_over) {
        SDL_Rect paddle1 = {10, static_cast<int>(paddle1_y), PADDLE_WIDTH, PADDLE_HEIGHT};
        SDL_RenderCopy(renderer, paddle1_texture, nullptr, &paddle1);

        SDL_Rect paddle2 = {WINDOW_WIDTH - 60, static_cast<int>(paddle2_y), PADDLE_WIDTH, PADDLE_HEIGHT};
        SDL_RenderCopy(renderer, paddle2_texture, nullptr, &paddle2);

        SDL_Rect ball = {static_cast<int>(ball_x), static_cast<int>(ball_y), BALL_SIZE, BALL_SIZE};
        SDL_RenderCopy(renderer, ball_texture, nullptr, &ball);
    } else {
        SDL_Rect result_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, score1 > score2 ? result1_texture : result2_texture, nullptr, &result_rect);
    }

    SDL_RenderPresent(renderer);
}
void shutdown() { SDL_Quit(); }

void load_textures(SDL_Renderer* renderer) {
    paddle1_texture = IMG_LoadTexture(renderer, "spaceship1.png");
    paddle2_texture = IMG_LoadTexture(renderer, "spaceship2.png");
    ball_texture = IMG_LoadTexture(renderer, "meteor.png");
    background_texture = IMG_LoadTexture(renderer,"space_background.jpg");
    result1_texture = IMG_LoadTexture(renderer, "result2.png");
    result2_texture = IMG_LoadTexture(renderer, "result1.jpg");

    if (!paddle1_texture || !paddle2_texture || !ball_texture || !background_texture || !result1_texture || !result2_texture) {
        std::cerr << "Failed to load textures! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    SDL_Window* window = SDL_CreateWindow("Pong Clone - Space Battle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    load_textures(renderer);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        handle_input(keys);
        move_ball();
        if (is_single_player) {
            ai_move_paddle();
        }
        render(renderer);

        if (game_over) {
            std::cout << "Game Over! Player 1: " << score2 << " | Player 2: " << score1 << std::endl;
            SDL_Delay(2000);
            quit = true;
        }
        SDL_Delay(10);
    }
    shutdown();
    return 0;
}




