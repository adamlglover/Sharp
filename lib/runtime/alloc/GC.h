//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_GC_H
#define SHARP_GC_H

#include "../../../stdimports.h"
#include "../internal/Monitor.h"
#include "../oo/Object.h"
#include "../internal/sh_asp.h"


enum gc_sig{
    gc_COLLECT_CONCURRENT=1
};

struct _gc_object;

class GC {
public:
    static GC* gc;

    static void GCStartup();
    static void GCShutdown();
    static void _init_GC();
    static void _collect_GC_CONCURRENT();
    static void _collect_GC_EXPLICIT();
    static void _insert(Sh_object*);
    static void _insert_stack(Sh_object*, unsigned long);
    static void _insert_stack(data_stack* st, unsigned long sz);
    static void notify(int sig);

private:
    Monitor mutex, sigMutex;
    _gc_object* gc_alloc_heap;
    unsigned long allocptr;
    int signal;

    static void _collect();

    static
#ifdef WIN32_
    DWORD WINAPI
#endif
#ifdef POSIX_
    void*
#endif
     _GCThread_start(void *);

    void _GC_run();
};

struct _gc_object{
    double *HEAD;
    Sh_object *_Node;
    Sh_object* _rNode;
    int64_t size;

    void free(){
        if(HEAD != NULL)
            std::free(HEAD); HEAD = NULL;
        if(_Node != NULL) {
            for(int64_t i = 0; i < size; i++) {
                _Node[i].free();
            }
            std::free(_Node); _Node = NULL;
        }

        size = 0;
    }
};

#define _GC_CAP_THRESHOLD 1.5

#define GC_SLEEP_INTERVAL 10

/**
 * This number must be low considering that the Garbage collector will
 * not be collecting data every second. We want the garbage collector
 * to be asleep as much as possible.
 */
#define GC_SPIN_MULTIPLIER 512

#endif //SHARP_GC_H
