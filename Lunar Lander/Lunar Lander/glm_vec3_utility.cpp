/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "glm_vec3_utility.h"
#include "glm/gtc/matrix_transform.hpp"

// Hash function for glm::vec3
namespace std {
    std::size_t hash<glm::vec3>::operator()(const glm::vec3& v) const noexcept {
        std::size_t h1 = std::hash<float>{}(v.x);
        std::size_t h2 = std::hash<float>{}(v.y);
        std::size_t h3 = std::hash<float>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }

    bool equal_to<glm::vec3>::operator()(const glm::vec3& lhs, const glm::vec3& rhs) const noexcept {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
}

float dotProduct(const glm::vec3& vectorA, const glm::vec3& vectorB) {
    return vectorA.x * vectorB.x + vectorA.y * vectorB.y;
}

glm::vec3 normalizeAxis(const glm::vec3& axis) {
    glm::vec3 normalizedAxis = glm::normalize(axis);
    if (normalizedAxis.x < 0 || (normalizedAxis.x == 0 && normalizedAxis.y < 0)) {
        normalizedAxis = -normalizedAxis;
    }
    return normalizedAxis;
}

glm::vec3 perpendicular(const glm::vec3& vector) {
    return glm::vec3(-vector.y, vector.x, 0);
}
