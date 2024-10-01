/**
* Author: Sahil Kumar Singh
* Assignment: Simple 2D Scene
* Date due: 2024-09-28, 11:59pm
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
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"


enum ApplicationState { ACTIVE, CLOSED };
constexpr int SCREEN_WIDTH = 1280,
              SCREEN_HEIGHT = 960;

constexpr float CLEAR_COLOR_R = 0.976f,
                CLEAR_COLOR_G = 0.973f,
                CLEAR_COLOR_B = 0.961f,
                CLEAR_COLOR_A = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0;

constexpr char VERTEX_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               FRAGMENT_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0f;

constexpr GLint TEXTURE_COUNT = 1,
                TEXTURE_LEVEL = 0,
                TEXTURE_BORDER = 0;

constexpr char TEXTURE_1[] = "ronaldo.jpg",
               TEXTURE_2[] = "messi.jpg";

constexpr glm::vec3 OBJECT_SCALE = glm::vec3(3.0f, 5.0f, 0.0f),
                    OBJECT_1_POSITION = glm::vec3(3.0f, 0.0f, 0.0f),
                    OBJECT_2_POSITION = glm::vec3(-1.5f, 0.0f, 0.0f);

constexpr float ROTATION_SPEED = 1.0f;

SDL_Window* window;
ApplicationState app_state = ACTIVE;
ShaderProgram shader_program;

glm::mat4 view_matrix,
          object1_matrix,
          object2_matrix,
          projection_matrix;

float last_frame_ticks = 0.0f;

glm::vec3 object1_rotation = glm::vec3(0.0f, 0.0f, 0.0f),
          object2_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint texture1_id,
       texture2_id;

constexpr float BASE_SCALE = 2.0f,
                PULSATING_SPEED = 10.0f;

float pulse_timer = 0.0f;

GLuint load_texture(const char* filepath) {
    int width, height, components;
    unsigned char* image_data = stbi_load(filepath, &width, &height, &components, STBI_rgb_alpha);

    if (!image_data) {
        LOG("Failed to load texture file.");
        assert(false);
    }

    GLuint texture_id;
    glGenTextures(TEXTURE_COUNT, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, TEXTURE_LEVEL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image_data);

    return texture_id;
}

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Animated Scene",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    if (!window) {
        std::cerr << "Error: Could not create SDL window.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
    shader_program.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    object1_matrix = glm::mat4(1.0f);
    object2_matrix = glm::mat4(1.0f);
    view_matrix = glm::mat4(1.0f);
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    shader_program.set_projection_matrix(projection_matrix);
    shader_program.set_view_matrix(view_matrix);
    glUseProgram(shader_program.get_program_id());
    glClearColor(CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B, CLEAR_COLOR_A);
    texture1_id = load_texture(TEXTURE_1);
    texture2_id = load_texture(TEXTURE_2);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            app_state = CLOSED;
        }
    }
}

void update_scene() {
    float current_ticks = (float)SDL_GetTicks()/MILLISECONDS_IN_SECOND;
    float delta_time = current_ticks - last_frame_ticks;
    last_frame_ticks = current_ticks;
    pulse_timer += PULSATING_SPEED * delta_time;
    float scale_factor = BASE_SCALE + glm::sin(pulse_timer);
    float ronaldo_radius = 2.0f;
    object1_rotation.y += ROTATION_SPEED * delta_time;
    float ronaldo_x_offset = ronaldo_radius * glm::cos(object1_rotation.y);
    float ronaldo_y_offset = ronaldo_radius * glm::sin(object1_rotation.y);
    object1_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(ronaldo_x_offset, ronaldo_y_offset, 0.0f));
    float ronaldo_scale_factor = BASE_SCALE + glm::sin(pulse_timer) * 0.5f;
    object1_matrix = glm::scale(object1_matrix, glm::vec3(ronaldo_scale_factor, ronaldo_scale_factor, 1.0f));
    object1_matrix = glm::rotate(object1_matrix, object1_rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
    float messi_radius = 1.5f;
    float messi_x_offset = ronaldo_x_offset + messi_radius * glm::cos(object1_rotation.y + glm::radians(90.0f));
    float messi_y_offset = ronaldo_y_offset + messi_radius * glm::sin(object1_rotation.y + glm::radians(90.0f));
    object2_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(messi_x_offset, messi_y_offset, 0.0f));
    object2_matrix = glm::scale(object2_matrix, glm::vec3(scale_factor, scale_factor, 1.0f));
    object2_matrix = glm::rotate(object2_matrix, object1_rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
}




void draw_object(glm::mat4 &model_matrix, GLuint &texture_id) {
    shader_program.set_model_matrix(model_matrix);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coords[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    glVertexAttribPointer(shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(shader_program.get_position_attribute());

    glVertexAttribPointer(shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coords);
    glEnableVertexAttribArray(shader_program.get_tex_coordinate_attribute());

    draw_object(object1_matrix, texture1_id);
    draw_object(object2_matrix, texture2_id);

    glDisableVertexAttribArray(shader_program.get_position_attribute());
    glDisableVertexAttribArray(shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(window);
}

void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[]) {
    initialize();
    while (app_state == ACTIVE) {
        handle_input();
        update_scene();
        render_scene();
    }
    shutdown();
    return 0;
}
