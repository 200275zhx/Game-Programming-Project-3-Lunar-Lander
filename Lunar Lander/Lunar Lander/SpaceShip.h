#pragma once
#include "GameObject.h"
#include "TextureRender.h"

const float MAX_FUEL = 100.0f;
const float ANIMATION_SEQUENCE_FRAME = 1;
const float RADIUS_FACTOR = 0.5f;
const float BOOSTER_POWER = 0.3f;
const float DRAG_COEFFICIENT = 0.36f; // 0.3 DRAG COEFFICIENT * 1.2 AIR DENSITY * 1 AREA
const float ANGULAR_DRAG = 100.0f;
const float GRAVITY = 0.098f;

class SpaceShip : public GameObject {
public:
	bool alive;
	bool on_ground;

	SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame, 
		std::vector<std::vector<int>> animation_sequences);
	SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame,
		std::vector<std::vector<int>> animation_sequences, std::vector<int> active_animation_sequence, int animation_index);
	SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame,
		std::vector<std::vector<int>> animation_sequences, std::vector<int> active_animation_sequence, int animation_index,
		glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE);

	void update(float delta_time) override;
	void render(ShaderProgram* shader_program) const override;

	void left_fuel();
	void right_fuel();
	void left_idle();
	void right_idle();
	float get_fuel() { return fuel; }

	void debug_move_left() { position += glm::vec3(-0.03f, 0.0f, 0.0f); }
	void debug_move_right() { position += glm::vec3(0.03f, 0.0f, 0.0f); }
	void debug_move_up() { position += glm::vec3(0.0f, 0.03f, 0.0f); }
	void debug_move_down() { position += glm::vec3(0.0f, -0.03f, 0.0f); }
	void debug_rotate_left() { rotation += glm::vec3(0.0f, 0.0f, 0.03f); }
	void debug_rotate_right() { rotation += glm::vec3(0.0f, 0.0f, -0.03f); }

private:
	GLuint textureID;

	int anim_col;
	int anim_row;

	std::vector<std::vector<int>> anim_sequences;
	std::vector<int> active_anim_sequence;
	int anim_index;
	int anim_frame;
	float anim_time;

	float fuel;

	float mass; // mass = scale
	float width; // width = scale
	float height; // height = width / 2
	float inertia; // I = (1/12) * mass * (width^2 + height^2)
	float radius; // r = width / 2
	float left_power; // left_power = scale
	float right_power; // right_power = scale

	float left_force;
	float right_force;
	float angular_vel;
	float linear_vel;
	float gravitational_vel;
};