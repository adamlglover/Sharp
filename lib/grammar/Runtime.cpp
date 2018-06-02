//
// Created by bknun on 9/12/2017.
//

#include "Runtime.h"
#include "parser/ErrorManager.h"
#include "List.h"
#include "parser/Parser.h"
#include "../util/File.h"

options c_options;
void help();

int _bootstrap(int argc, const char* argv[])
{
    if(argc < 2) {
        help();
        return 1;
    }

    initalizeErrors();
    List<string> files;
    for (int i = 1; i < argc; ++i) {
        args_:
        if(opt("-a")){
            c_options.aggressive_errors = true;
        }
        else if(opt("-c")){
            c_options.compile = true;
        }
        else if(opt("-o")){
            if(i+1 >= argc)
                rt_error("output file required after option `-o`");
            else
                c_options.out = string(argv[++i]);
        }
        else if(opt("-V")){
            printVersion();
            exit(0);
        }
        else if(opt("-O")){
            c_options.optimize = true;
        }
        else if(opt("-h") || opt("-?")){
            help();
            exit(0);
        }
        else if(opt("-R") || opt("-release")){
            c_options.optimize = true;
            c_options.debug = false;
            c_options.strip = true;
        }
        else if(opt("-s")){
            c_options.strip = true;
        }
        else if(opt("-magic")){
            c_options.magic = true;
        }
        else if(opt("-debug")) {
            c_options.debugMode = true;
        }
        else if(opt("-showversion")){
            printVersion();
            cout << endl;
        }
        else if(opt("-target")){
            if(i+1 >= argc)
                rt_error("file version required after option `-target`");
            else {
                std::string x = std::string(argv[++i]);
                if(all_integers(x))
                    c_options.target = strtol(x.c_str(), NULL, 0);
                else {
                    if(to_lower(x) == "base") {
                        rt_error("base is no longer supported");
                    } else if(to_lower(x) == "alpha") {
                        c_options.target = versions.ALPHA;
                    }
                    else {
                        rt_error("unknown target " + x);
                    }
                }
            }
        }
        else if(opt("-w")){
            c_options.warnings = false;
        }
        else if(opt("-v")){
            if(i+1 >= argc)
                rt_error("file version required after option `-v`");
            else
                c_options.vers = string(argv[++i]);
        }
        else if(opt("-u") || opt("-unsafe")){
            c_options.unsafe = true;
        }
        else if(opt("-werror")){
            c_options.werrors = true;
            c_options.warnings = true;
        }
        else if(opt("-errlmt")) {
            std::string lmt = std::string(argv[++i]);
            if(all_integers(lmt)) {
                c_options.error_limit = strtoul(lmt.c_str(), NULL, 0);

                if(c_options.error_limit > 100000) {
                    rt_error("cannot set the max errors allowed higher than (100,000) - " + lmt);
                } else if(c_options.error_limit == 0) {
                    rt_error("cannot have an error limit of 0 ");
                }
            }
            else {
                rt_error("invalid error limit set " + lmt);
            }
        }
        else if(opt("-objdmp")){
            c_options.objDump = true;
        }
        else if(string(argv[i]).at(0) == '-'){
            rt_error("invalid option `" + string(argv[i]) + "`, try bootstrap -h");
        }
        else {
            // add the source files
            do {
                if(string(argv[i]).at(0) == '-')
                    goto args_;

                files.addif(string(argv[i++]));
            }while(i<argc);
            break;
        }
    }

    if(files.size() == 0){
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

