/////**
////* Author: Sahil Kumar Singh
////* Assignment: Simple 2D Scene
////* Date due: 2024-09-28, 11:59pm
////* I pledge that I have completed this assignment without
////* collaborating with anyone else, in conformance with the
////* NYU School of Engineering Policies and Procedures on
////* Academic Misconduct.
////**/
////
////#define GL_SILENCE_DEPRECATION
////#define STB_IMAGE_IMPLEMENTATION
////#define LOG(argument) std::cout << argument << '\n'
////#define GL_GLEXT_PROTOTYPES 1
////
////#ifdef _WINDOWS
////#include <GL/glew.h>
////#endif
////
////#include <SDL.h>
////#include <SDL_opengl.h>
////#include "glm/mat4x4.hpp"
////#include "glm/gtc/matrix_transform.hpp"
////#include "ShaderProgram.h"
////#include "stb_image.h"
////
////
////enum ApplicationState { ACTIVE, CLOSED };
////constexpr int SCREEN_WIDTH = 1280,
////              SCREEN_HEIGHT = 960;
////
////constexpr float CLEAR_COLOR_R = 0.976f,
////                CLEAR_COLOR_G = 0.973f,
////                CLEAR_COLOR_B = 0.961f,
////                CLEAR_COLOR_A = 1.0f;
////
////constexpr int VIEWPORT_X = 0,
////              VIEWPORT_Y = 0;
////
////constexpr char VERTEX_SHADER_PATH[] = "shaders/vertex_textured.glsl",
////               FRAGMENT_SHADER_PATH[] = "shaders/fragment_textured.glsl";
////
////constexpr float MILLISECONDS_IN_SECOND = 1000.0f;
////
////constexpr GLint TEXTURE_COUNT = 1,
////                TEXTURE_LEVEL = 0,
////                TEXTURE_BORDER = 0;
////
////constexpr char TEXTURE_1[] = "ronaldo.jpg",
////               TEXTURE_2[] = "messi.jpg";
////
////constexpr glm::vec3 OBJECT_SCALE = glm::vec3(3.0f, 5.0f, 0.0f),
////                    OBJECT_1_POSITION = glm::vec3(3.0f, 0.0f, 0.0f),
////                    OBJECT_2_POSITION = glm::vec3(-1.5f, 0.0f, 0.0f);
////
////constexpr float ROTATION_SPEED = 1.0f;
////
////SDL_Window* window;
////ApplicationState app_state = ACTIVE;
////ShaderProgram shader_program;
////
////glm::mat4 view_matrix,
////          object1_matrix,
////          object2_matrix,
////          projection_matrix;
////
////float last_frame_ticks = 0.0f;
////
////glm::vec3 object1_rotation = glm::vec3(0.0f, 0.0f, 0.0f),
////          object2_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
////
////GLuint texture1_id,
////       texture2_id;
////
////constexpr float BASE_SCALE = 2.0f,
////                PULSATING_SPEED = 10.0f;
////
////float pulse_timer = 0.0f;
////
////GLuint load_texture(const char* filepath) {
////    int width, height, components;
////    unsigned char* image_data = stbi_load(filepath, &width, &height, &components, STBI_rgb_alpha);
////
////    if (!image_data) {
////        LOG("Failed to load texture file.");
////        assert(false);
////    }
////
////    GLuint texture_id;
////    glGenTextures(TEXTURE_COUNT, &texture_id);
////    glBindTexture(GL_TEXTURE_2D, texture_id);
////    glTexImage2D(GL_TEXTURE_2D, TEXTURE_LEVEL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
////
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
////
////    stbi_image_free(image_data);
////
////    return texture_id;
////}
////
////void initialize() {
////    SDL_Init(SDL_INIT_VIDEO);
////
////    window = SDL_CreateWindow("Animated Scene",
////                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
////                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
////
////    SDL_GLContext context = SDL_GL_CreateContext(window);
////    SDL_GL_MakeCurrent(window, context);
////
////    if (!window) {
////        std::cerr << "Error: Could not create SDL window.\n";
////        SDL_Quit();
////        exit(1);
////    }
////
////#ifdef _WINDOWS
////    glewInit();
////#endif
////
////    glViewport(VIEWPORT_X, VIEWPORT_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
////    shader_program.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
////    object1_matrix = glm::mat4(1.0f);
////    object2_matrix = glm::mat4(1.0f);
////    view_matrix = glm::mat4(1.0f);
////    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
////    shader_program.set_projection_matrix(projection_matrix);
////    shader_program.set_view_matrix(view_matrix);
////    glUseProgram(shader_program.get_program_id());
////    glClearColor(CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B, CLEAR_COLOR_A);
////    texture1_id = load_texture(TEXTURE_1);
////    texture2_id = load_texture(TEXTURE_2);
////    glEnable(GL_BLEND);
////    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
////}
////
////
////void handle_input() {
////    SDL_Event event;
////    while (SDL_PollEvent(&event)) {
////        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
////            app_state = CLOSED;
////        }
////    }
////}
////
////void update_scene() {
////    float current_ticks = (float)SDL_GetTicks()/MILLISECONDS_IN_SECOND;
////    float delta_time = current_ticks - last_frame_ticks;
////    last_frame_ticks = current_ticks;
////    pulse_timer += PULSATING_SPEED * delta_time;
////    float scale_factor = BASE_SCALE + glm::sin(pulse_timer);
////    float ronaldo_radius = 2.0f;
////    object1_rotation.y += ROTATION_SPEED * delta_time;
////    float ronaldo_x_offset = ronaldo_radius * glm::cos(object1_rotation.y);
////    float ronaldo_y_offset = ronaldo_radius * glm::sin(object1_rotation.y);
////    object1_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(ronaldo_x_offset, ronaldo_y_offset, 0.0f));
////    float ronaldo_scale_factor = BASE_SCALE + glm::sin(pulse_timer) * 0.5f;
////    object1_matrix = glm::scale(object1_matrix, glm::vec3(ronaldo_scale_factor, ronaldo_scale_factor, 1.0f));
////    object1_matrix = glm::rotate(object1_matrix, object1_rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
////    float messi_radius = 1.5f;
////    float messi_x_offset = ronaldo_x_offset + messi_radius * glm::cos(object1_rotation.y + glm::radians(90.0f));
////    float messi_y_offset = ronaldo_y_offset + messi_radius * glm::sin(object1_rotation.y + glm::radians(90.0f));
////    object2_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(messi_x_offset, messi_y_offset, 0.0f));
////    object2_matrix = glm::scale(object2_matrix, glm::vec3(scale_factor, scale_factor, 1.0f));
////    object2_matrix = glm::rotate(object2_matrix, object1_rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
////}
////
////
////
////
////void draw_object(glm::mat4 &model_matrix, GLuint &texture_id) {
////    shader_program.set_model_matrix(model_matrix);
////    glBindTexture(GL_TEXTURE_2D, texture_id);
////    glDrawArrays(GL_TRIANGLES, 0, 6);
////}
////
////void render_scene() {
////    glClear(GL_COLOR_BUFFER_BIT);
////
////    float vertices[] = {
////        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
////        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
////    };
////
////    float texture_coords[] = {
////        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
////        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
////    };
////
////    glVertexAttribPointer(shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
////    glEnableVertexAttribArray(shader_program.get_position_attribute());
////
////    glVertexAttribPointer(shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coords);
////    glEnableVertexAttribArray(shader_program.get_tex_coordinate_attribute());
////
////    draw_object(object1_matrix, texture1_id);
////    draw_object(object2_matrix, texture2_id);
////
////    glDisableVertexAttribArray(shader_program.get_position_attribute());
////    glDisableVertexAttribArray(shader_program.get_tex_coordinate_attribute());
////
////    SDL_GL_SwapWindow(window);
////}
////
////void shutdown() { SDL_Quit(); }
////
////int main(int argc, char* argv[]) {
////    initialize();
////    while (app_state == ACTIVE) {
////        handle_input();
////        update_scene();
////        render_scene();
////    }
////    shutdown();
////    return 0;
////}
///**
//* Author: Sahil Singh
//* Assignment: Lunar Lander
//* Date due: 2024-10-27, 11:59pm
//* I pledge that I have completed this assignment without
//* collaborating with anyone else, in conformance with the
//* NYU School of Engineering Policies and Procedures on
//* Academic Misconduct.
//**/
//    #define GL_SILENCE_DEPRECATION
//    #define STB_IMAGE_IMPLEMENTATION
//    #define LOG(argument) std::cout << argument << '\n'
//
//    #ifdef _WINDOWS
//    #include <GL/glew.h>
//    #endif
//
//    #define GL_GLEXT_PROTOTYPES 1
//    #include <SDL.h>
//    #include <SDL_opengl.h>
//    #include "glm/mat4x4.hpp"
//    #include "glm/gtc/matrix_transform.hpp"
//    #include "ShaderProgram.h"
//    #include "stb_image.h"
//    #include "cmath"
//    #include <ctime>
//
//    enum AppStatus { RUNNING, DISPLAY_MESSAGE, TERMINATED };
//
// 
//    constexpr float GRAVITY = -0.2f;
//    constexpr float HORIZONTAL_ACCELERATION = 0.5f;
//    constexpr float FRICTION = 1.0f;
//    constexpr float DISPLAY_DELAY_SECONDS = 2.0f;
//    float end_time = 0.0f;
//    GLuint g_mission_failed_texture_id;
//    GLuint g_mission_accomplished_texture_id;
//    bool mission_success = false;
//
//
//
//    glm::vec3 m_acceleration = glm::vec3(0.0f, GRAVITY, 0.0f);
//
//    constexpr float WINDOW_SIZE_MULT = 1.0f;
//    constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
//                  WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;
//    constexpr float BG_RED     = 1.0f,
//                    BG_GREEN   = 1.0f,
//                    BG_BLUE    = 1.0f,
//                    BG_OPACITY = 1.0f;
//    constexpr int VIEWPORT_X = 0,
//              VIEWPORT_Y = 0,
//              VIEWPORT_WIDTH  = WINDOW_WIDTH,
//              VIEWPORT_HEIGHT = WINDOW_HEIGHT;
//    constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
//               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
//    constexpr GLint NUMBER_OF_TEXTURES = 1;
//    constexpr GLint LEVEL_OF_DETAIL    = 0;
//    constexpr GLint TEXTURE_BORDER     = 0;
//    constexpr float MILLISECONDS_IN_SECOND = 1000.0;
//    constexpr char BEAKER_SPRITE_FILEPATH[] = "assets/cactus.png";
//    constexpr char DROPS_SPRITE_FILEPATH[]  = "assets/dino.png";
//    constexpr char DESERT_SPRITE_FILEPATH[] = "assets/desert.png";
//    constexpr char CLOUD_SPRITE_FILEPATH[] = "assets/cloud.png";
//    constexpr char FAIL_SPRITE_FILEPATH[] = "assets/fail.png";
//    constexpr char SUCCESS_SPRITE_FILEPATH[] = "assets/success.png";
//    GLuint g_cloud_texture_id;
//    
//
//    std::vector<glm::vec3> cloud_positions = {
//        glm::vec3(-3.0f, 0.5f, 0.0f),
//        glm::vec3(2.0f, 0.3f, 0.0f),
//        glm::vec3(4.0f, 1.1f, 0.0f)
//    };
//
//    GLuint g_desert_texture_id;
//    glm::mat4 g_cactus_matrix;
//
//
//
//    constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
//    constexpr glm::vec3 INIT_POS_DROPS    = glm::vec3(0.0f, 3.0f, 0.0f);
//    constexpr glm::vec3 INIT_SCALE_DROPS  = glm::vec3(1.0f, 1.0f, 0.0f);
//    constexpr glm::vec3 INIT_SCALE_BEAKER = glm::vec3(0.5f, 1.0f, 0.0f);
//    constexpr glm::vec3 INIT_POS_BEAKER   = glm::vec3(3.0f, -2.0f, 0.0f);
//    
//    std::vector<glm::vec3> cactus_positions = {
//        glm::vec3(-3.0f, -3.0f, 0.0f), glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(3.5f, -3.0f, 0.0f), glm::vec3(1.0f, -3.0f, 0.0f)
//    };
//
//    SDL_Window* g_display_window;
//
//    AppStatus g_app_status = RUNNING;
//    ShaderProgram g_shader_program = ShaderProgram();
//    glm::mat4 g_view_matrix, g_beaker_matrix, g_projection_matrix, g_drops_matrix;
//
//    float g_previous_ticks = 0.0f;
//
//    GLuint g_beaker_texture_id;
//    GLuint g_drops_texture_id;
//  
//    constexpr float DROPS_SPEED = 3.0f;
//
//    glm::vec3 g_beaker_position = glm::vec3(0.0f, 0.0f, 0.0f);
//    glm::vec3 g_beaker_movement = glm::vec3(0.0f, 0.0f, 0.0f);
//
//    glm::vec3 g_drops_position = glm::vec3(0.0f, 0.0f, 0.0f);
//    glm::vec3 g_drops_movement = glm::vec3(0.0f, 0.0f, 0.0f);
//
//    glm::vec3 g_drops_scale = glm::vec3(0.0f, 0.0f, 0.0f);
//    glm::vec3 g_drops_size  = glm::vec3(1.0f, 1.0f, 0.0f);
//
//    void initialise();
//    void process_input();
//    void update();
//    void render();
//    void shutdown();
//
//    GLuint load_texture(const char* filepath)
//    {
//        
//        int width, height, number_of_components;
//        unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
//
//        if (image == NULL)
//        {
//            LOG("Unable to load image. Make sure the path is correct.");
//            assert(false);
//        }
//
//   
//        GLuint textureID;
//        glGenTextures(NUMBER_OF_TEXTURES, &textureID);
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
//
// 
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//    
//        stbi_image_free(image);
//
//        return textureID;
//    }
//    
//    void mission_failed() {
//        mission_success = false;
//        g_app_status = DISPLAY_MESSAGE;
//        end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//    }
//
//    void mission_accomplished() {
//        mission_success = true;
//        g_app_status = DISPLAY_MESSAGE;
//        end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//    }
//
//
//    void initialise()
//    {
//        SDL_Init(SDL_INIT_VIDEO);
//        g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
//                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//                                            WINDOW_WIDTH, WINDOW_HEIGHT,
//                                            SDL_WINDOW_OPENGL);
//
//        SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
//        SDL_GL_MakeCurrent(g_display_window, context);
//
//        if (g_display_window == nullptr) shutdown();
//
//    #ifdef _WINDOWS
//        glewInit();
//    #endif
//
//        glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
//        g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
//        g_beaker_matrix = glm::mat4(1.0f);
//        g_drops_matrix = glm::mat4(1.0f);
//        g_drops_matrix = glm::translate(g_drops_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
//        g_drops_position += g_drops_movement;
//        g_view_matrix = glm::mat4(1.0f);
//        g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
//        g_shader_program.set_projection_matrix(g_projection_matrix);
//        g_shader_program.set_view_matrix(g_view_matrix);
//        glUseProgram(g_shader_program.get_program_id());
//        glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
//        g_beaker_texture_id = load_texture(BEAKER_SPRITE_FILEPATH);
//        g_drops_texture_id = load_texture(DROPS_SPRITE_FILEPATH);
//        g_desert_texture_id = load_texture(DESERT_SPRITE_FILEPATH);
//        g_cloud_texture_id = load_texture(CLOUD_SPRITE_FILEPATH);
//        g_mission_failed_texture_id = load_texture(FAIL_SPRITE_FILEPATH);
//        g_mission_accomplished_texture_id = load_texture(SUCCESS_SPRITE_FILEPATH);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    }
//
//
//    void process_input()
//    {
//        m_acceleration.x = 0.0f;
//        SDL_Event event;
//        while (SDL_PollEvent(&event))
//        {
//            switch (event.type)
//            {
//                case SDL_QUIT:
//                case SDL_WINDOWEVENT_CLOSE:
//                    g_app_status = TERMINATED;
//                    break;
//                case SDL_KEYDOWN:
//                    switch (event.key.keysym.sym)
//                    {
//                        case SDLK_q:
//                            g_app_status = TERMINATED;
//                            break;
//                        default:
//                            break;
//                    }
//                default:
//                    break;
//            }
//        }
//
//        const Uint8 *key_state = SDL_GetKeyboardState(NULL);
//        
//  
//        if (key_state[SDL_SCANCODE_A]) {
//            m_acceleration.x = -HORIZONTAL_ACCELERATION;
//        }
//        else if (key_state[SDL_SCANCODE_D]) {
//            m_acceleration.x = HORIZONTAL_ACCELERATION;
//        }
//    }
//
//
//    void update()
//    {
//        
//        float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//        float delta_time = ticks - g_previous_ticks;
//        g_previous_ticks = ticks;
//        m_acceleration.y = GRAVITY;
//        g_drops_movement += m_acceleration * delta_time;
//        g_drops_movement.x *= FRICTION;
//        g_drops_position += g_drops_movement * delta_time;
//        g_drops_matrix = glm::mat4(1.0f);
//        g_drops_matrix = glm::translate(g_drops_matrix, INIT_POS_DROPS + g_drops_position);
//
//       
//        for (const auto& cloud_position : cloud_positions) {
//            float x_distance = fabs(g_drops_position.x - cloud_position.x);
//            float y_distance = fabs(g_drops_position.y - cloud_position.y);
//            if (x_distance < 1.0f && y_distance < 1.0f) {
//                mission_failed();
//                return;
//            }
//        }
//
//
//        for (const auto& cactus_position : cactus_positions) {
//            float x_distance = fabs(g_drops_position.x - cactus_position.x);
//            float y_distance = fabs(g_drops_position.y - cactus_position.y);
//            if (g_drops_position.y <= -3.0f && x_distance < 0.5f && y_distance < 0.1f) {
//                mission_failed();
//                return;
//            }
//        }
//
//  
//        if (g_drops_position.y <= -3.75f) {
//            mission_accomplished();
//        }
//    }
//
//    void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
//    {
//        g_shader_program.set_model_matrix(object_model_matrix);
//        glBindTexture(GL_TEXTURE_2D, object_texture_id);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//    }
//
//    void render() {
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        float vertices[] = {
//            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
//            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
//        };
//
//        float texture_coordinates[] = {
//            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
//            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
//        };
//
//        glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
//        glEnableVertexAttribArray(g_shader_program.get_position_attribute());
//
//        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
//        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
//
//        if (g_app_status == DISPLAY_MESSAGE) {
//            glm::mat4 message_matrix = glm::mat4(1.0f);
//            message_matrix = glm::scale(message_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
//            GLuint message_texture_id = (mission_success) ? g_mission_accomplished_texture_id : g_mission_failed_texture_id;
//            draw_object(message_matrix, message_texture_id);
//            
//        } else if (g_app_status == RUNNING) {
//            
//            glm::mat4 desert_matrix = glm::mat4(1.0f);
//            desert_matrix = glm::translate(desert_matrix, glm::vec3(0.0f, -2.8f, 0.0f));
//            desert_matrix = glm::scale(desert_matrix, glm::vec3(10.0f, 3.5f, 1.0f));
//            draw_object(desert_matrix, g_desert_texture_id);
//
//            for (const auto& cactus_position : cactus_positions) {
//                g_beaker_matrix = glm::mat4(1.0f);
//                g_beaker_matrix = glm::translate(g_beaker_matrix, cactus_position + glm::vec3(0.0f, 2.0f, 0.0f));
//                g_beaker_matrix = glm::scale(g_beaker_matrix, INIT_SCALE_BEAKER);
//                draw_object(g_beaker_matrix, g_beaker_texture_id);
//            }
//
//            for (const auto& cloud_position : cloud_positions) {
//                glm::mat4 cloud_matrix = glm::mat4(1.0f);
//                cloud_matrix = glm::translate(cloud_matrix, cloud_position);
//                cloud_matrix = glm::scale(cloud_matrix, glm::vec3(1.5f, 1.0f, 1.0f));
//                draw_object(cloud_matrix, g_cloud_texture_id);
//            }
//
//            draw_object(g_drops_matrix, g_drops_texture_id);
//        }
//
//
//        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
//        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
//
//        SDL_GL_SwapWindow(g_display_window);
//    }
//
//    void shutdown() { SDL_Quit(); }
//
//
//    int main(int argc, char* argv[])
//    {
//        initialise();
//
//        while (g_app_status == RUNNING || g_app_status == DISPLAY_MESSAGE)
//        {
//            if (g_app_status == RUNNING) {
//                process_input();
//                update();
//            }
//            render();
//
//            if (g_app_status == DISPLAY_MESSAGE) {
//                float current_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//                if (current_time - end_time >= DISPLAY_DELAY_SECONDS) {
//                    g_app_status = TERMINATED;
//                }
//            }
//        }
//        shutdown();
//        return 0;
//    }
//
//

/**
* Author: Andy Ng
* Assignment: Lunar Lander
* Date due: 2024-09-03, 11:59pm
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.hpp"

#include <iostream>
#include <vector>
using namespace std;

#define PLATFORM_COUNT 31
struct GameEntities {
    Entity* player;
    Entity* platforms;
};

const int FONTBANK_SIZE =16;

GameEntities entity;

SDL_Window* displayWindow;
bool gameIsRunning = true;

float status = 0;


ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

GLuint loadTexture(const char* filePath) {
    // STEP 1: Loading the image file
    int width, height, n;
    unsigned char* image = stbi_load(filePath, &width, &height, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        cout << "Unable to load image. Make sure the path is correct";
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    return textureID;
}

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    // Load up our shaders
    program.load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    // Defines every translation, rotations, or scaling applied to an object
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
     
    program.set_projection_matrix(projectionMatrix);
    program.set_view_matrix(viewMatrix);
    
    glUseProgram(program.get_program_id());
    
    glClearColor(0.17f, 0.13f, 0.07f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    entity.player = new Entity();
    entity.player->mPosition = glm::vec3(0, 3.35, 0);
    entity.player->mMovement = glm::vec3(0);
    entity.player->mAcceleration = glm::vec3(0, -0.2f, 0);
    entity.player->mSpeed = 1.2f;
    entity.player->textureID = loadTexture("assets/charizard.png");
    
    entity.player->height = 0.8f;
    entity.player->width = 0.8f;
    
        
    // Initialize game platform
    entity.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID =loadTexture("assets/platform.png");
    GLuint landingSiteTextureID = loadTexture("assets/landingplatform.png");
    fontTextureID = loadTexture("assets/characters.png");
    
    // Building game world
   
        float x = -5.5f;
        float y = -3.25f;
        for (int i = 0; i < 11; ++i) {
            if (i == 3 || i == 4 || i == 7 || i == 8){
                entity.platforms[i].textureID = landingSiteTextureID;
                entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            }
            else{
                entity.platforms[i].textureID = platformTextureID;
                entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            }
            x += 1;
        }
        x = -5.0f;
        y = -2.25f;
        for (int i = 11; i <18; ++i) {
            entity.platforms[i].textureID = platformTextureID;
            entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            y += 1;
        }
        x = 5.0f;
        y = -2.25f;
        for (int i = 18; i < 25; ++i){
            entity.platforms[i].textureID = platformTextureID;
            entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            y += 1;
        }
        x = 1;
        y = 1;
        for (int i = 25; i <27; ++i) {
            if (i == 26){
                entity.platforms[i].textureID = landingSiteTextureID;
                entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            }
            else{
                entity.platforms[i].textureID = platformTextureID;
                entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            }
            x += 1;
        }
        x = -3;
        y = 0;
        for(int i = 27; i < 29; ++i){
            entity.platforms[i].textureID = platformTextureID;
            entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            y += 1;
        }
    
        x = -2;
        y = 0;
        for(int i = 29; i < 30; ++i){
            entity.platforms[i].textureID = platformTextureID;
            entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            
        }
        
    
        x = 2;
        y = -1;
        for(int i = 30; i < PLATFORM_COUNT; ++i){
            entity.platforms[i].textureID = platformTextureID;
            entity.platforms[i].mPosition = glm::vec3(x, y, 0);
            x += 1;
        }
        
        for (int i = 0; i < PLATFORM_COUNT; i++) {
            entity.platforms[i].update(0, NULL, 0);
        }
     
    
 
}

void processInput() {
    
    entity.player->mMovement = glm::vec3(0);
    
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
                        break;
                        
                    case SDLK_RIGHT:
                        break;
                        
                   
                }
                break;
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        entity.player->mAcceleration.x += -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        entity.player->mAcceleration.x += 1.0f;
    }
    
    if (glm::length(entity.player->mMovement) > 1.0f) {
        entity.player->mMovement = glm::normalize(entity.player->mMovement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void update() {
    if (entity.player->hitsLeft || entity.player->hitsRight || entity.player->hitsTop) {
        status = -1;
    }
    if (entity.player->hitsBottom == true) {
        if (entity.platforms[3].hitsTop == true || entity.platforms[4].hitsTop == true ||entity.platforms[7].hitsTop == true ||
            entity.platforms[8].hitsTop == true ||   entity.platforms[26].hitsTop == true) {
            status = 1;
        }
        else {
            status = -1;
        }
    }
    if (status == -1 || status == 1) {
        return;
    }
   float ticks = (float)SDL_GetTicks() / 1000.0f;
   float deltaTime = ticks - lastTicks;
   lastTicks = ticks;
   deltaTime += accumulator;
   if (deltaTime < FIXED_TIMESTEP) {
       accumulator = deltaTime;
       return;
   }
   while (deltaTime >= FIXED_TIMESTEP) {
       entity.player->update(FIXED_TIMESTEP, entity.platforms, PLATFORM_COUNT);
       deltaTime -= FIXED_TIMESTEP;
   }
   accumulator = deltaTime;
}


void drawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float screeenSize, float spacing, glm::vec3 position) {
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;
    std::vector<float> vertices;
    std::vector<float> texCoordinates;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (screeenSize + spacing) * i;
        
        float uCoord = (float)(index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float vCoord = (float)(index / FONTBANK_SIZE) / FONTBANK_SIZE;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screeenSize), 0.5f * screeenSize,
            offset + (-0.5f * screeenSize), -0.5f * screeenSize,
            offset + (0.5f * screeenSize), 0.5f * screeenSize,
            offset + (0.5f * screeenSize), -0.5f * screeenSize,
            offset + (0.5f * screeenSize), 0.5f * screeenSize,
            offset + (-0.5f * screeenSize), -0.5f * screeenSize,
        });
        texCoordinates.insert(texCoordinates.end(), {
            uCoord, vCoord,
            uCoord, vCoord + height,
            uCoord + width, vCoord,
            uCoord + width, vCoord + height,
            uCoord + width, vCoord,
            uCoord, vCoord + height,
        });
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    program->set_model_matrix(modelMatrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        entity.platforms[i].render(&program);
    }
    entity.player->render(&program);
    if (status == -1) {
        drawText(&program, fontTextureID, "Mission Failed!", 1, -0.5, glm::vec3(-3.375f, 1, 0));
    }
    else if (status == 1) {
        drawText(&program, fontTextureID, "Mission Accomplish!", 1, -0.5, glm::vec3(-4.125f, 1, 0));
    }
    SDL_GL_SwapWindow(displayWindow);
}


void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialize();
    
    while (gameIsRunning) {
        processInput();
        update();
        render();
    }
    
    shutdown();
    
    return 0;
}
