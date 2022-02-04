
#ifndef STATE_H
#define STATE_H

#include "defines.hpp"
#include "pattern.hpp"

#include <set>

class State
{
	private:
	public:

		bitboard 	b_board;
		bitboard 	w_board;
		bitboard	live_board;
		int			last_move;

		int			player					= BLACK;
		int			w_captures				= 0;
		int			b_captures				= 0;
		int			free_threes				= 0;
		int			score					= 0;
		int			score_board[BOARD_SIZE]	= {};
		bool 		game_win				= false;
		bool		last_chance				= false;
		int			last_chance_move;
		int			winner					= 0;


		int 		(*coord_evaluation_function)(State&, int coord);
		State(/* args */);
		~State();

		void				set_piece(int row, int col);
		void				set_piece(int coord);
		int					get_square(int row, int col) const;
		int					get_square(int coord) const;
		void				print(bool print_empty = false);
		void				print_score_board();


		void				update_live_board(void);
		int					compute_captures(void);
		// int					find_pattern_around_last_move(pattern_generator gen, int player) const;

		bitboard&			get_player_board(void);
		bitboard&			get_enemy_board(void);
		int&				get_enemy_captures(void);
		int&				get_player_captures(void);

		bool				contains(pattern& pat) const;
		inline bool 		operator==(const pattern& rhs) const;
		bool 				operator<(const State& rhs) const;
		bool 				operator>(const State& rhs) const;

		bool				is_win(void);
		bool				count_to_5(int row, int col, int r_delta, int c_delta, int player);
		bool				count_to_5(int row, int col, int player);
		bool				can_capture_to_avoid_defeat(void);
		bitboard			make_illegal_move_board(void);

		State				make_baby_from_coord(int coord);
		State				make_baby_from_coord_no_eval(int coord);
		State				make_baby_from_coord_precalc(int coord, int score_delta);

};

bool		is_illegal(State &s);
#endif