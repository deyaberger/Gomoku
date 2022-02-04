#include "defines.hpp"
#include <iostream>




int			get_move_keyboard(void)
{
	int r;
	int c;
	std::cout << "Enter r: "; // no flush needed
	std::cin >> r;
	std::cout << "Enter c: "; // no flush needed
	std::cin >> c;

	return (r * BOARD_WIDTH + c);
}



