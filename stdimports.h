//
// Created by bknun on 1/6/2017.
//

#ifndef SHARP_STDIMPORTS_H
#define SHARP_STDIMPORTS_H

#include <iostream>
#include <stdint-gcc.h>
#include <cstdlib>
#include <sstream>
#include <list>
#include <stdexcept>
#include <string>
#include "lib/util/time.h"


using namespace std;

#ifdef __unix__
#define POSIX_
#elif defined(_WIN32) || defined(WIN32)
#define WIN32_
#endif

#ifdef WIN32_
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #ifndef __wtypes_h__
    #include <wtypes.h>
    #endif

    #ifndef __WINDEF_
    #include <windef.h>
    #endif
#endif
#ifdef POSIX_
    #include <pthread.h>
    #include <unistd.h>

    #define POSIX_USEC_INTERVAL 1000
#endif

struct Sharp {
    /**
     * Jan 13, 2017 Initial build release of sharp 1.3, this contains
     * all the base level support for running sharp. Sharp was
     * developed to support both windows and linux operating systems
     */
    int BASE  = 1;
};

extern Sharp versions;

void* memalloc(size_t bytes);
void* memcalloc(size_t n, size_t bytes);
void* memrealloc(void *ptr, size_t bytes);
void __os_sleep(int64_t);

#define CXX11_INLINE inline

#endif //SHARP_STDIMPORTS_H
