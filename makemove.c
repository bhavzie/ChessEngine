


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
