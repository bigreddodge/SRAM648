#include <stdio.h>
#include <tchar.h>
#include "Serial.h"	// Library described above
#include <string>

#define ROWS 10
#define COLUMNS 65536

// Function prototypes:
void printStatus();

// SRAM data storage 2D array
//             row   col
char SRAM_DATA[ROWS][COLUMNS + 1]; // One extra for the '\0'

int _tmain(int argc, _TCHAR* argv[]){
	printf("SRAM Characterization\n---------------------\n");
	Serial* SP = new Serial("COM5");    // over COM9, use: "\\\\.\\COM10"
	if (SP->IsConnected())
		printf("Connected.\n\n");

	char incomingData[256] = "";			// don't forget to pre-allocate memory
	int dataLength = 255;
	int readResult = 0;
    int row = 0, col = 0;

	while(SP->IsConnected()){
		readResult = SP->ReadData(incomingData,dataLength);
        incomingData[readResult] = 0;
        if (readResult != 0){
            for (int parse = 0; parse < readResult; parse++){
                if (col >= COLUMNS){
                    row++;
                    col = 0;
                    printf("%s", ".");
                }
                SRAM_DATA[row][col] = incomingData[parse];
                //printf("\nRow: %u   Col: %u   Value: %c", row, col, incomingData[parse]);
                //printf("%c", incomingData[parse]);
                col++;
                if ((row == (ROWS - 1)) && (col == COLUMNS)){
                    printf("DONE.");
                    printStatus();
                    return 0;
                }
            }
        }
	}

	return 0;
}

void printStatus(){
    printf("\n\n\nComplete.\n");
    for (int r = 0; r < ROWS; r++){
        for (int c = 0; c < 8; c++){
            printf("%c", SRAM_DATA[r][c]);
        }
        printf("...");
        for (int c = 0; c < 8; c++){
            printf("%c", SRAM_DATA[r][COLUMNS - 11 + c]);
        }
        printf("\n");
    }
}
