#include "GameObject.h"
#include "ComplexCollisionBody.h"

GameObject::GameObject() :

    collisionBody(nullptr), INIT_POSITION(glm::vec3(0.0f)), INIT_ROTATION(glm::vec3(0.0f)), INIT_SCALE(glm::vec3(1.0f)),
    position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), model_matrix(glm::mat4(1.0f)) {}

GameObject::GameObject(glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE) :

    collisionBody(nullptr), INIT_POSITION(INIT_POSITION), INIT_ROTATION(INIT_ROTATION), INIT_SCALE(INIT_SCALE),
    position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), model_matrix(glm::mat4(1.0f)) {}

GameObject::~GameObject() {
    if (collisionBody) delete collisionBody;
}

void GameObject::create_complex_collision_body(const std::vector<glm::vec3>& vertices, bool dynamic) {
    if (collisionBody) delete collisionBody;
    collisionBody = new ComplexCollisionBody(vertices, dynamic);
    collisionBody->setOwner(this);
}

ComplexCollisionBody* GameObject::getCollisionBody() {
    return collisionBody;
}

void GameObject::move(const glm::vec3& translation) {
    position = position + translation;
}

glm::vec3 GameObject::getPosition() const {
    return position + INIT_POSITION;
}

glm::vec3 GameObject::getRotation() const {
    return rotation + INIT_ROTATION;
}

glm::vec3 GameObject::getScale() const {
    return scale * INIT_SCALE;
}

void GameObject::update(float delta_time) {
    model_matrix = glm::translate(model_matrix, INIT_POSITION);
}