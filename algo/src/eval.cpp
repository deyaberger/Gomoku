#include "state.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "pattern.hpp"

/**
 * @brief Count the free threes caused by a move
 * 
 * @param state 
 * @param coord coordinate of the move around which to count free threes
 * @return The number of free threes caused by move at coord
 */
int			count_free_threes(const State &state, int coord)
{
	const static int	r_d_arr[4] = {0, 1, 1,  1};
	const static int 	c_d_arr[4] = {1, 0, 1, -1};
	int r_d;
	int c_d;
	int	row		= coord / BOARD_WIDTH;
	int	col 	= coord % BOARD_WIDTH;
	int player 	= state.get_square(row, col);
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

/**
 * @brief Score caused by stone at (row, col) in direction r_delta, c_delta
 * (1, 0) is vertical for example
 * 
 * 
 * @param state 
 * @param row 
 * @param col 
 * @param r_delta row variation
 * @param c_delta col variation 
 * @param player 
 * @return int 
 */
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
	return (1 << (2 * count));
}

/**
 * @brief Computes the number of ways the stone at (row, col) can be captured in a certain direction (r_delta, c_delta) MULTIPLIED BY potential_capture_value
 * 
 * @param state 
 * @param row 
 * @param col 
 * @param r_delta 
 * @param c_delta 
 * @return (the number of ways the stone at (row, col) can be captured) * (potential_capture_value)
 */
inline int	potential_captures(State &state, int row, int col, int r_delta, int c_delta)
{
	int square;
	int	delta;
	int potential_captures = 0;
	int player		= state.get_square(row, col);
	int enemy 		= next_player(player);
	int lst[5];

	if (player == EMPTY)
		return (0);
	for (delta = -2; delta <= 2; delta++)
	{
		if (not is_in_bounds(row + delta * r_delta, col + delta * c_delta))
		{
			lst[delta + 2] = EDGE;
			continue;
		}
		square = state.get_square(row + delta * r_delta, col + delta * c_delta);
		lst[delta + 2] = square;
	}
	if (((lst[2] == player) && (lst[3] == player)) && (((lst[1] == enemy) && (lst[4] == EMPTY)) || ((lst[4] == enemy) && (lst[1] == EMPTY))))
		potential_captures += 1;
	if (((lst[1] == player) && (lst[2] == player)) && (((lst[0] == enemy) && (lst[3] == EMPTY)) || ((lst[3] == enemy) && (lst[0] == EMPTY))))
		potential_captures += 1;
	
	if (enemy == BLACK)
		return (potential_captures * potential_capture_value * (-1));
	else
		return (potential_captures * potential_capture_value);
}

/**
 * @brief Score caused by square at (row, col)
 * 
 * @param state 
 * @param row 
 * @param col 
 * @return int score
 */
int			new_eval(State &state, int row, int col)
{
	int score						= 0;
	int player						= state.get_square(row, col);

	if (player != EMPTY) // ! MAYBE DONT DO THIS
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

/**
 * @brief Score variation caused by the square at coord (takes into account how it affects the surrounding squares)
 * 
 * @param state 
 * @param coord
 * @return int score variation
 */
int			eval_surround_square(State &state, int coord)
{
	int	row			= coord / BOARD_WIDTH;
	int	col			= coord % BOARD_WIDTH;
	int score_diff	= 0;
	int tmp_score;
	int	r_d;
	int	c_d;

	tmp_score = new_eval(state, row, col);
	#ifdef POTENTIAL_CAPTURE
		tmp_score += potential_captures(state, row, col, 0, 1);
		tmp_score += potential_captures(state, row, col, 1, 1);
		tmp_score += potential_captures(state, row, col, 1, 0);
		tmp_score += potential_captures(state, row, col, -1, 1);
	#endif
	score_diff += tmp_score - state.score_board[flat_coord(row, col)];
	state.score_board[flat_coord(row, col)] = tmp_score;
	for (int delta = -SURROUND_SIZE; delta <= SURROUND_SIZE; delta++) // * CAN BE OPTIMIZED TO AVOID vicinity of EDGES + - SURROUND_SIZE WHERE SCORE IS 0
	{
		if (delta == 0)
			continue;

		if (is_in_bounds(row, col + 1 * delta))
		{
			tmp_score = new_eval(state, row, col + 1 * delta);
			#ifdef POTENTIAL_CAPTURE
				tmp_score += potential_captures(state, row, col + 1 * delta, 0, 1);
			#endif
			score_diff += tmp_score - state.score_board[flat_coord(row, col + 1 * delta)];
			state.score_board[flat_coord(row, col + 1 * delta)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col))
		{
			tmp_score = new_eval(state, row + 1 * delta, col);
			#ifdef POTENTIAL_CAPTURE
				tmp_score += potential_captures(state, row + 1 * delta, col, 1, 0);
			#endif
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col)];
			state.score_board[flat_coord(row + 1 * delta, col)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col + 1 * delta))
		{
			tmp_score = new_eval(state, row + 1 * delta, col + 1 * delta);
			#ifdef POTENTIAL_CAPTURE
				tmp_score += potential_captures(state, row + 1 * delta, col + 1 * delta, 1, 1);
			#endif
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col + 1 * delta)];
			state.score_board[flat_coord(row + 1 * delta, col + 1 * delta)] = tmp_score;
		}

		if (is_in_bounds(row + 1 * delta, col - 1 * delta))
		{
			tmp_score = new_eval(state, row + 1 * delta, col - 1 * delta);
			#ifdef POTENTIAL_CAPTURE
				tmp_score += potential_captures(state, row + 1 * delta, col - 1 * delta, 1, -1);
			#endif
			score_diff += tmp_score - state.score_board[flat_coord(row + 1 * delta, col - 1 * delta)];
			state.score_board[flat_coord(row + 1 * delta, col - 1 * delta)] = tmp_score;
		}
	}
	return (score_diff);
}
