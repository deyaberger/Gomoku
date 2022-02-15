#include "defines.hpp"
#include "state.hpp"
#include "eval.hpp"

#include "utils.hpp"
#include "pattern.hpp"


#include <string>
#include <iostream>
#include <iomanip>

State::State(/* args */)
{
	this->w_captures = 0;
	this->b_captures = 0;

}

State::~State()
{
}


void			State::set_piece(int row, int col)
{
	/*
	Sets a piece at the given position.
	*/
	if (this->player == WHITE)
	{
		if (not this->w_board[row * BOARD_WIDTH + col])
		{
			this->w_board.set(row * BOARD_WIDTH + col, true);
		}
	}
	else
	{
		if (not this->b_board[row * BOARD_WIDTH + col])
		{
			this->b_board.set(row * BOARD_WIDTH + col, true);
		}		
	}
	this->last_move = row * BOARD_WIDTH + col;
}


void			State::set_piece(int coord)
{
	/*
	Sets a piece at the given position.
	*/
	if (this->player == WHITE)
	{
		if (not this->w_board.test(coord))
		{
			this->w_board.set(coord, true);
		}
	}
	else
	{
		if (not this->b_board.test(coord))
		{
			this->b_board.set(coord, true);
		}		
	}
	this->last_move = coord;
}


int				State::get_square(int coord) const
{
	if (this->b_board[coord])
	{
		return  (BLACK);
	}
	else if (this->w_board[coord])
	{
		return  (WHITE);
	}
	else
	{
		return  (EMPTY);
	}
}


int				State::get_square(int row, int col) const
{
	if (this->b_board[row * BOARD_WIDTH + col])
	{
		return  (BLACK);
	}
	else if (this->w_board[row * BOARD_WIDTH + col])
	{
		return  (WHITE);
	}
	else
	{
		return  (EMPTY);		
	}
}


void			State::print(bool print_empty)
{
	// std::string symbols[4] = {"O", "X", " ", "-"};
	std::string symbols[4] = {"\x1b[31mO\x1b[0m", "\x1b[34mX\x1b[0m", " ", "-"};
	std::setfill(' ');
	int sym;

	if (this->player == BLACK)
		std::cout << "Next player: BLACK"  << std::endl;
	else
		std::cout << "Next player: WHITE"  << std::endl;

	std::cout << "W captures: " << this->w_captures << std::endl;
	std::cout << "B captures: " << this->b_captures << std::endl;
	std::cout << "Win: " << this->game_win << std::endl;
	std::cout << "Last chance: " << this->last_chance << std::endl;
	std::cout << "Winner: " << this->winner << std::endl;
	std::cout << "Score: " << this->score << std::endl;

	for (int c = 0; c < BOARD_WIDTH; c++)
	{
		std::cout << " " << std::setw(2) << c << " ";
	}
	std::cout << std::endl;

	for (int r = 0; r <= BOARD_HEIGHT; r++)
	{
		for (int c = 0; c <= BOARD_WIDTH; c++)
		{
			std::cout << "+   ";
		}
		std::cout << std::endl;
		if (r != BOARD_HEIGHT)
		{
			for (int c = 0; c < BOARD_WIDTH; c++)
			{
				sym = this->get_square(r, c);
				if (print_empty and sym == EMPTY)
				{
					if (this->live_board.test(r * BOARD_WIDTH + c))
						sym = LIVE - 2;
				}

				std::cout << "  " << symbols[sym / 2] << " ";
			}
			std::cout << std::setw(4) << r << std::endl;			
		}
	}
	std::cout << std::endl;
}


void			State::print_score_board()
{

	std::cout << "Score: " << this->score << std::endl;

	for (int c = 0; c < BOARD_WIDTH; c++)
	{
		std::cout << " " << std::setw(2) << c << " ";
	}
	std::cout << std::endl;

	for (int r = 0; r <= BOARD_HEIGHT; r++)
	{
		for (int c = 0; c <= BOARD_WIDTH; c++)
		{
			std::cout << "+   ";
		}
		std::cout << std::endl;
		if (r != BOARD_HEIGHT)
		{
			for (int c = 0; c < BOARD_WIDTH; c++)
			{
				if (this->score_board[flat_coord(r, c)] != 0)
				{
					std::cout << std::setw(4) << this->score_board[flat_coord(r, c)];			
				}
				else
				{
					std::cout << std::setw(4) << "  ";			
				}
			}
			std::cout << std::setw(4) << r << std::endl;			
		}
	}
	std::cout << std::endl;
}


