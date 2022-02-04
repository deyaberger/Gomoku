#include "defines.hpp"
#include "utils.hpp"
#include "pattern.hpp"

#include <iostream>
#include <iomanip>

bool		is_shiftable_to(pattern &pat, int row, int col)
{
	return is_in_bounds(pat.start_r +  row, pat.start_c + col) and is_in_bounds(pat.end_r +  row, pat.end_c + col);
}


bool 		shift_pattern_to(pattern &pat, int row, int col)
{
	/*
	Shifts the pattern to the desired coordinates and updates pat.r_shift and pat.c_shift
	*/
	row = row - pat.r_delta;
	col = col - pat.c_delta;

	if (not (is_in_bounds(row, col) and is_in_bounds(pat.end_r - pat.start_r + row, pat.end_c - pat.start_c + col)))
		return (false);
	
	int shift = (row - pat.r_shift - pat.start_r) * BOARD_WIDTH + (col - pat.c_shift - pat.start_c);
	// std::cout << "shift: " << shift << std::endl;
	if (shift > 0)
	{
		if (pat.color & WHITE)
			pat.w_bits = pat.w_bits << shift;
		if (pat.color & BLACK)
			pat.b_bits = pat.b_bits << shift;
		if (pat.color & EMPTY)
			pat.e_bits = pat.e_bits << shift;
	}
	else
	{
		if (pat.color & WHITE)
			pat.w_bits = pat.w_bits >> -shift;
		if (pat.color & BLACK)
			pat.b_bits = pat.b_bits >> -shift;
		if (pat.color & EMPTY)
			pat.e_bits = pat.e_bits >> -shift;
	}
	// std::cout << "r_shift: " << pat.r_shift << "row:" std::endl;
	pat.r_shift = row - pat.start_r;
	pat.c_shift = col - pat.start_c;

	return (true);
}


void 		print_pattern(pattern &pat)
{
	std::string symbols[3] = {"O", "X", " "};
	std::cout << "r_shift: " << pat.r_shift << ", c_shift: " <<  pat.c_shift << ", start_r: " << pat.start_r <<  ", start_c: " << pat.start_c << ", end_r: " << pat.end_r << ", end_c: " << pat.end_c << std::endl;
	std::cout << "Colors: ";
	if (pat.color & WHITE)
		std::cout << "WHITE ";
	if (pat.color & BLACK)
		std::cout << "BLACK ";
	if (pat.color & EMPTY)
		std::cout << "EMPTY ";
	std::cout << std::endl;

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
		for (int c = 0; c < BOARD_WIDTH; c++)
		{
			if (pat.b_bits[r * BOARD_WIDTH + c])
				std::cout << "  " << symbols[1] << " ";
			else if (pat.w_bits[r * BOARD_WIDTH + c])
				std::cout << "  " << symbols[0] << " ";
			else
				std::cout << "  " << symbols[2] << " ";
		}
		std::cout << std::setw(4) << r << std::endl;			
	}
	std::cout << std::endl;
}


void 		swap_colors(pattern &pat)
{
	bitboard tmp = pat.b_bits;
	pat.b_bits = pat.w_bits;
	pat.w_bits = tmp;
	int out = 0;
	if (pat.color & WHITE)
		out = out | BLACK;

	if (pat.color & BLACK)
		out = out | WHITE;

	if (pat.color & EMPTY)
		out = out | EMPTY;

	pat.color = out;
}


pattern 	create_capture_pattern(int direction, int player, int variant)
{
	pattern p;
	p.direction = direction;
	if (direction == DOWN)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(3, 0)] = true; 
		p.b_bits[flat_coord(1, 0)] = true; 
		p.b_bits[flat_coord(2, 0)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 3;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = 0 * variant;
	}
	if (direction == RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(0, 3)] = true; 
		p.b_bits[flat_coord(0, 1)] = true; 
		p.b_bits[flat_coord(0, 2)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 0;
		p.end_c = 3;
		p.r_delta = 0 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(3, 3)] = true; 
		p.b_bits[flat_coord(1, 1)] = true; 
		p.b_bits[flat_coord(2, 2)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 3;
		p.end_c = 3;
		p.r_delta = 1 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_LEFT)
	{
		p.w_bits[flat_coord(0, 3)] = true; 
		p.b_bits[flat_coord(1, 2)] = true;
		p.b_bits[flat_coord(2, 1)] = true; 
		p.w_bits[flat_coord(3, 0)] = true;
		p.start_r = 0;
		p.start_c = 3;
		p.end_r = 3;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = -1 * variant;
	}
	p.color = BLACK | WHITE;
	if (player == BLACK)
	{
		swap_colors(p);
	}
	p.size = 4;
	p.r_shift = 0;
	p.c_shift = 0;
	// ! THIS IS BS
	return (p);
}


