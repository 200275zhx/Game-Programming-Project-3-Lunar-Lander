/**
* This Collision Detection is using SAT Theorem, it only works for CONVEX shape not CONCAVE shapes
**/

#pragma once
#include "ComplexCollisionBody.h"
#include "GameObject.h"

bool resolveCollision(GameObject& objectA, GameObject& objectB);
std::pair<float, glm::vec3> getSeparation(ComplexCollisionBody& bodyA, ComplexCollisionBody& bodyB);


float get_separation(ComplexCollisionBody& a, ComplexCollisionBody& b);