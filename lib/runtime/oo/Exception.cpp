//
// Created by BraxtonN on 2/27/2017.
//
#include "Exception.h"
#include "../internal/Environment.h"
#include "ClassObject.h"

string Throwable::buildMessage(CallStack &cs) {
    return "";
}

void Throwable::drop() {
    this->throwable = NULL;
    this->message.free();
}

Exception::Exception(const char *msg)
        :
        throwable(&Environment::RuntimeErr, msg),
        runtime_error(msg)
{
}

Exception::Exception(const std::string &__arg)
        :
        throwable(&Environment::RuntimeErr, __arg),
        runtime_error(__arg)
{
}

Exception::Exception(ClassObject* throwable, const std::string &__arg)
        :
        throwable(throwable, __arg),
        runtime_error(__arg)
{
}

Exception::~Exception()
{
    throwable.drop();
}
