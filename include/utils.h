// utils.h

#ifndef AOS_PROJECT_UTILS_H
#define AOS_PROJECT_UTILS_H

#include <iostream>


#ifdef DEBUG
    #define dout std::cout << "DEBUG: "
#else
    #define dout 0 && std::cout
#endif


#endif //AOS_PROJECT_UTILS_H
