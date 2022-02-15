#ifndef MINIMAX_H
#define MINIMAX_H

#include "defines.hpp"
#include "pattern.hpp"
#include "state.hpp"
#include "eval.hpp"
#include "thread_pool.hpp"

#include <deque>

int		minimax_single_fred(State state, int limit, std::deque<int> past_scores = std::deque<int>(),  int depth = 0, int alpha = BLACK_WIN, int beta =  WHITE_WIN, int *out_eval = nullptr);
int		minimax_beam(State state, int limit, int depth = 0, int alpha = BLACK_WIN, int beta =  WHITE_WIN);

// int		minimax_no_len(State state, int limit,  int depth = 0, int alpha = BLACK_WIN, int beta =  WHITE_WIN);

int		minimax_fred_start(thread_pool& pool, State state, int limit, std::deque<int> past_scores = std::deque<int>(), bool return_eval = false);
// int		minimax_fred_start_brother(State state, int limit);
int		minimax_fred_start_brother(State state, int limit, int *out_eval = nullptr);
int		minimax_fred_start_brother_k_beam(State state, int limit, int *out_eval = nullptr);
int		minimax_fred(State state, int limit, std::deque<int> past_scores = std::deque<int>(),  int depth = 1, int alpha = BLACK_WIN, int beta =  WHITE_WIN);


#endif // !MINIMAX_H