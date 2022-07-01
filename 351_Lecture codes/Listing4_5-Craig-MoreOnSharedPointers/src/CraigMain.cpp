// Adapted by Craig Scratchley from Listing 4.1
#include <thread>
#include <iostream>
#include <memory>

#include "threadsafe_queue.hpp"
#include "datachunk.h"

using namespace std;

int counter = 1;
int copyCounter = 0;

bool more_data_to_prepare()
{
    return counter <= 3;
    //return true; // false;
}

struct data_chunk2
{};

data_chunk prepare_data()
{
    return data_chunk();
}

void process(data_chunk&)
{}

bool is_last_chunk(data_chunk&)
{
    return false; // true;
}

threadsafe_queue<data_chunk> ts_queue; // renamed from data_queue

void data_preparation_thread()
{
    while(more_data_to_prepare())
    {
        data_chunk const data=prepare_data();
        ts_queue.push(data);
    }
}

void data_processing_thread()
{
//    while(true)
    {
        // examples of the use of shared pointers
        auto myRes = ts_queue.wait_and_pop();
        //auto myPtr(make_shared<data_chunk>(prepare_data()));
        //auto myPtr(new data_chunk(prepare_data()));
        auto myPtr(make_ptr(data_chunk, prepare_data()));
        data_chunk data=*myRes;
        process(data);
        process(*myPtr);
        process(*ts_queue.wait_and_pop());
        //if(is_last_chunk(data))
        //    break;

        //myPtr = myRes; // the prepared data will be destroyed
        myPtr = nullptr;
        if(myPtr) { // but myPtr is now nullptr
            std::cout << "Won't go here!" <<
            (*myPtr).i <<
            myPtr->i << std::endl;
        }
        myRes = ts_queue.try_pop();
        myRes = ts_queue.try_pop();
    }
}

int main()
{
    std::thread t2(data_processing_thread); // re-ordered thread creation
    std::cout << "Between thread creation" << std::endl; // delay next thread creation
    std::thread t1(data_preparation_thread);

    t1.join();
    t2.join();
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    cout << "copyCounter:  " << copyCounter << endl;
}
