#ifndef EVAL_H
#define EVAL_H

#include "defines.hpp"
#include "pattern.hpp"
#include "state.hpp"

int		eval_surround_square(State &state, int coord);
int			count_free_threes(const State &state, int coord);


typedef int (*evaluation_function)(State&);
typedef int (*cood_evaluation_function)(State&, int coord);



#endif