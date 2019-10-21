#ifndef __virtual_memory__
#define __virtual_memory__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_results(int frames,int pageFault,int total_W, int total_R,int traceCounter);
int virtual_Memory(int k, int frames, int q, int max);



#endif
