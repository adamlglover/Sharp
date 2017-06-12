//
// Created by BraxtonN on 2/24/2017.
//

#ifndef SHARP_THREAD_H
#define SHARP_THREAD_H


#include "../../../stdimports.h"
#include "Monitor.h"
#include "../oo/ClassObject.h"
#include "../oo/Exception.h"
#include "sh_asp.h"

class ThreadPanic : public std::runtime_error {
public:
    ThreadPanic(char *msg, List<sh_asp*>& calls, List<long long>& x);
    ThreadPanic(std::string &__arg, List<sh_asp*>& calls, List<long long>&);

    ~ThreadPanic();
    nString &getMessage();
    List<sh_asp *> &getCalls();
    List<long long> &getPcs();

private:
    nString message;
    List<sh_asp*> calls;
    List<long long> pcs;
};

#define MAX_THREADS 0x40fe

#define STACK_SIZE 0xefba

#define STACK_INIT_SIZE 5

enum ThreadState {
    thread_init=0x000,
    thread_running=0x001,
    thread_suspend=0x002,
    thread_killed=0x003,
    thread_panicked=0xffff0
};

class Thread {
public:
    Thread()
    :
            id(-1),
            monitor(),
            daemon(false),
            state(thread_killed),
            suspended(false),
            name(""),
            main(NULL),
            exitVal(1),
            suspendPending(false),
            exceptionThrown(false),
            throwable()

    {

#ifdef WIN32_
        thread = NULL;
#endif
    }

    static void Startup();
    static void suspendSelf();
    static int start(int32_t);
    static int destroy(int64_t);
    static int interrupt(int32_t);
    static int join(int32_t);
    static Thread* getThread(int32_t);
    static void waitForThreadSuspend(Thread* thread);
    static void waitForThreadExit(Thread* thread);
    static int waitForThread(Thread *thread);
    static void killAll();
    static void shutdown();

    static int startDaemon(
#ifdef WIN32_
            DWORD WINAPI
#endif
#ifdef POSIX_
    void*
#endif
    (*threadFunc)(void*), Thread* thread);


    static int32_t Create(int32_t, unsigned long);
    void Create(string);
    void CreateDaemon(string);
    void exit();

    static int32_t tid;
    static Thread** threads;
    static Monitor threads_monitor;
    static unsigned int tp;

    int32_t id;
    Monitor monitor;
    bool daemon;
    unsigned int state;
    bool suspended;
    bool exited;
    nString name;
    sh_asp* main;
    int exitVal;
    bool suspendPending;
    bool exceptionThrown;

    uint64_t pc, curr_adsp;
    int64_t cache_size;
    data_stack* __stack;
    unsigned long stack_lmt;
    sharp_cache cache;
    Throwable throwable;
#ifdef WIN32_
    HANDLE thread;
#endif
#ifdef POSIX_
    pthread_t thread;
#endif

    void term();

    static void suspendAllThreads();

    static void resumeAllThreads();

    void run();

    void init_frame();

    void send_panic_message(ThreadPanic&);

private:
    void call_asp(int64_t id);
    void return_asp();

    void wait();

    void thread_panic(string message, List<sh_asp*> calls, List<long long> pcs);
    static int threadjoin(Thread*);
    static int unsuspendThread(Thread*);
    static void suspendThread(Thread*);
    static int interrupt(Thread*);

    static void push_thread(Thread *thread);
    static void pop_thread(Thread *thread);

    bool TryThrow(sh_asp* asp, Object* exceptionObject);
    void Throw(Object *exceptionObject);

    void fillStackTrace(Object *exceptionObject);

    void fillStackTrace(nString &str);

    string getPrettyErrorLine(long line, long sourceFile);

    bool execFinally(int);
};

extern thread_local Thread* thread_self;
extern thread_local double __rxs[12];

#define SP64 (int64_t)__rxs[sp]
#define FP64 (int64_t)__rxs[fp]

#define _SP __rxs[sp]
#define _FP __rxs[fp]

#define main_threadid 0x0

#endif //SHARP_THREAD_H
