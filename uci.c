#include<stdio.h>
#include<string.h>
#include"defs.h"
#include<stdlib.h>

#define INPUTBUFFER 400 * 6	// Largest input string 



// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
    
	int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
	char *ptr = NULL;
	info->timeset = FALSE;
	
	if ((ptr = strstr(line,"infinite")))
	{
		;
	} 
	
	if ((ptr = strstr(line,"binc")) && pos->side == BLACK)
	{
		inc = atoi(ptr + 5);
	}
	
	if ((ptr = strstr(line,"winc")) && pos->side == WHITE)
	{
		inc = atoi(ptr + 5);
	} 
	
	if ((ptr = strstr(line,"wtime")) && pos->side == WHITE)
	{
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"btime")) && pos->side == BLACK)
	{
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"movestogo")))
	{
		movestogo = atoi(ptr + 10);
	} 
	  
	if ((ptr = strstr(line,"movetime")))
	{
		movetime = atoi(ptr + 9);
	}
	  
	if ((ptr = strstr(line,"depth")))
	{
		depth = atoi(ptr + 6);
	} 
	
	if(movetime != -1)
	{
		time = movetime;
		movestogo = 1;
	}
	
	info->starttime = GetTimeMs();
	info->depth = depth;
	
	if(time != -1)
	{
		info->timeset = TRUE;
		time /= movestogo;
		time -= 50;		
		info->stoptime = info->starttime + time + inc;
	} 
	
	if(depth == -1)
	{
		info->depth = MAXDEPTH;
	}
	
	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time,info->starttime,info->stoptime,info->depth,info->timeset);
	SearchPosition(pos, info);
}


/*
 * Ways in which you can parse the string input : 
 * position fen
 * position startpos
 * [ moves e2e4 e7e5 b7b8q ]
 */
void ParsePosition(char *lineIn,S_BOARD *pos)
{
	lineIn+=9;	// moving pointer to start of FEN or startpos
	char *ptrChar=lineIn;
	
	if(strncmp(lineIn,"startpos",8)==0)
	{
		ParseFen(START_FEN,pos);
	}
	else
	{
		ptrChar=strstr(lineIn,"fen");
		if(ptrChar==NULL)
		{
			ParseFen(START_FEN,pos);
		}
		else
		{
			ptrChar+=4;
			ParseFen(ptrChar,pos);
		}
	}
	
	ptrChar=strstr(lineIn,"moves");
	int move;
	
	if(ptrChar!=NULL)
	{
		ptrChar+=6;
		while(*ptrChar)
		{
			move=ParseMove(ptrChar,pos);
			if(move==NOMOVE)
				break;
			MakeMove(pos,move);
			pos->ply=0;
			while(*ptrChar && *ptrChar!=' ')
				ptrChar++;
			ptrChar++;
		}
	}
	
	PrintBoard(pos);
	
}


void Uci_Loop()
{
	// setting the buffer off so as to not mess with gui I/O
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);
	
	char line[INPUTBUFFER];		// line to be read in
	
	printf("id name %s\n",NAME);
	printf("id author Bhavzi\n");
	printf("uciok\n");
	
	S_BOARD pos[1];
	S_SEARCHINFO info[1];
	InitPvTable(pos->PvTable);
	
	while(TRUE)
	{
		memset(&line[0],0,sizeof(line));	// clearing the line
		fflush(stdout);
		
		if(!fgets(line,INPUTBUFFER,stdin))	
		{
			continue;
		}
		
		if(line[0]=='\n')
		{
			continue;
		}
		
		if(!strncmp(line,"isready",7))
		{
			printf("readyok\n");
			continue;
		}
		else if (!strncmp(line, "position", 8))
		{
			ParsePosition(line, pos);
		}
		else if(!strncmp(line,"ucinewgame",19))
		{
			ParsePosition("position startpos\n",pos);
		}
		else if(!strncmp(line,"go",2))		// thinking part 
		{
			ParseGo(line,info,pos);
		}
		else if(!strncmp(line,"quit",4))
		{
			info->quit=TRUE;
			break;
		}
		else if(!strncmp(line,"uci",3))
		{
			printf("id name %s\n",NAME);
			printf("id author Bhavzi\n");
			printf("uciok\n");
		}
		if(info->quit)
		{
			break;
		}
		
	}
	
	free(pos->PvTable->pTable);	
	
}

