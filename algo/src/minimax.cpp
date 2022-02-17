#include "minimax.hpp"
#include "utils.hpp"
#include "eval.hpp"

#include <iostream>

#include <vector>
#include <array>

#include <algorithm>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include "thread_pool.hpp"



bool	compare_score(const std::pair<int, int>& s1, const std::pair<int, int>& s2)
{
	return (s1.first > s2.first);
}

bool	compare_score_reverse(const std::pair<int, int>& s1, const std::pair<int, int>& s2)
{
	return (s1.first < s2.first);
}

int		init_past_score(std::deque<int> &past_scores, int state_score, bool maximizer)
{
	past_scores.push_front(state_score);
	int	start_score;
	if (maximizer)
		start_score = INT32_MIN;
	else
		start_score = INT32_MAX;

	if (past_scores.size() > TACTICS_LEN)
	{
	 	start_score = past_scores.back();
		past_scores.pop_back();
	}
	return start_score;
}

void	fill_baby_tables(std::pair<int, int>* babies , State* babie_states, State &state, int &counter)
{
	for (int c = 0; c < BOARD_SIZE; c++)
	{
		if (state.live_board.test(c))
		{
			babie_states[counter] = state.make_baby_from_coord(c);
			babies[counter] = std::pair<int, int>(babie_states[counter].score, counter);
			counter += 1;
			if (counter == 200)
				break;
		}
	}
}



int		minimax_single_fred(State state, int limit, std::deque<int> past_scores, int depth, int alpha, int beta, int *out_eval)
{
	bool				maximizer = (state.player == WHITE);
	int 				best_move = -12;
	int					counter   = 0;
	int 				eval;
	int					start_score;
	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];
	int					bestEval;


	if (state.free_threes == 2)
		return ILLEGAL;
	if (state.game_win)
		return state.score;
	if (depth == limit)
		return (state.score);

	start_score = init_past_score(past_scores, state.score, maximizer);
	if (past_scores.size() == TACTICS_LEN and ((maximizer && (start_score > state.score)) || ((!maximizer) && (start_score < state.score))))
		return state.score;

	fill_baby_tables(babies, babie_states, state, counter);

	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_single_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				alpha = std::max(alpha, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		if (depth == 0)
		{
			if (out_eval != nullptr)
				*out_eval = bestEval;
			return best_move;
		}
		else
			return (bestEval);
	}
	else
	{
		bestEval = INT32_MAX;

		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_single_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				beta = std::min(beta, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
	}
	if (depth == 0)
	{
		if (out_eval != nullptr)
			*out_eval = bestEval;
		return best_move;
	}
	else
		return (bestEval);
}

