//
// Created by BraxtonN on 2/24/2017.
//

#include "Thread.h"
#include "Exe.h"
#include "../interp/vm.h"
#include "../interp/Opcode.h"
#include "Environment.h"
#include "../alloc/GC.h"
#include "../interp/register.h"
#include "../../util/time.h"

int32_t Thread::tid = 0;
thread_local Thread* thread_self = NULL;
Thread** Thread::threads = NULL;
Monitor Thread::threads_monitor;
unsigned int Thread::tp = 0;

/*
 * Local registers for the thread to use
 */
thread_local double __rxs[12];

void Thread::Startup() {
    threads = (Thread**)malloc(sizeof(Thread**)*MAX_THREADS);
    for(unsigned int i = 0; i < MAX_THREADS; i++) {
        threads[i] = NULL;
    }

    Thread* main = (Thread*)malloc(
            sizeof(Thread)*1);
    main->main = &env->__address_spaces[manifest.entry];
    main->Create("Main");
}

int32_t Thread::Create(int32_t method, unsigned long stack_size) {
    if(method < 0 || method >= manifest.addresses)
        return -1;
    sh_asp* asp = &env->__address_spaces[method];
    if(asp->param_size>0)
        return -2;
    if(stack_size < (STACK_INIT_SIZE + (asp->frame_init+asp->self)))
        return -3;

    Thread* thread = (Thread*)malloc(
            sizeof(Thread)*1);

    thread->monitor = Monitor();
    thread->name.init();
    thread->main = asp;
    thread->id = Thread::tid++;
    thread->__stack = NULL;
    thread->suspendPending = false;
    thread->exceptionThrown = false;
    thread->suspended = false;
    thread->throwable.init();
    thread->exited = false;
    thread->daemon = false;
    thread->state = thread_init;
    thread->exitVal = 0;
    thread->stack_lmt = stack_size;

    push_thread(thread);

    stringstream ss;
    ss << "Thread@" << thread->id;
    thread->name = ss.str();
    return thread->id;
}

void Thread::Create(string name) {
    this->monitor = Monitor();
    this->name.init();

    this->name = name;
    this->id = Thread::tid++;
    this->__stack = (data_stack*)memalloc(sizeof(data_stack)*STACK_SIZE);
    this->suspendPending = false;
    this->exceptionThrown = false;
    this->suspended = false;
    this->exited = false;
    this->throwable.init();
    this->daemon = false;
    this->state = thread_init;
    this->exitVal = 0;
    this->stack_lmt = STACK_SIZE;

    push_thread(this);
}

void Thread::CreateDaemon(string) {
    this->monitor = Monitor();
    this->name.init();

    this->name = name;
    this->id = Thread::tid++;
    this->__stack = NULL;
    this->suspendPending = false;
    this->exceptionThrown = false;
    this->suspended = false;
    this->exited = false;
    this->daemon = true;
    this->throwable.init();
    this->state = thread_init;
    this->exitVal = 0;
    this->stack_lmt=0;

    push_thread(this);
}

void Thread::push_thread(Thread *thread) {
    threads_monitor.acquire(INDEFINITE);
    for(unsigned int i = 0; i < tp; i++) {
        if(threads[i] == NULL) {
            /*
             * Recycle old thread address
             */
            threads[i] = thread;
            tid--;
            thread->id = i;
            threads_monitor.release();
            return;
        }
    }

    threads[tp++]=thread;
    threads_monitor.release();
}

void Thread::pop_thread(Thread *thread) {
    threads_monitor.acquire(INDEFINITE);
    for(unsigned int i = 0; i < tp; i++) {
        if(threads[i] == thread) {
            threads[i] = NULL;
            return;
        }
    }
    threads_monitor.release();
}

Thread *Thread::getThread(int32_t id) {
    for(int32_t i = 0 ; i < tp; i++) {
        Thread* thread = threads[i];

        if(thread != NULL && thread->id == id)
            return thread;
    }

    return NULL;
}

void Thread::suspendSelf() {
    thread_self->suspended = true;
    thread_self->suspendPending = false;

    /*
	 * We call wait upon suspend. This function will
	 * sleep the thread most of the time. notify() or
	 * resumeAllThreads() should be called to revive thread.
	 */
    thread_self->wait();
}

void Thread::wait() {
    const long sMaxRetries = 128 * 1024;

    long spinCount = 0;
    long retryCount = 0;

    this->state = thread_suspend;

    while (this->suspended)
    {
        if (retryCount++ == sMaxRetries)
        {
            spinCount++;
            retryCount = 0;
#ifdef WIN32_
            Sleep(5);
#endif
#ifdef POSIX_
            usleep(5*POSIX_USEC_INTERVAL);
#endif
        } else if(this->state == thread_killed) {
            this->suspended = false;
            return;
        }
    }

    this->state = thread_running;
}

