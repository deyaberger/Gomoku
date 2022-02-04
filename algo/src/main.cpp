#include <bitset>
#include <iostream>
#include <iomanip>

#include "defines.hpp"
#include "utils.hpp"
#include "state.hpp"
#include "eval.hpp"
#include "pattern.hpp"

#include "minimax.hpp"
#include "server.hpp"

#include <chrono>
#include <deque>

// #include "thread_pool.hpp"
// typedef std::chrono::steady_clock::time_point timepoint;


void	print_live_board_size(State &s)
{
	std::cout << "Live board size: " << s.live_board.count() << std::endl;
}

State play_game(int depth, bool cpu1, bool cpu2, int limit = 10, State s = State())
{
	// thread_pool	pool(std::thread::hardware_concurrency() - 1);
	s.coord_evaluation_function = eval_surround_square;
	int folds = 0;
	int move;
	if (cpu1)
		s.live_board.set(9 * BOARD_WIDTH + 9, true);
	while(true)
	{
		if (s.player == WHITE)
		{
			if (s.game_win)
			{
				std::cout << "BLACK WON" << std::endl;
				return s;
			}
			std::cout << "White to play" << std::endl;
			if (cpu1)
			{
				print_live_board_size(s);
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				move = minimax_single_fred(s, depth);
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000 << "." << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() % 1000000) / 100000 << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() % 100000) / 10000 <<  "s" << std::endl;
				std::setfill(' ');
			}
			else
			{
				move = get_move_keyboard();				
			}
		}
		else
		{
			if (s.game_win)
			{
				std::cout << "WHITE WON" << std::endl;
				return s;
			}
			std::cout << "Black to play" << std::endl;
			if (cpu2)
			{
				print_live_board_size(s);
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				move = minimax_single_fred(s, depth);

				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000 << "." << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() % 1000000) / 100000 << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() % 100000) / 10000 <<  "s" << std::endl;
				
				std::setfill(' ');
			}
			else
			{
				move = get_move_keyboard();				
			}
		}
		std::cout << "Move: " << move /  BOARD_WIDTH << ", " << move % BOARD_WIDTH  << std::endl;
		if (move == -12)
		{
			std::cout << "NO MORE MOVES" << std::endl;
			return s;
		}
		s = s.make_baby_from_coord(move);
		std::cout << std::endl;

		std::cout << "Fold: " << folds << std::endl;
		// s.print();
		// s.print_score_board();

		folds += 1;
		if (folds >= limit and cpu1 and cpu2)
		{
			s.print();
			// s.print_score_board();
			break;
		}
	}
	return s;
}


// #include "thread_pool.hpp"


int main()
{
	// thread_pool pool(std::thread::hardware_concurrency() - 1);
	// std::cout << pool.get_thread_count() << std::endl;
	// std::future<int> fut2 = pool.submit(lolilol, 2);

	// fut2.wait_for(std::chrono::seconds(1));

	run_websocket_server("0.0.0.0", 16784);
	play_game(7, true, true, 20);
	// play_game_beam(7, true, true, 15);

	// State s;
	// s.coord_evaluation_function = eval_surround_square;
	// s = s.make_baby_from_coord(flat_coord(9, 9));
	// // s = s.make_baby_from_coord(flat_coord(15, 10));
	// // s = s.make_baby_from_coord(flat_coord(4, 9));
	// // // s = s.make_baby_from_coord(flat_coord(8, 7));

	// std::cout << "classic: " << minimax_fred(s, 6) << std::endl;
	// std::cout << "ref:     " << minimax_fred_start(s, 7, true)  << std::endl;
	// std::cout << "bro:     " << minimax_fred_start_brother(s, 7)  << std::endl;
	// s = s.make_baby_from_coord(flat_coord(9, 9));
	// s = s.make_baby_from_coord(flat_coord(8, 9));
	// s = s.make_baby_from_coord(flat_coord(8, 8));
	// s = s.make_baby_from_coord(flat_coord(7, 7));
	// s = s.make_baby_from_coord(flat_coord(10, 10));
	// s = s.make_baby_from_coord(flat_coord(6, 7));
	// s = s.make_baby_from_coord(flat_coord(5, 7));
	// s = s.make_baby_from_coord(flat_coord(7, 8));
	// s = s.make_baby_from_coord(flat_coord(11, 11));
	// s = s.make_baby_from_coord(flat_coord(12, 12));
	// s = s.make_baby_from_coord(flat_coord(7, 6));
	// s = s.make_baby_from_coord(flat_coord(9, 10));
	// s.print();
	// s = s.make_baby_from_coord(flat_coord(5, 6));
	// s.print();
	// std::cout << "win: " << s.game_win << " last chance: " << s.last_chance << " last chance move: " << s.last_chance_move << std::endl;
	// s = s.make_baby_from_coord(flat_coord(10, 11));
	// s.print();
	// std::cout << "win: " << s.game_win << " last chance: " << s.last_chance << " last chance move: " << s.last_chance_move << std::endl;
	// s = s.make_baby_from_coord(flat_coord(11, 12));
	// s.print();
	// // s = s.make_baby_from_coord(flat_coord(15, 15));
	// // s.print();

	// std::cout << "win: " << s.game_win << " last chance: " << s.last_chance << " last chance move: " << s.last_chance_move <<std::endl;
	// run_websocket_server("0.0.0.0", 16784);
}