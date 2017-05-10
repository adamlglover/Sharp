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
        thread_self->__stack[0].object.createclass(&Environment::RuntimeErr);
        thread_self->__stack[(int64_t)++_SP]
                .object.mutate(&thread_self->__stack[0].object);
    }
}
