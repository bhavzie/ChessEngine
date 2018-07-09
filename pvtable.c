#include<stdio.h>
#include"defs.h"


/*
 * The principle variation table holds the best line or best move for a given position,helps in further searches
 */

const int PvSize=0x100000*2;	// 2 megabytes

int GetPvLine(const int depth,S_BOARD *pos)		// Stores in the PVArray and returns count of moves
{
	ASSERT(depth<MAXDEPTH);
	
	int move=ProbePvTable(pos);
	int count=0;
	
	while(move!=NOMOVE&&count<depth)
	{
		ASSERT(count<MAXDEPTH);
		
		if(MoveExists(pos,move))
		{
			MakeMove(pos,move);
			pos->PvArray[count++]=move;
		}
		else		// Illegal Move or reached Max count
		{
			break;
		}
		
		move=ProbePvTable(pos);		// ?
	}
	
	while(pos->ply>0)
	{
		TakeMove(pos);
	}
	return count;
}

void ClearPvTable(S_PVTABLE *table)
{
	S_PVENTRY *pvEntry;
	
	for(pvEntry=table->pTable;pvEntry<table->pTable+table->numEntries;pvEntry++)
	{
		pvEntry->posKey=0ULL;
		pvEntry->move=NOMOVE;
	}
}

void InitPvTable(S_PVTABLE *table)
{
	table->numEntries=PvSize/sizeof(S_PVENTRY);
	table->numEntries-=2;
	
	if((table->pTable)!=NULL)
	{
		free(table->pTable);
	}
	table->pTable=(S_PVENTRY*)malloc(table->numEntries*sizeof(S_PVENTRY));
	ClearPvTable(table);
	printf("PvTable init complete with %d entries\n",table->numEntries);
	
}

void StorePvMove(const S_BOARD *pos,const int move)	// Store a move in the PV table
{
	int index=pos->posKey%pos->PvTable->numEntries;		
	ASSERT(index>=0&&index<=pos->PvTable->numEntries-1);
	
	pos->PvTable->pTable[index].move=move;
	pos->PvTable->pTable[index].posKey=pos->posKey;
	
}

int ProbePvTable(const S_BOARD *pos)	// Retrieve the move
{
	int index=pos->posKey%pos->PvTable->numEntries;		
	ASSERT(index>=0&&index<=pos->PvTable->numEntries-1);
	
	if(pos->PvTable->pTable[index].posKey==pos->posKey)
	{
		return pos->PvTable->pTable[index].move;
	}
	
	return NOMOVE;
	
}




