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
//% * Your group name should be "P3_<userid1>_<userid2>" (eg. P3_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : myIO.cpp
// Version     : September 28, 2021
// Description : Wrapper I/O functions for ENSC-351
// Copyright (c) 2021 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <unistd.h>         // for read/write/close
#include <fcntl.h>          // for open/creat
#include <sys/socket.h>     // for socketpair
#include <stdarg.h>         // for va stuff
#include <termios.h>        // for tcdrain()
#include <mutex>
#include <condition_variable>
#include <map>
#include <iostream>

#include "SocketReadcond.h"

using namespace std;


//ssize_t myWrite( int des, const void* buf, size_t nbyte );
//ssize_t myRead( int des, void* buf, size_t nbyte );
//int myReadcond(int des, void * buf, int n, int min, int time, int timeout);
class mySocket;

mutex mutexMap; //lock global mutex
                //protect the file from being modified by other socket pairs (one at a time)
                //therefore protect the socketMap from being modified by other threads
map<int, mySocket*> socketMap;


class mySocket {
private:
    mutex socketMutex; //member mutex
                       //protect the socket from being modified by class member functions (one at a time)
    condition_variable socket_cvDrain; //wait(), notify (all data is drained OR not enough data (<min) to read)
                                       //notify(OR for closing)
    condition_variable socket_cvRead; //wait(for write), notify (when just read)
public:
    int pair; //two connected descriptors in a reverse path
    int numBytes; //number of bytes in buffer

    /*non-default constructor*/
    mySocket(int pair){
        numBytes = 0;
        this -> pair = pair;
    }

    /*Write into the socket*/
    int socketWrite(int des, const void* buf, size_t nbyte) {
        lock_guard<mutex> socketLk(socketMutex);
        //check to see if des is in socketMap and (?)is opened
        int bytesWritten = write(des, buf, nbyte);
        numBytes += bytesWritten; //update
        if (numBytes >= 0) //notify if there are bytes written to
            socket_cvRead.notify_one();
        return bytesWritten;
    }

    /*Read into the socket (draining data)*/
    int socketReadcond(int des, void * buf, int n, int min, int time, int timeout) {
        unique_lock<mutex> socketLk(socketMutex);
        int bytesRead;
        if (numBytes >= min) { //minimum requirement to make socketRead() work
            bytesRead = wcsReadcond(des, buf, n, min, time, timeout);
            if (bytesRead != -1) { //make sure there is no error
                numBytes -= bytesRead; //data after being drained
                if (!numBytes)
                    socket_cvDrain.notify_all(); //all data is drained
            }
        }
        else { //numBytes < min
            numBytes -= min; //used to count how many bytes left to reach the minimum to call wcsReadcond
            socket_cvDrain.notify_all(); //notify that wcsReadcond needs more bytes to be written
            /*unlock member mutex, switch to another thread to write more data to buffer until reach the minimum*/

            //numBytes is now < 0
            socket_cvRead.wait(socketLk, [this]{return (numBytes >= 0);});
            bytesRead = wcsReadcond(des, buf, n, min, time, timeout);

            numBytes += min; //back to normal numBytes
            numBytes -= bytesRead; //data after being drained
        }
        return bytesRead;
    }

    int socketClose (int des){
        unique_lock<mutex> socketLk(socketMutex);
        int closeOk = close (des); // return 0 on success; -1 on failure
        if (closeOk != -1) { //close is OK
            if (numBytes < 0)
                socket_cvRead.notify_one(); //notify for thread containing myWrite() to stop waiting for data to be read
            else //numBytes >= 0
                socket_cvDrain.notify_all();

            numBytes = 0; //reset
            socketMap.erase(des); //Erase the socket from mapping
        }
        return closeOk;
    }

    int socketDrain (unique_lock<mutex>& mapLk) {
        unique_lock<mutex> socketLk(socketMutex);
        mapLk.unlock();
        socket_cvDrain.wait(socketLk, [this] {return numBytes <= 0;});
        return 0;
    }

};



/*Open the file*/
int myOpen( const char *pathname, int flags, mode_t mode ) //, mode_t mode)
{
    // in theory we should check here whether mode is needed.
    //lock_guard is not needed because there is no modification to socketMap
    return open(pathname, flags, mode); // return des
}

int myCreat( const char *pathname, mode_t mode )
{
    //lock_guard is not needed because there is no modification to socketMap
    return creat(pathname, mode); //return des
}

/* create a socket pair with 2 descriptors and put them in socketMap*/
int mySocketpair( int domain, int type, int protocol, int des_array[2] )
{
    lock_guard<mutex>mapLk(mutexMap);

    //constructors
    mySocket des0 (des_array[1]);
    mySocket des1 (des_array[0]);

    mySocket* des0Ptr = &des0;
    mySocket* des1Ptr = &des1;


    //mapping
    socketMap[des_array[0]] = des0Ptr;
    socketMap[des_array[1]] = des1Ptr;

    return socketpair(domain, type, protocol, des_array); //return des
    //returns 0 on success and -1 on failure
}

int myReadcond( int des, void * buf, int n, int min, int time, int timeout )
{
    if (socketMap.find(des) != socketMap.end()) //check to see if des is in socketMap
        return socketMap[des]->socketReadcond(des, buf, n, min, time, timeout);
    else //des is not in socketMap
        return wcsReadcond(des, buf, n, min, time, timeout);
         //should return -1, myReadcond is always in socketMap
}

ssize_t myRead( int des, void* buf, size_t nbyte )
{
   if (socketMap.find(des) != socketMap.end()) //check to see if des is in socketMap
       return myReadcond(des, buf, nbyte, 1, 0, 0);
   else //des is not in socketMap
       return read(des, buf, nbyte); //des is not in socketMap
}

ssize_t myWrite( int des, const void* buf, size_t nbyte )
{
    unique_lock<mutex> mapLk(mutexMap);

    //check to see if des is in socketMap
    if ( (socketMap.find(des) != socketMap.end()) )
        return socketMap[des]->socketWrite(des, buf, nbyte);
    else //des is not in socketMap
        return write(des, buf, nbyte);
}

int myClose( int des )
{
    lock_guard<mutex> mapLk(mutexMap); //in case socketpair() is called when closing
                                       //OR paired socket is closed at the same time
    //check to see if des is in socketMap
    if ( (socketMap.find(des) != socketMap.end()) )
        return socketMap[des]->socketClose(des);
    else //des is not in socketMap
        return close(des);
}

int myTcdrain( int des )
{
    unique_lock<mutex> mapLk(mutexMap);
    //check to see if des is in socketMap
    if ( (socketMap.find(des) != socketMap.end()) )
        return socketMap[des]->socketDrain(mapLk);
    else { //des is not in socketMap
        mapLk.unlock();
        return tcdrain(des);
    }
}

/* Arguments:
des
    The file descriptor associated with the terminal device that you want to read from.
buf
    A pointer to a buffer into which readcond() can put the data.
n
    The maximum number of bytes to read.
min, time, timeout
    When used in RAW mode, these arguments override the behavior of the MIN and TIME members of the terminal's termios structure. For more information, see...
 *
 *  https://developer.blackberry.com/native/reference/core/com.qnx.doc.neutrino.lib_ref/topic/r/readcond.html
 *
 *  */


