//
// Created by bknun on 9/12/2017.
//

#include "Runtime.h"
#include "parser/ErrorManager.h"
#include "List.h"
#include "parser/Parser.h"
#include "../util/File.h"

void help();

int _bootstrap(int argc, const char* argv[])
{
    if(argc < 2) {
        help();
        return 1;
    }

    return 0;
}

void help() {
    cout << "Usage: bootstrap" << "{OPTIONS} SOURCE FILE(S)" << std::endl;
    cout << "Source file must have a .sharp extion to be compiled.\n" << endl;
    cout << "[-options]\n\n    -V                print compiler version and exit"                       << endl;
    cout <<               "    -showversion      print compiler version and continue"                   << endl;
    cout <<               "    -o<file>          set the output object file"                            << endl;
    cout <<               "    -c                compile only and do not generate exe"                  << endl;
    cout <<               "    -a                enable aggressive error reporting"                     << endl;
    cout <<               "    -s                string debugging info"                                 << endl;
    cout <<               "    -O                optimize executable"                                   << endl;
    cout <<               "    -w                disable all warnings"                                  << endl;
    cout <<               "    -errlmt<count>    set max errors the compiler allows before quitting"    << endl;
    cout <<               "    -v<version>       set the application version"                           << endl;
    cout <<               "    -unsafe -u        allow unsafe code"                                     << endl;
    cout <<               "    -objdmp           create dump file for generated assembly"               << endl;
    cout <<               "    -target           target the specified platform of sharp to run on"      << endl;
    cout <<               "    -werror           enable warnings as errors"                             << endl;
    cout <<               "    -release -r       generate a release build exe"                          << endl;
    cout <<               "    --h -?            display this help message"                             << endl;
}

