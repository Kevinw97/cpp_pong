#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_1_pos, player_1_dpos, player_2_pos, player_2_dpos;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5;
float player_half_size_y = 12;
float ball_pos_x, ball_pos_y, ball_dpos_x = 100, ball_dpos_y;
float ball_half_size = 1;

internal void 
simulate_player(float *pos, float *dpos, float ddpos, float dt) {
	*pos = *pos + *dpos * dt + ddpos * dt * dt * .5f;
	*dpos = *dpos + ddpos * dt;

	if (*pos + player_half_size_y > arena_half_size_y) {
		*pos = arena_half_size_y - player_half_size_y;
		*dpos = 0;
	}
	else if (*pos - player_half_size_y < -arena_half_size_y) {
		*pos = -arena_half_size_y + player_half_size_y;
		*dpos = 0;
	}
}

internal bool
aabb_vs_aabb(float a_x, float a_y, float a_hs_x, float a_hs_y,
	float b_x, float b_y, float b_hs_x, float b_hs_y) {
	return (a_x + a_hs_x > b_x - b_hs_x &&
		a_x - a_hs_x < b_x + b_hs_x &&
		a_y + a_hs_y > b_y - b_hs_y &&
		a_y - a_hs_y < b_y + b_hs_y);
}

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0xff0000);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);

	// Player 1
	float player_1_ddpos = 0.f;
	if (is_down(BUTTON_W)) player_1_ddpos += 2000;
	if (is_down(BUTTON_S)) player_1_ddpos -= 2000;

	player_1_ddpos -= player_1_dpos * 10.f;

	simulate_player(&player_1_pos, &player_1_dpos, player_1_ddpos, dt);

	// Player 2
	float player_2_ddpos = 0.f;
	if (is_down(BUTTON_UP)) player_2_ddpos += 2000;
	if (is_down(BUTTON_DOWN)) player_2_ddpos -= 2000;

	player_2_ddpos -= player_2_dpos * 10.f;

	simulate_player(&player_2_pos, &player_2_dpos, player_2_ddpos, dt);

	// Simulate ball
	{
		ball_pos_x += ball_dpos_x * dt;
		ball_pos_y += ball_dpos_y * dt;

		// Ball collision with players
		if (aabb_vs_aabb(ball_pos_x, ball_pos_y, ball_half_size, ball_half_size,
			-80, player_1_pos, player_half_size_x, player_half_size_y)) {
			ball_pos_x = -80 + player_half_size_x + ball_half_size;
			ball_dpos_x *= -1;
			ball_dpos_y = (ball_pos_y - player_1_pos) * 2;
		}
		else if (aabb_vs_aabb(ball_pos_x, ball_pos_y, ball_half_size, ball_half_size,
			80, player_2_pos, player_half_size_x, player_half_size_y)) {
			ball_pos_x = 80 - player_half_size_x - ball_half_size;
			ball_dpos_x *= -1;
			ball_dpos_y = (ball_pos_y - player_2_pos) * 2;
		}

		// Collision with arena
		if (ball_pos_y + ball_half_size > arena_half_size_y) {
			ball_pos_y = arena_half_size_y - ball_half_size;
			ball_dpos_y *= -1;
		}
		else if (ball_pos_y - ball_half_size < -arena_half_size_y) {
			ball_pos_y = -arena_half_size_y + ball_half_size;
			ball_dpos_y *= -1;
		}

		if (ball_pos_x + ball_half_size > arena_half_size_x) {
			ball_pos_x = arena_half_size_x - ball_half_size;
			ball_dpos_x *= -1;
		}
		else if (ball_pos_x - ball_half_size < -arena_half_size_x) {
			ball_pos_x = -arena_half_size_x + ball_half_size;
			ball_dpos_x *= -1;
		}
	}

	draw_rect(ball_pos_x, ball_pos_y, ball_half_size, ball_half_size, 0xffffff);

	draw_rect(-80, player_1_pos, player_half_size_x, player_half_size_y, 0x000000);
	draw_rect(80, player_2_pos, player_half_size_x, player_half_size_y, 0x000000);
}