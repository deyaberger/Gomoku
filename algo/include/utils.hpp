#ifndef UTILS_H
#define UTILS_H

#include "defines.hpp"

#include <iostream>

inline int	flat_coord(int row, int col)
{
	return (BOARD_WIDTH * row + col);
}

inline int	next_player(int player)
{
	if (player == BLACK)
		return (WHITE);
	return BLACK;
}


int			get_move_keyboard(void);

inline void		print_player(int player)
{
	if (player == BLACK)
		std::cout << "BLACK";
	if (player == WHITE)
		std::cout << "WHITE";
}

inline bool	is_in_bounds(int row, int col)
{
	return (col < BOARD_WIDTH and row < BOARD_HEIGHT and col >= 0 and row >= 0);
}



#endif // !UTILS_H