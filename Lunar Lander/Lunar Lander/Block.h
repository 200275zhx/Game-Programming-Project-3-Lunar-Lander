#pragma once
#include "GameObject.h"
#include "TextureRender.h"

class Block : public GameObject {
public:
	Block(GLuint textureID, glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE);
	void render(ShaderProgram* shader_program) const override;
private:
	GLuint textureID;
};