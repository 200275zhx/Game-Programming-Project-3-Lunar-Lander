/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "ComplexCollision.h"
#include <unordered_set>
#include <algorithm>

std::pair<float, glm::vec3> getSeparation(ComplexCollisionBody& bodyA, ComplexCollisionBody& bodyB) {
    std::vector<glm::vec3> vertices_A = bodyA.getVertices();
    std::vector<glm::vec3> vertices_B = bodyB.getVertices();

    std::vector<glm::vec3> axesA = bodyA.getAxes(vertices_A);
    std::vector<glm::vec3> axesB = bodyB.getAxes(vertices_B);
    std::unordered_set<glm::vec3> uniqueAxes;
    float maxSeparation = std::numeric_limits<float>::lowest();
    glm::vec3 separationAxis;

    // Normalize and combine axes, ensuring uniqueness
    for (const auto& axis : axesA) { uniqueAxes.insert(axis); }
    for (const auto& axis : axesB) { uniqueAxes.insert(axis); }

    for (const auto& axis : uniqueAxes) {
        auto projectionA = bodyA.project(vertices_A, axis);
        auto projectionB = bodyB.project(vertices_B, axis);

        // Print debug information
        /*std::cout << "Axis: (" << axis.x << ", " << axis.y << ")" << std::endl;
        std::cout << "Projection A: (" << projectionA.first << ", " << projectionA.second << ")" << std::endl;
        std::cout << "Projection B: (" << projectionB.first << ", " << projectionB.second << ")" << std::endl;*/

        float separation = 0.0f;

        if (projectionA.second < projectionB.first) {
            separation = projectionB.first - projectionA.second;
        }
        else if (projectionB.second < projectionA.first) {
            separation = projectionA.first - projectionB.second;
        }
        else {
            float overlap = std::min(projectionA.second, projectionB.second) - std::max(projectionA.first, projectionB.first);
            //std::cout << "Overlap on this axis: " << overlap << std::endl;
            separation = -overlap;
        }

        // Update maximum separation and the corresponding axis
        if (separation > maxSeparation) {
            maxSeparation = separation;
            separationAxis = (projectionA.second > projectionB.second) ? axis : -axis;
        }

        // Check if there is a separating axis
        if (separation > 0) {
            //std::cout << "Separating axis found, objects are not colliding." << std::endl;
            return { separation, axis };
        }
    }

    //std::cout << "Maximum separation (negative indicates collision): " << maxSeparation << std::endl;
    return { maxSeparation, separationAxis };
}

bool resolveCollision(GameObject& objectA, GameObject& objectB) {
    ComplexCollisionBody* bodyA = objectA.getCollisionBody();
    ComplexCollisionBody* bodyB = objectB.getCollisionBody();

    // Get separation vector
    std::pair<float, glm::vec3> separationResult = getSeparation(*bodyA, *bodyB);
    float separation = separationResult.first;
    glm::vec3 separationAxis = separationResult.second;

    // Move the objects if there's an overlap
    if (separation < 0) { // Only resolve if there's an overlap
        // Compute MTV (Minimum Translation Vector)
        glm::vec3 mtv = separationAxis * separation;

        if (bodyA->isDynamic) {
            if (!bodyB->isDynamic) {
                // Collision between dynamic and static object
                objectA.move(-mtv);
            }
            else { 
                // Collision between two dynamic objects
                objectA.move(-mtv / glm::vec3(2));
                objectB.move(-mtv / glm::vec3(2));
            }
        } else if (bodyB->isDynamic) { objectB.move(-mtv); }
        return true;
    }
    else { return false; }
}
