#include <stdio.h>
#include <tchar.h>
#include "Serial.h"	// Library described above
#include <string>
#include <fstream>
#include <iostream>

#define ROWS 10
#define COLUMNS 65536 // 2x 32768 since storing by nibble vs. byte

// Function prototypes:
void printStatus();
void processData();
void writeDataFiles();
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
                    //writeDataFiles();
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
            prb << counter[c] / (double)ROWS << char(10);   //Write probability output
    }
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

//void writeDataFiles(){
//    std::cout << "\nWriting data...";
//    std::fstream fs;
//    fs.open("data.csv", std::fstream::out);
//    for (int c = 0; c < COLUMNS; c++){
//        for (int r = 0; r < ROWS; r++){
//            fs << SRAM_DATA[r][c];
//            if (r < (ROWS-1))
//                fs << ',';
//        }
//        fs <<  char(10);
//    }
//    fs.close();
//    fs.open("prob.csv", std::fstream::out);
//    for (int p = 0; p < COLUMNS * 4; p++){
//        fs << counter[p]/(double)ROWS << char(10);
//    }
//    fs.close();
//    std::cout << "DONE.";
//}

