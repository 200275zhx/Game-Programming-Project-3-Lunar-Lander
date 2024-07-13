/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "ComplexCollisionBody.h"
#include "GameObject.h"
#include "glm/gtx/rotate_vector.hpp"

ComplexCollisionBody::ComplexCollisionBody(const std::vector<glm::vec3>& vertices, bool dynamic)
    : vertices(vertices), isDynamic(dynamic), owner(nullptr) {

}

void ComplexCollisionBody::setOwner(GameObject* gameObject) {
    owner = gameObject;
}

std::vector<glm::vec3> ComplexCollisionBody::getVertices() const {
    glm::vec3 translation = owner->getPosition();
    glm::vec3 rotation = owner->getRotation();
    glm::vec3 scale = owner->getScale();

    // Create the transformation matrices
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 model_matrix = translation_matrix * rotation_matrix * scale_matrix;

    // Transform the vertices
    std::vector<glm::vec3> transformedVertices;
    transformedVertices.reserve(vertices.size());

    for (const auto& vertex : vertices) {
        glm::vec4 transformedVertex = model_matrix * glm::vec4(vertex, 1.0f);
        transformedVertices.push_back(glm::vec3(transformedVertex));
    }

    return transformedVertices;
}

std::vector<glm::vec3> ComplexCollisionBody::getAxes(const std::vector<glm::vec3>& object_vertices) const {
    std::vector<glm::vec3> axes;
    int vertexCount = object_vertices.size();
    //std::cout << "\n\n\n" << vertices[0][0] << ", " << vertices[0][1] << ", " << vertices[0][2] << "\n\n\n";

    for (int i = 0; i < vertexCount; ++i) {
        glm::vec3 p1 = object_vertices[i];
        glm::vec3 p2 = object_vertices[(i + 1) % vertexCount];
        glm::vec3 edge = p2 - p1;
        glm::vec3 normal = perpendicular(edge);
        axes.push_back(normal);
    }

    return axes;
}

std::pair<float, float> ComplexCollisionBody::project(const std::vector<glm::vec3>& object_vertices, const glm::vec3& axis) const {
    float minProjection = std::numeric_limits<float>::infinity();
    float maxProjection = -std::numeric_limits<float>::infinity();

    for (const auto& vertex : object_vertices) {
        float projection = dotProduct(vertex, glm::normalize(axis));
        if (projection < minProjection) {
            minProjection = projection;
        }
        if (projection > maxProjection) {
            maxProjection = projection;
        }
    }

    return { minProjection, maxProjection };
}