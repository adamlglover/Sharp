//
// Created by BraxtonN on 2/17/2017.
//

#include "GC.h"
#include "../internal/Thread.h"
#include "../internal/Environment.h"

size_t gc_max_heap_size = 128 * 1024 ;
GC* GC::gc = NULL;

void* memalloc(size_t bytes) {
    void* ptr =NULL;
    bool gc=false;
    alloc_bytes:
        ptr=malloc(bytes);

        if(GC::gc != NULL && ptr == NULL) {
            if(gc) {
                throw Exception("out of memory");
            } else {
                gc = true;
                GC::_collect_GC_EXPLICIT();
                goto alloc_bytes;
            }
        } else {
            return ptr;
        }
}

void* memcalloc(size_t count, size_t bytes) {
    void* ptr =NULL;
    bool gc=false;
    alloc_bytes:
        ptr=calloc(count, bytes);

        if(ptr == NULL) {
            if(gc) {
                throw Exception("out of memory");
            } else {
                gc=true;
                GC::_collect_GC_EXPLICIT();
                goto alloc_bytes;
            }
        } else {
            return ptr;
        }
}

void* memrealloc(void* Ptr, size_t bytes) {
    void* ptr =NULL;
    bool gc=false;
    alloc_bytes:
    ptr=realloc(Ptr, bytes);

    if(ptr == NULL) {
        if(gc) {
            throw Exception("out of memory");
        } else {
            gc=true;
            GC::_collect_GC_EXPLICIT();
            goto alloc_bytes;
        }
    } else {
        return ptr;
    }
}

void GC::_collect_GC_CONCURRENT() {
    gc->mutex.acquire(INDEFINITE);
        _collect();
    gc->mutex.release();
}

void GC::_collect_GC_EXPLICIT() {
    gc->mutex.acquire(INDEFINITE);
    Thread::suspendAllThreads();
        _collect();
    Thread::resumeAllThreads();
    gc->mutex.release();
}

void GC::_collect() {
    Environment::freesticky(gc->gc_alloc_heap, gc->allocptr);
    gc->allocptr=0;
}

void GC::_init_GC() {
    gc=(GC*)malloc(sizeof(GC)*1);
    gc->mutex = Monitor();
    gc->sigMutex = Monitor();
    gc->signal = 0;
    gc->gc_alloc_heap=(_gc_object*)malloc(sizeof(_gc_object)*gc_max_heap_size);
    Environment::init(gc->gc_alloc_heap, gc_max_heap_size);
    gc->allocptr=0;
}

void GC::_insert(Sh_object *gc_obj) {
    gc->mutex.acquire(INDEFINITE);
    if(gc->allocptr == gc_max_heap_size) {
        _collect_GC_EXPLICIT();
    }

    if(gc_obj->_rNode != NULL) {
        gc_obj->del_ref();
        gc->mutex.release();
        return;
    }

    if(gc_obj->refs.size() > 0) {
        for(unsigned long i=0; i < gc_obj->refs.size(); i++) {
            Sh_InvRef(gc_obj->refs.at(i));
        }
        gc_obj->refs.free();
    }

    gc->gc_alloc_heap[gc->allocptr].nxt=gc_obj->nxt;
    gc->gc_alloc_heap[gc->allocptr].prev=gc_obj->prev;
    gc->gc_alloc_heap[gc->allocptr].HEAD=gc_obj->HEAD;
    gc->gc_alloc_heap[gc->allocptr]._Node=gc_obj->_Node;
    gc->gc_alloc_heap[gc->allocptr].size=gc_obj->size;

    gc->allocptr++;
    gc->mutex.release();
}

void GC::GCStartup() {
    _init_GC();

    Thread* gcThread=(Thread*)malloc(
            sizeof(Thread)*1);
    gcThread->CreateDaemon("gc");
    Thread::startDaemon(
            GC::_GCThread_start, gcThread);
}

void GC::GCShutdown() {
    if(gc != NULL) {
        if(gc->gc_alloc_heap != NULL) {
            std::free(gc->gc_alloc_heap);
            gc->gc_alloc_heap=NULL;
        }
        gc->allocptr=0;

        std::free(gc);
        gc=NULL;
    }
}

