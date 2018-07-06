  /* Search functioning :
  * 
  * int search(depth)		// Search Function
  * {
  * 	int currDepth=1;
  * 	iterativeDeepen(depth)		// Move Ordering to optimize alphabeta 
  * 	{
  *		#This is where we store a PV(Principle variation) which is the optimum line till a depth X
  *		#So next time you can compare with this optimum line directly rather than searching throughout
  *		#Store historyTables,alphaCutoffMoves,BetaCutoffMoves,KillerMoves 
  * 
  *		alphabeta();
  * 		currDepth++;
  * 	}
  * }
  * 
  * 
  * int alphabeta()
  * {
  * 	IsRepeated()	return 0;	// If move is repeated don't need to search the same thing again
  * 	IsInterrupt()	{ SetQuit() ; return 0; } // If say you need to stop the search if there is an overflow / quit being called
  * 
  *	GenMoves();	
  * 	ProbePvMove();		//Store This in a HashTable
  * 	if(have PvMove)	OrderPvMove();		// Ordering pvMove is imp
  * 	
  * 	LoopMoves()
  * 	{
  *		MakeMove();
  *		alphabeta(depth-1);
  *		Take();
  *		isBetaCut()	-> return beta;
  *		isAlpha() { note bestMove,increase alpha };
  * 	}
  * 
  * 	NoLegalMovesMade() { if InCheck -> return Mate ; else return 0; }	// No move possible and already in check
  * 	if(Alpha improved)	{ store PvMove };
  * 	return alpha;
  * }
  * 
  */
 
#include<stdio.h>
#include"defs.h"

int IsRepetition(const S_BOARD *pos)
{
	int index=0;
	
	for(index=pos->hisPly-pos->fiftyMove;index<pos->hisPly-1;++index)
	{
		ASSERT(index>=0&&index<MAXGAMEMOVES);
		
		if(pos->posKey==pos->history[index].posKey)
			return TRUE;
	}
	return FALSE;
}

void SearchPosition(S_BOARD *pos)
{
	
}
  
