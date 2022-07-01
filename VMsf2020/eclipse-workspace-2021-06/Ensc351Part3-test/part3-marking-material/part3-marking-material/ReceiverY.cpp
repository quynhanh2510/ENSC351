//============================================================================
//
//% Colton Koop
//% 301380869
//% ckoop (ckoop@sfu.ca)
//
// File Name   : ReceiverY.cpp
// Version     : September 24th, 2021
// Description : Starting point for ENSC 351 Project Part 2
// Original portions Copyright (c) 2021 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include "ReceiverY.h"

#include <string.h> // for memset()
#include <fcntl.h>
#include <stdint.h>
#include <iostream>
#include "myIO.h"
#include "VNPE.h"

//using namespace std;

ReceiverY::
ReceiverY(int d)
:PeerY(d),
closedOk(1),
anotherFile(0xFF),
NCGbyte('C'),
goodBlk(false), 
goodBlk1st(false), 
syncLoss(false), // transfer will end when syncLoss becomes true
numLastGoodBlk(0xFF),
currentState(ST_FIRSTBYTESTAT),
fileSize(0),
bytesWritten(0)
{
}

/* Only called after an SOH character has been received.
The function receives the remaining characters to form a complete
block.
The function will set or reset a Boolean variable,
goodBlk. This variable will be set (made true) only if the
calculated checksum or CRC agrees with the
one received and the received block number and received complement
are consistent with each other.
Boolean member variable syncLoss will only be set to
true when goodBlk is set to true AND there is a
fatal loss of syncronization as described in the XMODEM
specification.
The member variable goodBlk1st will be made true only if this is the first
time that the block was received in "good" condition. Otherwise
goodBlk1st will be made false.
*/

// ********* this function must be improved ***********
void ReceiverY::getRestBlk()
{
    PE_NOT(myReadcond(mediumD, &rcvBlk[1], REST_BLK_SZ_CRC, REST_BLK_SZ_CRC, 0, 0), REST_BLK_SZ_CRC);
    //@@@@@@@@@@@@@@@@@@@@
    uint8_t recalcCRC[2];
    crc16ns((uint16_t*)&recalcCRC[0], &rcvBlk[DATA_POS]);
    if(recalcCRC[0]==rcvBlk[PAST_CHUNK] && recalcCRC[1]==rcvBlk[PAST_CHUNK+1] && rcvBlk[SOH_OH]==(uint8_t)~rcvBlk[SOH_OH+1]){
        goodBlk = true;
        if(rcvBlk[SOH_OH]==(uint8_t)(numLastGoodBlk+0x01)){
            syncLoss = false;
            goodBlk1st = true;
        }
        else if(rcvBlk[SOH_OH]==numLastGoodBlk){
            syncLoss = false;
            goodBlk1st = false;
        }
        else{ //unexpected block #
            syncLoss = true;
        }
        numLastGoodBlk = rcvBlk[SOH_OH];
    }
    else{ //block corruption detected
        goodBlk = false;
        syncLoss = false;
        goodBlk1st = false;
    }
    //@@@@@@@@@@@@@@@@@@@@
}

//Write chunk (data) in a received block to disk
void ReceiverY::writeChunk()
{
    if((unsigned)(fileSize-bytesWritten) <= 128){
        unsigned bytesLeft = (unsigned)(fileSize-bytesWritten);
        PE_NOT(myWrite(transferringFileD, &rcvBlk[DATA_POS], bytesLeft), bytesLeft);
        bytesWritten += bytesLeft;
    }
    else{
        PE_NOT(myWrite(transferringFileD, &rcvBlk[DATA_POS], CHUNK_SZ), CHUNK_SZ);
        bytesWritten += CHUNK_SZ;
    }

}

int
ReceiverY::
openFileForTransfer()
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    transferringFileD = myCreat((const char *) &rcvBlk[DATA_POS], mode);
    return transferringFileD;
}

int
ReceiverY::
closeTransferredFile()
{
    closedOk = myClose(transferringFileD);
    return closedOk; // or return 0 or errno
}

