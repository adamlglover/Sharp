//
// Created by BraxtonN on 2/27/2017.
//
#include "Exception.h"
#include "../internal/Environment.h"
#include "ClassObject.h"
#include "../internal/Thread.h"
#include "../interp/register.h"

void Throwable::drop() {
    this->throwable = NULL;
    this->message.free();
    stackTrace.free();
}

Exception::Exception(const char *msg, bool native)
        :
        throwable(&Environment::RuntimeErr, msg, native),
        runtime_error(msg)
{
    setupFrame();
}

Exception::Exception(const std::string &__arg, bool native)
        :
        throwable(&Environment::RuntimeErr, __arg, native),
        runtime_error(__arg)
{
    setupFrame();
}

Exception::Exception(ClassObject* throwable, const std::string &__arg, bool native)
        :
        throwable(throwable, __arg, native),
        runtime_error(__arg)
{
    setupFrame();
}

Exception::~Exception()
{
    throwable.drop();
}

void Exception::setupFrame() {
    if(thread_self != NULL && throwable.native) {
        thread_self->__stack[0].object.createclass(throwable.throwable);
        thread_self->__stack[(int64_t)++_SP]
                .object.mutate(&thread_self->__stack[0].object);
    }
}

Exception::Exception(Throwable &throwable)
        :
        runtime_error("")
{
    this->throwable.init();
    this->throwable = throwable;
}

ThreadPanic::ThreadPanic(char *msg, List<sh_asp*>& calls, List<long long>& pcs) : runtime_error(msg) {
    message.init();
    message= string(msg);
    this->calls.init();
    this->pcs.init();
    this->calls.addAll(calls);
    this->pcs.addAll(pcs);
}

ThreadPanic::ThreadPanic(std::string &__arg, List<sh_asp*> &calls, List<long long> &pcs) : runtime_error(__arg) {
    message.init();
    message= __arg;
    this->calls.init();
    this->pcs.init();
    this->calls.addAll(calls);
    this->pcs.addAll(pcs);
}

ThreadPanic::~ThreadPanic() {
    message.free();
    calls.free();
    pcs.free();
}

nString &ThreadPanic::getMessage() {
    return message;
}

List<sh_asp *> &ThreadPanic::getCalls() {
    return calls;
}

List<long long> &ThreadPanic::getPcs() {
    return pcs;
}
