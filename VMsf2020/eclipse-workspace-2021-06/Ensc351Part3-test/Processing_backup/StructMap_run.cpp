//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__
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

struct mySocket{
    int paired; // the other socket (des' partner)
    bool isOpen; // status of partner (i.e: if mySocket has des = 0, its partner socket has des = 1)
    int numBytes; //number of bytes in buffer
};

//map vars
mutex mutexSocketInMap; //this mutex lock protects global variable
map<int, mySocket> socketMap;
condition_variable cv;

bool desIsOpenSocket(int des)
    {
    std::lock_guard<std::mutex> mapLk(mutexSocketInMap);
    return (socketMap.find(des) != socketMap.end()) && (socketMap[des].isOpen);
    }

bool desIsSocket(int des)
    {
    std::lock_guard<std::mutex> mapLk(mutexSocketInMap);
    return socketMap.find(des) != socketMap.end();
    }

int myOpen( const char *pathname, int flags, ... ) //, mode_t mode)
{
    mode_t mode = 0;
    // in theory we should check here whether mode is needed.
    va_list arg;
    va_start (arg, flags);
    mode = va_arg (arg, mode_t);
    va_end (arg);
    return open(pathname, flags, mode);
}

int myCreat( const char *pathname, mode_t mode )
{
    return creat(pathname, mode);
}

/* create a socket pair with 2 descriptors and put them in socketMap*/
int mySocketpair( int domain, int type, int protocol, int des_array[2] )
{
    int returnVal = socketpair(domain, type, protocol, des_array);

    lock_guard<mutex> mapLk(mutexSocketInMap); //lock global mutex

    //desciptors
    mySocket des0 = {.paired = des_array[1], .isOpen = true, .numBytes = 0};
    mySocket des1 = {.paired = des_array[0], .isOpen = true, .numBytes = 0};

    //mapping
    socketMap[des_array[0]] = des0;
    socketMap[des_array[1]] = des1;

    return returnVal;
}

/*call wcsReadcond() to read from partner socket */
int myReadcond( int des, void * buf, int n, int min, int time, int timeout )
{
    int bytesRead = 0;
    int numBytesRead = 0;
    //des is not in socketMap OR socket partner is not openning
    if ( (socketMap.find(des) == socketMap.end()) || !(socketMap[des].isOpen) ) { //error case
        bytesRead = wcsReadcond(des, buf, n, min, time, timeout); //return -1;
    }
    else if (min == 0) {
        bytesRead = wcsReadcond(des, buf, n, min, time, timeout);
        if (bytesRead != -1) {
            lock_guard<mutex> mapLk(mutexSocketInMap); //lock global mutex
            socketMap[socketMap[des].paired].numBytes -= bytesRead;
        }
    }
    else {
        do {
            unique_lock<mutex> mapLk(mutexSocketInMap); //lock global mutex

            //If there is something in the buffer to be read or our socket pair gets closed
            cv.wait(mapLk, [des]{return socketMap[socketMap[des].paired].numBytes > 0 || !socketMap[des].isOpen;});

            //Read the output
            numBytesRead = wcsReadcond(des, ((char*) buf) + bytesRead , n, 1, time, timeout);

            bytesRead  += numBytesRead; //Increment return value by the number of bytes read

            //Modify the number of bytes in the socket to match the current state
            if(numBytesRead != -1)
                socketMap[socketMap[des].paired].numBytes -= numBytesRead;

            //Notify our tcdrain() cond_varaible to check again
            cv.notify_all();

            } while(bytesRead < min && (lock_guard<mutex> (mutexSocketInMap), socketMap[des].isOpen));
    }

    return bytesRead;
}

/* Read from a file or call myReadcond() to read from socket*/
ssize_t myRead( int des, void* buf, size_t nbyte )
{
   ssize_t res = -1;       //Assume error

   if (desIsSocket(des)) //check to see if des in socketMap
       res = myReadcond(des, buf, nbyte, 1, 0, 0);
   else //des is not in socketMap
       res = read(des, buf, nbyte);
   return res;
}

ssize_t myWrite( int des, const void* buf, size_t nbyte )
{
    ssize_t rv;         //return value

    if ( desIsOpenSocket(des) ){
        lock_guard<mutex> mapLk(mutexSocketInMap); //lock global mutex
        rv = write(des, buf, nbyte);
        if ( rv != -1) //returns 0 on success and -1 on failure
            socketMap[des].numBytes += nbyte; //update
        cv.notify_all();
        //return write(des, buf, nbyte);
    }
    else
        rv = write(des, buf, nbyte);
    return rv;
}

int myClose( int des )
{
    if ( desIsOpenSocket(des) ){
        lock_guard<mutex> mapLk(mutexSocketInMap); //lock global mutex
        if (socketMap[des].isOpen) {
            socketMap[socketMap[des].paired].isOpen = false; //change partner's status
            cv.notify_all(); //notify that it is closing so that myRead dont need to wait
        }
        socketMap.erase(des);
    }
    return close(des);
}

int myTcdrain( int des )
{ //is also included for purposes of the course.
    int rv;         //return value

    if ( desIsOpenSocket(des) ){
        unique_lock<mutex> mapLk(mutexSocketInMap); //lock global mutex
        //wait until all the data from buffer has been drained or socket is closed
        cv.wait( mapLk, [des]{return socketMap[des].numBytes <= 0 || !socketMap[des].isOpen;} );
        rv = 0;
    }
    else
        rv = tcdrain(des); //Use standard tcdrain() for file descriptors
    return rv;
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


