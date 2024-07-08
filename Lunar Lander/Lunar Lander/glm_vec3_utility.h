#pragma once
#include <iostream>
#include <cmath>
#include <limits>
#include "glm/glm.hpp"

// Hash function for glm::vec3 - SOURCE : CHATGPT
namespace std {
    template <>
    struct hash<glm::vec3> {
        std::size_t operator()(const glm::vec3& v) const noexcept;
    };

    template <>
    struct equal_to<glm::vec3> {
        bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const noexcept;
    };
}

float dotProduct(const glm::vec3& vectorA, const glm::vec3& vectorB);

glm::vec3 normalizeAxis(const glm::vec3& axis);
glm::vec3 perpendicular(const glm::vec3& vector);