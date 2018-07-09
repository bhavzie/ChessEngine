#include<stdio.h>
#include<stdlib.h>
#include"defs.h"


/*
 * The main engine or the main file
 */

#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

int main()
{	

	AllInit();		
	
	S_BOARD board[1];
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
			info->depth=4;
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
	return 0;		

	
}