int Thread::start(int32_t id) {
    Thread *thread = getThread(id);

    if (thread == NULL)
        return 1;

    if(thread->state == thread_running)
        return 2;

    thread->exited = false;
    thread->state = thread_init;
#ifdef WIN32_
    thread->thread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            &SharpVM::InterpreterThreadStart,       // thread function caller
            thread,                 // thread self when thread is created
            0,                      // use default creation flags
            NULL);
    if(thread->thread == NULL) return 3; // thread was not started
    else
        return waitForThread(thread);
#endif
#ifdef POSIX_
    if(pthread_create( &thread->thread, NULL, vm->InterpreterThreadStart, (void*) thread))
        return 3; // thread was not started
    else {
        return waitForThread(thread);
    }
#endif

}

int Thread::destroy(int64_t id) {
    if(id == thread_self->id)
        return 1; // cannot destroy thread_self

    Thread* thread = getThread(id);
    if(thread == NULL || thread->daemon)
        return 1;

    if (thread->state == thread_killed)
    {
        if (thread->id == main_threadid)
        {
            return 3; // should never happen
        }
        else
        {
            pop_thread(thread);
            thread->term(); // terminate thread
            std::free (thread);
            return 0;
        }
    } else {
        return 2;
    }
}

int Thread::interrupt(int32_t id) {
    if(id == thread_self->id)
        return 1; // cannot interrupt thread_self

    Thread* thread = getThread(id);
    if(thread == NULL || thread->daemon)
        return 1;

    return interrupt(thread);
}

void Thread::waitForThreadSuspend(Thread *thread) {
    const int sMaxRetries = 10000000;
    const int sMaxSpinCount = 25;

    int spinCount = 0;
    int retryCount = 0;

    suspendThread(thread);
    while (thread->state == thread_running && !thread->suspended)
    {
        if (retryCount++ == sMaxRetries)
        {
            retryCount = 0;
            if(++spinCount >= sMaxSpinCount)
            {
                return; // give up
            } else if(thread->state >= thread_suspend)
                return;
        }
    }
}

void Thread::waitForThreadExit(Thread *thread) {
    const int sMaxRetries = 10000000;
    const int sMaxSpinCount = 25;

    int spinCount = 0;
    int retryCount = 0;

    while (!thread->exited)
    {
        if (retryCount++ == sMaxRetries)
        {
            retryCount = 0;
            if(++spinCount >= sMaxSpinCount)
            {
                return; // give up
            } else if(thread->exited)
                return;
        }
    }
}

int Thread::waitForThread(Thread *thread) {
    const int sMaxRetries = 10000000;
    const int sMaxSpinCount = 25;

    int spinCount = 0;
    int retryCount = 0;

    while (thread->state != thread_running)
    {
        if (retryCount++ == sMaxRetries)
        {
            retryCount = 0;
            if(++spinCount >= sMaxSpinCount)
            {
                return -255; // give up
            } else if(thread->state != thread_running)
                return 0;
        }
    }
    return 0;
}

void Thread::suspendAllThreads() {
    Thread* thread;
    for(unsigned int i= 0; i < tp; i++) {
        thread=threads[i];

        if(thread!=NULL &&
                (thread->id != thread_self->id)){
            suspendThread(thread);
            waitForThreadSuspend(thread);
        }
    }
}

void Thread::resumeAllThreads() {
    Thread* thread;
    for(unsigned int i= 0; i < tp; i++) {
        thread=threads[i];

        if(thread!=NULL &&
           (thread->id != thread_self->id)){
            unsuspendThread(thread);
        }
    }
}

int Thread::unsuspendThread(Thread *thread) {
    thread->suspended = false;
    return 0;
}

void Thread::suspendThread(Thread *thread) {
    if(thread->id == thread_self->id)
        suspendSelf();
    else {
        thread->suspendPending = true;
    }
}

void Thread::term() {
    this->monitor.release();
    if(__stack != NULL)
        GC::_insert_stack(__stack, this->stack_lmt);
    __stack = NULL;
    this->name.free();
}

int Thread::join(int32_t id) {
    if (id == thread_self->id)
        return 1;

    Thread* thread = getThread(id);
    if (thread == NULL || thread->daemon)
        return 1;

    return threadjoin(thread);
}

int Thread::threadjoin(Thread *thread) {
    if (thread->state == thread_running)
    {
#ifdef WIN32_
        WaitForSingleObject(thread->thread, INFINITE);
        return 0;
#endif
#ifdef POSIX_
        if(pthread_join(thread->thread, NULL))
            return 3;
#endif
    }

    return 2;
}

void Thread::killAll() {
    Thread* thread;
    suspendAllThreads();

    for(unsigned int i = 0; i < tp; i++) {
        thread = threads[i];

        if(thread != NULL && thread->id != thread_self->id) {
            if(thread->state == thread_running){
                interrupt(thread);
                waitForThreadExit(thread);
            }

            thread->term();
        } else if(thread != NULL){
            thread->term();
        }
    }
}

