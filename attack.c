#include<stdio.h>
#include"defs.h"

// Directions in which these pieces move
const int KnDir[8]={-8,-19,-21,-12,8,19,21,12};
const int RkDir[4]={-1,-10,1,10};
const int BiDir[4]={-9,-11,11,9};
const int KiDir[8]={-1,-10,1,10,-9,-11,11,9};


/*
 * Check whether the specified sq is being attacked by any piece on the board by the specified side
 * The type of piece is irrelevant here and only TRUE / FALSE is returned on whether our piece is being attacked or not
 */
int SqAttacked(const int sq,const int side,const S_BOARD *pos)
{

	int pce,index,t_sq,dir;
	
	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(CheckBoard(pos));
	
	// pawns
	if(side==WHITE)
	{
		if(pos->pieces[sq-11]==wP||pos->pieces[sq-9]==wP)
		{
			return TRUE;
		}		
	} 
	else
	{
		if(pos->pieces[sq+11]==bP||pos->pieces[sq+9]==bP)
		{
			return TRUE;
		}	
	}
	
	// knights
	for(index=0;index<8;++index)
	{		
		pce=pos->pieces[sq+KnDir[index]];
		if(pce!=OFFBOARD&&IsKn(pce)&&PieceCol[pce]==side)
		{
			return TRUE;
		}
	}
	
	// rooks, queens
	for(index=0;index<4;++index)
	{		
		dir=RkDir[index];
		t_sq=sq+dir;
		pce=pos->pieces[t_sq];
		while(pce!=OFFBOARD)
		{
			if(pce!=EMPTY)
			{
				if(IsRQ(pce)&&PieceCol[pce]==side)
				{
					return TRUE;
				}
				break;
			}
			t_sq+=dir;
			pce=pos->pieces[t_sq];
		}
	}
	
	// bishops, queens
	for(index=0;index<4;++index)
	{		
		dir=BiDir[index];
		t_sq=sq+dir;
		pce=pos->pieces[t_sq];
		while(pce!=OFFBOARD)
		{
			if(pce!=EMPTY)
			{
				if(IsBQ(pce)&&PieceCol[pce]==side)
				{
					return TRUE;
				}
				break;
			}
			t_sq+=dir;
			pce=pos->pieces[t_sq];
		}
	}
	
	// kings
	for(index=0;index<8;++index)
	{		
		pce=pos->pieces[sq+KiDir[index]];
		if(pce!=OFFBOARD&&IsKi(pce)&&PieceCol[pce]==side)
		{
			return TRUE;
		}
	}
	
	return FALSE;
	
}
