/*
 * SRAM v0.6
 * Performs 100x SRAM dumps after a button press.
 * Works in tandem with PC console application for data harvesting.
 */

#include <SPI.h>

// Designed for SparkFun Arduino Pro Micro 3.3V
// Hardware pin configuration:
#define VCC 4
#define GO 7
#define HOLD 8
#define CS 9
#define SCK 15
#define MISO 14
#define MOSI 16
#define BAUD_RATE 115200

// Address Range 0x0000 - 0x7FFF

// Commands
const byte WRSR = 0x01,         // Write Status Register command
           RDSR = 0x05,         // Read Status Register command
           WR   = 0x02,         // Write byte command
           RD   = 0x03;         // Read byte command

const byte byte_mode = 0x00,    // Byte Mode parameter
           seq_mode  = 0x40,    // Sequential Mode parameter
           page_mode = 0x80;    // Page Mode parameter

const byte null_byte = 0x00;    // Null byte
const char delim = ' ';

void setup() {
  SPI.begin();
  Serial.begin(BAUD_RATE);
  pinMode(VCC, OUTPUT);
  pinMode(HOLD, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(GO, INPUT);

  resetSRAM();

  //Serial.println("\nREADY!\n");
}

void loop() {
  onButton();
  //Serial.println("\nDumping...");
  for (int i = 0; i < 100; i++){
    resetSRAM();  
    SRmode(seq_mode) ;
    RAMdump();
    //Serial.print('\n');
  }
  //Serial.println("\nDONE!");
  
}

void RAMdumpRAW(){
  digitalWrite(CS, LOW);
  SPI.transfer(RD);
  SPI.transfer16(0x0000);
  
  for (unsigned int ctr = 0; ctr < 32768; ctr++){
    byte rx = SPI.transfer(null_byte);
    Serial.write(rx);
  }
  digitalWrite(CS, HIGH);
}

void RAMdump(){
  digitalWrite(CS, LOW);
  SPI.transfer(RD);
  SPI.transfer16(0x0000);
  
  for (unsigned int ctr = 0; ctr < 32768; ctr++){
    byte rx = SPI.transfer(null_byte);
    Serial.print(hex2char((rx >> 4) & 0x0F));
    //Serial.print(delim);
    Serial.print(hex2char(rx & 0x0F));
    //Serial.print(delim);
  }
  digitalWrite(CS, HIGH);
}

// Writes a byte of data to the specified address
void writeByte(unsigned int address, byte value){
  digitalWrite(CS, LOW);
  SPI.transfer(WR);
  SPI.transfer16(address);
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}

// Reads a byte of data from the specified address
byte readByte(unsigned int address){
  digitalWrite(CS, LOW);
  SPI.transfer(RD);
  SPI.transfer16(address);
  byte readVal = SPI.transfer(null_byte);
  digitalWrite(CS, HIGH);
  return readVal;
}

// Selects the status register mode
// Valid arguments: byte_mode | seq_mode | page_mode
void SRmode(byte mode){
  digitalWrite(CS, LOW);
  SPI.transfer(WRSR);
  SPI.transfer(mode);
  digitalWrite(CS, HIGH);
}

// Full SRAM reboot including SPI bus
void resetSRAM(){
  //Serial.print("Resetting...");
  digitalWrite(VCC, LOW);
  digitalWrite(CS, LOW);
  digitalWrite(HOLD, LOW);
  SPI.end();
  delay(1000);
  digitalWrite(VCC, HIGH);
  digitalWrite(CS, HIGH);
  digitalWrite(HOLD, HIGH);
  SPI.begin();
  //Serial.println("DONE!");
}

void onButton(){
  while(digitalRead(GO));
  delay(100);
  while(!digitalRead(GO));
  delay(100);
}

char hex2char(byte hdigit){
  if ((hdigit >=0) && (hdigit <= 9))
    return char(hdigit+48);
  else if ((hdigit >=10) && (hdigit <= 15))
    return char(hdigit+55);
  else return 0;
}

/* Graveyard
 *  // Reset all power
  digitalWrite(VCC, LOW);
  digitalWrite(CS, LOW);
  digitalWrite(HOLD, LOW);
  delay(1000);
  
  // Preset CS, HOLD, & VCC:
  digitalWrite(HOLD, HIGH);
  digitalWrite(CS, HIGH);
  delay(1000);
  
  // Power on SRAM, wait 1s
  digitalWrite(VCC, HIGH);
  delay(1000);

 */

