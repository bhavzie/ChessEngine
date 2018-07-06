#include<stdio.h>
#include"defs.h"
#include<sys/time.h>



int GetTimeMs()		// How much time has elapsed
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000+tv.tv_usec/1000;
	
}