bitboard&		State::get_player_board(void)
{
	if (this->player == WHITE)
	{
		return this->w_board;
	}
	return this->b_board;
}


bitboard&		State::get_enemy_board(void)
{
	if (this->player == WHITE)
	{
		return this->b_board;
	}
	return this->w_board;
}


int&			State::get_player_captures(void)
{
	if (this->player == WHITE)
	{
		return this->w_captures;
	}
	return this->b_captures;
}


int&			State::get_enemy_captures(void)
{
	if (this->player == WHITE)
	{
		return this->b_captures;
	}
	return this->w_captures;
}


inline void		increase_score_capture(int &score, int player_captures, int mult)
{
	#ifdef INCREASING_CAPTURE_VALUE
		score += (CAPTURE_VALUE << (player_captures)) * mult;
	#else
		score += CAPTURE_VALUE * 2 * mult;
	#endif
}

int				State::compute_captures(void)
{
	/*
	Finds and removes the captured pieces
	updates this -< captures
	*/
	pattern p;
	int score = 0;

	int last_move_r = this->last_move / BOARD_WIDTH;
	int last_move_c = this->last_move % BOARD_WIDTH;
	int last_coord  = this->last_move;

	bitboard& player_board = this->get_player_board();
	bitboard& enemy_board  = this->get_enemy_board();
	int& player_captures   = this->get_player_captures();
	int mult = (player == WHITE) ? 1 : -1;

	p = create_capture_pattern(RIGHT, this->player);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord + 1);
		enemy_board.set(last_coord + 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord + 2);
		enemy_board.set(last_coord + 2, false);
		increase_score_capture(this->score, player_captures, mult);
		score += 1;
		player_captures += 1;
		
	}
	p = create_capture_pattern(RIGHT, this->player, 3);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord - 1);
		enemy_board.set(last_coord - 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord - 2);
		enemy_board.set(last_coord - 2, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}
	
	p = create_capture_pattern(DOWN_RIGHT, this->player);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord + BOARD_WIDTH + 1);
		enemy_board.set(last_coord + BOARD_WIDTH + 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord + 2 * BOARD_WIDTH + 2);
		enemy_board.set(last_coord + 2 * BOARD_WIDTH + 2, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}
	p = create_capture_pattern(DOWN_RIGHT, this->player, 3);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord - BOARD_WIDTH - 1);
		enemy_board.set(last_coord - BOARD_WIDTH - 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord - 2 * BOARD_WIDTH - 2);
		enemy_board.set(last_coord - 2 * BOARD_WIDTH - 2, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}

	p = create_capture_pattern(DOWN, this->player);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord + BOARD_WIDTH);
		enemy_board.set(last_coord + BOARD_WIDTH, false);
		this->score += this->coord_evaluation_function(*this, last_coord + 2 * BOARD_WIDTH);
		enemy_board.set(last_coord + 2 * BOARD_WIDTH, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}
	p = create_capture_pattern(DOWN, this->player, 3);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord - BOARD_WIDTH);
		enemy_board.set(last_coord - BOARD_WIDTH, false);
		this->score += this->coord_evaluation_function(*this, last_coord - 2 * BOARD_WIDTH);
		enemy_board.set(last_coord - 2 * BOARD_WIDTH, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}

	p = create_capture_pattern(DOWN_LEFT, this->player);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord + BOARD_WIDTH - 1);
		enemy_board.set(last_coord + BOARD_WIDTH - 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord + 2 * BOARD_WIDTH - 2);
		enemy_board.set(last_coord + 2 * BOARD_WIDTH - 2, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}
	p = create_capture_pattern(DOWN_LEFT, this->player, 3);
	if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
	{
		this->score += this->coord_evaluation_function(*this, last_coord - BOARD_WIDTH);
		enemy_board.set(last_coord - BOARD_WIDTH + 1, false);
		this->score += this->coord_evaluation_function(*this, last_coord - 2 * BOARD_WIDTH);
		enemy_board.set(last_coord - 2 * BOARD_WIDTH + 2, false);
		score += 1;
		increase_score_capture(this->score, player_captures, mult);
		player_captures += 1;
	}
	return (score);
}


