// Adapted by Craig Scratchley from Listing 4.1
#include <thread>
#include <iostream>
#include "threadsafe_queue.hpp"

bool more_data_to_prepare()
{
    return true; // false;
}

struct data_chunk
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
    while(true)
    {
        data_chunk data=*ts_queue.wait_and_pop();
        process(data);
        if(is_last_chunk(data))
            break;
    }
}

int main()
{
    std::thread t2(data_processing_thread); // re-ordered thread creation
    std::cout << "Between thread creation" << std::endl; // delay next thread creation
    std::thread t1(data_preparation_thread);

    t1.join();
    t2.join();
}
