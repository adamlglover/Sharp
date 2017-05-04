//
// Created by BraxtonN on 2/17/2017.
//

#include "../internal/Environment.h"
#include "vm.h"
#include "../internal/Exe.h"
#include "../internal/Thread.h"
#include "../oo/Field.h"
#include "../oo/Object.h"
#include "../alloc/GC.h"
#include "register.h"
#include "../../util/time.h"

SharpVM* vm;
Environment* env;

int CreateSharpVM(std::string exe, std::list<string>& pArgs)
{
    vm = (SharpVM*)memalloc(sizeof(SharpVM)*1);
    env = (Environment*)memalloc(sizeof(Environment)*1);

    if(Process_Exe(exe) != 0)
        return 1;

    vm->class_ids = manifest.classes;
    Thread::Startup();
    GC::GCStartup();

    /**
     * Aux classes
     */
    env->Throwable = ClassObject(
            "sharp.lang#Throwable",
            NULL,
            0,
            NULL,
            ++vm->class_ids
    );

    env->RuntimeException = ClassObject(
            "sharp.lang#RuntimeException",
            NULL,
            0,
            &env->Throwable,
            ++vm->class_ids
    );

    env->StackOverflowErr = ClassObject(
            "sharp.lang#StackOverflowErr",
            NULL,
            0,
            &env->RuntimeException,
            ++vm->class_ids
    );

    env->ThreadStackException = ClassObject(
            "sharp.lang#ThreadStackException",
            NULL,
            0,
            &env->RuntimeException,
            ++vm->class_ids
    );

    env->IndexOutOfBoundsException = ClassObject(
            "sharp.lang#IndexOutOfBoundsException",
            NULL,
            0,
            &env->RuntimeException,
            ++vm->class_ids
    );

    env->NullptrException = ClassObject(
            "sharp.lang#NullptrException",
            NULL,
            0,
            &env->RuntimeException,
            ++vm->class_ids
    );
    cout.precision(16);
    env->init(env->global_heap, manifest.classes);

    return 0;
}

void SharpVM::DestroySharpVM() {
    if(thread_self != NULL) {
        exitVal = thread_self->exitVal;
    } else
        exitVal = 1;
    Thread::shutdown();
    GC::GCShutdown();
}

#ifdef WIN32_
DWORD WINAPI
#endif
#ifdef POSIX_
void*
#endif
    SharpVM::InterpreterThreadStart(void *arg) {
        thread_self = (Thread*)arg;
        thread_self->state = thread_running;

        try {
            thread_self->run();
        } catch (Exception &e) {
            thread_self->throwable = e.getThrowable();
            thread_self->exceptionThrown = true;
        }

        /*
         * Check for uncaught exception in thread before exit
         */
        thread_self->exit();

        if (thread_self->id == main_threadid)
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

#ifdef WIN32_
        return 0;
#endif
#ifdef POSIX_
        return NULL;
#endif
    }

void SharpVM::Shutdown() {
    DestroySharpVM();
    env->shutdown();
}

void SharpVM::interrupt(int32_t signal) {
    switch (signal) {
        case 0x9f:
            cout << env->strings[(int64_t )thread_self->__stack[(int64_t)__rxs[sp]--].var].value.str();
            break;
        case 0xa0:
            __rxs[bmr]= __os_time((int) __rxs[ebx]);
            break;
        default:
            // unsupported
            break;
    }
}
