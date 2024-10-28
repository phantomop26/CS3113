/**
* Author: Sahil Singh
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <cmath>
#include <ctime>

enum AppStatus { RUNNING, DISPLAY_MESSAGE, TERMINATED };

constexpr float GRAVITY = -0.25f;
constexpr float HORIZONTAL_ACCELERATION = 0.4f;
constexpr float FRICTION = 0.9f;
constexpr float DISPLAY_DELAY_SECONDS = 3.0f;
float end_time = 0.0f;
GLuint g_mission_failed_texture_id;
GLuint g_mission_accomplished_texture_id;
bool mission_success = false;

glm::vec3 m_acceleration = glm::vec3(0.0f, GRAVITY, 0.0f);

constexpr float WINDOW_SIZE_MULT = 1.2f;
constexpr int WINDOW_WIDTH  = 720 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 540 * WINDOW_SIZE_MULT;
constexpr float BG_RED     = 0.9f,
                BG_GREEN   = 0.9f,
                BG_BLUE    = 1.0f,
                BG_OPACITY = 1.0f;
constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;
constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char WALL_SPRITE_FILEPATH[] = "wall.png";
constexpr char BALL_SPRITE_FILEPATH[]  = "ball.png";
constexpr char BASE_SPRITE_FILEPATH[] = "base.png";
constexpr char CLOUD_SPRITE_FILEPATH[] = "cloud.png";
GLuint g_cloud_texture_id;

std::vector<glm::vec3> cloud_positions = {
    glm::vec3(-4.0f, 1.0f, 0.0f),
    glm::vec3(1.5f, 0.8f, 0.0f),
    glm::vec3(5.0f, 1.6f, 0.0f)
};

GLuint g_base_texture_id;
glm::mat4 g_wall_matrix;

constexpr float MINIMUM_COLLISION_DISTANCE = 1.2f;
constexpr glm::vec3 INIT_POS_BALL    = glm::vec3(0.0f, 3.5f, 0.0f);
constexpr glm::vec3 INIT_SCALE_BALL  = glm::vec3(0.8f, 0.8f, 0.0f);
constexpr glm::vec3 INIT_SCALE_WALL = glm::vec3(0.6f, 1.2f, 0.0f);
constexpr glm::vec3 INIT_POS_WALL   = glm::vec3(2.5f, -1.5f, 0.0f);

std::vector<glm::vec3> wall_positions = {
    glm::vec3(-2.0f, -2.5f, 0.0f), glm::vec3(1.0f, -2.5f, 0.0f), glm::vec3(4.0f, -2.5f, 0.0f)
};

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix, g_ball_matrix;

float g_previous_ticks = 0.0f;

GLuint g_wall_texture_id;
GLuint g_ball_texture_id;

constexpr float BALL_SPEED = 2.5f;

glm::vec3 g_wall_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_wall_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_scale = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_size  = glm::vec3(1.0f, 1.0f, 0.0f);

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}

void mission_failed() {
    mission_success = false;
    g_app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
}

void mission_accomplished() {
    mission_success = true;
    g_app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
}

void initialise()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG("SDL Initialization failed: " << SDL_GetError());
        return;
    }
    g_display_window = SDL_CreateWindow("Lunar Lander - Enhanced Version",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    if (g_display_window == nullptr) {
        LOG("Window creation failed: " << SDL_GetError());
        SDL_Quit();
        return;
    }
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    if (!context) {
        LOG("OpenGL Context creation failed: " << SDL_GetError());
        SDL_DestroyWindow(g_display_window);
        SDL_Quit();
        return;
    }
    SDL_GL_MakeCurrent(g_display_window, context);
#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_ball_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::translate(g_ball_matrix, glm::vec3(1.2f, 1.2f, 0.0f));
    g_ball_position += g_ball_movement;
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-6.0f, 6.0f, -4.5f, 4.5f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    g_wall_texture_id = load_texture(WALL_SPRITE_FILEPATH);
    g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    g_base_texture_id = load_texture(BASE_SPRITE_FILEPATH);
    g_cloud_texture_id = load_texture(CLOUD_SPRITE_FILEPATH);
    g_mission_failed_texture_id = load_texture("lose.png");
    g_mission_accomplished_texture_id = load_texture("win.png");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void process_input()
{
    m_acceleration.x = 0.0f;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_A]) {
        m_acceleration.x = -HORIZONTAL_ACCELERATION;
    }
    else if (key_state[SDL_SCANCODE_D]) {
        m_acceleration.x = HORIZONTAL_ACCELERATION;
    }
}
void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    m_acceleration.y = GRAVITY;
    g_ball_movement += m_acceleration * delta_time;
    g_ball_movement.x *= FRICTION;
    g_ball_position += g_ball_movement * delta_time;
    g_ball_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::translate(g_ball_matrix, INIT_POS_BALL + g_ball_position);
    for (const auto& cloud_position : cloud_positions) {
        float x_distance = fabs(g_ball_position.x - cloud_position.x);
        float y_distance = fabs(g_ball_position.y - cloud_position.y);
        if (x_distance < 1.0f && y_distance < 1.0f) {
            mission_failed();
            return;
        }
    }
    for (const auto& wall_position : wall_positions) {
        float x_distance = fabs(g_ball_position.x - wall_position.x);
        float y_distance = fabs(g_ball_position.y - wall_position.y);
        if (g_ball_position.y <= -3.0f && x_distance < 0.5f && y_distance < 0.1f) {
            mission_failed();
            return;
        }
    }
    if (g_ball_position.y <= -4.5f) {
        mission_accomplished();
    }
}
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void render() {
    LOG("Rendering frame...");
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    if (g_app_status == DISPLAY_MESSAGE) {
        glm::mat4 message_matrix = glm::mat4(1.0f);
        message_matrix = glm::scale(message_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
        GLuint message_texture_id = (mission_success) ? g_mission_accomplished_texture_id : g_mission_failed_texture_id;
        draw_object(message_matrix, message_texture_id);  
    } else if (g_app_status == RUNNING) { 
        glm::mat4 base_matrix = glm::mat4(1.0f);
        base_matrix = glm::translate(base_matrix, glm::vec3(0.0f, -2.8f, 0.0f));
        base_matrix = glm::scale(base_matrix, glm::vec3(10.0f, 3.5f, 1.0f));
        draw_object(base_matrix, g_base_texture_id);
        for (const auto& wall_position : wall_positions) {
            g_wall_matrix = glm::mat4(1.0f);
            g_wall_matrix = glm::translate(g_wall_matrix, wall_position + glm::vec3(0.0f, 2.0f, 0.0f));
            g_wall_matrix = glm::scale(g_wall_matrix, INIT_SCALE_WALL);
            draw_object(g_wall_matrix, g_wall_texture_id);
        }
        for (const auto& cloud_position : cloud_positions) {
            glm::mat4 cloud_matrix = glm::mat4(1.0f);
            cloud_matrix = glm::translate(cloud_matrix, cloud_position);
            cloud_matrix = glm::scale(cloud_matrix, glm::vec3(1.5f, 1.0f, 1.0f));
            draw_object(cloud_matrix, g_cloud_texture_id);
        }
        draw_object(g_ball_matrix, g_ball_texture_id);
    }
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { 
    LOG("Shutting down...");
    SDL_Quit(); 
}

int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING || g_app_status == DISPLAY_MESSAGE)
    {
        if (g_app_status == RUNNING) {
            LOG("App Status: RUNNING");
            process_input();
            update();
        }
        render();

        if (g_app_status == DISPLAY_MESSAGE) {
            float current_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
            if (current_time - end_time >= DISPLAY_DELAY_SECONDS) {
                g_app_status = TERMINATED;
            }
        }
    }
    shutdown();
    return 0;
}
