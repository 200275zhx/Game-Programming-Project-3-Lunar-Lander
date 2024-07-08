#include "Block.h"

Block::Block(GLuint textureID, glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE) :

	GameObject(INIT_POSITION, INIT_ROTATION, INIT_SCALE), textureID(textureID) 
{
	model_matrix = glm::translate(model_matrix, INIT_POSITION);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE);
}

void Block::render(ShaderProgram* shader_program) const {
	shader_program->set_model_matrix(model_matrix);
	draw_sprite_from_texture_atlas(shader_program, textureID, 0, 1, 1);
}