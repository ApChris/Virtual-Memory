
#include "virtual_Memory.h"

int main(int argc, char** argv)
{
	int i, k, q, max;

	if (argc != 7)
	{
		printf("ERROR input: Give for example : ./main -k 10 -q 2 -m 2000\n");
		return -1;
	}


	// Get input arguments
	if ( (strcmp(argv[1], "-k") == 0) && (strcmp(argv[3], "-q") == 0) && (strcmp(argv[5], "-m") == 0) )
	{
		k = atoi(argv[2]);
		q = atoi(argv[4]);
		max = atoi(argv[6]);
	}
	else
	{
		// End of program
		printf("Wrong input arguments! \n");
		return -1;
	}

	// Print just for feedback
	printf("Your input was: %s %s %s %s %s %s %s\n",argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);

	// k 	---> is FWF parameter for example if k = 10 ,then we allow 10 page faults at a single process, and when page faults are going to be 11 we flush all pages
	// 2*k 	--> total frames
	// q 	--> quantum for Round Robin
	// max 	--> traces to examine
	virtual_Memory(k, 2*k, q, max);

	return 0;
}
