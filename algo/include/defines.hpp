#ifndef DEF_H
#define DEF_H

#include <bitset>


#define BOARD_WIDTH		19
#define BOARD_HEIGHT	19
#define BOARD_SIZE		BOARD_WIDTH * BOARD_HEIGHT

typedef std::bitset<BOARD_SIZE> bitboard;

#define WHITE			1
#define BLACK			2
#define EMPTY			4
#define LIVE			8
#define EDGE			16


#define DOWN			BOARD_WIDTH
#define RIGHT			1
#define DOWN_RIGHT		(DOWN + RIGHT)
#define DOWN_LEFT		(-(DOWN - RIGHT))

#define ILLEGAL			-1
#define BLACK_WIN		(INT32_MIN + 1)
#define WHITE_WIN		(INT32_MAX - 1)
#define PATTERN_MISSING	-1

#define MINMAX_CLASSIC 	1
#define MINMAX_BEAM  	2

#define CAPTURE_VALUE	512
#define TACTICS_LEN		4		// * tactics need to induce a positive score TACTICS_LEN folds further or they will be pruned
#define	SURROUND_SIZE	2		// * SIZE OF THE SQUARE CONSIDERED IN THE EVAL FUNCTION (IN EITHER DIRECTION) eval cpp assumes this is set to 2 in certain lline be carefull
#define	K_BEAM			3

#define POTENTIAL_CAPTURE_VALUE 	(CAPTURE_VALUE / 8)
static int potential_capture_value 	= POTENTIAL_CAPTURE_VALUE;


#define INCREASING_CAPTURE_VALUE
// #define POTENTIAL_CAPTURE
// #define SINGLE_THREAD
// #define SSL
// #define V_TERM



#endif
