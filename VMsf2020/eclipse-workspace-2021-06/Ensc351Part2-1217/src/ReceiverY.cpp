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
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P2_<userid1>_<userid2>" (eg. P2_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
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
#include "myIO.h"
#include "VNPE.h"
#include <iostream>
using namespace std;

enum State {FirstByteStat, FirstByteData, EOT_R, CANCELLED, END};

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
numLastGoodBlk(-1)
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
void ReceiverY::getRestBlk()
{
    PE_NOT(myReadcond(mediumD, &rcvBlk[1], REST_BLK_SZ_CRC, REST_BLK_SZ_CRC, 0, 0), REST_BLK_SZ_CRC);
    if(rcvBlk[1] == 0)
    	numLastGoodBlk = -1;
    if (rcvBlk[1] + rcvBlk[2] != 255) { //check the sum of block number (at index 1) and its complement (at index 2)
        goodBlk = goodBlk1st = syncLoss = false;
    }
    else if (rcvBlk[1] + rcvBlk[2] == 255) {
        if (rcvBlk[1] == (uint8_t) (numLastGoodBlk + 1)) { //check if current blokc number = last verified block number + 1
            syncLoss = false;

            // Using CRC to check data error
            uint16_t CRC;
            crc16ns(&CRC, &rcvBlk[DATA_POS]);
            if (*((uint16_t*) &rcvBlk[PAST_CHUNK]) == CRC) {
                goodBlk = goodBlk1st = true;
                numLastGoodBlk = rcvBlk[1];
            }
            else
                goodBlk = goodBlk1st = false;
        }
        else if (rcvBlk[1] != (uint8_t) (numLastGoodBlk + 1)) {
            goodBlk1st = false;
            // check loss of synchronization
            if((rcvBlk[1] == numLastGoodBlk)) {//check if current block number = last verified block number
                syncLoss = false;
                goodBlk = true;///////////////////
            }
            else if ((rcvBlk[1] != numLastGoodBlk)) {
                syncLoss = true;
                goodBlk = false;
            }
        }
    }
}

//Write chunk (data) in a received block to disk
void ReceiverY::writeChunk()
{
	PE_NOT(myWrite(transferringFileD, &rcvBlk[DATA_POS], CHUNK_SZ), CHUNK_SZ);
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
    myClose(transferringFileD);
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

uint8_t
/*check if rcvBlk[3] is NULL,
 * if so it reaches to the end of the batch transmission,
 * and break out of the while loop (
 */
ReceiverY::
checkForAnotherFile()
{
    return (anotherFile = rcvBlk[DATA_POS]);
}

void ReceiverY::receiveFiles()
{
    ReceiverY& ctx = *this; // needed to work with SmartState-generated code


    // ***** improve this member function *****

    // below is just an example template.  You can follow a
    //  different structure if you want.

    //Entry Code:
    ctx.sendByte(ctx.NCGbyte);
    ctx.errCnt = 0;
    transferringFileD = -1;
    State state = FirstByteStat;

    while(state != END) {
        PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
        switch(state) {
            case FirstByteStat: {
                if (rcvBlk[0] == SOH) {
                    ctx.getRestBlk();
                    //CondTransientStat
                    if(!ctx.syncLoss && (ctx.errCnt < errB) && ctx.goodBlk) {
                        //CondTransientCheck
                        ctx.checkForAnotherFile();
                        if(ctx.anotherFile){
                            ctx.openFileForTransfer();
                            //CondTransientOpen
                            if(transferringFileD == -1) {
                                cans();
                                ctx.result = "CreatError";
                                state = END;
                            }
                            else if (transferringFileD != -1) {
                                ctx.sendByte(ACK);
                                ctx.sendByte(ctx.NCGbyte);
                                state = FirstByteData;
                            }
                        }
                        else if (!ctx.anotherFile) {
                            //ctx.sendByte(ACK);
                            ctx.result = "Done";
                            state = END;
                        }
                    }
                    if(!ctx.syncLoss && (ctx.errCnt < errB) && !ctx.goodBlk) {
                        ctx.sendByte(NAK);
                        ctx.errCnt++;
                        state = FirstByteStat;
                    }
                    else if(ctx.syncLoss || (ctx.errCnt >= errB)) {
                        cans();
                        if(ctx.syncLoss)
                            ctx.result = "LossOfSyncronization";
                        else
                            ctx.result = "ExcessiveErrors";
                        state = END;
                    }
                }
                else if (rcvBlk[0] == CAN)
                    state = CANCELLED;
                else //unexpected error
                    break;
                continue;
            }

            case FirstByteData: {
                if (rcvBlk[0] == SOH) {
                    ctx.getRestBlk();
                    if(ctx.goodBlk1st) {
                        ctx.errCnt = 0;
                        ctx.anotherFile = 0; //reset
                    }
                    else
                        ctx.errCnt++;
                    //CondTransientData
                    if(!ctx.syncLoss && (ctx.errCnt < errB)) {
                        if(ctx.goodBlk) {
                            ctx.sendByte(ACK);
                            if(ctx.anotherFile)
                                ctx.sendByte(ctx.NCGbyte);
                        }
                        else
                            ctx.sendByte(NAK);
                        if(ctx.goodBlk1st)
                            ctx.writeChunk();
                        state = FirstByteData;
                    }
                    else if(ctx.syncLoss || (ctx.errCnt >= errB)) {
                        cans();
                        ctx.closeTransferredFile();
                        if(ctx.syncLoss)
                            ctx.result = "LossOfSyncronization";
                        else
                            ctx.result = "ExcessiveErrors";
                        state = END;
                    }
                }
                else if(rcvBlk[0] == EOT) {
                    ctx.sendByte(NAK);
                    state = EOT_R;
                }
                else if (rcvBlk[0] == CAN)
                    state = CANCELLED;
                else //unexpected error
                    break;
                continue;
            }

            case EOT_R: {
                if(rcvBlk[0] == EOT) {
                    ctx.closeTransferredFile();
                    //CondTransientEOT
                    if(ctx.closedOk) {
                        ctx.sendByte(ACK);
                        ctx.sendByte(ctx.NCGbyte);
                        ctx.errCnt = 0;
                        state = FirstByteStat;
                    }
                    else if(!ctx.closedOk) {
                        ctx.cans();
                        ctx.result = "CloseError";
                        state = END;
                    }
                    continue;
                }
                break;
            }

            case CANCELLED: {
               if(rcvBlk[0] == CAN) {
                   if (transferringFileD != -1) {
                       ctx.closeTransferredFile();
                   }
                   ctx.result = "SndCancelled";
                   state = END;
                   continue;
               }
               break;
            }

            case END:
                continue;
        }//out of switch

        cerr << "Sender received totally unexpected char #" << rcvBlk[0] << ": " << (char) rcvBlk[0] << endl;
        exit(EXIT_FAILURE);

    }//out of while loop

sendByte(ACK); // acknowledge empty block 0.

}
