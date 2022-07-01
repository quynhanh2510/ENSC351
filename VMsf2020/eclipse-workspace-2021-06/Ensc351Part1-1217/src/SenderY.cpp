//============================================================================
//
//% Student Name 1: Thanh Huy  Ho
//% Student 1 #: 301385295
//% Student 1 userid (email): thh1 (thh1@sfu.ca)
//
//% Student Name 2: Ngoc Quynh Anh  Vo
//% Student 2 #: 301391358
//% Student 2 userid (email): vongocv (vongocv@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: Craig
//
// Also, list any resources beyond the course textbooks and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  //https://www.geeksforgeeks.org/how-to-convert-c-style-strings-to-stdstring-and-vice-versa/
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P1_<userid1>_<userid2>" (eg. P1_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : SenderY.cpp
// Version     : September 3rd, 2021
// Description : Starting point for ENSC 351 Project
// Original portions Copyright (c) 2021 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include "SenderY.h"

#include <iostream>
#include <stdint.h> // for uint8_t
#include <string.h> // for memset(), memcpy()
#include <errno.h>
#include <fcntl.h>  // for O_RDWR
#include <sys/stat.h>
#include <math.h>
#include <string>

#include "myIO.h"

using namespace std;

SenderY::
SenderY(vector<const char*> iFileNames, int d)
:PeerY(d),
 bytesRd(-1), 
 fileNames(iFileNames),
 fileNameIndex(0),
 blkNum(0)
{
}

//-----------------------------------------------------------------------------

/* generate a block (numbered 0) with filename and filesize */
//void SenderY::genStatBlk(blkT blkBuf, const char* fileName)
void SenderY::genStatBlk(uint8_t blkBuf[BLK_SZ_CRC], const char* fileName)
{
    // 0eth Block format: <SOH><blk #><255-blk#><fileName><NULL><fileSize><NULL[remaining_spot]><MSB_CRC><LSB_CRC>

    // ********* additional code must be written ***********
    blkBuf[0] = SOH;
    if(blkNum > 255)
        blkNum = 0;
    blkBuf[1] = blkNum;
    blkBuf[2] = 255 - blkNum; //blkNum > 255, set blkNum=0

        struct stat st;
        stat(fileName, &st);

        unsigned fileSize = st.st_size; //HINT from Craig

    //**fileName**
    const char* baseFileName = basename(fileName);
    //memcpy(dest, fileName, sizeof(filesize))
    memcpy (&blkBuf[DATA_POS], baseFileName, strlen(baseFileName) + 1); // +1 is for the NUL character after the filename

    //**fileSize**
    string str = to_string(fileSize); //Convert int to std::string
    char* charSize =  &(str[0]);      //Convert std::string to C-string

    //memcpy(dest, fileSize, sizeof(filesize))
    memcpy (&blkBuf[DATA_POS + strlen(baseFileName) + 1], charSize, strlen(charSize) + 1);

    //**fill up NUL**
    for(int i = DATA_POS + strlen(baseFileName) + strlen(charSize) + 2; i < PAST_CHUNK; i++)
        blkBuf[i] = '\0'; // NUL

    /* calculate and add CRC in network byte order */
    // ********* The next couple lines need to be changed ***********
    uint16_t myCrc16ns;
    crc16ns(&myCrc16ns, &blkBuf[DATA_POS]);

    blkBuf[PAST_CHUNK] = myCrc16ns >> 8; //MSB
    blkBuf[PAST_CHUNK + 1] = myCrc16ns;  //LSB


}

/* tries to generate a block.  Updates the
variable bytesRd with the number of bytes that were read
from the input file in order to create the block. Sets
bytesRd to 0 and does not actually generate a block if the end
of the input file had been reached when the previously generated block
was prepared or if the input file is empty (i.e. has 0 length).
*/
//void SenderY::genBlk(blkT blkBuf)
void SenderY::genBlk(uint8_t blkBuf[BLK_SZ_CRC])
{
    // Block format: <SOH><blk #><255-blk#><128 bytes><MSB_CRC><LSB_CRC>

    // ********* The next line needs to be changed ***********
    if (-1 == (bytesRd = myRead(transferringFileD, &blkBuf[DATA_POS], CHUNK_SZ )))
        ErrorPrinter("myRead(transferringFileD, &blkBuf[DATA_POS], CHUNK_SZ )", __FILE__, __LINE__, errno);
    // ********* and additional code must be written ***********

    blkBuf[0] = SOH;
    if(blkNum > 255)
        blkNum = 0;
    blkBuf[1] = blkNum;
    blkBuf[2] = 255 - blkNum; //blkNum > 255, set blkNum=0
    //128-byte data
    if(bytesRd != 128)  //incomplete block with less than 128 bytes
    {
        for(int i = bytesRd + 3; i < PAST_CHUNK; i++)
        {
                blkBuf[i] = CTRL_Z; //Fill the empty space with CPMEOF
        }
    }

        /* calculate and add CRC in network byte order */
        // ********* The next couple lines need to be changed ***********
        uint16_t myCrc16ns;
        crc16ns(&myCrc16ns, &blkBuf[DATA_POS]);

        blkBuf[PAST_CHUNK] = myCrc16ns >> 8; //MSB
        blkBuf[PAST_CHUNK + 1] = myCrc16ns; //LSB

    // ...
}

void SenderY::cans()
{
    // ********* send CAN_LEN of CAN characters ***********
    for(int i = 0; i < CAN_LEN; i++)
        sendByte(CAN);
}

//uint8_t SenderY::sendBlk(blkT blkBuf)
void SenderY::sendBlk(uint8_t blkBuf[BLK_SZ_CRC])
{
    // ********* fill in some code here to send a block (write to mediumD) ***********
    myWrite(mediumD, blkBuf, BLK_SZ_CRC);

}

void SenderY::statBlk(const char* fileName)
{
    blkNum = 0;
    // assume 'C' received from receiver to enable sending with CRC
    genStatBlk(blkBuf, fileName); // prepare 0eth block
    sendBlk(blkBuf); // send 0eth block
    // assume sent block will be ACK'd
}

void SenderY::sendFiles()
{
    for (auto fileName : fileNames) {
        // const char* fileName = fileNames[fileNameIndex];
        transferringFileD = myOpen(fileName, O_RDWR, 0);
        if(transferringFileD == -1) {
            // ********* fill in some code here to write 2 CAN characters ***********
            cans();
            cout /* cerr */ << "Error opening input file named: " << fileName << endl;
            result = "OpenError";
            return;
        }
        else {
            cout << "Sender will send " << fileName << endl;

            // do the protocol, and simulate a receiver that positively acknowledges every
            //  block that it receives.

            statBlk(fileName); //for 0eth block
            blkNum ++;
            // assume 'C' received from receiver to enable sending with CRC
            genBlk(blkBuf); // prepare 1st block
            while (bytesRd)
            {
                blkNum ++; // 1st block about to be sent or previous block was ACK'd

                sendBlk(blkBuf); // send block

                // assume sent block will be ACK'd
                genBlk(blkBuf); // prepare next block
                // assume sent block was ACK'd
            };
            // finish up the file transfer, assuming the receiver behaves normally and there are no transmission errors
            // ********* fill in some code here ***********
            sendByte(EOT);
            sendByte(EOT);

            //(myClose(transferringFileD));
            if (-1 == myClose(transferringFileD))
                ErrorPrinter("myClose(transferringFileD)", __FILE__, __LINE__, errno);
        }
    }
    // indicate end of the batch.
    statBlk("");

    result = "Done";
}

