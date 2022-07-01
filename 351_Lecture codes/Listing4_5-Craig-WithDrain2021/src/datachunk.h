/*
 * datachunk.h
 *
 *  Created on: Oct 8, 2019
 *      Author: osboxes
 */

#ifndef DATACHUNK_H_
#define DATACHUNK_H_

extern int counter;
extern int copyCounter;

class data_chunk {
public:
    int i;
    data_chunk() : i(counter) {
        std::cout << "Constructor: " << i << std::endl;
        ++counter;
        ++copyCounter;
    };
    data_chunk(const data_chunk &d0) : i(d0.i) {
        std::cout << "Copy Constructor: " << i << std::endl;
        ++copyCounter;
    };

    virtual ~data_chunk() {
        std::cout << "Destructor: " << i << std::endl;
        --copyCounter;
    };
};

#endif /* DATACHUNK_H_ */
