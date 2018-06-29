#include<stdio.h>
#include"defs.h"


#define HASH_PCE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))


// ca_perm &= CastlePerm[from]
// 1111 = 15
// Say from square[where black king moved] is E8 that is 3 we do ca_perm &=3 which gives 0011 so black can't castle anymore
const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

void TakeMove(S_BOARD *pos);

/* Making a move  
 * 1. make(move) called
 * 2. Get from,to,cap from move
 * 3. Store the current pos in pos->history array
 * 4. Move current piece from -> to
 * 5. if a capture was made,remove captured from piece list
 * 6. Update 50 move rule,see if pawn moved
 * 7. Promotions
 * 8. EnPas Captures
 * 9. Set Enpas squares if PStart ( pawn moves 2 steps forward at the start )
 * 10 For all pieces added,moved,removed update all params from defs.h
 * 11 Position counters,piece lists
 * 12 Maintain poskey
 * 13 Don't forget castle perms
 * 14 Change side,increment ply and hisply
 */


static void ClearPiece(const int sq,S_BOARD *pos)
{
	ASSERT(SqOnBoard(sq));
	int pce=pos->pieces[sq];
	ASSERT(PieceValid(pce));
	int col=PieceCol[pce];
	int index=0;
	int t_pceNum=-1;	
	HASH_PCE(pce,sq);	// XORing the pce from sq so removing it out ( shown in posKey generation )
	
	pos->pieces[sq]=EMPTY;
	pos->material[col]-=PieceVal[pce];
	
	if(PieceBig[pce])	// Not a pawn
	{
		pos->bigPce[col]--;
		if(PieceMaj[pce])	// RQK
			pos->majPce[col]--;
		else
			pos->minPce[col]--;	// NB	
	}
	else	// pawns
	{
		CLRBIT(pos->pawns[col],SQ64(sq));
		CLRBIT(pos->pawns[BOTH],SQ64(sq));
	}
	
	for(index=0;index<pos->pceNum[pce];++index)
	{
		if(pos->pList[pce][index]==sq)
		{
			t_pceNum=index;
			break;
		}
	}
	ASSERT(t_pceNum!=-1);	
	pos->pceNum[pce]--;
	pos->pList[pce][t_pceNum]=pos->pList[pce][pos->pceNum[pce]];
		
}

static void AddPiece(const int sq,S_BOARD *pos,const int pce)
{
	ASSERT(PieceValid(pce));
	ASSERT(SqOnBoard(sq));
	int col=PieceCol[pce];
	HASH_PCE(pce,sq);
	pos->pieces[sq]=pce;
	
	if(PieceBig[pce])
	{
		pos->bigPce[col]++;
		if(PieceMaj[pce])
			pos->majPce[col]++;
		else
			pos->minPce[col]++;
	}
	else
	{
		SETBIT(pos->pawns[col],SQ64(sq));
		SETBIT(pos->pawns[BOTH],SQ64(sq));
	}
	pos->material[col]+=PieceVal[pce];
	pos->pList[pce][pos->pceNum[pce]++]=sq;
		
}

static void MovePiece(const int from,const int to,S_BOARD *pos)
{
	ASSERT(SqOnBoard(from));	
	ASSERT(SqOnBoard(to));
	int index=0;
	int pce=pos->pieces[from];
	int col=PieceCol[pce];
	
#ifdef DEBUG
	int t_PieceNum=FALSE;
#endif
	HASH_PCE(pce,from);
	pos->pieces[from]=EMPTY;
	
	HASH_PCE(pce,to);
	pos->pieces[to]=pce;
	
	if(!PieceBig[pce])
	{
		CLRBIT(pos->pawns[col],SQ64(from));
		CLRBIT(pos->pawns[BOTH],SQ64(from));
		SETBIT(pos->pawns[col],SQ64(to));
		SETBIT(pos->pawns[BOTH],SQ64(to));		
	}
	
	for(index=0;index<pos->pceNum[pce];++index)
	{
		if(pos->pList[pce][index]==from)
		{
			pos->pList[pce][index]=to;
#ifdef DEBUG
			t_PieceNum=TRUE;
#endif
			break;
		}
	}
	
	ASSERT(t_PieceNum)	
}

