#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include "ComplexCollision.h"
#include "SpaceShip.h"
#include "TextureRender.h"
#include "LandingPlatform.h"
#include "Block.h"

#define LOG(argument) std::cout << argument << '\n'

const int WINDOW_WIDTH = 1280,
WINDOW_HEIGHT = 960;

const float BG_RED = 0.5608f,
BG_BLUE = 0.5608f,
BG_GREEN = 0.6608f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char FONT_SPRITE_FILEPATH[] = "sprites/font1.png",
BLOCK_FILEPATH[] = "sprites/Block.png",
SPACESHIP_FILEPATH[] = "sprites/SpaceShip.png",
LANDINGPLATFORM_FILEPATH[] = "sprites/LandingPlatform.png";

SDL_Window* g_display_window;
bool g_game_is_running = true;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix,
g_projection_matrix;
float g_previous_ticks = 0.0f;
GLuint g_text_texture_id;
SpaceShip* g_spaceship = nullptr;
std::vector<LandingPlatform*> g_platforms;
std::vector<Block*> g_blocks;

void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    g_display_window = SDL_CreateWindow("Hello, Animation!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // ————— INSTANTIATING VIEW AND PROJ MATRICES ———— //
    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    // ————— SPACESHIP ———— //
    std::vector<std::vector<int>> spaceship_anim_sequences{ {0}, {1}, {2}, {3} };
    std::vector<glm::vec3> spaceship_collision_vertices{
        glm::vec3(-0.5f, 0.25f, 0.0f),
        glm::vec3(0.5f, 0.25f, 0.0f),
        glm::vec3(0.5f, -0.25f, 0.0f),
        glm::vec3(-0.5f, -0.25f, 0.0f)
    };
    g_spaceship = new SpaceShip(load_texture(SPACESHIP_FILEPATH), 1, 4, 1, spaceship_anim_sequences);
    g_spaceship->create_complex_collision_body(spaceship_collision_vertices, true);

    // ———— PLATFORM ———— //
    std::vector<int> platform_anim_sequence{ 0, 1, 2, 3 };
    std::vector<glm::vec3> platform_collision_vertices{
        glm::vec3(-0.5f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.0f)
    };
    LandingPlatform* starting_platform = new LandingPlatform(load_texture(LANDINGPLATFORM_FILEPATH), 1, 4, 4,
        platform_anim_sequence, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    starting_platform->create_complex_collision_body(platform_collision_vertices, false);
    g_platforms.push_back(starting_platform);

    // ———— BLOCKS ———— //
    std::vector<glm::vec3> block_vertices{
    glm::vec3(-0.5f, 0.5f, 0.0f),
    glm::vec3(0.5f, 0.5f, 0.0f),
    glm::vec3(0.5f, -0.5f, 0.0f),
    glm::vec3(-0.5f, -0.5f, 0.0f)
    };
    GLuint block_texture = load_texture(BLOCK_FILEPATH);
    Block* block1 = new Block(block_texture, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(1.0f));
    Block* block2 = new Block(block_texture, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.4f), glm::vec3(1.0f));
    Block* block3 = new Block(block_texture, glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(1.0f));
    Block* block4 = new Block(block_texture, glm::vec3(3.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.6f), glm::vec3(1.0f));
    Block* block5 = new Block(block_texture, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.7f), glm::vec3(1.0f));
    
    g_blocks.push_back(block1);
    g_blocks.push_back(block2);
    g_blocks.push_back(block3);
    g_blocks.push_back(block4);
    g_blocks.push_back(block5);

    for (Block* block : g_blocks) { block->create_complex_collision_body(block_vertices, false); }

    // ———— TEXT ———— //
    glUseProgram(g_shader_program.get_program_id());
    g_text_texture_id = load_texture(FONT_SPRITE_FILEPATH);

    // ————— GENERAL ———— //
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_spaceship->left_idle();
    g_spaceship->right_idle();

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_WINDOWEVENT_CLOSE:
        case SDL_QUIT:
            g_game_is_running = false;
            break;

        /*case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_game_is_running = false;
                break;

            default:
                break;
            }*/
        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // ---------- LEFT SIDE PADDLE CONTROL ----------- //

    if (key_state[SDL_SCANCODE_A])
    {
        g_spaceship->debug_move_left();
        //g_spaceship->left_fuel();
    }
    if (key_state[SDL_SCANCODE_D])
    {
        g_spaceship->debug_move_right();
        //g_spaceship->right_fuel();
    }
    if (key_state[SDL_SCANCODE_W])
    {
        g_spaceship->debug_move_up();
    }
    if (key_state[SDL_SCANCODE_S])
    {
        g_spaceship->debug_move_down();
    }
    if (key_state[SDL_SCANCODE_Q])
    {
        g_spaceship->debug_rotate_left();
    }
    if (key_state[SDL_SCANCODE_E])
    {
        g_spaceship->debug_rotate_right();
    }
}
float g_timer;

void update()
{
    // ———— DELTA TIME CALCULATIONS ———— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;
    g_timer += delta_time;

    if (g_spaceship->alive) { g_spaceship->update(delta_time); }
    for (LandingPlatform* platform : g_platforms) { 
        platform->update(delta_time);
        resolveCollision(*g_spaceship, *platform);
    }
    for (Block* block : g_blocks) { resolveCollision(*g_spaceship, *block); }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    g_spaceship->render(&g_shader_program);
    for (LandingPlatform* platform : g_platforms) { platform->render(&g_shader_program); }
    for (Block* block : g_blocks) { block->render(&g_shader_program); }

    draw_text(&g_shader_program, g_text_texture_id, std::string("PRESS [A] TO USE LEFT BOOSTER"), 0.25f, 0.0f, glm::vec3(-2.5f, 2.0f, 0.0f));
    draw_text(&g_shader_program, g_text_texture_id, std::string("PRESS [D] TO USE RIGHT BOOSTER"), 0.25f, 0.01f, glm::vec3(-2.5f, 1.5f, 0.0f));

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
