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

struct GcObject;

class GC {
public:
    static GC* gc;

    static void GCStartup();
    static void GCShutdown();
    static void _init_GC();
    static void _collect_GC_CONCURRENT();
    static void _collect_GC_EXPLICIT();
    static void _insert(Object*);
    static void _insert_stack(Object*, unsigned long);
    static void _insert_stack(data_stack* st, unsigned long sz);
    static void notify(int sig);

#ifdef DEBUGGING
    static void print_stack();
#endif

private:
    Monitor mutex, sigMutex;
    Object* gc_alloc_heap;
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

#define _GC_CAP_THRESHOLD 1.5

#define GC_SLEEP_INTERVAL 10

/**
 * This number must be low considering that the Garbage collector will
 * not be collecting data every second. We want the garbage collector
 * to be asleep as much as possible.
 */
#define GC_SPIN_MULTIPLIER 512

#endif //SHARP_GC_H
