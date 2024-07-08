#pragma once

#include "glm_vec3_utility.h"
#include <vector>

class GameObject;  // Forward declaration

class ComplexCollisionBody {
public:
    bool isDynamic;

    ComplexCollisionBody(const std::vector<glm::vec3>& vertices, bool dynamic = true);
    void setOwner(GameObject* gameObject);
    std::vector<glm::vec3> getVertices() const;
    std::vector<glm::vec3> getAxes(const std::vector<glm::vec3>& object_vertices) const;
    std::pair<float, float> project(const std::vector<glm::vec3>& object_vertices, const glm::vec3& axis) const;

private:
    std::vector<glm::vec3> vertices;
    GameObject* owner;
};