//Send CAN_LEN CAN characters in a row to the XMODEM sender, to inform it of
//	the cancelling of a file transfer
void ReceiverY::cans()
{
	// no need to space in time CAN chars coming from receiver
    char buffer[CAN_LEN];
    memset( buffer, CAN, CAN_LEN);
    PE_NOT(myWrite(mediumD, buffer, CAN_LEN), CAN_LEN);
}

uint8_t ReceiverY::checkForAnotherFile()
{
    return (anotherFile = rcvBlk[DATA_POS]);
}


void ReceiverY::receiveFiles()
{
    //entry code
    errCnt = 0;
    transferringFileD = -1;
    sendByte(NCGbyte);
    currentState = ST_FIRSTBYTESTAT;
    fileSize = 0;
    bytesWritten = 0;

    //finite state machine
    while(true){ //using return statements to exit
        switch (currentState) {
           case ST_FIRSTBYTESTAT://done//SER waiting state
               PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
               if(rcvBlk[0] == SOH){
                   numLastGoodBlk = 0xFF;//need to reset so leftovers from previous file don't trigger syncloss
                   getRestBlk();
                   currentState = ST_CONDLTRANSIENTSTAT;
               } else if (rcvBlk[0] == CAN) {
                   currentState = ST_CAN;
               } else {//totally unexpected input
                   char c = rcvBlk[0];
                   std::cout << "Receiver received totally unexpected char #" << c << ": " << (char) c << std::endl;
                   exit(EXIT_FAILURE);
               }
               break;
           case ST_CONDLTRANSIENTSTAT://done
               if(!syncLoss && (errCnt<errB) && !goodBlk){
                   sendByte(NAK);
                   errCnt++;
                   currentState = ST_FIRSTBYTESTAT;
               } else if (!syncLoss && (errCnt<errB) && goodBlk){
                   checkForAnotherFile();
                   currentState = ST_CONDTRANSIENTCHECK;
               } else if (syncLoss || errCnt>=errB){
                   cans();
                   syncLoss ? result="LossOfSync at Stat Blk" : result="ExcessiveErrors at Stat";
                   return; //using return to exit the state machine
               }
               break;
           case ST_CONDTRANSIENTCHECK://done
               if(anotherFile){
                   openFileForTransfer();
                   currentState = ST_CONDTRANSIENTOPEN;
               } else { //!anotherFile
                   sendByte(ACK);
                   if(myReadcond(mediumD, rcvBlk, 1, 1, 10, 10) == 0){ //sender received the ACK okay
                       result="Done";
                       return;
                   }
                   else if(rcvBlk[0] == SOH){
                       numLastGoodBlk = 0xFF;//need to reset so leftovers from previous file don't trigger syncloss
                       getRestBlk();
                       currentState = ST_CONDLTRANSIENTSTAT;
                   }
                   else if (rcvBlk[0] == CAN) {
                      currentState = ST_CAN;
                   }
                   else {//totally unexpected input
                      char c = rcvBlk[0];
                      std::cout << "Receiver received totally unexpected char #" << c << ": " << (char) c << std::endl;
                      exit(EXIT_FAILURE);
                   }
               }
               break;
           case ST_CONDTRANSIENTOPEN://done
               if(transferringFileD!=-1){
                   //extract the filesize from the stat block and reset bytesWritten
                   bytesWritten = 0;
                   unsigned fileNameSize = (unsigned)strlen((char*)&rcvBlk[3]);
                   sscanf((char*)&rcvBlk[3+fileNameSize+1],"%u", &fileSize);
                   ///////////////////////
                   sendByte(ACK);
                   sendByte(NCGbyte);
                   currentState = ST_FIRSTBYTEDATA;
               } else { //transferringFileD=-1
                   cans();
                   result="CreatError";
                   return;
               }
               break;
           case ST_FIRSTBYTEDATA://done//SER waiting state
               PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
               if(rcvBlk[0] == SOH){
                   getRestBlk();
                   if(goodBlk1st){
                       errCnt = 0;
                       anotherFile = 0;
                   } else {
                       errCnt++;
                   }
                   currentState = ST_CONDTRANSIENTDATA;
               } else if (rcvBlk[0] == EOT){
                   sendByte(NAK);
                   currentState = ST_EOT;
               } else if (rcvBlk[0] == CAN){
                   currentState = ST_CAN;
               } else {//totally unexpected input
                   char c = rcvBlk[0];
                   std::cout << "Receiver received totally unexpected char #" << c << ": " << (char) c << std::endl;
                   exit(EXIT_FAILURE);
               }
               break;
           case ST_CONDTRANSIENTDATA://done
               if(!syncLoss && (errCnt<errB)){
                   if(goodBlk){
                       sendByte(ACK);
                       if(anotherFile){sendByte(NCGbyte);}
                   } else {sendByte(NAK);}
                   if(goodBlk1st){
                       writeChunk();
                   }
                   currentState = ST_FIRSTBYTEDATA;
               } else if (syncLoss || errCnt >= errB){
                   cans();
                   closeTransferredFile();
                   syncLoss ? result="LossOfSyncronization" : result="ExcessiveErrors";
                   return;
               }
               break;
           case ST_EOT://done//SER waiting state
               PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
               if(rcvBlk[0] == EOT){
                   closeTransferredFile();
                   currentState = ST_CONDTRANSIENTEOT;
               } else if (rcvBlk[0] == CAN){
                   currentState = ST_CAN;
               } else {//totally unexpected input
                   char c = rcvBlk[0];
                   std::cout << "Receiver received totally unexpected char #" << c << ": " << (char) c << std::endl;
                   exit(EXIT_FAILURE);
               }
               break;
           case ST_CONDTRANSIENTEOT://done
               if(closedOk == 0){//0 means success
                   sendByte(ACK);
                   sendByte(NCGbyte);
                   errCnt = 0;
                   currentState = ST_FIRSTBYTESTAT;
               } else { // !closedOk
                   cans();
                   result = "CloseError";
                   return;
               }
               break;
           case ST_CAN://done//SER waiting state
               PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
               if(rcvBlk[0] == CAN){
                   if(transferringFileD != -1){closeTransferredFile();}
                   result = "SndCancelled";
                   return;
               } else {//totally unexpected input
                   char c = rcvBlk[0];
                   std::cout << "Receiver received totally unexpected char #" << c << ": " << (char) c << std::endl;
                   exit(EXIT_FAILURE);
               }
               break;
        }
    }
}

