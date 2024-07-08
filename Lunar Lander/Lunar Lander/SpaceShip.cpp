#include "SpaceShip.h"

// --------------- CONSTRUCTORS --------------- //

SpaceShip::SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame,
	std::vector<std::vector<int>> animation_sequences) :

	GameObject(), textureID(textureID), anim_col(animation_col), anim_row(animation_row),
	fuel(MAX_FUEL), mass(1.0f), width(1.0f), height(0.5f), inertia(0.166f), radius(RADIUS_FACTOR),
	left_power(BOOSTER_POWER), right_power(BOOSTER_POWER), left_force(0.0f), right_force(0.0f),
	angular_vel(0.0f), linear_vel(0.0f), gravitational_vel(0.0f), anim_sequences(animation_sequences),
	active_anim_sequence(animation_sequences[0]), anim_index(0), anim_time(0.0f),
	anim_frame(animation_sequence_frame), alive(true), on_ground(false) {}

SpaceShip::SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame, 
	std::vector<std::vector<int>> animation_sequences, std::vector<int> active_animation_sequence, int animation_index) :
	
	GameObject(), textureID(textureID), anim_col(animation_col), anim_row(animation_row),
	fuel(MAX_FUEL), mass(1.0f), width(1.0f), height(0.5f), inertia(0.166f), radius(RADIUS_FACTOR),
	left_power(BOOSTER_POWER), right_power(BOOSTER_POWER), left_force(0.0f), right_force(0.0f),
	angular_vel(0.0f), linear_vel(0.0f), gravitational_vel(0.0f), anim_sequences(animation_sequences),
	active_anim_sequence(active_animation_sequence), anim_index(animation_index),
	anim_time(0.0f), anim_frame(animation_sequence_frame), alive(true), on_ground(false) {}

SpaceShip::SpaceShip(GLuint textureID, int animation_col, int animation_row, int animation_sequence_frame, 
	std::vector<std::vector<int>> animation_sequences, std::vector<int> active_animation_sequence, 
	int animation_index, glm::vec3 INIT_POSITION, glm::vec3 INIT_ROTATION, glm::vec3 INIT_SCALE) :
	
	GameObject(INIT_POSITION, INIT_ROTATION, INIT_SCALE), textureID(textureID),
	fuel(MAX_FUEL), mass(INIT_SCALE[0]), width(INIT_SCALE[0]), height(INIT_SCALE[0]/2), 
	inertia((1 / 12) * INIT_SCALE[0] * (INIT_SCALE[0] * INIT_SCALE[0] + INIT_SCALE[0] * INIT_SCALE[0])), 
	radius(INIT_SCALE[0] * RADIUS_FACTOR), left_power(INIT_SCALE[0] * BOOSTER_POWER), right_power(INIT_SCALE[0] * BOOSTER_POWER),
	left_force(0.0f), right_force(0.0f), angular_vel(0.0f), linear_vel(0.0f), gravitational_vel(0.0f), anim_col(animation_col),
	anim_row(animation_row), active_anim_sequence(active_animation_sequence), anim_sequences(animation_sequences), 
	anim_index(animation_index), anim_time(0.0f), anim_frame(animation_sequence_frame), alive(true), on_ground(false) {}


void SpaceShip::update(float delta_time) {

	// --------------- SWITCH ANIMATION SEQUENCE --------------- //
	if (left_force > 0.0f) { right_force > 0.0f ? 
		active_anim_sequence = anim_sequences[3] : active_anim_sequence = anim_sequences[1]; }
	else { right_force > 0.0f ? 
		active_anim_sequence = anim_sequences[2] : active_anim_sequence = anim_sequences[0]; }

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

	// --------------- APPLY FORCE --------------- //
	//float linear_drag = DRAG_COEFFICIENT * linear_vel * linear_vel / -2; // Linear Drag Force = (1/2) * (Fluid Density * Drag Coefficient * Area) * Linear Velocity ^ 2
	//float gravitational_drag = DRAG_COEFFICIENT * gravitational_vel * gravitational_vel / -2;
	//float angular_drag = DRAG_COEFFICIENT * ANGULAR_DRAG * angular_vel * angular_vel * radius * radius * radius / -2; // Angular Drag Force = (1/2) * (Fluid Density * Drag Coefficient * Area) * Angular Velocity ^ 2 * Radius ^ 3

	//float net_torque = radius * (right_force - left_force); // Net Torque = Radius * (Net Force Applied)
	//if (angular_vel <= 0) { net_torque -= angular_drag; } // Apply Drag Force Based on Current Rotation Direction
	//else { net_torque += angular_drag; }
	//float angular_acc = net_torque / inertia; // Angular Acceleration = Net Torque / Inertia
	//angular_vel += angular_acc * delta_time; // Angular Velocity = Angular Velocity + Angular Acceleration * Delta Time
	//rotation.z += angular_vel * delta_time;

	//float net_force = left_force + right_force + linear_drag; // Net Force = Total Force Applied - Drag Force
	//float linear_acc = net_force / mass; // Linear Acceleration = Net Force / Mass
	//linear_vel += linear_acc * delta_time; // Linear Velocity = Linear Velocity + Linear Acceleration * Delta Time
	//gravitational_vel += (GRAVITY + gravitational_drag) * delta_time; // Falling Velocity = Gravity * Time
	//glm::vec3 forward = glm::vec3(-sin(rotation.z), cos(rotation.z), 0.0f); // Get Normalized Moving Direction Through Rotation
	//position += linear_vel * forward * delta_time + gravitational_vel * glm::vec3(0.0f, -1.0f, 0.0f) * delta_time;

	//std::cout << "Forward Direction: (" << forward.x << ", " << forward.y << ", " << forward.z << ")\n";

	// --------------- UPDATE TRANSFORM --------------- //
	model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, INIT_POSITION + position);
	model_matrix = glm::rotate(model_matrix, INIT_ROTATION.z + rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::scale(model_matrix, INIT_SCALE);

	// --------------- UPDATE FUEL --------------- //
	fuel -= delta_time;
	if (fuel <= 0.0f) { alive = false; }
}

void SpaceShip::render(ShaderProgram* shader_program) const {
	// --------------- CAMERA FOLLOW --------------- //
	glm::mat4 camera_translation_matrix = glm::translate(glm::mat4(1.0f), -position);
	shader_program->set_view_matrix(camera_translation_matrix);

	// --------------- UPDATE MODEL MATRIX --------------- //
	shader_program->set_model_matrix(model_matrix);
	draw_sprite_from_texture_atlas(shader_program, textureID, active_anim_sequence[anim_index], anim_row, anim_col);
}

// --------------- MOVEMENT STATUS --------------- //

void SpaceShip::left_fuel() { left_force = left_power; }
void SpaceShip::right_fuel() { right_force = right_power; }
void SpaceShip::left_idle() { left_force = 0.0f; }
void SpaceShip::right_idle() { right_force = 0.0f; }