int MakeMove(S_BOARD *pos,int move)	
{
	/*
	 * Return a 0 ( false ) when the side to make the move keeps his king on Check
	 * Return a 1 otherwise
	 */
	
	ASSERT(CheckBoard(pos));
	int from=FROMSQ(move);
	int to=TOSQ(move);
	int side=pos->side;
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(SideValid(side));
	ASSERT(PieceValid(pos->pieces[from]));
	
	pos->history[pos->hisPly].posKey=pos->posKey;
	
	if(move&MFLAGEP)
	{
		if(side==WHITE)
		{
			ClearPiece(to-10,pos);
		}
		else
		{
			ClearPiece(to+10,pos);
		}		
	}
	else if(move&MFLAGCA)
	{
		switch(to)
		{
			case C1:
				MovePiece(A1,D1,pos);
				break;
			case C8:
				MovePiece(A8,D8,pos);
				break;
			case G1:
				MovePiece(H1,F1,pos);
				break;
			case G8:
				MovePiece(H8,F8,pos);
				break;
				default:
				ASSERT(FALSE);
				break;
		}	
	}
	
	if(pos->enPas!=NO_SQ)
		HASH_EP;
	HASH_CA;
	
	pos->history[pos->hisPly].move=move;
	pos->history[pos->hisPly].fiftyMove=pos->fiftyMove;
	pos->history[pos->hisPly].enPas=pos->enPas;
	pos->history[pos->hisPly].castlePerm=pos->castlePerm;
	
	pos->castlePerm&=CastlePerm[from];
	pos->castlePerm&=CastlePerm[to];
	pos->enPas=NO_SQ;
	
	HASH_CA;
	
	int captured=CAPTURED(move);
	pos->fiftyMove++;
	
	if(captured!=EMPTY)
	{
		ASSERT(PieceValid(captured));
		ClearPiece(to,pos);
		pos->fiftyMove=0;
	}
	
	pos->hisPly++;
	pos->ply++;
	
	if(PiecePawn[pos->pieces[from]])
	{
		pos->fiftyMove=0;
		if(move&MFLAGPS)
		{
			if(side==WHITE)
			{
				pos->enPas=from+10;
				ASSERT(RanksBrd[pos->enPas]==RANK_3);
			}
			else
			{
				pos->enPas=from-10;
				ASSERT(RanksBrd[pos->enPas]==RANK_6);
			}
			HASH_EP;
		}			
	}
	
	MovePiece(from,to,pos);
	
	int prPce=PROMOTED(move);
	if(prPce!=EMPTY)
	{
		ASSERT(PieceValid(prPce)&&!PiecePawn[prPce]);
		ClearPiece(to,pos);
		AddPiece(to,pos,prPce);
	}
	
	if(PieceKing[pos->pieces[to]])
		pos->KingSq[pos->side]=to;
	
	pos->side^=1;
	HASH_SIDE;
	
	ASSERT(CheckBoard(pos));
	
	if(SqAttacked(pos->KingSq[side],pos->side,pos))
	{
		TakeMove(pos);
		return FALSE;
	}
	return TRUE;
	
}

void TakeMove(S_BOARD *pos)
{
	ASSERT(CheckBoard(pos));
	pos->hisPly--;
	pos->ply--;
	
	int move=pos->history[pos->hisPly].move;
	int from=FROMSQ(move);
	int to=TOSQ(move);
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	
	if(pos->enPas!=NO_SQ)
		HASH_EP;
	HASH_CA;
	
	pos->castlePerm=pos->history[pos->hisPly].castlePerm;
	pos->fiftyMove=pos->history[pos->hisPly].fiftyMove;
	pos->enPas=pos->history[pos->hisPly].enPas;
	
	if(pos->enPas!=NO_SQ)
		HASH_EP;
	HASH_CA;
	
	pos->side^=1;
	HASH_SIDE;
	
	if(MFLAGEP&move)
	{
		if(pos->side==WHITE)
		{
			AddPiece(to-10,pos,bP);
		}
		else
		{
			AddPiece(to+10,pos,wP);
		}
	}
	else if(MFLAGCA&move)
	{
		switch(to)
		{
			case C1:MovePiece(D1,A1,pos);break;
			case C8:MovePiece(D8,A8,pos);break;
			case G1:MovePiece(F1,H1,pos);break;
			case G8:MovePiece(F8,H8,pos);break;
			default:ASSERT(FALSE);break;
		}
	}
	MovePiece(to,from,pos);
	if(PieceKing[pos->pieces[from]])
	{
		pos->KingSq[pos->side]=from;
	}
	
	int captured=CAPTURED(move);
	if(captured!=EMPTY)
	{
		ASSERT(PieceValid(captured));
		AddPiece(to,pos,captured);
	}
	
	if(PROMOTED(move)!=EMPTY)
	{
		ASSERT(PieceValid(PROMOTED(move))&&!PiecePawn[PROMOTED(move)]);
		ClearPiece(from,pos);
		AddPiece(from,pos,(PieceCol[PROMOTED(move)]==WHITE?wP:bP));
	}
	
	ASSERT(CheckBoard(pos));

	
}
