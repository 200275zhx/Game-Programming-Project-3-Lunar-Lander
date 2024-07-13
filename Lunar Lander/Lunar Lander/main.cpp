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
#include <fstream>
#include <nlohmann/json.hpp>
#include <SDL_mixer.h>
#include "GameUI.h"
#include "AnimationSoundNotifier.h"

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

const float MILLISECONDS_IN_SECOND = 1000.0;

SDL_Window* g_display_window;
bool g_game_is_running = true;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix,
g_projection_matrix;
float g_previous_ticks = 0.0f;
GLuint g_text_texture_id;

// ————— FILE PATH ———— //

constexpr char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
                F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char  FONT_SPRITE_FILEPATH[] = "sprites/font1.png",
                BLOCK_FILEPATH[] = "sprites/Block.png",
                SPACESHIP_FILEPATH[] = "sprites/SpaceShip.png",
                LANDINGPLATFORM_FILEPATH[] = "sprites/LandingPlatform.png",
                ARROW_FILEPATH[] = "sprites/Arrow.png",
                FUELFRAME_FILEPATH[] = "sprites/FuelFrame.png",
                WHITEBOX_FILEPATH[] = "sprites/WhiteBox.png",
                MISSIONFAILED_FILEPATH[] = "sprites/MissionFailed.png",
                MISSIONCOMPLETE_FILEPATH[] = "sprites/MissionComplete.png";

constexpr char  BGM_FILEPATH[] = "sounds/bgm_mixdown.mp3",
                BOOSTER_SOUND_FILEPATH[] = "sounds/booster.wav",
                BROKEN_SOUND_FILEPATH[] = "sounds/broken.wav";

// ————— GAME OBJECT ———— //

SpaceShip* g_spaceship;
std::vector<LandingPlatform*> g_platforms;
std::vector<Block*> g_blocks;
GameUI* g_fuel_frame;
GameUI* g_fuel;
GameUI* g_mission_failed;
GameUI* g_mission_complete;
GameUI* g_arrow;

const glm::vec3 START = glm::vec3(0.0f, -0.5f, 0.0f);
const glm::vec3 GOAL = glm::vec3(39.0f, -11.6f, 0.0f);
bool g_is_mission_complete = false;

// ————— MUSIC ———— //

constexpr int CD_QUAL_FREQ = 44100,  // compact disk (CD) quality frequency
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;
constexpr int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

Mix_Music* g_music;
Mix_Chunk* g_boost_sfx;
Mix_Chunk* g_broken_sfx;

constexpr int PLAY_ONCE = 0,
NEXT_CHNL = -1,  // next available channel
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;

int booster_channel = -1;
int broken_channel = -1;

bool broken_play_once = true;


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

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
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
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
    GLuint platform_texture = load_texture(LANDINGPLATFORM_FILEPATH);

    LandingPlatform* starting_platform = new LandingPlatform(platform_texture, 1, 4, 4,
        platform_anim_sequence, START, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    LandingPlatform* ending_platform = new LandingPlatform(platform_texture, 1, 4, 4,
        platform_anim_sequence, GOAL, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    starting_platform->create_complex_collision_body(platform_collision_vertices, false);
    ending_platform->create_complex_collision_body(platform_collision_vertices, false);
    g_platforms.push_back(starting_platform);
    g_platforms.push_back(ending_platform);

    // ———— BLOCKS ———— //
    std::vector<glm::vec3> block_vertices{
    glm::vec3(-0.5f, 0.5f, 0.0f),
    glm::vec3(0.5f, 0.5f, 0.0f),
    glm::vec3(0.5f, -0.5f, 0.0f),
    glm::vec3(-0.5f, -0.5f, 0.0f)
    };
    GLuint block_texture = load_texture(BLOCK_FILEPATH);

    std::ifstream inputFile("Blocks.json");
    if (!inputFile) {
        std::cerr << "Unable to open file";
        exit(1);
    }

    nlohmann::json LevelAssets;
    inputFile >> LevelAssets;
    inputFile.close();

    // Accessing various parts of the JSON
    auto block_transform = [&LevelAssets](size_t index, const std::string& transformType) -> glm::vec3 {
        return glm::vec3(LevelAssets["Blocks"][index]["transform"][transformType]["x"],
            LevelAssets["Blocks"][index]["transform"][transformType]["y"],
            LevelAssets["Blocks"][index]["transform"][transformType]["z"]);
        };

    for (size_t index{}; index < LevelAssets["Blocks"].size(); ++index) {
        Block* block = new Block(block_texture,
            block_transform(index, "translation"), block_transform(index, "rotation"), block_transform(index, "scale"));
        block->create_complex_collision_body(block_vertices, false);
        g_blocks.push_back(block);
    }

    // ———— GAME UI ———— //
    g_fuel_frame = new GameUI(load_texture(FUELFRAME_FILEPATH), glm::vec3(0.0f, 3.5f, 0.0f), 0.0f, glm::vec3(2.0f));
    g_fuel = new GameUI(load_texture(WHITEBOX_FILEPATH), glm::vec3(0.0f, 3.295f, 0.0f), 0.0f, glm::vec3(1.8f, 0.15f, 1.0f));
    g_arrow = new GameUI(load_texture(ARROW_FILEPATH), glm::vec3(0.0f), 0.0f, glm::vec3(0.3f));
    g_mission_complete = new GameUI(load_texture(MISSIONCOMPLETE_FILEPATH), glm::vec3(0.0f, -2.0f, 0.0f), 0.0f, glm::vec3(5.0f));
    g_mission_failed = new GameUI(load_texture(MISSIONFAILED_FILEPATH), glm::vec3(0.0f, -2.0f, 0.0f), 0.0f, glm::vec3(5.0f));

    // ———— TEXT ———— //
    glUseProgram(g_shader_program.get_program_id());
    g_text_texture_id = load_texture(FONT_SPRITE_FILEPATH);

    // ————— GENERAL ———— //
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Start Audio
    if (Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE) == -1) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    g_music = Mix_LoadMUS(BGM_FILEPATH);
    if (!g_music) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_PlayMusic(g_music, LOOP_FOREVER);

    g_boost_sfx = Mix_LoadWAV(BOOSTER_SOUND_FILEPATH);
    if (!g_boost_sfx) {
        std::cerr << "Failed to load booster sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    g_broken_sfx = Mix_LoadWAV(BROKEN_SOUND_FILEPATH);
    if (!g_broken_sfx) {
        std::cerr << "Failed to load broken sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_VolumeChunk(g_boost_sfx, MIX_MAX_VOLUME / 2);  // Set the volume to a quarter of the maximum
    Mix_VolumeChunk(g_broken_sfx, MIX_MAX_VOLUME / 2); // Set the volume to a quarter of the maximum
}

void process_input()
{
    g_spaceship->left_idle();
    g_spaceship->right_idle();

    SDL_Event event;
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_WINDOWEVENT_CLOSE:
        case SDL_QUIT:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d) {
                if (g_spaceship->alive) { FadeIn(g_boost_sfx, booster_channel, LOOP_FOREVER, MILLISECONDS_IN_SECOND / 3); }
            }
            break;

        case SDL_KEYUP:
            if ((event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d) &&
                (!key_state[SDL_SCANCODE_A] && !key_state[SDL_SCANCODE_D])) {
                if (g_spaceship->alive) { FadeOut(booster_channel, MILLISECONDS_IN_SECOND / 3); }
            }
            break;

        default:
            break;
        }
    }

    // ---------- SPACESHIP CONTROL ----------- //
    if (g_spaceship->alive) {
        if (key_state[SDL_SCANCODE_A])
        {
            //g_spaceship->debug_move_left();
            g_spaceship->left_fuel();
        }
        if (key_state[SDL_SCANCODE_D])
        {
            //g_spaceship->debug_move_right();
            g_spaceship->right_fuel();
        }
    }
    /*if (key_state[SDL_SCANCODE_W])
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
    }*/
}

