/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LandingPlatform.h"
#include "ComplexCollisionBody.h"

LandingPlatform::LandingPlatform(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame,
	std::vector<int> animation_sequence, glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION,
	glm::vec3 INIT_SCALE) :

	GameObject(INIT_POSITION, INIT_ROTATION, INIT_SCALE), textureID(textureID), anim_col(animation_col), anim_row(animation_row),
	anim_sequence(animation_sequence), anim_index(0), anim_time(0.0f), anim_frame(animation_sequence_frame) 
{
	model_matrix = glm::translate(model_matrix, INIT_POSITION);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE);
}

void LandingPlatform::update(float delta_time) {
	// --------------- ANIMATION INDEX UPDATE --------------- //
	anim_time += delta_time;
	float spf = 1.0f / ANIMATION_FPS;

	if (anim_time >= spf)
	{
		anim_time = 0.0f;
		anim_index++;

		if (anim_index >= anim_frame)
		{
			anim_index = 0;
		}
	}
}

void LandingPlatform::render(ShaderProgram* shader_program) const {
	shader_program->set_model_matrix(model_matrix);
	draw_sprite_from_texture_atlas(shader_program, textureID, anim_sequence[anim_index], anim_row, anim_col);
}