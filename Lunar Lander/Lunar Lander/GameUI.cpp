/**
* Author: [Hexun Zhang]
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "GameUI.h"

GameUI::GameUI(GLuint textureID, glm::vec3 INIT_POSITION, float INIT_ROTATION, glm::vec3 INIT_SCALE) :

textureID(textureID), INIT_POSITION(INIT_POSITION), INIT_SCALE(INIT_SCALE), INIT_ROTATION(INIT_ROTATION), model_matrix(glm::mat4(1.0f)){}

void GameUI::update(glm::vec3 position) {
	model_matrix = glm::translate(glm::mat4(1.0f), position + INIT_POSITION);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE);
}
void GameUI::update(glm::vec3 position, glm::vec3 scale) {
	model_matrix = glm::translate(glm::mat4(1.0f), position + INIT_POSITION);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE * scale);
}
void GameUI::update(glm::vec3 position, float rotation) {
	model_matrix = glm::translate(glm::mat4(1.0f), position + INIT_POSITION);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION + rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE);
}

void GameUI::render(ShaderProgram* shader_program) const {
	shader_program->set_model_matrix(model_matrix);
	draw_sprite_from_texture_atlas(shader_program, textureID, 0, 1, 1);
}