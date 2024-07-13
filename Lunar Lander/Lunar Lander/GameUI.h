#pragma once
#include <GL/glew.h>
#include "glm/gtc/matrix_transform.hpp"
#include "TextureRender.h"
#include "ShaderProgram.h"

class GameUI {
public:
    GameUI(GLuint textureID, glm::vec3 INIT_POSITION, float INIT_ROTATION, glm::vec3 INIT_SCALE);

    virtual void update(glm::vec3 position);
    virtual void update(glm::vec3 position, glm::vec3 scale);
    virtual void update(glm::vec3 position, float rotation);
    virtual void render(ShaderProgram* program) const;

protected:
    glm::vec3 INIT_POSITION;
    glm::vec3 INIT_SCALE;
    float INIT_ROTATION;

    glm::mat4 model_matrix;
    GLuint textureID;
};