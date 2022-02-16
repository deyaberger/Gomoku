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
	if (cpu2)
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
				move = minimax_fred_start_brother(s, depth);
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
				move = minimax_fred_start_brother(s, depth);

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
		s.print();
		// s.print_score_board();

		folds += 1;
		if (folds >= limit and cpu1 and cpu2)
		{
			s.print();
			s.print_score_board();
			break;
		}
	}
	return s;
}


// #include "thread_pool.hpp"

#include "test.hpp"
int main()
{
	// thread_pool pool(std::thread::hardware_concurrency() - 1);
	// std::cout << pool.get_thread_count() << std::endl;
	// std::future<int> fut2 = pool.submit(lolilol, 2);

	// fut2.wait_for(std::chrono::seconds(1));

	// play_game(10, true, true, 100);
	// runino();
	run_websocket_server("0.0.0.0", 16784);
}