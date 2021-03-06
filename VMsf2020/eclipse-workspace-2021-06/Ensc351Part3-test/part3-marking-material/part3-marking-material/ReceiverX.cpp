//============================================================================
// File Name   : ReceiverX.cpp
// Version     : October, 2020
// Description : A solution for ENSC 351 Project Part 2
// Copyright (c) 2020 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <string.h> // for memset()
#include <fcntl.h>
#include <stdint.h>
#include <memory>
#include "myIO.h"
#include "ReceiverX.h"
#include "ReceiverSS.h"
#include "VNPE.h"
#include "AtomicCOUT.h"

using namespace std;
using namespace Receiver_SS;

ReceiverX::
ReceiverX(int d, const char *fname, bool useCrc)
:PeerX(d, fname, useCrc), 
NCGbyte(useCrc ? 'C' : NAK),
goodBlk(false), 
goodBlk1st(false), 
syncLoss(false), // transfer will end when syncLoss becomes true
numLastGoodBlk(0),
firstBlock(true)
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
void ReceiverX::getRestBlk()
{
	const int restBlkSz = Crcflg ? REST_BLK_SZ_CRC : REST_BLK_SZ_CS;
	PE_NOT(myReadcond(mediumD, &rcvBlk[1], restBlkSz, restBlkSz, 0, 0), restBlkSz);
	// consider receiving checksum/CRC after calculating local checksum/CRC

	syncLoss = false; // but might be made true below

	//(blkNumsOk) = ( block # and its complement are matched );
	bool blkNumsOk = (rcvBlk[2] == (255 - rcvBlk[1]));
	if (!blkNumsOk) {
		goodBlk = goodBlk1st = false;
	}
	else {
		goodBlk1st = (rcvBlk[1] == (uint8_t) (numLastGoodBlk + 1)); // but might be made false below
		if (!goodBlk1st) {
			// determine fatal loss of synchronization
			if (firstBlock || (rcvBlk[1] != numLastGoodBlk)) {
				syncLoss = true;
				goodBlk = false;
				COUT << "(s" << (unsigned) rcvBlk[1] << ":" << (unsigned) numLastGoodBlk << ")" << flush;
				return;
			}
#define ALLOW_DEEMED_GOOD
#ifdef ALLOW_DEEMED_GOOD
			else { // (rcvBlk[1] == numLastGoodBlk)
				goodBlk = true; // "deemed" good block
				COUT << "(d" << (unsigned) rcvBlk[1] << ")" << flush;
				return;
			}
#endif
		}
		// detect if data error in chunk
		// consider receiving checksum/CRC after calculating local checksum/CRC
		if (Crcflg) {
			uint16_t CRCbytes;
			crc16ns(&CRCbytes, &rcvBlk[DATA_POS]);
			goodBlk = (*((uint16_t*) &rcvBlk[PAST_CHUNK]) == CRCbytes);
		}
		else {
			uint8_t sum;
			checksum(&sum, rcvBlk);
			goodBlk = (rcvBlk[PAST_CHUNK] == sum);
		}
		if (!goodBlk) {
			goodBlk1st = false; // but the block was "bad".
			COUT << "(b" << (unsigned) rcvBlk[1] << ")" << flush;
			return;
		}
#ifndef ALLOW_DEEMED_GOOD
		else if (!goodBlk1st) {
			COUT << "(r" << (unsigned) rcvBlk[1] << ")" << flush; // "resent" good block
			return;
		}
#endif
		// good block for the "first" time.
		numLastGoodBlk = rcvBlk[1];
		firstBlock = false; // if that was "first" block, next blocks won't be
		COUT << "(f" << (unsigned) rcvBlk[1] << ")" << endl;
	}
}

//Write chunk (data) in a received block to disk
void ReceiverX::writeChunk()
{
	PE_NOT(myWrite(transferringFileD, &rcvBlk[DATA_POS], CHUNK_SZ), CHUNK_SZ);
}

int
ReceiverX::
openFileForTransfer()
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    transferringFileD = myCreat(fileName, mode);
    return transferringFileD;
}

int
ReceiverX::
closeTransferredFile()
{
    return myClose(transferringFileD);
}

//Send CAN_LEN CAN characters in a row to the XMODEM sender, to inform it of
//	the cancelling of a file transfer
void ReceiverX::can8()
{
	// no need to space in time CAN chars coming from receiver
    char buffer[CAN_LEN];
    memset( buffer, CAN, CAN_LEN);
    PE_NOT(myWrite(mediumD, buffer, CAN_LEN), CAN_LEN);
}

void ReceiverX::receiveFile()
{
//	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
//	transferringFileD = PE2(myCreat(fileName, mode), fileName);

	auto myReceiverSmSp(make_shared<ReceiverSS>(this)); // or use make_unique
	myReceiverSmSp->setDebugLog(NULL);

	// Because one source of receiver behaviour is automatically generated by SmartState Studio,
	//	the behaviour has been factored out into a class ReceiverSS, but for your Part 2 submission
	//	we are not expecting a separate class as has been done here.
	while(myReceiverSmSp->isRunning()) {
		unsigned char byteToReceive;
		PE(myRead(mediumD, &byteToReceive, 1));
		myReceiverSmSp->postEvent(SER, byteToReceive);
	}

	COUT << "\n"; // insert new line.
}
