#include <iostream>
#include "stdimports.h"

#ifdef MAKE_COMPILER
#include "lib/grammar/runtime.h"
#include "lib/runtime/interp/Opcode.h"

#else
#include "lib/runtime/startup.h"
#include "lib/util/file.h"
#include "lib/runtime/internal/Exe.h"
#include "lib/runtime/interp/Opcode.h"
#include "lib/runtime/interp/register.h"
#include "lib/runtime/oo/Object.h"
#include "lib/util/time.h"
#endif

Sharp versions;

int main(int argc, const char* argv[]) {

#ifndef MAKE_COMPILER
    uint64_t past= realTimeInNSecs(),now;
    runtimeStart( argc, argv );
    now= realTimeInNSecs();
    cout << endl << NANO_TOMILL(now-past) << "ms";
#else
    _bootstrap( argc, argv );
#endif
    cout << " program exiting..." << endl;
    return 0;
}