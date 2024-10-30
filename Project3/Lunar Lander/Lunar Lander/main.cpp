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
#define GL_GLEXT_PROTOTYPES 1
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <cassert>

enum AppStatus { RUNNING, DISPLAY_MESSAGE, END };

constexpr float GRAVITY = -0.2f;
constexpr float HORIZONTAL_ACCELERATION = 0.5f;
constexpr float FRICTION = 1.0f;
constexpr float DISPLAY_DELAY_SECONDS = 2.0f;
float end_time = 0.0f;
GLuint g_fail_texture_id;
GLuint g_success_texture_id;
bool success = false;

glm::vec3 m_acceleration = glm::vec3(0.0f, GRAVITY, 0.0f);

constexpr float WINDOW_SIZE_MULT = 1.0f;
constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED     = 1.0f,
               BG_GREEN   = 1.0f,
               BG_BLUE    = 1.0f,
               BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint TEXTURE_BORDER     = 0;
constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char OCEAN_PATH[] = "ocean.png";
constexpr char CLOUD_PATH[] = "cloud.png";
constexpr char BUOY_PATH[] = "buoy.png";
constexpr char DIVER_PATH[]  = "diver.png";

GLuint g_cloud_texture_id;



std::vector<glm::vec3> cloud_pos = {
    glm::vec3(-3.0f, 1.0f, 0.0f),
    glm::vec3(1.5f, 0.6f, 0.0f),
    glm::vec3(4.0f, 1.3f, 0.0f)
};

std::vector<glm::vec3> cactus_pos = {
    glm::vec3(-3.0f, -2.5f, 0.0f),
    glm::vec3(-1.0f, -2.8f, 0.0f),
    glm::vec3(1.5f, -2.7f, 0.0f),
    glm::vec3(3.0f, -3.0f, 0.0f),
    glm::vec3(2.0f, -2.9f, 0.0f),
    glm::vec3(5.0f, -2.3f, 0.0f),
    glm::vec3(-5.0f, -2.5f, 0.0f)
};


GLuint desert_textures;
glm::mat4 g_cactus_matrix;

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_POS_DROPS    = glm::vec3(0.0f, 3.0f, 0.0f);
constexpr glm::vec3 INIT_SCALE_DROPS  = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_SCALE_BEAKER = glm::vec3(0.5f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_POS_BEAKER   = glm::vec3(3.0f, -2.0f, 0.0f);



SDL_Window* g_display_window;

glm::mat4 g_view_matrix, g_buoy_matrix, g_projection_matrix, g_diver_matrix;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

float g_previous_ticks = 0.0f;

GLuint g_beaker_texture_id;
GLuint g_drops_texture_id;
glm::vec3 g_beaker_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_beaker_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_diver_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_diver_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_drops_scale = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_drops_size  = glm::vec3(1.0f, 1.0f, 0.0f);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    stbi_image_free(image);

    return textureID;
}

void fail() {
    
    g_app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    success = false;
}

void succ() {
    
    g_app_status = DISPLAY_MESSAGE;
    end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    success = true;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_diver_matrix = glm::mat4(1.0f);
    g_buoy_matrix = glm::mat4(1.0f);
    
    g_diver_matrix = glm::translate(g_diver_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_diver_position += g_diver_movement;
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    g_beaker_texture_id = load_texture(BUOY_PATH);
    g_drops_texture_id = load_texture(DIVER_PATH);
    desert_textures = load_texture(OCEAN_PATH);
    g_cloud_texture_id = load_texture(CLOUD_PATH);
    
    g_fail_texture_id = load_texture("fail.png");
    g_success_texture_id = load_texture("success.png");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
  
    m_acceleration.x = 0.0f;

   
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
            g_app_status = END;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    m_acceleration.x = (key_state[SDL_SCANCODE_A]) ? -HORIZONTAL_ACCELERATION :
                       (key_state[SDL_SCANCODE_D]) ? HORIZONTAL_ACCELERATION : 0.0f;
}


void update()
{
    float ticks = static_cast<float>(SDL_GetTicks()) / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;


    m_acceleration.y = GRAVITY;
    g_diver_movement += m_acceleration * delta_time;
    g_diver_movement.x *= FRICTION;
    g_diver_position += g_diver_movement * delta_time;


    g_diver_matrix = glm::translate(glm::mat4(1.0f), INIT_POS_DROPS + g_diver_position);

    for (const auto& cloud_position : cloud_pos) {
        float x_distance = std::abs(g_diver_position.x - cloud_position.x);
        float y_distance = std::abs(g_diver_position.y - cloud_position.y);
        if (x_distance < 1.0f && y_distance < 1.0f) {
            fail();
            return;
        }
    }

 
    for (const auto& cactus_position : cactus_pos) {
        float x_distance = std::abs(g_diver_position.x - cactus_position.x);
        float y_distance = std::abs(g_diver_position.y - cactus_position.y);
        if (x_distance < 0.5f && y_distance < 0.5f) {
            fail();
            return;
        }
    }

    if (g_diver_position.y <= -3.75f) {
        succ();
    }
}


void draw_object(const glm::mat4 &object_model_matrix, GLuint object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {

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
        glm::mat4 message_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 7.5f, 1.0f));
        GLuint message_texture_id = success ? g_success_texture_id : g_fail_texture_id;
        draw_object(message_matrix, message_texture_id);
    }
 
    else if (g_app_status == RUNNING) {

        glm::mat4 desert_matrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.8f, 0.0f)), glm::vec3(10.0f, 3.5f, 1.0f));
        draw_object(desert_matrix, desert_textures);

        for (const auto& cactus_position : cactus_pos) {
            glm::mat4 cactus_matrix = glm::scale(glm::translate(glm::mat4(1.0f), cactus_position + glm::vec3(0.0f, 2.0f, 0.0f)), INIT_SCALE_BEAKER);
            draw_object(cactus_matrix, g_beaker_texture_id);
        }

        for (const auto& cloud_position : cloud_pos) {
            glm::mat4 cloud_matrix = glm::scale(glm::translate(glm::mat4(1.0f), cloud_position), glm::vec3(1.5f, 1.0f, 1.0f));
            draw_object(cloud_matrix, g_cloud_texture_id);
        }


        draw_object(g_diver_matrix, g_drops_texture_id);
    }

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[])
{
    initialise();

    bool is_running = (g_app_status == RUNNING || g_app_status == DISPLAY_MESSAGE);
    float elapsed_time = 0.0f;

    while (is_running)
    {
        switch (g_app_status) {
            case RUNNING:
                process_input();
                update();
                render();
                break;

            case DISPLAY_MESSAGE:
                render();
                elapsed_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND - end_time;
                if (elapsed_time >= DISPLAY_DELAY_SECONDS) {
                    g_app_status = END;
                }
                break;

            case END:
                is_running = false;
                break;
        }
    }

    shutdown();
    return 0;
}

