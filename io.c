#include<stdio.h>
#include"defs.h"

char *PrSq(const int sq)	// Print sq in the form say b2
{
	static char SqStr[3];
	int file=FilesBrd[sq];
	int rank=RanksBrd[sq];
	
	sprintf(SqStr,"%c%c",('a'+file),('1'+rank));
	return SqStr;
	
}

char *PrMove(const int move)	// Print move in algebraic notation just saying b2b4q for b2-b4 queen
{
	static char MvStr[6];
	int ff=FilesBrd[FROMSQ(move)];
	int rf=RanksBrd[FROMSQ(move)];
	int ft=FilesBrd[TOSQ(move)];
	int rt=RanksBrd[TOSQ(move)];
	
	int promoted=PROMOTED(move);
	
	if(promoted)
	{
		char pchar='q';
		if(IsKn(promoted))
		{
			pchar='n';
		}
		else if(IsRQ(promoted)&&!IsBQ(promoted))
		{
			pchar='r';
		}
		else if(!IsRQ(promoted)&&IsBQ(promoted))
		{
			pchar='b';
		}
		sprintf(MvStr,"%c%c%c%c%c",('a'+ff),('1'+rf),('a'+ft),('1'+rt),pchar);
	}
	else
	{
		sprintf(MvStr,"%c%c%c%c",('a'+ff),('1'+rf),('a'+ft),('1'+rt));
	}
	return MvStr;
}