int Thread::interrupt(Thread *thread) {
    if (thread->state == thread_running)
    {
        if (thread->id == main_threadid)
        {
            /*
            * Shutdown all running threads
            * and de-allocate all allocated
            * memory. If we do not call join()
            * to wait for all other threads
            * regardless of what they are doing, we
            * stop them.
            */
            vm->Shutdown();
        }
        else
        {
            thread->state = thread_killed; // terminate thread
            return 0;
        }
    }

    return 2;
}

void Thread::shutdown() {
    if(threads != NULL) {
        Thread::killAll();

        for(unsigned int i = 0; i < tp; i++) {
            if(threads[i] != NULL) {
                std::free(threads[i]); threads[i] = NULL;
            }
        }

        std::free (threads);
    }
}

void Thread::exit() {
    try {
        this->exitVal = 0;//(int)this->stack.pop()->obj->prim;
    } catch (Exception) {
        this->exitVal = 203;
    }

    if(this->exceptionThrown) {
        // TODO: handle exception
    }

    this->exited = true;
    this->state = thread_killed;
}

int Thread::startDaemon(
#ifdef WIN32_
        DWORD WINAPI
#endif
#ifdef POSIX_
void*
#endif
(*threadFunc)(void *), Thread *thread) {
    if (thread == NULL || !thread->daemon)
        return 1;

    if(thread->state == thread_running)
        return 2;

    thread->exited = false;
#ifdef WIN32_
    thread->thread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            (LPTHREAD_START_ROUTINE)threadFunc,       // thread function caller
            thread,                 // thread self when thread is created
            0,                      // use default creation flags
            NULL);
    if(thread->thread == NULL) return 3; // thread was not started
    else
        return waitForThread(thread);
#endif
#ifdef POSIX_
    if(pthread_create( &thread->thread, NULL, threadFunc, (void*) thread)!=0)
        return 3; // thread was not started
    else
        return waitForThread(thread);
#endif
}

double exponent(int64_t n){
    if (n < 100000){
        // 5 or less
        if (n < 100){
            // 1 or 2
            if (n < 10)
                return n*0.1;
            else
                return n*0.01;
        }else{
            // 3 or 4 or 5
            if (n < 1000)
                return n*0.001;
            else{
                // 4 or 5
                if (n < 10000)
                    return n*0.0001;
                else
                    return n*0.00001;
            }
        }
    } else {
        // 6 or more
        if (n < 10000000) {
            // 6 or 7
            if (n < 1000000)
                return n*0.000001;
            else
                return n*0.0000001;
        } else if(n < 1000000000) {
            // 8 to 10
            if (n < 100000000)
                return n*0.00000001;
            else {
                // 9 or 10
                if (n < 1000000000)
                    return n*0.000000001;
                else
                    return n*0.0000000001;
            }
        } else if(n < 1000000000000000) {
            // 11 to 15
            if (n < 100000000000)
                return n*0.00000000001;
            else {
                // 12 to 15
                if (n < 1000000000000)
                    return n*0.000000000001;
                else if (n < 10000000000000)
                    return n*0.0000000000001;
                else if (n < 100000000000000)
                    return n*0.00000000000001;
                else
                    return n*0.000000000000001;
            }
        }
        else {
            return n*0.0000000000000001;
        }
    }
}

