#include<stdio.h>
#include"defs.h"
#include<stdlib.h>

#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
int main()
{

	AllInit();	
	
	S_BOARD board[1];
	ParseFen(FEN4,board);
	PrintBoard(board);
	ASSERT(CheckBoard(board));
	
	
	
	
	return 0;
}