/*void ReceiverY::receiveFiles()
{
    ReceiverY& ctx = *this; // needed to work with SmartState-generated code

    // ***** improve this member function *****

    // below is just an example template.  You can follow a
    //  different structure if you want.

    while ( sendByte(ctx.NCGbyte),
            PE_NOT(myRead(mediumD, rcvBlk, 1), 1), // Should be SOH
            ctx.getRestBlk(), // get block 0 with fileName and filesize
            checkForAnotherFile()) {

        if(openFileForTransfer() == -1) {
            cans();
            result = "CreatError"; // include errno or so
            return;
        }
        else {
            sendByte(ACK); // acknowledge block 0 with fileName.

            // inform sender that the receiver is ready and that the
            //      sender can send the first block
            sendByte(ctx.NCGbyte);

            while(PE_NOT(myRead(mediumD, rcvBlk, 1), 1), (rcvBlk[0] == SOH))
            {
                ctx.getRestBlk();
                ctx.sendByte(ACK); // assume the expected block was received correctly.
                ctx.writeChunk();
            };
            // assume EOT was just read in the condition for the while loop
            ctx.sendByte(NAK); // NAK the first EOT
            PE_NOT(myRead(mediumD, rcvBlk, 1), 1); // presumably read in another EOT

            // Check if the file closed properly.  If not, result should be "CloseError".
            if (ctx.closeTransferredFile()) {
                ; // ***** fill this in *****
            }
            else {
                 ctx.sendByte(ACK);  // ACK the second EOT
                 ctx.result="Done";
            }
        }
    }
    sendByte(ACK); // acknowledge empty block 0.
}*/