pattern 	create_pair_pattern(int direction, int player, int variant)
{
	pattern p;
	p.direction = direction;
	if (direction == DOWN)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 0)] = true; 
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 1;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = 0 * variant;
	}
	if (direction == RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(0, 1)] = true; 
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 0;
		p.end_c = 1;
		p.r_delta = 0 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 1)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 1;
		p.end_c = 1;
		p.r_delta = 1 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_LEFT)
	{
		p.w_bits[flat_coord(0, 1)] = true;
		p.w_bits[flat_coord(1, 0)] = true; 
		p.start_r = 0;
		p.start_c = 1;
		p.end_r = 1;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = -1 * variant;
	}
	p.color = WHITE;
	p.size = 2;
	p.r_shift = 0;
	p.c_shift = 0;
	if (player == BLACK)
	{
		swap_colors(p);
	}
	return (p);	
}


pattern 	create_triplet_pattern(int direction, int player, int variant)
{
	pattern p;
	p.direction = direction;
	if (direction == DOWN)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 0)] = true;
		p.w_bits[flat_coord(2, 0)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 2;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = 0 * variant;
	}
	if (direction == RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(0, 1)] = true; 
		p.w_bits[flat_coord(0, 2)] = true; 
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 0;
		p.end_c = 2;
		p.r_delta = 0 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 1)] = true;
		p.w_bits[flat_coord(2, 2)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 2;
		p.end_c = 2;
		p.r_delta = 1 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_LEFT)
	{
		p.w_bits[flat_coord(0, 2)] = true;
		p.w_bits[flat_coord(1, 1)] = true; 
		p.w_bits[flat_coord(2, 0)] = true; 
		p.start_r = 0;
		p.start_c = 2;
		p.end_r = 2;
		p.end_c = 0; 
		p.r_delta = 1 * variant;
		p.c_delta = -1 * variant;
	}
	p.color = WHITE;
	p.size = 3;
	if (player == BLACK)
	{
		swap_colors(p);
	}
	return (p);	
}


pattern 	create_quator_pattern(int direction, int player, int variant)
{
	pattern p;
	p.direction = direction;
	if (direction == DOWN)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 0)] = true;
		p.w_bits[flat_coord(2, 0)] = true;
		p.w_bits[flat_coord(3, 0)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 3;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = 0 * variant;
	}
	if (direction == RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(0, 1)] = true; 
		p.w_bits[flat_coord(0, 2)] = true; 
		p.w_bits[flat_coord(0, 3)] = true; 
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 0;
		p.end_c = 3;
		p.r_delta = 0 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 1)] = true;
		p.w_bits[flat_coord(2, 2)] = true;
		p.w_bits[flat_coord(3, 3)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 3;
		p.end_c = 3;
		p.r_delta = 1 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_LEFT)
	{
		p.w_bits[flat_coord(0, 3)] = true;
		p.w_bits[flat_coord(1, 2)] = true; 
		p.w_bits[flat_coord(2, 1)] = true; 
		p.w_bits[flat_coord(3, 0)] = true; 

		p.start_r = 0;
		p.start_c = 3;
		p.end_r = 3;
		p.end_c = 0; 
		p.r_delta = 1 * variant;
		p.c_delta = -1 * variant;
	}
	p.color = WHITE;
	p.size = 4;
	if (player == BLACK)
	{
		swap_colors(p);
	}
	return (p);	
}


pattern 	create_penta_pattern(int direction, int player, int variant)
{
	pattern p;
	p.direction = direction;
	if (direction == DOWN)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 0)] = true;
		p.w_bits[flat_coord(2, 0)] = true;
		p.w_bits[flat_coord(3, 0)] = true;
		p.w_bits[flat_coord(4, 0)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 4;
		p.end_c = 0;
		p.r_delta = 1 * variant;
		p.c_delta = 0 * variant;
	}
	if (direction == RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(0, 1)] = true; 
		p.w_bits[flat_coord(0, 2)] = true; 
		p.w_bits[flat_coord(0, 3)] = true; 
		p.w_bits[flat_coord(0, 4)] = true; 
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 0;
		p.end_c = 4;
		p.r_delta = 0 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_RIGHT)
	{
		p.w_bits[flat_coord(0, 0)] = true;
		p.w_bits[flat_coord(1, 1)] = true;
		p.w_bits[flat_coord(2, 2)] = true;
		p.w_bits[flat_coord(3, 3)] = true;
		p.w_bits[flat_coord(4, 4)] = true;
		p.start_r = 0;
		p.start_c = 0;
		p.end_r = 4;
		p.end_c = 4; 
		p.r_delta = 1 * variant;
		p.c_delta = 1 * variant;
	}
	if (direction == DOWN_LEFT)
	{
		p.w_bits[flat_coord(0, 4)] = true;
		p.w_bits[flat_coord(1, 3)] = true; 
		p.w_bits[flat_coord(2, 2)] = true; 
		p.w_bits[flat_coord(3, 1)] = true; 
		p.w_bits[flat_coord(4, 0)] = true; 
		p.start_r = 0;
		p.start_c = 4;
		p.end_r = 4;
		p.end_c = 0; 
		p.r_delta = 1 * variant;
		p.c_delta = -1 * variant;
	}
	p.color = WHITE;
	p.size = 5;
	if (player == BLACK)
	{
		swap_colors(p);
	}
	return (p);	
}
