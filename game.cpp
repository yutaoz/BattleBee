#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

#include <stdlib.h>

float player_pos_x = 0, player_dp_x = 0.f;
float player_pos_y = -40, player_dp_y = 0.f;

float enemy_pos_x = 0, enemy_dp_x = 0.f;
float enemy_pos_y = 40, enemy_dp_y = 0.f;

float arena_border_right = 95;
float arena_border_left = -95;
float arena_border_bottom = 50;
float arena_border_top = -40;
float player_half_size_x = 3;
float player_half_size_y = 4.5;
bool wall = false;
auto wallTimer = std::chrono::steady_clock::now();
auto wall_cooldown = std::chrono::steady_clock::now();
bool wallOnCd = false;
bool bullet_exists = false;
float bullet_x = 0;
float bullet_y = 0;
float player_limit = -30;

int enemy_direction = 0;
auto enemy_change_timer = std::chrono::steady_clock::now();
int enemy_movement_cooldown = 0;

int WALL_COOLDOWN = 3;
int WALL_DURATION = 3;

internal bool check_hit(float x1, float y1, float half_x_1, float half_y_1, float x2, float y2, float half_x_2, float half_y_2) {
	if ((y1 + half_y_1 >= y2 - half_y_2) && (x1 + half_x_1 >= x2 - half_x_2) && (x1 - half_x_1 <= x2 + half_x_2)) {
		return true;
	}
	return false;
}


internal void simulate_game(Input* input, float dt) {
	clear_screen(0xC76D7E);

	float player_ddp_x = 0.f, player_ddp_y = 0.f;
	float enemy_ddp_x = 0.f;
	if (is_down(BUTTON_UP)) {
		player_ddp_y += 2000;
	}
	if (is_down(BUTTON_DOWN)) {
		player_ddp_y -= 2000;
	}
	if (is_down(BUTTON_LEFT)) {
		player_ddp_x -= 2000;
	}
	if (is_down(BUTTON_RIGHT)) {
		player_ddp_x += 2000;
	}
	if (pressed(BUTTON_SHOOT)) {
		if (!bullet_exists) {
			bullet_exists = true;
			bullet_x = player_pos_x;
			bullet_y = player_pos_y;
		}
	}
	if (pressed(BUTTON_DASH)) {
		if (!wallOnCd && !wall) {
			wall = true;
			wallTimer = std::chrono::steady_clock::now();
		}
	}

	player_ddp_x -= player_dp_x * 10.f; // friction
	player_ddp_y -= player_dp_y * 10.f; // friction

	// draw player
	draw_rect(player_pos_x, player_pos_y, 3, 1.5, 0x000000);
	draw_rect(player_pos_x - 1.5, player_pos_y, 0.5, 1.5, 0xFFFFFF);
	draw_rect(player_pos_x + 1.5, player_pos_y, 0.5, 1.5, 0xFFFFFF);
	draw_rect(player_pos_x, player_pos_y - 3.5, 3, 3, 0xA6CFD5);
	draw_rect(player_pos_x - 2, player_pos_y - 3.5, 1.5, 1, 0xFFFFFF);
	draw_rect(player_pos_x + 2, player_pos_y - 3.5, 1.5, 1, 0xFFFFFF);

	player_pos_x = player_pos_x + player_dp_x * dt + player_ddp_x * dt * dt * .5f;
	player_pos_y = player_pos_y + player_dp_y * dt + player_ddp_y * dt * dt * .5f;

	player_dp_x = player_dp_x + player_ddp_x * dt;
	player_dp_y = player_dp_y + player_ddp_y * dt;

	

	if (player_pos_x + player_half_size_x > arena_border_right) {
		player_pos_x = arena_border_right - player_half_size_x;
		player_dp_x *= -2;
	}
	else if (player_pos_x - player_half_size_x < arena_border_left) {
		player_pos_x = arena_border_left + player_half_size_x;
		player_dp_x *= -2;
	}

	if (player_pos_y + player_half_size_y > arena_border_bottom - 20) {
		player_pos_y = arena_border_bottom - 20 - player_half_size_y;
		player_dp_y *= 0;
	}
	else if (player_pos_y - player_half_size_y < arena_border_top) {
		player_pos_y = arena_border_top + player_half_size_y;
		player_dp_y *= 0;
	}

	if (wall) {
		draw_rect(player_pos_x, player_pos_y + 10, 7, 1, 0x000000);
		std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - wallTimer;
		if (elapsed_seconds.count() >= WALL_DURATION) {
			wall = false;
			wallOnCd = true;
			wall_cooldown = std::chrono::steady_clock::now();;
			
		}
	}

	std::chrono::duration<double> elapsed_cd = std::chrono::steady_clock::now() - wall_cooldown;

	if (elapsed_cd.count() >= WALL_COOLDOWN) {
		wallOnCd = false;
	}

	enemy_change_timer = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_cd_enemy = std::chrono::steady_clock::now() - enemy_change_timer;

	if (elapsed_cd_enemy.count() >= enemy_movement_cooldown) {
		enemy_direction = rand() % 2;
		enemy_movement_cooldown = rand() % 4 + 2;
	}
	if (enemy_direction == 0) {
		enemy_ddp_x -= 1500;
	}
	else {
		enemy_ddp_x += 1500;
	}

	if (enemy_pos_x + player_half_size_x > arena_border_right) {
		enemy_pos_x = arena_border_right - player_half_size_x;
	}
	else if (enemy_pos_x - player_half_size_x < arena_border_left) {
		enemy_pos_x = arena_border_left + player_half_size_x;
	}

	//enemy_ddp_x -= enemy_dp_x * 10.f; // friction

	//enemy_pos_x = enemy_pos_x + enemy_dp_x * dt + enemy_ddp_x * dt * dt * .5f;

	//enemy_dp_x = enemy_dp_x + enemy_ddp_x * dt;

	
	// draw enemy
	draw_rect(enemy_pos_x, enemy_pos_y, 13, 7, 0x4D4861);
	draw_rect(enemy_pos_x + 5, enemy_pos_y - 5, 1, 7, 0x4D4861);
	draw_rect(enemy_pos_x - 5, enemy_pos_y - 5, 1, 7, 0x4D4861);
	draw_rect(enemy_pos_x + 4, enemy_pos_y + 3, 2, 2, 0xEDCB96);
	draw_rect(enemy_pos_x - 4, enemy_pos_y + 3, 2, 2, 0xEDCB96);

	if (bullet_exists) {
		draw_rect(bullet_x, bullet_y, 1, 1, 0x000000);
		bullet_y = bullet_y + dt * 200;
		if (bullet_y >= arena_border_bottom) {
			bullet_exists = false;
		}

		if (check_hit(bullet_x, bullet_y, 1, 1, enemy_pos_x, enemy_pos_y, 13, 7)) {
			clear_screen(0xFFFFFF);
		}
	}


	

	
}