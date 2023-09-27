// Import Libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Maximum number of Records in a Data Block {400B / 23B} shud be 17, but I use 10 to prevent memory issues for now
#define MAX_RECORDS 10
// Maximum number of Blocks (500MB / 400B)
#define MAX_BLOCKS 1250000

typedef struct record {
    // Record Header is a bit array of 9 bits to correspond to each of the columns
    // unsigned char recordHeader[9];
    short int recorderHeader;

    // Field Headers for the 9 Attributes
    unsigned char gameDateEstBitArray[((10 + 7) / 8)]; // Refers to the Number of Bytes

    unsigned char teamIdHomeBitArray[((31 + 7) / 8)];

    unsigned char ptsHomeBitArray[((1 + 7) / 8)];

    // Primary Key; Float
    unsigned char fgPctHomeBitArray[((11 + 7) / 8)];

    // Requires 11 bits
    unsigned char ftPctHomeBitArray[((11 + 7) / 8)];

    // Requires 11 bits
    unsigned char fg3PctHomeBitArray[((11 + 7) / 8)];

    unsigned char astHomeBitArray[((1 + 7) / 8)];

    unsigned char rebHomeBitArray[((1 + 7) / 8)];

    bool homeTeamWins;
} Record;


typedef struct dataBlock {
    int numRecords;
    Record records[MAX_RECORDS];

    // I don't think we can use hashmap since it is unordered
    // struct hashmap {
    //     float key;
    //     Record *value;  // Pointer to the actual record
    // } map[MAX_RECORDS];

} DataBlock;

typedef struct blockManager {
    // Attributes
    int numDataBlocks = 0;
    int numRecords = 0;
    DataBlock *listBlocks[MAX_BLOCKS];     // Array of DataBlocks

} BlockManager;