// int				State::find_pattern_around_last_move(pattern_generator gen, int player) const
// {
// 	// ! UNTESTED !!
// 	int directions[4] = {DOWN, RIGHT, DOWN_RIGHT, DOWN_LEFT};
// 	int score = 0;
// 	pattern p;
// 	int last_move_r = this->last_move / BOARD_WIDTH;
// 	int last_move_c = this->last_move % BOARD_WIDTH;
// 	for (int dir : directions)
// 	{
// 		p = gen(dir, player, 0);
// 		// print_pattern(p);
// 		if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
// 		{
// 			score += 1;
// 		}
// 		for (int variant = 1; variant < p.size; variant++)
// 		{
// 			p = gen(dir, player, variant);
// 			if (shift_pattern_to(p, last_move_r, last_move_c) and (*this == p))
// 			{
// 				score += 1;
// 			}
// 		}
// 	}
// 	return (score);
// }


inline bool 	State::operator==(const pattern& rhs) const
{
	return (((not rhs.color & WHITE) or ((this->w_board & rhs.w_bits) == rhs.w_bits)) 
		and ((not rhs.color & BLACK) or ((this->b_board & rhs.b_bits) == rhs.b_bits))) 
		and ((not rhs.color & EMPTY) or ((this->b_board | this->w_board) & rhs.e_bits).count() == 0);
}


bool 			State::operator<(const State& rhs) const
{
	return (this->score < rhs.score);
}


bool 			State::operator>(const State& rhs) const
{
	return (this->score > rhs.score);
}


// bool			State::contains(pattern& pat) const
// {
// 	return (*this == pat);
// }


void			State::update_live_board(void)
{
	this->live_board = ((this->b_board << 1) | ((this->b_board << 1) << BOARD_WIDTH) | ((this->b_board << 1) >> BOARD_WIDTH) | (this->b_board >> 1) | ((this->b_board >> 1) << BOARD_WIDTH) | ((this->b_board >> 1) >> BOARD_WIDTH) | (this->b_board >> BOARD_WIDTH) | (this->b_board << BOARD_WIDTH) | (this->w_board << 1) | ((this->w_board << 1) >> BOARD_WIDTH) | ((this->w_board << 1) << BOARD_WIDTH) | (this->w_board >> 1) | ((this->w_board >> 1) >> BOARD_WIDTH) | ((this->w_board >> 1) << BOARD_WIDTH) | (this->w_board >> BOARD_WIDTH) | (this->w_board << BOARD_WIDTH)) & ~(this->w_board | this->b_board);
}


State			State::make_baby_from_coord_no_eval(int coord)
{
	State s = *this;
	s.coord_evaluation_function = this->coord_evaluation_function;
	// std::cout << "Set piece: " << coord << std::endl;
	s.set_piece(coord);
	// std::cout << "Captures" << std::endl;
	s.compute_captures();

	
	// std::cout << "Player and live board" << std::endl;
	s.player = next_player(this->player);
	s.update_live_board();
	s.free_threes = count_free_threes(s, s.last_move);

	// std::cout << "baby done" << std::endl;
	// std::cout  << std::endl;
	return (s);
}


State			State::make_baby_from_coord(int coord) const
{
	State s = *this;
	s.coord_evaluation_function = this->coord_evaluation_function;
	// std::cout << "Set piece: " << coord << std::endl;
	s.set_piece(coord);
	// std::cout << "Captures" << std::endl;
	s.compute_captures();

	// std::cout << "Update eval" << std::endl;
	s.player = next_player(this->player);
	s.update_live_board();
	s.free_threes = count_free_threes(s, s.last_move);
	s.is_win();
	if (s.game_win)
	{
		if (s.winner == WHITE)
		{
			s.score = WHITE_WIN;
		}
		else
		{
			s.score = BLACK_WIN;
		}
	}
	else
	{
		s.score += s.coord_evaluation_function(s, s.last_move);
	}
	
	// std::cout << "Player and live board" << std::endl;


	// std::cout << "baby done" << std::endl;
	// std::cout  << std::endl;
	return (s);
}


State			State::make_baby_from_coord_precalc(int coord, int score_delta)
{
	State s = *this;
	// std::cout << "Set piece: " << coord << std::endl;
	s.set_piece(coord);

	// std::cout << "Captures" << std::endl;
	s.compute_captures();

	// std::cout << "Update eval" << std::endl;
	if (s.is_win())
	{
		s.game_win = true;
	}
	else
	{
		s.score += score_delta;
	}
	// std::cout << "Player and live board" << std::endl;
	s.player = next_player(this->player);
	s.update_live_board();

	// std::cout << "baby done" << std::endl;
	// std::cout  << std::endl;
	return (s);
}