void update()
{
    // ———— DELTA TIME CALCULATIONS ———— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // ———— GAME OBJECT UPDATE ———— //
    if (!g_is_mission_complete) { g_spaceship->update(delta_time); }
    g_platforms[0]->update(delta_time);
    resolveCollision(*g_spaceship, *g_platforms[0]);
    g_platforms[1]->update(delta_time);
    if (resolveCollision(*g_spaceship, *g_platforms[1]) && g_spaceship->alive && abs(g_spaceship->getRotation().z) < 0.05f) { g_is_mission_complete = true; };
    for (Block* block : g_blocks) { if (resolveCollision(*g_spaceship, *block)) { g_spaceship->alive = false; } }

    // ———— GAME UI UPDATE ———— //
    glm::vec3 UI_position = g_spaceship->getPosition();
    float fuel_left = g_spaceship->get_fuel();
    float fuel_percent = fuel_left / MAX_FUEL;
    glm::vec3 dir = glm::normalize(GOAL - UI_position);

    g_fuel_frame->update(UI_position);
    g_fuel->update(UI_position + glm::vec3(fuel_percent * 1.8 / 2 - 0.9, 0.0f, 0.0f), glm::vec3(fuel_percent, 1.0f, 1.0f));
    g_arrow->update(UI_position + dir, atan2(dir.y, dir.x));
    if (g_is_mission_complete) { g_mission_complete->update(UI_position); }
    if (!g_spaceship->alive) { g_mission_failed->update(UI_position); }

    // ———— PLAY SOUND ———— //
    if (!g_spaceship->alive && broken_play_once) { 
        FadeOut(booster_channel, MILLISECONDS_IN_SECOND / 3);
        Mix_HaltChannel(booster_channel);

        Mix_PlayChannel(NEXT_CHNL, g_broken_sfx, PLAY_ONCE);
        std::cout << "played broken\n";
        broken_play_once = false;
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    g_spaceship->render(&g_shader_program);
    for (LandingPlatform* platform : g_platforms) { platform->render(&g_shader_program); }
    for (Block* block : g_blocks) { block->render(&g_shader_program); }
    g_fuel_frame->render(&g_shader_program);
    g_fuel->render(&g_shader_program);
    g_arrow->render(&g_shader_program);
    if (g_is_mission_complete) { g_mission_complete->render(&g_shader_program); }
    if (!g_spaceship->alive) { g_mission_failed->render(&g_shader_program); }

    draw_text(&g_shader_program, g_text_texture_id, std::string("PRESS [A] TO USE LEFT BOOSTER"), 0.25f, 0.0f, glm::vec3(-2.5f, 2.0f, 0.0f));
    draw_text(&g_shader_program, g_text_texture_id, std::string("PRESS [D] TO USE RIGHT BOOSTER"), 0.25f, 0.01f, glm::vec3(-2.5f, 1.5f, 0.0f));

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    Mix_HaltChannel(ALL_SFX_CHN);
    Mix_FreeChunk(g_boost_sfx);
    Mix_FreeMusic(g_music);
    SDL_Quit();
}


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
