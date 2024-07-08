#pragma once

#include "glm_vec3_utility.h"
#include <vector>
#include <GL/glew.h>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

const float ANIMATION_FPS = 3;

class ComplexCollisionBody;  // Forward declaration

class GameObject {
public:
    GameObject();
    GameObject(glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE);
    ~GameObject();

    virtual void update(float delta_time);
    virtual void render(ShaderProgram* shader_program) const = 0;

    void create_complex_collision_body(const std::vector<glm::vec3>& vertices, bool dynamic);

    virtual ComplexCollisionBody* getCollisionBody();
    virtual glm::vec3 getPosition() const;
    virtual glm::vec3 getRotation() const;
    virtual glm::vec3 getScale() const;

    virtual void move(const glm::vec3& translation);

protected:
    ComplexCollisionBody* collisionBody;

    glm::vec3 INIT_POSITION;
    glm::vec3 INIT_SCALE;
    glm::vec3 INIT_ROTATION;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 model_matrix;
};
