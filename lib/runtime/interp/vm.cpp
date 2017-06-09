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

int CreateSharpVM(std::string exe)
{
    vm = (SharpVM*)memalloc(sizeof(SharpVM)*1);
    env = (Environment*)memalloc(sizeof(Environment)*1);

    if(Process_Exe(exe) != 0)
        return 1;

    Thread::Startup();
    GC::GCStartup();

    /**
     * Aux classes
     */
    Field* fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);

    env->Throwable = ClassObject(
            "std.err#Throwable",
            fields,
            2,
            NULL,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->Throwable;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->RuntimeErr = ClassObject(
            "std.err#RuntimeErr",
            fields,
            2,
            &env->Throwable,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->RuntimeErr;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->StackOverflowErr = ClassObject(
            "std.err#StackOverflowErr",
            fields,
            2,
            &env->RuntimeErr,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->StackOverflowErr;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->classes[manifest.classes].init();
    env->ThreadStackException = ClassObject(
            "std.err#ThreadStackException",
            fields,
            2,
            &env->RuntimeErr,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->ThreadStackException;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->IndexOutOfBoundsException = ClassObject(
            "std.err#IndexOutOfBoundsException",
            fields,
            2,
            &env->RuntimeErr,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->IndexOutOfBoundsException;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->NullptrException = ClassObject(
            "std.err#NullptrException",
            fields,
            2,
            &env->RuntimeErr,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->NullptrException;

    fields=(Field*)malloc(sizeof(Field)*2);
    fields[0].init("message", 0, nativeint, false, false, &env->Throwable);
    fields[1].init("stackTrace", 0, nativeint, false, false, &env->Throwable);
    env->ClassCastException = ClassObject(
            "std.err#ClassCastException",
            fields,
            2,
            &env->RuntimeErr,
            manifest.classes
    );
    env->classes[manifest.classes].init();
    env->classes[manifest.classes++] = env->ClassCastException;

    cout.precision(16);
    env->init(env->global_heap, manifest.classes - AUX_CLASSES);

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
        } catch(ThreadPanic& e) {
            thread_self->send_panic_message(e);
        } catch (Exception &e) {
            if(thread_self->state != thread_panicked)
            {
//                if(thread_self->exceptionThrown) {
//                    cout << thread_self->throwable.stackTrace.str();
//                }
                thread_self->throwable = e.getThrowable();
                thread_self->exceptionThrown = true;
                cout << e.throwable.message.str();
            }
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
            return;
        case 0xa0:
            __rxs[bmr]= __os_time((int) __rxs[ebx]);
            return;
        case 0xa1:
            GC::_collect_GC_EXPLICIT();
            return;
        case 0xa2:
            GC::notify(gc_COLLECT_CONCURRENT);
            return;
        case 0xa3:
            __rxs[bmr]= realTimeInNSecs();
            return;
        case 0xa4:
            __rxs[cmt]=Thread::start((int32_t )__rxs[adx]);
            return;
        case 0xa5:
            __rxs[cmt]=Thread::join((int32_t )__rxs[adx]);
            return;
        case 0xa6:
            __rxs[cmt]=Thread::interrupt((int32_t )__rxs[adx]);
            return;
        case 0xa7:
            __rxs[cmt]=Thread::destroy((int32_t )__rxs[adx]);
            return;
        case 0xa8:
            __rxs[cmt]=Thread::Create((int32_t )__rxs[adx], (unsigned long)__rxs[egx]);
            return;
        default:
            // unsupported
            break;
    }
}