void Thread::run() {
    thread_self = this;
    Object *ptr=NULL; // ToDO: when ptr is derefrenced assign pointer to null pointer data struct in environment

    if(id != main_threadid) {
        __rxs[sp] = -1;
        __rxs[fp] = 0;
        this ->__stack = (data_stack*)memalloc(sizeof(data_stack)*stack_lmt);
        Environment::init(__stack, stack_lmt);
    }

    pc = 0;

    call_asp(main->id);
    _init_opcode_table
    try {
        for (;;) {
            interp:
            if(suspendPending)
                suspendSelf();
            if(state == thread_killed)
                return;

            DISPATCH();
            _NOP:
                NOP
            _INT:
                _int(GET_Da(cache[pc]))
            MOVI:
                movi(GET_Da(cache[pc]))
            RET:
                ret
            HLT:
                hlt
            NEWi: /* Requires register value */
                _newi(GET_Da(cache[pc]))
            CHECK_CAST:
                check_cast(GET_Da(cache[pc]))
            MOV8:
                mov8(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOV16:
                mov16(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOV32:
                mov32(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOV64:
                mov64(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            PUSHR:
                pushr(GET_Da(cache[pc]))
            ADD:
                _add(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SUB:
                _sub(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MUL:
                _mul(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            DIV:
                _div(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOD:
                mod(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            POP:
                _pop
            INC:
                inc(GET_Da(cache[pc]))
            DEC:
                dec(GET_Da(cache[pc]))
            MOVR:
                movr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOVX: /* Requires register value */
                movx(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            LT:
                lt(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            BRH:
                brh
            BRE:
                bre
            IFE:
                ife
            IFNE:
                ifne
            GT:
                gt(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            GTE:
                gte(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            LTE:
                lte(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOVL:
                movl(GET_Da(cache[pc]))
            RMOV:
                CHK_NULL(_nativewrite2((int64_t)__rxs[GET_Ca(cache[pc])], __rxs[GET_Cb(cache[pc])]) _brh)
            MOV:
                CHK_NULL(_nativewrite3((int64_t)__rxs[GET_Ca(cache[pc])], GET_Cb(cache[pc])) _brh)
            MOVD:
                CHK_NULL(_nativewrite2((int64_t)__rxs[GET_Ca(cache[pc])], GET_Cb(cache[pc])) _brh)
            MOVBI:
                movbi(GET_Da(cache[pc]) + exponent(cache[pc + 1]))
            _SIZEOF:
                _sizeof(GET_Da(cache[pc]))
            PUT:
                _put(GET_Da(cache[pc]))
            PUTC:
                putc(GET_Da(cache[pc]))
            CHECKLEN:
                _checklen(GET_Da(cache[pc]))
            GOTO:
                _goto(GET_Da(cache[pc]))
            MOVG:
                movg(GET_Da(cache[pc]))
            LOADX:
                _loadx(GET_Da(cache[pc]))
            NEWstr:
                _newstr(GET_Da(cache[pc]))
            PUSHREF:
                pushref()
            DELREF:
                delref(ptr)
            INIT_FRAME:
                _init_frame()
            CALL:
                call(GET_Da(cache[pc]))
            NEW_CLASS:
                new_class(GET_Da(cache[pc]))
            MOVN:
                movn(GET_Da(cache[pc]))
            SMOV:
                smov(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SMOVR:
                smovr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SMOVOBJ:
                smovobj(GET_Da(cache[pc]))
            IADD:
                _iadd(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            ISUB:
                _isub(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IMUL:
                _imul(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IDIV:
                _idiv(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IMOD:
                imod(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            _SLEEP:
                _sleep(GET_Da(cache[pc]))
            TEST:
                test(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            LOCK:
                __lock(GET_Da(cache[pc]))
            ULOCK:
                __ulock()
            EXP:
                exp(GET_Da(cache[pc]))
            ADDL:
                _addl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SUBL:
                _subl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MULL:
                _mull(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            DIVL:
                _divl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MODL:
                modl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            MOVSL:
                movsl(GET_Da(cache[pc]))
            DEL:
                del()
            MOVND:
                movnd(GET_Da(cache[pc]))
            SDELREF:
                sdelref()
            NEW_OBJ_ARRY:
                new_obj_arry(GET_Da(cache[pc]))
            NOT:
                _not(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SKP:
                skp(GET_Da(cache[pc]))
            LOADF:
                loadf(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IADDL:
                _iaddl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            ISUBL:
                _isubl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IMULL:
                _imull(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IDIVL:
                _idivl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            IMODL:
                imodl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            POPR:
                popr(GET_Da(cache[pc]))
            SHL:
                shl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            SHR:
                shr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            TNE:
                tne(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            POPREF:
                popref()
            MUTL:
                mutl(GET_Da(cache[pc]))
            SKPE:
                skpe(GET_Da(cache[pc]))
            SKPNE:
                skpne(GET_Da(cache[pc]))
            AND:
                _and(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            uAND:
                uand(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            OR:
                _or(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            uNOT:
                _unot(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            ANDL:
                _andl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            ORL:
                _orl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            NOTL:
                _notl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
            _THROW:
                _throw()
            CHKNULL:
                _chknull()
        }
    } catch (bad_alloc &e) {
        cout << "std::bad_alloc\n";
        // TODO: throw out of memory error
    } catch (Exception &e) {
        throwable = e.getThrowable();
        exceptionThrown = true;
        Throw(&__stack[SP64].object);

        DISPATCH();
    }
}

bool Thread::execFinally(int command) {
    Object *ptr=NULL; // ToDO: when ptr is derefrenced assign pointer to null pointer data struct in environment
    _init_opcode_table

    int64_t  start;
    sh_asp* currMethod=env->__address_spaces+curr_adsp;

    if(command ==EXEC_ALL_FINALLY) {
        pc = 0;
        start=0;
    } else {
        start=currMethod->finallyBlocks.size()+1;
        for(unsigned int i = 0; i < currMethod->finallyBlocks.size(); i++) {
            FinallyTable &ft = currMethod->finallyBlocks.get(i);
            if(ft.start_pc>pc) {
                start=i;
                break;
            }
        }
    }

    try {
        for(unsigned int i = start; i < currMethod->finallyBlocks.size(); i++) {
            FinallyTable &ft = currMethod->finallyBlocks.get(i);
            int64_t code_base=ft.end_pc-ft.start_pc;
            pc = ft.start_pc;

            /*
             * We only want to execute the code in the first finally block
             */
            if(i > start && command==EXEC_SINGLE_FINALLY) {
                return true;
            }

            for(;;) {
                interp:
                if((pc <ft.start_pc || pc > ft.end_pc) && curr_adsp==currMethod->id)
                    break;

                if(suspendPending)
                    suspendSelf();
                if(state == thread_killed)
                    return true;


                DISPATCH();
                _NOP:
                    NOP
                _INT:
                    _int(GET_Da(cache[pc]))
                MOVI:
                    movi(GET_Da(cache[pc]))
                RET:
                    ret_frame2(return_asp(); , true)  _brh
                HLT:
                    hlt
                NEWi: /* Requires register value */
                    _newi(GET_Da(cache[pc]))
                CHECK_CAST:
                    check_cast(GET_Da(cache[pc]))
                MOV8:
                    mov8(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOV16:
                    mov16(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOV32:
                    mov32(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOV64:
                    mov64(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                PUSHR:
                    pushr(GET_Da(cache[pc]))
                ADD:
                    _add(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SUB:
                    _sub(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MUL:
                    _mul(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                DIV:
                    _div(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOD:
                    mod(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                POP:
                    _pop
                INC:
                    inc(GET_Da(cache[pc]))
                DEC:
                    dec(GET_Da(cache[pc]))
                MOVR:
                    movr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOVX: /* Requires register value */
                    movx(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                LT:
                    lt(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                BRH:
                    brh
                BRE:
                    bre
                IFE:
                    ife
                IFNE:
                    ifne
                GT:
                    gt(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                GTE:
                    gte(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                LTE:
                    lte(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOVL:
                    movl(GET_Da(cache[pc]))
                RMOV:
                    CHK_NULL(_nativewrite2((int64_t)__rxs[GET_Ca(cache[pc])], __rxs[GET_Cb(cache[pc])]) _brh)
                MOV:
                    CHK_NULL(_nativewrite3((int64_t)__rxs[GET_Ca(cache[pc])], GET_Cb(cache[pc])) _brh)
                MOVD:
                    CHK_NULL(_nativewrite2((int64_t)__rxs[GET_Ca(cache[pc])], GET_Cb(cache[pc])) _brh)
                MOVBI:
                    movbi(GET_Da(cache[pc]) + exponent(cache[pc + 1]))
                _SIZEOF:
                    _sizeof(GET_Da(cache[pc]))
                PUT:
                    _put(GET_Da(cache[pc]))
                PUTC:
                    putc(GET_Da(cache[pc]))
                CHECKLEN:
                    _checklen(GET_Da(cache[pc]))
                GOTO:
                    _goto(GET_Da(cache[pc]))
                MOVG:
                    movg(GET_Da(cache[pc]))
                LOADX:
                    _loadx(GET_Da(cache[pc]))
                NEWstr:
                    _newstr(GET_Da(cache[pc]))
                PUSHREF:
                    pushref()
                DELREF:
                    delref(ptr)
                INIT_FRAME:
                    _init_frame()
                CALL:
                    call(GET_Da(cache[pc]))
                NEW_CLASS:
                    new_class(GET_Da(cache[pc]))
                MOVN:
                    movn(GET_Da(cache[pc]))
                SMOV:
                    smov(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SMOVR:
                    smovr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SMOVOBJ:
                    smovobj(GET_Da(cache[pc]))
                IADD:
                    _iadd(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                ISUB:
                    _isub(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IMUL:
                    _imul(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IDIV:
                    _idiv(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IMOD:
                    imod(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                _SLEEP:
                    _sleep(GET_Da(cache[pc]))
                TEST:
                    test(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                LOCK:
                    __lock(GET_Da(cache[pc]))
                ULOCK:
                    __ulock()
                EXP:
                    exp(GET_Da(cache[pc]))
                ADDL:
                    _addl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SUBL:
                    _subl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MULL:
                    _mull(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                DIVL:
                    _divl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MODL:
                    modl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                MOVSL:
                    movsl(GET_Da(cache[pc]))
                DEL:
                    del()
                MOVND:
                    movnd(GET_Da(cache[pc]))
                SDELREF:
                    sdelref()
                NEW_OBJ_ARRY:
                    new_obj_arry(GET_Da(cache[pc]))
                NOT:
                    _not(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SKP:
                    skp(GET_Da(cache[pc]))
                LOADF:
                    loadf(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IADDL:
                    _iaddl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                ISUBL:
                    _isubl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IMULL:
                    _imull(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IDIVL:
                    _idivl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                IMODL:
                    imodl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                POPR:
                    popr(GET_Da(cache[pc]))
                SHL:
                    shl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                SHR:
                    shr(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                TNE:
                    tne(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                POPREF:
                    popref()
                MUTL:
                    mutl(GET_Da(cache[pc]))
                SKPE:
                    skpe(GET_Da(cache[pc]))
                SKPNE:
                    skpne(GET_Da(cache[pc]))
                AND:
                    _and(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                uAND:
                    uand(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                OR:
                    _or(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                uNOT:
                    _unot(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                ANDL:
                    _andl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                ORL:
                    _orl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                NOTL:
                    _notl(GET_Ca(cache[pc]), GET_Cb(cache[pc]))
                _THROW:
                    _throw()
                CHKNULL:
                    _chknull()
            }
        }
    } catch (bad_alloc &e) {
        cout << "std::bad_alloc\n";
        // TODO: throw out of memory error
    } catch (Exception &e) {
        throwable=e.getThrowable();
        Object* exceptionObject = &__stack[SP64].object;

        throwable.throwable = exceptionObject->klass;
        fillStackTrace(exceptionObject);
        return false;
    }

    return true;
}

void Thread::send_panic_message(ThreadPanic& err) {
    cout << "Fatal thread 0x" << std::hex << id;
    cout << " panicked: " << err.getMessage().str() << " at 0x" << pc
       << std::dec << " (code=" << pc << ")" << endl;

    for(int i = 0; i < 13; i++)
        cout << "*** ";
    cout << endl;

    cout << "Revision: '" << manifest.version.str() << "' address space: "
         << (env->__address_spaces+curr_adsp)->name.str() << endl;
    cout << "platform: " << mvers << " thread-name: " << name.str()
       << " >>> " << manifest.application.str() << endl;
    cout << "adx "; printf("%08x",(int64_t)__rxs[adx]);
    cout << " cx "; printf("%08x",(int64_t)__rxs[cx]);
    cout << " cmt "; printf("%08x",(int64_t)__rxs[cmt]);
    cout << " ebx "; printf("%08x",(int64_t)__rxs[ebx]); cout << endl;

    cout << "ecx "; printf("%08x",(int64_t)__rxs[ecx]);
    cout << " ecf "; printf("%08x",(int64_t)__rxs[ecf]);
    cout << " edf "; printf("%08x",(int64_t)__rxs[edf]);
    cout << " ehf "; printf("%08x",(int64_t)__rxs[ehf]); cout << endl;

    cout << "bmr "; printf("%08x",(int64_t)__rxs[bmr]);
    cout << " egx "; printf("%08x",(int64_t)__rxs[egx]);
    cout << " sp "; printf("%08x",(int64_t)__rxs[sp]);
    cout << " fp "; printf("%08x",(int64_t)__rxs[fp]); cout << endl;

    cout << "backtrace:" << endl;
    for(unsigned int i = 0; i < err.getCalls().size(); i++) {
        sh_asp* call = err.getCalls().get(i);

        cout << "#" << i <<  " pc " << std::hex << err.getPcs().get(i) << std::dec << " " << env->sourceFiles[call->sourceFile].str();
        cout << ' ' << "(" << call->name.str() << ")" << endl;
    }

    if(cache != NULL) {
        cout << "cache around pc: " << endl;
        int iter=0;
        for(unsigned int i = pc; i > 0; i++) {
            if(iter++ > 24) break;
            if(i%4 == 0) cout << endl;

            printf("%08x ",cache[i]);
        }
    }


    state=thread_panicked;
}

bool Thread::TryThrow(sh_asp* asp, Object* exceptionObject) {
    if(asp->exceptions.size() > 0) {
        ExceptionTable* et, *tbl=NULL;
        for(unsigned int i = 0; i < asp->exceptions.size(); i++) {
            et = &asp->exceptions.get(i);

            if (et->start_pc <= pc && et->end_pc >= pc)
            {
                if (tbl == NULL || et->start_pc > tbl->start_pc)
                    tbl = et;
            }
        }

        if(tbl != NULL)
        {
            Object* object = &__stack[(int64_t)__rxs[fp]+tbl->local].object;
            ClassObject* klass = exceptionObject == NULL ? NULL : exceptionObject->klass;

            for(;;) {
                if(klass == NULL)
                    break;

                if(tbl->klass == klass->name) {
                    object->mutate(exceptionObject);
                    sh_asp* method = (curr_adsp+env->__address_spaces);
                    uint64_t oldpc = pc, newpc=tbl->handler_pc;
                    pc = tbl->handler_pc;


                    for(unsigned int i = 0; i < method->finallyBlocks.size(); i++) {
                        FinallyTable &ft = method->finallyBlocks.get(i);
                        if(ft.start_pc > oldpc && ft.start_pc < pc) {
                            pc = oldpc;
                            if(!execFinally(EXEC_SINGLE_FINALLY)) {
                                return false;
                            }
                            oldpc = pc;
                        }
                    }

                    pc = newpc;
                    return true;
                }

                klass = klass->super;
            }
        }
    }

    return false;
}

std::string Thread::getPrettyErrorLine(long line, long sourceFile) {
    stringstream ss;
    line -=2;

    if(line >= 0)
        ss << endl << "\t   " << line << ":    "; ss << metaData.getLine(line, sourceFile);
    line++;

    if(line >= 0)
        ss << endl << "\t   " << line << ":    "; ss << metaData.getLine(line, sourceFile);
    line++;

    ss << endl << "\t>  " << line << ":    "; ss << metaData.getLine(line, sourceFile);
    line++;

    if(metaData.hasLine(line, sourceFile))
        ss << endl << "\t   " << line << ":    "; ss << metaData.getLine(line, sourceFile);
    line++;

    if(metaData.hasLine(line, sourceFile))
        ss << endl << "\t   " << line << ":    "; ss << metaData.getLine(line, sourceFile);
    return ss.str();
}

void Thread::fillStackTrace(nString& stack_trace) {
    // fill message
    stringstream ss;
    sh_asp* m = env->__address_spaces+curr_adsp;
    int64_t pc = this->pc, _fp=FP64;
    List<sh_asp*> calls;
    List<long long> pcs;

    while(m != NULL)
    {
        calls.add(m);
        pcs.add(pc);
        if(m->id == main->id) {
            break;
        } else {
            if(_fp == 0) break;
            int64_t id = (int64_t )__stack[_fp-1].var;
            if(id < 0 || id >= manifest.addresses)
                break;

            m= env->__address_spaces+id;
            if(_fp-sp_offset < 0)
                thread_panic("stack is unaligned, check your assembly?", calls, pcs);

            pc = (int64_t )__stack[_fp-pc_offset].var;
            _fp = (int64_t )__stack[_fp-fp_offset].var;
        }
    }

    unsigned int len = calls.size() > EXCEPTION_PRINT_MAX ? EXCEPTION_PRINT_MAX : calls.size(), iter=0;
    for(long i = 0; i < calls.size(); i++)
    {
        if(iter++ > len)
            break;

        ss << "\tSource ";
        if(calls.get(i)->sourceFile != -1 && calls.get(i)->sourceFile < manifest.sourceFiles) {
            ss << "\""; ss << env->sourceFiles[calls.get(i)->sourceFile].str() << "\"";
        }
        else
            ss << "\"unknown file\"";

        long long x, line=-1;
        for(x = 0; x < calls.get(i)->lineNumbers.size(); x++)
        {
            if(calls.get(i)->lineNumbers.get(x).pc > pc)
                break;
        }

        if(x > 0) {
            ss << ", line " << (line = calls.get(i)->lineNumbers.get(x - 1).line_number);
        } else
            ss << ", line ?";

        ss << ", in "; ss << calls.get(i)->name.str() << "() [0x" << std::hex << calls.get(i)->id << "] $0x" << pcs.get(i)  << std::dec;

        if(line != -1 && metaData.sourceFiles.size() > 0) {
            ss << getPrettyErrorLine(line, calls.get(i)->sourceFile);
        }

        ss << "\n";
    }

    stack_trace = ss.str();
    calls.free();
    pcs.free();
}

void varObjectToStr(Object* obj, nString &str) {
    if(obj != NULL) {
        for(unsigned int i = 0; i < obj->size; i++) {
            if(obj->HEAD != NULL)
                str += obj->HEAD[i];
        }
    }
}

void Thread::fillStackTrace(Object* exceptionObject) {
    nString str;
    fillStackTrace(str);
    throwable.stackTrace = str;

    if(exceptionObject->klass != NULL) {

        Object* stackTrace = env->findfield("stackTrace", exceptionObject);
        Object* message = env->findfield("message", exceptionObject);

        if(stackTrace != NULL) {
            stackTrace->createstr(str);
        }
        if(message != NULL) {
            if(throwable.native)
                message->createstr(throwable.message);
            else {
                 varObjectToStr(message, throwable.message);
            }
        }
    }
}

void Thread::Throw(Object* exceptionObject) {
    if(exceptionObject->klass == NULL) {
        cout << "object ia not a class" << endl;
        return;
    }

    throwable.throwable = exceptionObject->klass;
    fillStackTrace(exceptionObject);

    __throw:
    if(TryThrow(env->__address_spaces+curr_adsp, exceptionObject)) {
        exceptionThrown = false;
        throwable.drop();
        return;
    }
    for(;;) {
        if(curr_adsp == main->id) {
            break;
        } else {
            try{
                return_asp();
            }catch (Exception &e) {
                throwable=e.throwable;
                exceptionObject = &__stack[SP64].object;
                goto __throw;
            }

            if(TryThrow(env->__address_spaces+curr_adsp, exceptionObject)) {
                exceptionThrown = false;
                throwable.drop();
                return;
            }
        }
    }

    stringstream ss;
    ss << "Unhandled exception on thread " << name.str() << " (most recent call last):\n"; ss << throwable.stackTrace.str();
    ss << endl << throwable.throwable->name.str() << " ("
       << throwable.message.str() << ")\n";
    throw Exception(ss.str());
}

#ifdef  DEBUGGING
int64_t getop(int64_t i) {
    return (i & OPCODE_MASK);
}
int64_t get_da(int64_t i){
    return (i >> 8);
}
int64_t get_ca(int64_t i) {
    return (((i >> 8) & 1) ? (-1*(i >> 9 & 0x7FFFFFF)) : (i >> 9 & 0x7FFFFFF));
}
int64_t get_cb(int64_t i) {
    return (i >> 36);
}

data_stack* stack_at(int64_t pos, bool usefp) {
    return &thread_self->__stack[(int64_t )__rxs[usefp ? fp : sp]+pos];
}
int64_t get_reg(int64_t i) {
    return (int64_t )__rxs[i];
}

sh_asp* curr_func() {
    return env->__address_spaces+thread_self->curr_adsp;
}

void print_stack() {
    cout << endl << "==============================\n";
    cout << "@" << thread_self->curr_adsp << ":"
         << (thread_self->curr_adsp+env->__address_spaces)->name.str() << " ";
    cout << "[[" << "sp:" << SP64 << " fp:" << FP64 << endl;
    for(unsigned int i = 0; i < SP64+15; i++) {
        if(i==FP64) cout << "#FP: ";
        if(i==SP64) cout << "#SP: ";
        cout << "{@" << i << " v:" << thread_self->__stack[i].var << ":"
             << thread_self->__stack[i].object.toString() << endl;
    }
    cout << endl << "]]";
}
#endif


void Thread::call_asp(int64_t id) {
    if(id < 0 || id >= manifest.addresses) {
        stringstream ss;
        ss << "could not call method @" << id << "; method not found.";
        throw Exception(ss.str());
    }

    sh_asp* asp = env->__address_spaces+id;

    /*
     * Do we have enough space to allocate this new frame?
     */
    if((__rxs[sp]+(asp->frame_init+asp->self)) < stack_lmt) {
        this->curr_adsp = asp->id;
        this->cache = asp->bytecode;
        this->cache_size=asp->cache_size;

        _FP= ((_SP+1)-asp->param_size)-asp->self;
        _SP = asp->frame_init == 0 ? _FP : _FP+(asp->frame_init+asp->self-1);
        if(_FP != 0) __stack[FP64-pc_offset].var = pc; // reset pc to call address
        pc = 0;
    } else {
        throw Exception(&Environment::StackOverflowErr, "");
    }
}

void Thread::init_frame() {
    int64_t old_sp = SP64, frame_alloc = 4;

    /*
     * Do we have enough space to allocate this new frame?
     */
    if(__rxs[sp]+frame_alloc < stack_lmt) {
        __stack[(int64_t )++_SP].var = old_sp; // store sp
        __stack[(int64_t )++_SP].var = _FP; // store frame pointer
        ++_SP; // store pc later
        __stack[(int64_t )++_SP].var = curr_adsp; // store address_space id
    } else {
        throw Exception(&Environment::StackOverflowErr, "");
    }
}

void Thread::return_asp() {
    if((FP64-1) < 0) {
        List<sh_asp*> calls;
        List<long long> pcs;

        calls.add(env->__address_spaces+curr_adsp);
        pcs.add(pc);

        thread_panic("stack is unaligned (ret), check your assembly?", calls, pcs);
    }

    int64_t id = (int64_t )__stack[FP64-1].var;
    if(id < 0 || id >= manifest.addresses) {
        stringstream ss;
        ss << "could not return from method @" << id << "; method not found.";
        throw Exception(ss.str());
    }

    if(!execFinally(EXEC_PRECEDING_FINALLY)) {
        throw Exception(throwable);
    }

    sh_asp* asp = &env->__address_spaces[id];
    curr_adsp = asp->id;
    cache = asp->bytecode;
    cache_size=asp->cache_size;

    pc = (uint64_t )__stack[FP64-pc_offset].var;
    _SP = __stack[FP64-sp_offset].var;
    _FP = __stack[FP64-fp_offset].var;
}

void Thread::thread_panic(string message, List<sh_asp*> calls, List<long long> pcs) {
    throw ThreadPanic(message, calls, pcs);
}

void __os_sleep(int64_t INTERVAL) {
#ifdef WIN32_
    Sleep(INTERVAL);
#endif
#ifdef POSIX_
    usleep(INTERVAL);
#endif
}
