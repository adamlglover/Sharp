//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_MANIFEST_H
#define SHARP_MANIFEST_H

#include "../../stdimports.h"
#include "oo/string.h"

class Method;

/**
 * Application info
 */
struct Manifest {
    nString executable;
    nString application;
    nString version;
    bool debug;
    int64_t entry;
    int64_t addresses, classes;
    int fvers;
    int target;
    long sourceFiles;
    int64_t strings;
};


#endif //SHARP_MANIFEST_H
