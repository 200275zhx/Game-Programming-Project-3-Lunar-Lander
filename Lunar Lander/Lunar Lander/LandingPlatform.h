#pragma once
#include "GameObject.h"
#include "TextureRender.h"

class LandingPlatform : public GameObject {
public:
	LandingPlatform(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame,
		std::vector<int> animation_sequence, glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION,
		glm::vec3 INIT_SCALE);

	void update(float delta_time) override;
	void render(ShaderProgram* shader_program) const override;
private:
	GLuint textureID;

	int anim_col;
	int anim_row;

	std::vector<int> anim_sequence;
	int anim_index;
	int anim_frame;
	float anim_time;
};