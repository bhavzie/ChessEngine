#include<stdio.h>
#include<stdlib.h>
#include"defs.h"


/*
 * The main engine or the main file
 */

#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define PERFT "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main()
{	

	AllInit();		
	
	S_BOARD board[1];
	InitPvTable(board->PvTable);
	S_MOVELIST list[1];
	S_SEARCHINFO info[1];
	int Max=0;
	int PvNum=0;
	
	ParseFen(WAC1,board);
	
	char input[6];	
	int Move=NOMOVE;
	
	while(TRUE)
	{
		PrintBoard(board);
		printf("Please enter a move > ");
		fgets(input,6,stdin);
		
		if(input[0]=='q')
		{
			break;
		}
		else if(input[0]=='t')
		{
			TakeMove(board);
		}
		else if(input[0]=='s')
		{
			info->depth=6;
			SearchPosition(board,info);
		}
		else
		{
			Move=ParseMove(input,board);
			if(Move!=NOMOVE)
			{
				StorePvMove(board,Move);
				MakeMove(board,Move);
				//if(IsRepetition(board))
					//printf("REP SEEN\n");
				
			}
			else
			{
				printf("Move not Parsed:%s\n",input);
			}
		}		
		fflush(stdin);
	}
	
	free(board->PvTable->pTable);
	
	return 0;		

	
}

