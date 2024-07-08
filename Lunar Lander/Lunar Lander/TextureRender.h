#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "ShaderProgram.h"
#include <vector>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

const int FONTBANK_SIZE = 16;
const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;


void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float screen_size, float spacing, glm::vec3 position);

void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id,
    int index, int rows, int cols);

GLuint load_texture(const char* filepath);