#ifdef WIN32_
DWORD WINAPI
#endif
#ifdef POSIX_
void*
#endif
 GC::_GCThread_start(void *pVoid) {
    thread_self =(Thread*)pVoid;
    thread_self->state = thread_running;

    try {
        gc->_GC_run();
    } catch(Exception &e){
        /* Should never happen */
        thread_self->exceptionThrown =true;
        thread_self->throwable=e.getThrowable();
    }

    /*
         * Check for uncaught exception in thread before exit
         */
    thread_self->exit();
    //cout << "gc exited" << endl;
#ifdef WIN32_
    return 0;
#endif
#ifdef POSIX_
    return NULL;
#endif
}

void GC::_GC_run() {
    const unsigned int sMaxRetries = 128 * GC_SPIN_MULTIPLIER;
    unsigned int retryCount = 0;

    for(;;) {
        if(thread_self->suspendPending)
            Thread::suspendSelf();
        if(thread_self->state == thread_killed) {
            GC::_collect_GC_CONCURRENT();
            return;
        }
        if(signal != 0) {
            switch(signal) {
                case gc_COLLECT_CONCURRENT:
                    GC::_collect_GC_CONCURRENT();
                    break;
                default:
                    break;
            }
            signal = 0;
            sigMutex.release();
        }

        if (retryCount++ == sMaxRetries)
        {
            retryCount = 0;
#ifdef WIN32_
        Sleep(GC_SLEEP_INTERVAL);
#endif
#ifdef POSIX_
        usleep(GC_SLEEP_INTERVAL*POSIX_USEC_INTERVAL);
#endif
        } else {
            /*
             * Try to keep all de-allocations running
             * asynchronously
             */
            if(gc->allocptr>(gc_max_heap_size/_GC_CAP_THRESHOLD)){
                GC::_collect_GC_CONCURRENT();
            }
        }

    }
}

void GC::_insert_stack(Sh_object *stack, unsigned long len) {
    gc->mutex.acquire(INDEFINITE);
    if(gc->allocptr == gc_max_heap_size) {
        _collect_GC_EXPLICIT();
    }

    Sh_object* ptr =&stack[0];
    for(unsigned long i=0; i < len; i++) {
        if(gc->allocptr == gc_max_heap_size) {
            _collect_GC_CONCURRENT();
        }

        if(ptr->_rNode != NULL) {
            ptr->del_ref();
            continue;
        }

        if(ptr->refs.size() > 0) {
            for(unsigned long x=0; i < ptr->refs.size(); x++) {
                Sh_InvRef(ptr->refs.at(x));
            }
            ptr->refs.free();
        }
        gc->gc_alloc_heap[gc->allocptr].nxt=ptr->nxt;
        gc->gc_alloc_heap[gc->allocptr].prev=ptr->prev;
        gc->gc_alloc_heap[gc->allocptr].HEAD=ptr->HEAD;
        gc->gc_alloc_heap[gc->allocptr]._Node=ptr->_Node;
        gc->gc_alloc_heap[gc->allocptr].size=ptr->size;

        gc->allocptr++;
        ptr++;
    }
    gc->mutex.release();
}

void GC::_insert_stack(data_stack *st, unsigned long stack_size) {
    gc->mutex.acquire(INDEFINITE);
    if(gc->allocptr == gc_max_heap_size) {
        _collect_GC_EXPLICIT();
    }

    if(stack_size > 0 && st != NULL) {
        for(int64_t i = 0; i < stack_size; i++) {
            if(gc->allocptr == gc_max_heap_size) {
                _collect_GC_CONCURRENT();
            }

            if(st[i].object._rNode != NULL) {
                st[i].object.del_ref();
                continue;
            }
            gc->gc_alloc_heap[gc->allocptr].nxt=st[i].object.nxt;
            gc->gc_alloc_heap[gc->allocptr].prev=st[i].object.prev;
            gc->gc_alloc_heap[gc->allocptr].HEAD=st[i].object.HEAD;
            gc->gc_alloc_heap[gc->allocptr]._Node=st[i].object._Node;
            gc->gc_alloc_heap[gc->allocptr]._rNode=NULL;
            gc->gc_alloc_heap[gc->allocptr].size=st[i].object.size;

            gc->allocptr++;

            st[i].object.HEAD=NULL;
            st[i].object._Node = NULL, st[i].object.prev=NULL,st[i].object.nxt=NULL;
            st[i].object.mark = gc_orange;
            st[i].object.size = 0;
            st[i].object.monitor = Monitor();
            st[i].object.refs.init();
        }
    }

    gc->mutex.release();
}

void GC::notify(int sig) {
    gc->sigMutex.acquire(INDEFINITE);
    gc->signal = sig;
}
