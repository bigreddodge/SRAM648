#include <stdio.h>
#include <tchar.h>
#include "Serial.h"
#include <string>
#include <fstream>
#include <iostream>

#define ROWS 100
#define COLUMNS 65536 // 2x 32768 since storing by nibble vs. byte

// Function prototypes:
void printStatus();
void processData();
int ascii2int(char ascii);

// SRAM data storage 2D array
//             row   col
char SRAM_DATA[ROWS][COLUMNS + 1]; // One extra for the '\0'
int counter[COLUMNS*4];
bool receiving = false;
char filename[3][20];

int _tmain(int argc, _TCHAR* argv[]){
	std::cout << "\nSRAM Characterization\n---------------------\nConnecting...";
	Serial* SP = new Serial("COM5");    // over COM9, use: "\\\\.\\COM10"
	if (SP->IsConnected()){
		std::cout << "DONE.\nEnter chip number: ";
        int chipnum = 0;
        std::cin >> chipnum;
        strcpy(filename[0], "sram");
        filename[0][4] = char((chipnum / 10) + 48);
        filename[0][5] = char((chipnum % 10) + 48);
        strcpy(filename[1], filename[0]);
        strcpy(filename[2], filename[0]);
        strcat(filename[0], "_bin.csv");
        strcat(filename[1], "_hex.csv");
        strcat(filename[2], "_prb.csv");
    }

    std::cout << "\nWaiting for data...";
	char incomingData[256] = "";			// don't forget to pre-allocate memory
	int dataLength = 255;
	int readResult = 0;
    int row = 0, col = 0;

	while(SP->IsConnected()){
		readResult = SP->ReadData(incomingData,dataLength);
        incomingData[readResult] = 0;
        if (readResult != 0){
            if (receiving == false){
                receiving = true;
                std::cout << "\nReceiving.";
            }
            for (int parse = 0; parse < readResult; parse++){
                if (col >= COLUMNS){
                    row++;
                    col = 0;
                    std::cout << '.';
                }
                SRAM_DATA[row][col] = incomingData[parse];
                col++;
                if ((row == (ROWS - 1)) && (col == COLUMNS)){
                    std::cout << "DONE.";
                    processData();
                    return 0;
                }
            }
        }
	}
	return 0;
}

void printStatus(){
    std::cout << "\n\n\nComplete.\n";
    for (int r = 0; r < ROWS; r++){
        for (int c = 0; c < 8; c++){
            std::cout << SRAM_DATA[r][c];
        }
        std::cout << "...";
        for (int c = 0; c < 8; c++){
            std::cout << SRAM_DATA[r][COLUMNS - 11 + c];
        }
        std::cout << std::endl;
    }
}

void processData(){
    int prob1ctr = 0;
    std::fstream raw(filename[0], std::fstream::out);
    std::fstream hex(filename[1], std::fstream::out);
    std::fstream prb(filename[2], std::fstream::out);

    std::cout << "\nProcessing data...";
//  Write raw data to file
    for (int c = 0; c < COLUMNS * 4; c++){
        counter[c] = 0;
        for (int r = 0; r < ROWS; r++){
            int dec = ascii2int(SRAM_DATA[r][c / 4]);
            int bit = 0;
            if      ((c % 4) == 0){                         //MSB of nibble
                bit = ((dec & 0x8) >> 3);}
            else if ((c % 4) == 1){
                bit = ((dec & 0x4) >> 2);}
            else if ((c % 4) == 2){
                bit = ((dec & 0x2) >> 1);}
            else if ((c % 4) == 3){                         //LSB of nibble
                bit = ((dec & 0x1));

                hex << SRAM_DATA[r][c/4];                   //Write hex output
                if (r < (ROWS - 1))                         //Choose delimiter
                    hex << ',';
                else
                    hex << char(10);
            }

            counter[c] += bit;                              //Accumulate occurrences

            raw << bit;                                     //Write binary output
            if (r < (ROWS - 1))                             //Choose delimiter
                raw << ',';
            else
                raw << char(10);
        }
            double prob = counter[c] / (double)ROWS;
            if (prob == int(prob))
                prob1ctr++;
            prb << counter[c] / (double)ROWS << char(10);   //Write probability output
    }
    prb << char(10) << prob1ctr;
    raw.close();
    hex.close();
    prb.close();

    std::cout << "DONE.";
}

int ascii2int(char ascii){
    int i = ascii;
    if (i < 58)
        return (i-48);
    else
        return (i-55);
}
