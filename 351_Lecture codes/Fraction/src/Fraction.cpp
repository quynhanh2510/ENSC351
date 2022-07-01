/*
 *  Fraction.cpp
 *
 *  Multiple threads accessing a simple fraction object
 *
 *  2019 C. Scratchley created to work with C++11/14
 *
 *  Copyright 2019, School of Engineering Science, SFU, Canada
*/

#include <iostream>
#include <signal.h>
#include <memory>
#include <sched.h>      // sched_yield()
#include <unistd.h>     /* required for sleep() */
#include <stdlib.h>     /* abs() ? */
#include <thread>
#include <mutex>

using namespace std;

mutex consoleMutex;
#define COUT (lock_guard<mutex>(consoleMutex), std::cout)

class fraction
{
public:
    unsigned num;
    unsigned denom;

    fraction(unsigned numerator, unsigned denominator) {
        num = numerator;
        denom = denominator;

    };
    void report(char id) {

        unsigned numC = num;
        unsigned denomC = denom;

        cout << id << ": Numerator is " << numC << ".  Denominator is " << denomC
                << ".  Floating representation is " <<  1.0 * numC / denomC
                << ".  Integer representation is " << numC / denomC
                << endl;

    };
    void invert() {
        int diff = num - denom; // subtract num count from denom count
        if (diff >= 0) {
//            sched_yield();
            num -= diff;        // remove from num
            denom += diff;      // and... insert into denom
        }
        else {
            unsigned adiff = abs(diff);
            denom -= adiff;     // remove from denom
            num += adiff;       // and... insert into num
        }
    };
};

//fraction fraction1(2, 3);
fraction fraction1(3, 6);

void threadFunc(char id) {
//    while (true) {
    for (int i=0; i < 3000; i++) {
        //
        fraction1.invert();
        // possibly some code here with random execution time.
        fraction1.report(id);
//        sched_yield();
    };
};

int main(int argc, char *argv[]) {
    /*  Define behaviour for divide-by-zero */
    //  SIGFPE is for integer divide-by-zero too.
    //  Single Unix Specification defines SIGFPE as "Erroneous arithmetic operation."
    //  https://stackoverflow.com/questions/6121623/catching-exception-divide-by-zero
    std::shared_ptr<void(int)> handler(
        signal(SIGFPE, [](int signum) {throw std::logic_error("FPE"); }),
        [](__sighandler_t f) { signal(SIGFPE, f); });

    fraction1.report('P');

    thread threadA(threadFunc, 'A');
    thread threadB(threadFunc, 'B');

    // sleep(1);
    threadB.join();
    threadA.join();

    cout << "Primary thread ending" << std::endl;
    return EXIT_SUCCESS;
}