#define BEAM_K 7
int		minimax_beam(State state, int limit, int depth, int alpha, int beta)
{
	bool	maximizer = (state.player == WHITE);

	if (state.free_threes == 2)
	{
		return ILLEGAL;
	}

	if (state.game_win)
	{
		return state.score;
	}
	if (depth == limit)
		return (state.score);

	int eval;
	int best_move = -12;

	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];

	int counter = 0;
	// int	skipper = 0;
	if (maximizer)
	{
		int maxEval = INT32_MIN;
		for (int c = 0; c < BOARD_SIZE; c++)
		{
			if (state.live_board.test(c))
			{
				babie_states[counter] = state.make_baby_from_coord(c);
				babies[counter] = std::pair<int, int>(babie_states[counter].score, counter);
				counter += 1;
				if (counter == 200)
					break;
			}
			std::sort(babies, babies + counter, compare_score);
		}
		for(int i = 0; i < BEAM_K; i++)
		{
			eval = minimax_beam(babie_states[babies[i].second], limit, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > maxEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					maxEval = eval;
				}
				alpha = std::max(alpha, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		if (depth == 0)
		{
			return best_move;
		}
		else
			return (maxEval);
	}
	else
	{
		int minEval = INT32_MAX;
		for (int c = 0; c < BOARD_SIZE; c++)
		{
			if (state.live_board.test(c))
			{
				babie_states[counter] = state.make_baby_from_coord(c);
				babies[counter] = std::pair<int, int>(babie_states[counter].score, counter);
				counter += 1;
				if (counter == 200)
					break;
			}
			std::sort(babies, babies + counter, compare_score_reverse); 

		}
		for(int i = 0; i < BEAM_K; i++)
		{
			eval = minimax_beam(babie_states[babies[i].second], limit, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < minEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					minEval = eval;
				}
				beta = std::min(beta, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		if (depth == 0)
		{
			return best_move;
		}
		else
			return (minEval);
	}
}




// std::mutex 			ab_mutex;
static std::shared_mutex 	ab_mutex;

inline int read_alpha_beta(int &ab)
{
	int	out;
	ab_mutex.lock_shared();
	out = ab;
	ab_mutex.unlock_shared();
	return out;
}

inline void update_alpha_beta(int &ab, int new_ab)
{
	ab_mutex.lock();
	ab = new_ab;
	ab_mutex.unlock();
	return;
}

inline void update_alpha_if_needed(int &alpha, int new_ab)
{
	if (read_alpha_beta(alpha) < new_ab)
	{
		// std::cout << "updato" << std::endl;
		update_alpha_beta(alpha, new_ab);
	}
}

inline void update_beta_if_needed(int &beta, int new_ab)
{
	if (read_alpha_beta(beta) > new_ab)
	{
		// std::cout << "updato" << std::endl;
		update_alpha_beta(beta, new_ab);
	}
}




int			minimax_fred(State state, int limit, std::deque<int> past_scores, int depth, int alpha, int beta)
{
	bool				maximizer		= (state.player == WHITE);
	int 				best_move 		= -12;
	int 				counter  		= 0;
	int 				eval;
	int 				start_score 	= init_past_score(past_scores, state.score, maximizer);
	int					bestEval;
	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];

	if (state.last_chance)
		limit += 1;
	if (state.free_threes == 2)
		return ILLEGAL;
	if (state.game_win)
		return state.score;
	if (depth == limit)
		return (state.score);
	if (past_scores.size() == TACTICS_LEN and ((maximizer && (start_score > state.score)) || ((!maximizer) && (start_score < state.score))))
		return state.score;

	fill_baby_tables(babies, babie_states, state, counter);



	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				alpha = std::max(alpha, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				beta = std::min(beta, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
	}
	if (depth == 0)
	{
		return best_move;
	}
	else
	{
		return (bestEval);
	}
}


int			minimax_fred_root(State state, int limit, std::deque<int> past_scores, int depth, int &root_alpha, int &root_beta)
{
	int					alpha			= root_alpha;
	int					beta			= root_beta;
	bool				maximizer		= (state.player == WHITE);
	int 				best_move 		= -12;
	int 				counter  		= 0;
	int 				eval;
	int 				start_score 	= init_past_score(past_scores, state.score, maximizer);
	int					bestEval;
	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];

	// std::cout << "alpha beta" << alpha << ", " << beta << std::endl;

	if (state.free_threes == 2)
		return ILLEGAL;
	if (state.game_win)
		return state.score;
	if (depth == limit)
		return (state.score);
	if (past_scores.size() == TACTICS_LEN and ((maximizer && (start_score > state.score)) || ((!maximizer) && (start_score < state.score))))
		return state.score;

	fill_baby_tables(babies, babie_states, state, counter);

	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				alpha = std::max(alpha, eval);
				beta  = std::min(read_alpha_beta(root_beta), beta);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		update_beta_if_needed(root_beta, bestEval);
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			eval = minimax_fred(babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				beta = std::min(beta, eval);
				alpha = std::min(read_alpha_beta(root_alpha), alpha);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		update_alpha_if_needed(root_alpha, bestEval);
	}
	if (depth == 0)
	{
		return best_move - (maximizer ? 1 : -1);
	}
	else
	{
		return (bestEval);
	}
}


int			minimax_fred_start_brother(State state, int limit, int *out_eval)
{
	int 							depth 	= 0;
	int 							alpha 	= BLACK_WIN;
	int 							beta 	= WHITE_WIN;
	bool							maximizer = (state.player == WHITE);
	int 							eval;
	int 							best_move = -12;
	int								counter = 0;
	bool							first 	= true;
	int								bestEval;
	thread_pool 					pool(11);
	std::queue<std::future<int>> 	fut_queue;
	std::queue<int>				 	move_queue;
	std::deque<int> 				past_scores;
	State							babie_states[200];
	std::pair<int, int>				babies[200]; 	// <Score, state_index>
	int								start_score = init_past_score(past_scores, state.score, maximizer);


	// std::cout << "Started thread pool with: " << pool.get_thread_count() << " threads." << std::endl;
	fill_baby_tables(babies, babie_states, state, counter);


	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			if (first)
			{
				eval = minimax_fred_start(pool, babie_states[babies[i].second], limit - 1, past_scores, true);
				if (eval != ILLEGAL)
				{
					first = false;
					if (eval > bestEval)
					{
						best_move = babie_states[babies[i].second].last_move;
						bestEval = eval;
					}
					alpha = std::max(alpha, eval);
					if (beta <= alpha)
					{
						break;
					}
				}
			}
			else
			{
				fut_queue.push(pool.submit(minimax_fred_root, babie_states[babies[i].second], limit, past_scores, depth + 1, std::ref(alpha), std::ref(beta)));
				// fut_queue.push(pool.submit(minimax_fred, babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta));
				move_queue.push(babie_states[babies[i].second].last_move);
			}
		}
		pool.wait_for_tasks();
		while(! fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			if (first)
			{
				eval = minimax_fred_start(pool, babie_states[babies[i].second], limit - 1, past_scores, true);
				if (eval != ILLEGAL)
				{
					first = false;
					if (eval < bestEval)
					{
						best_move = babie_states[babies[i].second].last_move;
						bestEval = eval;
					}
					beta = std::min(beta, eval);
					if (beta <= alpha)
					{
						break;
					}
				}
			}
			else
			{
				fut_queue.push(pool.submit(minimax_fred_root, babie_states[babies[i].second], limit, past_scores, depth + 1, std::ref(alpha), std::ref(beta)));
				// fut_queue.push(pool.submit(minimax_fred, babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta));
				move_queue.push(babie_states[babies[i].second].last_move);
			}
		}
		pool.wait_for_tasks();
		while(!fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	if (depth == 0)
	{
		// std::cout << "best eval bro: " << bestEval << std::endl;
		if (out_eval != nullptr)
			*out_eval = bestEval;
		return (best_move);
	}
	else
	{
		return (bestEval);
	}
}


int			minimax_fred_start(thread_pool& pool, State state, int limit, std::deque<int> past_scores, bool return_eval)
{
	int 							alpha 		= BLACK_WIN;
	int 							beta 		= WHITE_WIN;
	bool							maximizer 	= (state.player == WHITE);
	int 							eval;
	int 							best_move 	= -12;
	int								counter 	= 0;
	bool							first 		= false;
	int								bestEval;
	// thread_pool 					pool(std::thread::hardware_concurrency() - 1);
	std::queue<std::future<int>> 	fut_queue;
	std::queue<int>				 	move_queue;
	// std::deque<int> 				past_scores;
	State							babie_states[200];
	std::pair<int, int>				babies[200]; 	// <Score, state_index>
	int								start_score = init_past_score(past_scores, state.score, maximizer);

	// std::cout << "Started thread pool with: " << pool.get_thread_count() << " threads." << std::endl;
	fill_baby_tables(babies, babie_states, state, counter);


	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			fut_queue.push(pool.submit(minimax_fred_root, babie_states[babies[i].second], limit, past_scores, 1, std::ref(alpha), std::ref(beta)));
			move_queue.push(babie_states[babies[i].second].last_move);
		}
		pool.wait_for_tasks();
		while(! fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			fut_queue.push(pool.submit(minimax_fred_root, babie_states[babies[i].second], limit, past_scores,  1, std::ref(alpha), std::ref(beta)));
			move_queue.push(babie_states[babies[i].second].last_move);
		}
		pool.wait_for_tasks();
		while(!fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	if (return_eval)
	{
		return (bestEval);
	}
	else
	{
		return (best_move);
	}
}




int			minimax_fred_k_beam(State state, int limit, int depth, int alpha, int beta)
{
	bool				maximizer		= (state.player == WHITE);
	int 				best_move 		= -12;
	int 				counter  		= 0;
	int 				eval;
	int					bestEval;
	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];


	if (state.free_threes == 2)
		return ILLEGAL;
	if (state.game_win)
		return state.score;
	if (depth == limit)
		return (state.score);

	fill_baby_tables(babies, babie_states, state, counter);


	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			eval = minimax_fred_k_beam(babie_states[babies[i].second], limit, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				alpha = std::max(alpha, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			eval = minimax_fred_k_beam(babie_states[babies[i].second], limit, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				beta = std::min(beta, eval);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
	}
	if (depth == 0)
	{
		return best_move;
	}
	else
	{
		return (bestEval);
	}
}


int			minimax_fred_root_k_beam(State state, int limit,  int depth, int &root_alpha, int &root_beta)
{
	int					alpha			= root_alpha;
	int					beta			= root_beta;
	bool				maximizer		= (state.player == WHITE);
	int 				best_move 		= -12;
	int 				counter  		= 0;
	int 				eval;
	int					bestEval;
	std::pair<int, int>	babies[200]; 		// <Score, state_index>
	State				babie_states[200];

	// std::cout << "alpha beta" << alpha << ", " << beta << std::endl;

	if (state.free_threes == 2)
		return ILLEGAL;
	if (state.game_win)
		return state.score;
	if (depth == limit)
		return (state.score);


	fill_baby_tables(babies, babie_states, state, counter);

	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			eval = minimax_fred_k_beam(babie_states[babies[i].second], limit, depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				alpha = std::max(alpha, eval);
				beta  = std::min(read_alpha_beta(root_beta), beta);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		update_beta_if_needed(root_beta, bestEval);
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			eval = minimax_fred_k_beam(babie_states[babies[i].second], limit,  depth + 1, alpha, beta);
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = babie_states[babies[i].second].last_move;
					bestEval = eval;
				}
				beta = std::min(beta, eval);
				alpha = std::min(read_alpha_beta(root_alpha), alpha);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		update_alpha_if_needed(root_alpha, bestEval);
	}
	if (depth == 0)
	{
		return best_move;
	}
	else
	{
		return (bestEval);
	}
}


int			minimax_fred_start_k_beam(thread_pool& pool, State state, int limit, bool return_eval)
{
	int 							alpha 		= BLACK_WIN;
	int 							beta 		= WHITE_WIN;
	bool							maximizer 	= (state.player == WHITE);
	int 							eval;
	int 							best_move 	= -12;
	int								counter 	= 0;
	bool							first 		= false;
	int								bestEval;
	// thread_pool 					pool(std::thread::hardware_concurrency() - 1);
	std::queue<std::future<int>> 	fut_queue;
	std::queue<int>				 	move_queue;
	// std::deque<int> 				past_scores;
	State							babie_states[200];
	std::pair<int, int>				babies[200]; 	// <Score, state_index>

	// std::cout << "Started thread pool with: " << pool.get_thread_count() << " threads." << std::endl;
	fill_baby_tables(babies, babie_states, state, counter);


	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			fut_queue.push(pool.submit(minimax_fred_root_k_beam, babie_states[babies[i].second], limit, 1, std::ref(alpha), std::ref(beta)));
			move_queue.push(babie_states[babies[i].second].last_move);
		}
		pool.wait_for_tasks();
		while(! fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			fut_queue.push(pool.submit(minimax_fred_root_k_beam, babie_states[babies[i].second], limit,  1, std::ref(alpha), std::ref(beta)));
			move_queue.push(babie_states[babies[i].second].last_move);
		}
		pool.wait_for_tasks();
		while(!fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	if (return_eval)
	{
		return (bestEval);
	}
	else
	{
		return (best_move);
	}
}


int			minimax_fred_start_brother_k_beam(State state, int limit, int *out_eval)
{
	int 							depth 	= 0;
	int 							alpha 	= BLACK_WIN;
	int 							beta 	= WHITE_WIN;
	bool							maximizer = (state.player == WHITE);
	int 							eval;
	int 							best_move = -12;
	int								counter = 0;
	bool							first 	= true;
	int								bestEval;
	thread_pool 					pool(11);
	std::queue<std::future<int>> 	fut_queue;
	std::queue<int>				 	move_queue;
	State							babie_states[200];
	std::pair<int, int>				babies[200]; 	// <Score, state_index>


	// std::cout << "Started thread pool with: " << pool.get_thread_count() << " threads." << std::endl;
	fill_baby_tables(babies, babie_states, state, counter);


	if (maximizer)
	{
		bestEval = INT32_MIN;
		std::sort(babies, babies + counter, compare_score);
		for(int i = 0; i < counter; i++)
		{
			if (i == K_BEAM)
				break;
			if (first)
			{
				eval = minimax_fred_start_k_beam(pool, babie_states[babies[i].second], limit - 1, true);
				if (eval != ILLEGAL)
				{
					first = false;
					if (eval > bestEval)
					{
						best_move = babie_states[babies[i].second].last_move;
						bestEval = eval;
					}
					alpha = std::max(alpha, eval);
					if (beta <= alpha)
					{
						break;
					}
				}
			}
			else
			{
				fut_queue.push(pool.submit(minimax_fred_root_k_beam, babie_states[babies[i].second], limit,  depth + 1, std::ref(alpha), std::ref(beta)));
				// fut_queue.push(pool.submit(minimax_fred, babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta));
				move_queue.push(babie_states[babies[i].second].last_move);
			}
		}
		pool.wait_for_tasks();
		while(! fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval > bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	else
	{
		bestEval = INT32_MAX;
		std::sort(babies, babies + counter, compare_score_reverse); 
		for(int i = 0; i < counter; i++)
		{
			if (first)
			{
				eval = minimax_fred_start_k_beam(pool, babie_states[babies[i].second], limit - 1,  true);
				if (eval != ILLEGAL)
				{
					first = false;
					if (eval < bestEval)
					{
						best_move = babie_states[babies[i].second].last_move;
						bestEval = eval;
					}
					beta = std::min(beta, eval);
					if (beta <= alpha)
					{
						break;
					}
				}
			}
			else
			{
				fut_queue.push(pool.submit(minimax_fred_root_k_beam, babie_states[babies[i].second], limit,  depth + 1, std::ref(alpha), std::ref(beta)));
				// fut_queue.push(pool.submit(minimax_fred, babie_states[babies[i].second], limit, past_scores, depth + 1, alpha, beta));
				move_queue.push(babie_states[babies[i].second].last_move);
			}
		}
		pool.wait_for_tasks();
		while(!fut_queue.empty())
		{
			eval = fut_queue.front().get();
			int tmp_move = move_queue.front();
			move_queue.pop();
			fut_queue.pop();
			if (eval != ILLEGAL)
			{
				if (eval < bestEval)
				{
					best_move = tmp_move;
					bestEval = eval;
				}
			}
		}
	}
	if (depth == 0)
	{
		// std::cout << "best eval bro: " << bestEval << std::endl;
		if (out_eval != nullptr)
			*out_eval = bestEval;
		return (best_move);
	}
	else
	{
		return (bestEval);
	}
}

/**
 * @brief launches the single or multi threaded minmax determined by #SINGLE_THREAD
 * 
 * @param state root state
 * @param limit max depth
 * @param type MINMAX_CLASSIC or MINMAX_BEAM the type of algorithm
 * @return std::pair<int, int> = <best move, eval>
 */
std::pair<int, int> minimax_starter(State state, int limit, int type)
{
	std::pair<int, int> ret; // move, eval
	if (type == MINMAX_CLASSIC)
	{
		#ifdef SINGLE_TREAD
			ret.first = minimax_single_fred(state, limit, std::deque<int>(), 0, BLACK_WIN, WHITE_WIN, &ret.second);
		#else
			ret.first = minimax_fred_start_brother(state, limit, &ret.second);
		#endif
	}
	else
	{
		ret.first = minimax_fred_start_brother_k_beam(state, limit, &ret.second);
	}
	return ret;
}