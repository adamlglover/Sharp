//
// Created by BraxtonN on 2/17/2017.
//

#include <cstring>
#include <cstdio>
#include "../../stdimports.h"
#include "startup.h"
#include "../util/file.h"
#include "interp/vm.h"
#include "internal/Environment.h"
#include "internal/Exe.h"
#include "internal/Thread.h"
#include "interp/register.h"

options c_options;
int __vinit(string e, List<nString>& pArgs);

void init_main(List <nString>& list1);

void createStringArray(Sh_object *pObject, List<nString> &list1);

void version() {
    cout << progname << " " << progvers << endl;
}

void error(string message) {
    cout << "sharp:  error: " << message << endl;
    exit(1);
}

void help() {
    std::cerr << "Usage: sharp" << " {OPTIONS} EXECUTABLE" << std::endl;
    cout << "Source file must have a .sharp extension to be compiled\n" << endl;
    cout << "[-options]\n\n    -V                print the bootstrap version number and exit" << endl;
    cout <<               "    -showversion      print the bootstrap version number and continue." << endl;
    cout <<               "    --h -?            display this help message." << endl;
}

#define opt(v) strcmp(argv[i], v) == 0

int runtimeStart(int argc, const char* argv[])
{
    if (argc < 2) { // We expect at least 1 argument: the executable
        help();
        return 1;
    }

    string executable ="";
    List<nString> pArgs;
    for (int i = 1; i < argc; ++i) {
        if(opt("-V")){
            version();
            exit(0);
        }
        else if(opt("-h") || opt("-?")){
            help();
            exit(0);
        }
        else if(opt("-showversion")){
            version();
        }
        else if(string(argv[i]).at(0) == '-'){
            error("invalid option `" + string(argv[i]) + "`, try sharp -h");
        }
        else {
            // add the source files
            executable = argv[i++];
            nString arg;
            while(i < argc) {
                arg = string(argv[i++]);
                pArgs.add(arg);
            }
            break;
        }
    }

    if(executable == ""){
        help();
        return 1;
    }

    if(!file::exists(executable.c_str())){
        error("file `" + executable + "` doesnt exist!");
    }

    return __vinit(executable, pArgs);
}

int __vinit(string exe, List<nString>& pArgs) {
    int result;

    if(CreateSharpVM(exe) != 0) {
        fprintf(stderr, "Sharp VM init failed (check log file)\n");
        goto bail;
    }

    init_main(pArgs);
    vm->InterpreterThreadStart(Thread::threads[main_threadid]);
    result=vm->exitVal;

    std::free(vm);
    std::free(env);
    return result;

    bail:
        if(vm != NULL) {
            jobIndx = 0;

            vm->DestroySharpVM();
        }

        return 1;
}

void init_main(List <nString>& pArgs) {
    __rxs[sp] = 0;
    __rxs[fp] = 0;
    Environment::init(Thread::threads[main_threadid]->__stack, STACK_SIZE);
    Thread::threads[main_threadid]->init_frame();
    Sh_object* object = &Thread::threads[main_threadid]->__stack[(long)++__rxs[sp]].object;

    createStringArray(object, pArgs);
}

void createStringArray(Sh_object *pObject, List<nString> &args) {
    pObject->createstr(manifest.executable);
    int16_t MIN_ARGS = 4;
    int64_t size = MIN_ARGS+args.size();
    int64_t iter=0;

    pObject->_Node=(Sh_object*)memalloc(sizeof(Sh_object)*size);
    Environment::init(pObject->_Node, size);
    pObject->size = size+1;

    pObject->_Node[iter++].createstr(manifest.application);
    pObject->_Node[iter++].createstr(manifest.version);

    stringstream ss;
    ss << manifest.target;
    nString str(ss.str());
    pObject->_Node[iter++].createstr(str);

#ifdef WIN32_
    str = "win";
#endif
#ifdef POSIX
    str = "posix";
#endif
    pObject->_Node[iter++].createstr(str);

    /*
     * Assign program args to be passed to main
     */
    for(unsigned int i = 0; i < args.size(); i++) {
        pObject->_Node[iter++].createstr(args.get(i));
    }
}
