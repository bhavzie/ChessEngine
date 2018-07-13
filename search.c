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

#define INFINITE 30000
#define MATE 29000

static void CheckUp()	// Check if time up,or interrupt from GUI
{

	
}

static void PickNextMove(int moveNum,S_MOVELIST *list)		// Move Ordering
{
	S_MOVE temp;
	int index=0;
	int bestScore=0;
	int bestNum=moveNum;
	
	for(index=moveNum;index<list->count;++index)
	{
		if(list->moves[index].score>bestScore)
		{
			bestScore=list->moves[index].score;
			bestNum=index;
		}
	}
	
	temp=list->moves[moveNum];
	list->moves[moveNum]=list->moves[bestNum];
	list->moves[bestNum]=temp;
}

static int IsRepetition(const S_BOARD *pos)
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

static void ClearForSearch(S_BOARD *pos,S_SEARCHINFO *info)	// Clear the history arrays xyz ready for new search
{
	int index=0;
	int index2=0;
	
	for(index=0;index<13;++index)
	{
		for(index2=0;index2<BRD_SQ_NUM;++index2)
		{
			pos->searchHistory[index][index2]=0;
		}
	}
	
	for(index=0;index<13;++index)
	{
		for(index2=0;index2<BRD_SQ_NUM;++index2)
		{
			pos->searchKillers[index][index2]=0;
		}
	}
	
	ClearPvTable(pos->PvTable);
	pos->ply=0;
	
	info->starttime=GetTimeMs();
	info->stopped=0;
	info->nodes=0;	
	info->fh=0;
	info->fhf=0;
}

static int Quiescence(int alpha,int beta,S_BOARD *pos,S_SEARCHINFO *info)	// Improves alphabeta search check wiki
{
	
	ASSERT(CheckBoard(pos));
	info->nodes++;
	
	if(IsRepetition(pos)||pos->fiftyMove>=100)	// draw
	{
		return 0;
	}
	
	if(pos->ply>MAXDEPTH-1)		// reached max depth so just end search
	{
		return EvalPosition(pos);
	}
	
	int Score=EvalPosition(pos);
	
	// Comparing with evalpos() even before making a new move
	if(Score>=beta)
	{
		return beta;
	}
	if(Score>=alpha)
	{
		alpha=Score;
	}
	
	S_MOVELIST list[1];
	GenerateAllCaps(pos,list);
	
	int MoveNum=0;
	int Legal=0;	// say we do not find legal moves checkmate
	int OldAlpha=alpha;
	int BestMove=NOMOVE;
	Score=-INFINITE;
	int PvMove=ProbePvTable(pos);
	
	for(MoveNum=0;MoveNum<list->count;++MoveNum)
	{
		PickNextMove(MoveNum,list);
		
		if(!MakeMove(pos,list->moves[MoveNum].move))
		{
			continue;
		}
		Legal++;
		Score=-Quiescence(-beta,-alpha,pos,info);
		TakeMove(pos);
		
		if(Score>alpha)
		{
			if(Score>=beta)
			{
				if(Legal==1)
				{
					info->fhf++;
				}
				info->fh++;
				return beta;
			}
			
			alpha=Score;
			BestMove=list->moves[MoveNum].move;
		}
	}
	
	
	if(alpha!=OldAlpha)
	{
		StorePvMove(pos,BestMove);
	}
	
	return 0;
}

static int AlphaBeta(int alpha,int beta,int depth,S_BOARD *pos,S_SEARCHINFO *info,int DoNull)	// Returns the best score
{
	ASSERT(CheckBoard(pos));
	
	if(depth==0)
	{
		return Quiescence(alpha,beta,pos,info);
		//return EvalPosition(pos);
	}
	
	info->nodes++;
	
	if(IsRepetition(pos)||pos->fiftyMove>=100)	// draw
	{
		return 0;
	}
	
	if(pos->ply>MAXDEPTH-1)		// reached max depth so just end search
	{
		return EvalPosition(pos);
	}
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos,list);
	
	int MoveNum=0;
	int Legal=0;	// say we do not find legal moves checkmate
	int OldAlpha=alpha;
	int BestMove=NOMOVE;
	int Score=-INFINITE;
	int PvMove=ProbePvTable(pos);
	
	if(PvMove!=NOMOVE)
	{
		for(MoveNum=0;MoveNum<list->count;++MoveNum)
		{
			if(list->moves[MoveNum].move==PvMove)
			{
				list->moves[MoveNum].score=2000000;
				break;
			}
			
		}
	}
		
	
	for(MoveNum=0;MoveNum<list->count;++MoveNum)
	{
		PickNextMove(MoveNum,list);
		
		if(!MakeMove(pos,list->moves[MoveNum].move))
		{
			continue;
		}
		Legal++;
		Score=-AlphaBeta(-beta,-alpha,depth-1,pos,info,TRUE);
		TakeMove(pos);
		
		if(Score>alpha)
		{
			if(Score>=beta)
			{
				if(Legal==1)
				{
					info->fhf++;
				}
				info->fh++;
				
				
				/*
				 * How the searchKiller works is whenever you find a betacutoff move ;
				 * You shift whats on the 0th index to 1st index
				 * Addin the newer move onto the 0th index
				 */
				if(!(list->moves[MoveNum].move & MFLAGCAP))
				{
					pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
					pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
				}
				return beta;
				
			}
			alpha=Score;
			BestMove=list->moves[MoveNum].move;
			
			if(!(list->moves[MoveNum].move & MFLAGCAP))
			{
				pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}
			
		}
	}
	
	if(Legal==0)
	{
		if(SqAttacked(pos->KingSq[pos->side],pos->side^1,pos))
			return -MATE+pos->ply;		// returns Mate in how many moves made 
		else
			return 0;
	}
	
	if(alpha!=OldAlpha)
	{
		StorePvMove(pos,BestMove);
	}
	
	return alpha;
	
	
}

void SearchPosition(S_BOARD *pos,S_SEARCHINFO *info)	// Iterative Deepening , search init 
{
	int bestMove=NOMOVE;
	int bestScore=-INFINITE;
	int currentDepth=0;
	int pvMoves=0;
	int pvNum=0;
	
	ClearForSearch(pos,info);
	
	for(currentDepth=1;currentDepth<=info->depth;++currentDepth)
	{
		bestScore=AlphaBeta(-INFINITE,INFINITE,currentDepth,pos,info,TRUE);
		pvMoves=GetPvLine(currentDepth,pos);
		bestMove=pos->PvArray[0];
		
		printf("Depth:%d score:%d move:%s nodes:%ld ",currentDepth,bestScore,PrMove(bestMove),info->nodes);
		
		// Printing the entire PVarray
		pvMoves=GetPvLine(currentDepth,pos);
		printf("pv");
		for(pvNum=0;pvNum<pvMoves;++pvNum)
		{
			printf(" %s",PrMove(pos->PvArray[pvNum]));
		}
		printf("\n");
		printf("Ordering:%.2f\n",(info->fhf/info->fh));
	}
	
}
  
