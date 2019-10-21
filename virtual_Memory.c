#include "virtual_Memory.h"

void print_results(int frames,int pageFault,int total_W, int total_R,int traceCounter)
{
	printf("Total frames: %d\n", frames);
	printf("Page faults: %d\n" , pageFault);
	printf("Total < W > frames  (wrote on disk): %d\n", total_W);
	printf("Total < R > frames  (read from disk): %d\n", total_R);
	printf("Total of trace entries examined from files: %d\n", traceCounter);
}

int virtual_Memory(int k, int frames, int q, int max)
{
	// Variables
	int ** Inverted_PageTable;
	int i, j, pageNum, pid;
	int found_trace, traceCounter = 0;
	int pageFault = 0, pageFaults_process_1 = 0, pageFaults_process_2 = 0;
	int total_R = 0, total_W = 0;

	char pageNum_str[5], page_Operation[1], line[256];
	char *tok;

	// fileNumber_1 == bzip.trace
	FILE* fileNumber_1 = fopen("bzip.trace", "r");

	// Check if file has been opened
	if (fileNumber_1 == NULL)
	{
		printf("Error: bzip.trace can't be opened\n");
		// Return -10 , if something went wrong
		return -10;
	}

	// fileNumber_2 == bzip.trace
	FILE* fileNumber_2 = fopen("gcc.trace", "r");

	// Check if file has been opened
	if (fileNumber_2 == NULL)
	{
		printf("Error: gcc.trace can't be opened\n");
		// Return -10 , if something went wrong
		return -10;
	}

	// Create a Array of ( f * pointer) bytes
	if ((Inverted_PageTable = malloc(frames * sizeof(int*))) == NULL)
	{
		printf("Error: In malloc of --> Inverted_PageTable\n");

		// Return -100 , if something went wrong with Inverted_PageTable
		return -100;
	}

	// initial counter
	i = 0 ;

	// For all frames (frames == 2*k)
	while (i < frames)
	{
		// create 4 ints = 16 bytes
		if ((Inverted_PageTable[i] = malloc(4 * sizeof(int))) == NULL)
		{
			printf("Error: In malloc of --> Inverted_PageTable\n");
			return -100;
		}

		// Put at [3] int the value 0
		Inverted_PageTable[i][3] = 0;

		// increase counter
		i++;
	}

	// endless loop
	for(;;)
	{
		// Read one line of first trace file and update pageNum, page_Operation, pid
		// Examine traces of q-size block from 1st process
		i = 0 ;
		while (i < q)
		{
			// 1st process
			pid = 1;

			// Get current line
			fgets(line, sizeof(line), fileNumber_1);

			// Get bytes until \t , for example line : (0014ac60 R) and  tok:(0014ac60)
			tok = strtok(line, " /t");

			// Get the first 5 for offset
			strncpy(pageNum_str, tok, 5);

			// Null terminate destination
			pageNum_str[5] = 0;

			// get pagenumber
			pageNum = (int)strtol(pageNum_str, NULL, 16);

			// Token is going to get R or W
			tok = strtok(NULL, " /t");

			// copy tok to page_operation
			strcpy(page_Operation, tok);

			// Null terminate destination
			page_Operation[1] = 0;

			// initial flag of trace
			found_trace = 0;

			// initial counter
			j = 0 ;

			// Search in Inverted_PageTable to find process with pageNum
			// For all frames
			while (j < frames)
			{
				// if current position has something inside
				if (Inverted_PageTable[j][3] != 0)
				{
					// Check if pid is 1 AND if page Number is the same
					if( (Inverted_PageTable[j][0] == pid) && (Inverted_PageTable[j][1] == pageNum) )
					{
						// Check operation
						// if strcmp returned as 0, then page_Operation == W
						if (strcmp(page_Operation, "W") == 0)
						{
							// Set flag to 1
							Inverted_PageTable[j][2] = 1;
						}
						// The current trace exists in Inverted_PageTable
						found_trace = 1;
					}
				}
				// increase counter
				j++;
			}

			// Current Trace doesn't exist in Inverted_PageTable, therefore PAGE FAULT
			if (found_trace == 0)
			{
				// Increase number of page Faults from process_1
				pageFaults_process_1++;

				// global page fault number
				pageFault++;

				// Increase number of Reads
				total_R++;

				// Flush When Full ( FWF )
				// Allows k page Faults occur at a single process. When page faults are k+1 then we "flush" all pages from specific process
				if (pageFaults_process_1 > k)
				{
					// initial counter
					j = 0 ;

					// For all frames -> frames == 2*k
					while (j < frames)
					{
						// if pid == 1
						if (Inverted_PageTable[j][0] == pid)
						{
							// If flag == 1 ,it means that we have find it(look before)
							if (Inverted_PageTable[j][2] == 1)
							{
								// Then increase counter of Writes
								total_W++;
							}

							// And "Flush" current page
							Inverted_PageTable[j][3] = 0;
						}
						// Increase counter
						j++;
					}
					// initial again process_1's page Faults
					pageFaults_process_1 = 1;
				}

				// Update Inverted_PageTable with new trace
				j = 0 ;

				// For all frames
				while (j < frames)
				{
					//	If current frame has been flushed
					if (Inverted_PageTable[j][3] == 0 && (Inverted_PageTable[j][0] == pid || Inverted_PageTable[j][0] == 0))
					{
						// Set current frame pid to 1
						Inverted_PageTable[j][0] = pid;

						// Set current frame page Number with pageNum
						Inverted_PageTable[j][1] = pageNum;

						// Set current frame offset to 1
						Inverted_PageTable[j][3] = 1;

						// if page_Operation equals to 0
						if (strcmp(page_Operation, "W") == 0)
						{
							// Then set flag to 1
							Inverted_PageTable[j][2] = 1;
						}
						else // Else is read
						{
							// Set it clean
							Inverted_PageTable[j][2] = 0;
						}
						break;
					}
					j++;
				}
			}

			// increase trace counter
			traceCounter++;

			// Check if we already examine max entries from both trace files
			if (traceCounter == max)
			{
				// Execute FWF for whole Inverted_PageTable at exit and update statistic variables
				j = 0 ;
				// For all frames
				while ( j < frames )
				{
					// if
					if (Inverted_PageTable[j][3] != 0)
					{
						// If we found that ( look before )
						if (Inverted_PageTable[j][2] == 1)
						{
							// Increase counter of writes
							total_W++;
						}

						// set it clean
						Inverted_PageTable[j][3] = 0;
					}
					// increase counter
					j++;
				}

				print_results(frames,pageFault,total_W,total_R,traceCounter);

				// Close Files
				fclose(fileNumber_1);
				fclose(fileNumber_2);

				// Free for every frame and free the table
				i = 0 ;
				while (i < frames)
				{
					free(Inverted_PageTable[i]);
					i++;
				}
				free(Inverted_PageTable);

				return 0;
			}

			// increase counter
			i++;
		}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Examine traces of q-size block from 2nd process
		i = 0 ;
		while (i < q)
		{
			// 2nd process
			pid = 2;

			// Get current line
			fgets(line, sizeof(line), fileNumber_2);

			// Get bytes until \t , for example line : (0014ac60 R) and  tok:(0014ac60)
			tok = strtok(line, " /t");

			// Get the first 5 for offset
			strncpy(pageNum_str, tok, 5);

			// Null terminate destination
			pageNum_str[5] = 0;

			// Get pageNumber
			pageNum = (int)strtol(pageNum_str, NULL, 16);

			// Token is going to get R or W
			tok = strtok(NULL, " /t");

			// copy to tok to page_operation
			strcpy(page_Operation, tok);

			// Null terminate destination
			page_Operation[1] = 0;

			// initial flag of trace
			found_trace = 0;

			// initial counter
			j = 0 ;

			// Search in Inverted_PageTable to find process with pageNum
			// For all frames
			while (j < frames)
			{
				// if current position has something inside
				if (Inverted_PageTable[j][3] != 0)
				{
					// Check if pid is 1 AND if page Number is the same
					if( (Inverted_PageTable[j][0] == pid) && (Inverted_PageTable[j][1] == pageNum) )
					{
						// Check operation
						// if strcmp returned as 0, then page_Operation == W
						if (strcmp(page_Operation, "W") == 0)
						{
							// Set flag to 1
							Inverted_PageTable[j][2] = 1;
						}
						// The current trace exists in Inverted_PageTable
						found_trace = 1;
					}
				}
				// increase counter
				j++;
			}

			// Current Trace doesn't exist in Inverted_PageTable, therefore PAGE FAULT
			if (found_trace == 0)
			{
				// Increase number of page Faults from process_1
				pageFaults_process_2++;

				// global page fault number
				pageFault++;

				// Increase number of Reads
				total_R++;

				// Flush When Full ( FWF )
				// Allows k page Faults occur at a single process. When page faults are k+1 then we "flush" all pages from specific process
				if (pageFaults_process_2 > k)
				{
					// initial counter
					j = 0 ;

					// For all frames -> frames == 2*k
					while (j < frames)
					{
						// if pid == 1
						if (Inverted_PageTable[j][0] == pid)
						{
							// If flag == 1 ,it means that we have find it(look before)
							if (Inverted_PageTable[j][2] == 1)
							{
								// Then increase counter of Writes
								total_W++;
							}

							// And "Flush" current page
							Inverted_PageTable[j][3] = 0;
						}
						// Increase counter
						j++;
					}
					// initial again process_1's page Faults
					pageFaults_process_2 = 1;
				}

				// Update Inverted_PageTable with new trace
				j = 0 ;

				// For all frames
				while (j < frames)
				{
					//	If current frame has been flushed
					if (Inverted_PageTable[j][3] == 0 && (Inverted_PageTable[j][0] == pid || Inverted_PageTable[j][0] == 0))
					{
						// Set current frame pid to 1
						Inverted_PageTable[j][0] = pid;

						// Set current frame page Number with pageNum
						Inverted_PageTable[j][1] = pageNum;

						// Set current frame offset to 1
						Inverted_PageTable[j][3] = 1;

						// if page_Operation equals to 0
						if (strcmp(page_Operation, "W") == 0)
						{
							// Then set flag to 1
							Inverted_PageTable[j][2] = 1;
						}
						else // Else is read
						{
							// Set it clean
							Inverted_PageTable[j][2] = 0;
						}
						break;
					}
					j++;
				}
			}

			// increase trace counter
			traceCounter++;

			// Check if we already examine max entries from both trace files
			if (traceCounter == max)
			{
				// Execute FWF for whole Inverted_PageTable at exit and update statistic variables
				j = 0 ;
				// For all frames
				while ( j < frames )
				{
					// if o
					if (Inverted_PageTable[j][3] != 0)
					{
						// If we found that ( look before )
						if (Inverted_PageTable[j][2] == 1)
						{
							// Increase counter of writes
							total_W++;
						}

						// set it clean
						Inverted_PageTable[j][3] = 0;
					}
					// increase counter
					j++;
				}

				print_results(frames,pageFault,total_W,total_R,traceCounter);

				// Close Files
				fclose(fileNumber_1);
				fclose(fileNumber_2);

				// Free for every frame and free the table
				i = 0 ;
				while (i < frames)
				{
					free(Inverted_PageTable[i]);
					i++;
				}
				free(Inverted_PageTable);

				return 0;
			}

			// increase counter
			i++;
		}
	}
}
