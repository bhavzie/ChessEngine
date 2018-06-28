#ifndef DEFS_H
#define DEFS_H

#include<stdlib.h>

//Error handler
    #define DEBUG
    #ifndef DEBUG
    #define ASSERT(n)		// if N is false 
    #else
    #define ASSERT(n) \
    if(!(n)) { \
        printf("%s - Failed",#n); \
        printf("On %s ", __DATE__); \
        printf("At %s ",__TIME__); \
        printf("In file %s ",__FILE__); \
        printf("At Line %d\n",__LINE__); \
        exit(1);}
    #endif  
//


//Definitions :
    //Defining a 64 bit integer which is used on bitboards and many other places.
    typedef unsigned long long U64;

    #define NAME "Vice 1.0"
    #define BRD_SQ_NUM 120                  // The square board is made 120 out of which 64 ( 8x8 ) are legal rest are filler
    #define MAXGAMEMOVES 2048               // Max number of moves in a game
    #define MAXPOSITIONMOVES 256		    // Max moves for a given position
    
   #define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    
    
    
    enum{EMPTY,wP,wN,wB,wR,wQ,wK,bP,bN,bB,bR,bQ,bK};   // [ 0 to 12 ]     
    enum{FILE_A,FILE_B,FILE_C,FILE_D,FILE_E,FILE_F,FILE_G,FILE_H,FILE_NONE};
    enum{RANK_1,RANK_2,RANK_3,RANK_4,RANK_5,RANK_6,RANK_7,RANK_8,RANK_NONE};
    enum{WHITE,BLACK,BOTH};
    enum{
        A1=21,B1,C1,D1,E1,F1,G1,H1,
        A2=31,B2,C2,D2,E2,F2,G2,H2,
        A3=41,B3,C3,D3,E3,F3,G3,H3,
        A4=51,B4,C4,D4,E4,F4,G4,H4,
        A5=61,B5,C5,D5,E5,F5,G5,H5,
        A6=71,B6,C6,D6,E6,F6,G6,H6,
        A7=81,B7,C7,D7,E7,F7,G7,H7,
        A8=91,B8,C8,D8,E8,F8,G8,H8,NO_SQ,OFFBOARD
    };              // for the 8x8 board
    enum{FALSE,TRUE};
    enum{WKCA=1,WQCA=2,BKCA=4,BQCA=8};   // 0 0 0 0  ( each bit represents whether castle is possible ) If possible 1 else we change to 0

   

typedef struct {
	int move;
	int score;
	   
} S_MOVE;

typedef struct {
    
	S_MOVE moves[MAXPOSITIONMOVES];
	int count;	// Count of number of moves in the list
	    
} S_MOVELIST;
 
typedef struct {
        int move;           // the move last played is stored as a 32 bit integer 
        int castlePerm;
        int enPas;
        int fiftyMove;      
        U64 posKey;            // using a hashkey which is a unique key for each sq on the board 
        
	
} S_UNDO;           // Helps undo board by storing history of board before move is made

    typedef struct {
        
            int pieces[BRD_SQ_NUM];     //Developing a 120 block array for chessboard
            U64 pawns[3];               /* Pawns if present on a block set its value to 1 else set it to 0 - we choose 3 spaces since a pawn can be a white 
                                        pawn or a black pawn or both : 00000000 00100000 
                                                                        A1-H1    A2-H2 ( C2 is 1 so there is a pawn at C2)
                                        Array of 3 since we keep one U64 for white one for black and one for both
                                        */
            int KingSq[2];              // Holding where king is
            int side;                   // Who is moving
            int enPas;                  //En Passant square
            int fiftyMove;              //Check if 50 moves then draw
            int ply;                    //Half Moves during search
            int hisPly;                 //Check Repitions by storing ply , Hisply stores total number of half moves made till now
            U64 posKey;                 //Check position which is a unique key for each position
            int pceNum[13];             //Number of pieces on the board ( array of 13 since 13 types of pieces ( including empty )
            int bigPce[2];              //Number of big pieces( which are not pawns) for each color
            int majPce[2];              //Number of major pieces ( Rooks and queens ) for each color
            int minPce[2];              //Number of  pieces which are bishops knights for each color
            int material[2];		// value of material for black and white
            int castlePerm;             //Explained in Enum {wkca ...}
            
		// Piece List
            int pList[13][10];
            /*types of pieces are 13 and max number of a piece can be 10 ( if we promote all pawns to rooks and have 2 rooks at the start )
              so say pList[Wn][0]=E5 that is first white knight at e5 and so on
            */
            
            S_UNDO history[MAXGAMEMOVES]; // Hold information of the board before a move is moved , we use 2048 since we wanto to hold history for all the moves rather than only the last move

    } S_BOARD;

/* MOVES 
 * Considering we have a 32 bit integer to store a move,we will assign first 7 bits for the FROM position , since the board can go from 21 to 98 and all these can be obtained by using 7 bits
 * 0000 0000 0000 0000 0000 0111 1111	-> FROM 0x7F
 * 0000 0000 0000 0011 1111 1000 0000	-> TO >> 7 0x7F
 * 0000 0000 0011 1100 0000 0000 0000	-> CAPTURED piece type >> 14 0xF
 * 0000 0000 0100 0000 0000 0000 0000	-> EnPassant 0x40000 	; No shifting here since we just & these later
 * 0000 0000 1000 0000 0000 0000 0000	-> Pawn Start 0x80000
 * 0000 1111 0000 0000 0000 0000 0000	-> Promoted piece type since they can be of 12 types we assign 4 bits	>> 20 , 0xF
 * 0001 0000 0000 0000 0000 0000 0000	-> Castle 0x1000000
 */
    
/*Macros*/
	#define FR2SQ(f,r) ((21+(f))+((r)*10))                    // For a given file and rank it returns equivalent sq in 120 based board
	#define SQ64(sq120) Sq120ToSq64[sq120]                    // Just a shorter name 
	#define SQ120(sq64) (Sq64ToSq120[(sq64)])
	#define POP(b) PopBit(b)
	#define CNT(b) CountBits(b)
	#define CLRBIT(bb,sq) ((bb)&=ClearMask[(sq)])
	#define SETBIT(bb,sq) ((bb)|=SetMask[(sq)])
	#define IsBQ(p) (PieceBishopQueen[(p)])			// Just a shorter name
	#define IsRQ(p) (PieceRookQueen[(p)])
	#define IsKn(p) (PieceKnight[(p)])
	#define IsKi(p) (PieceKing[(p)])
	#define FROMSQ(m) ((m) & 0x7F)
	#define TOSQ(m) (((m)>>7) & 0x7F)
	#define CAPTURED(m) (((m)>>14) & 0xF)
	#define PROMOTED(m) (((m)>>20) & 0xF)
	#define MFLAGEP 0x40000
	#define MFLAGPS 0x80000		// PawnStart
	#define MFLAGCA 0x1000000	// Castle
	#define MFLAGCAP 0x7C000	// Captured 
	#define MFLAGPROM 0xF00000	// Promoted


/*Globals*/
    
	extern int Sq120ToSq64[BRD_SQ_NUM];
	extern int Sq64ToSq120[64];
	
	extern U64 SetMask[64];
	extern U64 ClearMask[64];
	
	extern U64 PieceKeys[13][120];
	extern U64 SideKey;		// Not just position but actually creating a unique key based on position and side and given castleperm 
	extern U64 CastleKeys[16];
	
	extern char PceChar[];
	extern char SideChar[];
	extern char RankChar[];
	extern char FileChar[];
	extern int PieceBig[13];	// To know whether a piece is big a major , whats its value amd what is its color
	extern int PieceMaj[13];
	extern int PieceMin[13];
	extern int PieceVal[13];
	extern int PieceCol[13];
	
	extern int FilesBrd[BRD_SQ_NUM];
	extern int RanksBrd[BRD_SQ_NUM];
	
	extern int PieceKnight[13];	// Just asking whether given piece is a knight or not - quite similar to PieceMin and PieceMaj
	extern int PieceKing[13];
	extern int PieceRookQueen[13];
	extern int PieceBishopQueen[13];
	extern int PieceSlides[13];
	
/*Functions*/


	// init.c
        extern void AllInit();

	// bitboards.c
        extern void PrintBitBoard(U64 bb);
        extern int PopBit(U64 *bb);
        extern int CountBits(U64 b);

	// hashkeys.c
	extern U64 GeneratePosKey(const S_BOARD *pos);
    
	// board.c
        extern void ResetBoard(S_BOARD *pos);
	extern int ParseFen(char *fen,S_BOARD *pos);
	extern void PrintBoard(const S_BOARD *pos);
	extern void UpdateListsMaterial(S_BOARD *pos);
	extern int CheckBoard(const S_BOARD *pos);
	
	// attack.c	
	extern int SqAttacked(const int sq,const int side,const S_BOARD *pos);

	// io.c
	extern char *PrSq(const int sq);
	extern char *PrMove(const int move);
	extern void PrintMoveList(const S_MOVELIST *list);

	// validate.c
	extern int SqOnBoard(const int sq);
	extern int SideValid(const int side);
	extern int FileRankValid(const int fr);
	extern int PieceValidEmpty(const int pce);
	extern int PieceValid(const int pce);
	
	// movegen.c
	extern void GenerateAllMoves(const S_BOARD *pos,S_MOVELIST *list);
	
#endif