bool			State::count_to_5(int row, int col, int r_delta, int c_delta, int player)
{
	int square;
	int score				= 0;
	int	count				= 1;

	if (this->get_square(row, col) != player)
		return false;


	for (int delta = 1; delta <= 5; delta++)
	{
		if (not is_in_bounds(row + delta * r_delta, col + delta * c_delta))
			break;

		square = this->get_square(row + delta * r_delta, col + delta * c_delta);

		if (square == next_player(player) or square == EMPTY)
			break;

		count += 1;
	}

	for (int delta = 1; delta <= 5; delta++)
	{
		if (not is_in_bounds(row - delta * r_delta, col - delta * c_delta))
			break;

		square = this->get_square(row - delta * r_delta, col - delta * c_delta);

		if (square == next_player(player) or square == EMPTY)
			break;
			
		count += 1;
	}	
	return (count >= 5);
}


bool			State::count_to_5(int row, int col, int player)
{
	if (count_to_5(row, col, 0, 1, player) or count_to_5(row, col, 1, 0, player) or	count_to_5(row, col, 1, 1, player) or count_to_5(row, col, 1, -1, player))
	{
		return true;
	}
	return false;
}


bool			State::can_capture_to_avoid_defeat(void)
{
	int		last_move	= this->last_move;
	int		last_row	= last_move / BOARD_WIDTH;
	int		last_col	= last_move % BOARD_WIDTH;

	int		previous_captures = this->get_player_captures();
	int		new_captures;

	State	bb;

	for(int c = 0; c < BOARD_SIZE; c++)
	{
		if (this->live_board.test(c) && (this->get_square(c) == EMPTY))
		{
			bb = this->make_baby_from_coord_no_eval(c);
			if (is_illegal(bb))
				continue;
			new_captures = bb.get_enemy_captures();
			if (new_captures == 5)
			{
				return true;
			}
			if ((new_captures != previous_captures) && not bb.count_to_5(last_row, last_col, bb.player))
			{
				return true;
			}
		}
	}
	return false;
}


bool			State::is_win(void)
{
	const int row = this->last_move / BOARD_WIDTH;
	const int col = this->last_move % BOARD_WIDTH;

	if (this->b_captures == 5 or this->w_captures == 5)
	{
		this->last_chance = false;
		this->game_win	= true;
		this->winner	= next_player(this->player);
		return true;
	}

	if (this->last_chance)
	{
		this->last_chance = false;
		if (count_to_5(last_chance_move / BOARD_WIDTH, last_chance_move % BOARD_WIDTH, this->player))
		{
			this->game_win	= true;
			this->winner	= this->player;
			return true;
		}
	}

	if (!this->game_win && count_to_5(row, col, next_player(this->player)))
	{
		this->last_chance		= this->can_capture_to_avoid_defeat();
		this->last_chance_move	= this->last_move;
		this->game_win			= !this->last_chance;

		if (this->game_win)
			this->winner		= next_player(this->player);
			
		return this->game_win;
	}

	return false;
}


bool			is_illegal(State &s)
{
	return (s.free_threes == 2);
}


bitboard		State::make_illegal_move_board(void)
{
	bitboard illegals;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (not (this->w_board.test(i) or this->b_board.test(i)))
		{
			if (this->make_baby_from_coord(i).free_threes == 2)
				illegals.set(i, true);
		}
	}
	return illegals;
}


bool			was_anything_captured(const State& s1, const State& s2)
{
	return ((s1.w_captures != s2.w_captures) || (s1.b_captures != s2.b_captures));
}


State			capture_baby(const State& state, int coord)
{
	State s = state;
	s.coord_evaluation_function = state.coord_evaluation_function;
	// std::cout << "Set piece: " << coord << std::endl;
	s.set_piece(coord);
	// std::cout << "Captures" << std::endl;
	s.compute_captures();
	return (s);
}


bool			State::is_possible_capture(void)
{
	for (int c = 0; c < BOARD_SIZE; c++)
	{
		if (this->live_board.test(c))
		{
			if (was_anything_captured(capture_baby(*this, c), *this))
			{
				return (true);
			}
		}
	}
	return (false);
}
