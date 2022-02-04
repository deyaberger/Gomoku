#include "state.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "pattern.hpp"


int			count_free_threes(const State &state, int coord)
{
	const static int	r_d_arr[4] = {0, 1, 1,  1};
	const static int 	c_d_arr[4] = {1, 0, 1, -1};
	int r_d;
	int c_d;
	int	row = coord / BOARD_WIDTH;
	int	col = coord % BOARD_WIDTH;
	int player = state.get_square(row, col);
	int	start_r;
	int	start_c;
	int	count;
	int	gap;

	int	free_threes = 0;

	for (int dir = 0; dir < 4; dir++)
	{
		r_d = r_d_arr[dir];
		c_d = c_d_arr[dir];

		for (int start_delta = -5; start_delta <= 0; start_delta++)
		{
			count	= 0;
			gap		= 0;
			start_r = row + (start_delta * r_d);
			start_c = col + (start_delta * c_d);

			if (not is_in_bounds(start_r, start_c) or not is_in_bounds(start_r + 5 * r_d, start_c + 5 * c_d) or state.get_square(start_r, start_c) != EMPTY or state.get_square(start_r + 5 * r_d, start_c + 5 * c_d) != EMPTY)
				continue;

			for (int delta = 1; delta < 5; delta++)
			{
				if (state.get_square(start_r + delta * r_d, start_c + delta * c_d) == EMPTY)
				{
					gap += 1;
				}
				else if (state.get_square(start_r + delta * r_d, start_c + delta * c_d) == player)
				{
					count += 1;
				}
				else
				{
					break;
				}
			}
			if (count == 3 and gap == 1)
			{
				free_threes += 1;
				start_delta += 5;
			}
		}
	}
	return (free_threes);
}

inline int	count_full_and_empty(State &state, int row, int col, int r_delta, int c_delta, int player)
{
	int square;
	int	count		= 0;
	int empties 	= 0;
	int	top_space	= 0;
	int	bot_space	= 0;
	int	gap			= 0;
	bool started	= false;
	int	delta;

	int enemy 		= next_player(player);

	for (delta = -SURROUND_SIZE; delta <= SURROUND_SIZE; delta++)
	{
		if (not is_in_bounds(row + delta * r_delta, col + delta * c_delta))
			continue;

		square = state.get_square(row + delta * r_delta, col + delta * c_delta);

		if (square == enemy and started)
			break;
		
		if (square == EMPTY)
		{
			empties += 1;
			if (not started)
				bot_space += 1;
			else
				top_space += 1;
		}

		if (square == player)
		{
			count += 1;
			started = true;
			gap += top_space;
			top_space = 0;
		}
		// std::cout << "r: " << row + delta * r_delta << " ,c: " << col + delta * c_delta << std::endl;
		// std::cout << "count_: " << count << " empties: " << empties << " b_s: " << bot_space << " t_s: " << top_space << " gap: " << gap << std::endl;
	}

	if (count + empties < 5)
		return 0;
	// if (count == 5)
	// 	return WHITE_WIN;
	return (1 << (2 * count));
}

int			new_eval(State &state, int row, int col)
{
	int score		= 0;
	int player		= state.get_square(row, col);

	if (player == EMPTY) // ! MAYBE DONT DO THIS
	{
		return (0);
	}
	else
	{
		score += count_full_and_empty(state, row, col, 0, 1, player);
		score += count_full_and_empty(state, row, col, 1, 0, player);
		score += count_full_and_empty(state, row, col, 1, 1, player);
		score += count_full_and_empty(state, row, col, 1, -1, player);
	}

	if (player == BLACK)
	{
		return (-score);
	}
	else
	{
		return (score);
	}
}

int			eval_surround_square(State &state, int coord)
{
	int	row			= coord / BOARD_WIDTH;
	int	col			= coord % BOARD_WIDTH;
	int score_diff	= 0;
	int tmp_score;
	int	r_d;
	int	c_d;

	tmp_score = new_eval(state, row, col);
	score_diff += tmp_score - state.score_board[flat_coord(row, col)];
	state.score_board[flat_coord(row, col)] = tmp_score;
	for (int delta = -SURROUND_SIZE; delta <= SURROUND_SIZE; delta++) // * CAN BE OPTIMIZED TO AVOID vicinity of EDGES + - SURROUND_SIZE WHERE SCORE IS 0
	{
		if (delta == 0)
			continue;

		if (is_in_bounds(row, col + 1 * delta))
		{
			tmp_score = new_eval(state, row, col + 1 * delta);
			// tmp_score = new_eval_dir(state, row, col + 1 * delta, 0, 1);
			score_diff += tmp_score - state.score_board[flat_coord(row, col + 1 * delta)];
			state.score_board[flat_coord(row, col + 1 * delta)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col))
		{
			tmp_score = new_eval(state, row + 1 * delta, col);
			// tmp_score = new_eval_dir(state, row + 1 * delta, col, 1, 0);
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col)];
			state.score_board[flat_coord(row + 1 * delta, col)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col + 1 * delta))
		{
			tmp_score = new_eval(state, row + 1 * delta, col + 1 * delta);
			// tmp_score = new_eval_dir(state, row + 1 * delta, col + 1 * delta, 1, 1);
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col + 1 * delta)];
			state.score_board[flat_coord(row + 1 * delta, col + 1 * delta)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col - 1 * delta))
		{
			// tmp_score = new_eval_dir(state, row + 1 * delta, col - 1 * delta, 1, -1);
			tmp_score = new_eval(state, row + 1 * delta, col - 1 * delta);
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col - 1 * delta)];
			state.score_board[flat_coord(row + 1 * delta, col - 1 * delta)] = tmp_score;
		}
	}
	return (score_diff);
}
