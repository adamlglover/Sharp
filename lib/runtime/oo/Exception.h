//
// Created by bknun on 2/24/2017.
//

#ifndef SHARP_EXCEPTION_H
#define SHARP_EXCEPTION_H

#include "../../../stdimports.h"
#include "string.h"
#include <stdexcept>

class ClassObject;
class Method;
struct sh_asp;

class Throwable {
public:
    Throwable()
            :
            throwable(NULL),
            native(false)
    {
        message.init();
        this->stackTrace.init();
    }

    void init() {
        throwable =NULL;
        message.init();
        this->stackTrace.init();
        native = false;
    }

    Throwable(ClassObject* throwable, string message, bool native)
            :
            throwable(throwable),
            native(native)
    {
        this->message.init();
        this->stackTrace.init();
        this->message=message;
    }

    void operator=(Throwable& t) {
        throwable=t.throwable;
        message = t.message;
        stackTrace = t.stackTrace;
        native = t.native;
    }

    void drop();

    bool native;
    ClassObject* throwable;
    nString message, stackTrace;
};

class Exception : public std::runtime_error {
public:
    Exception(const char *msg, bool native = true);
    Exception(const std::string &__arg, bool native = true);
    Exception(ClassObject* throwable, const std::string &__arg, bool native = true);
    Exception(Throwable& throwable);

    ~Exception();

    void operator=(Exception e) {
        throwable=e.throwable;
    }

    Throwable& getThrowable() { return throwable; }

    Throwable throwable;

private:
    void setupFrame();
};

struct ExceptionTable{
    ExceptionTable()
    :
            start_pc(0),
            end_pc(0),
            local(0)
    {
        klass.init();
    }

    void init() {
        start_pc=0;
        end_pc=0;
        local=0;
        klass.init();
    }

    ~ExceptionTable()
    {
        klass.free();
    }

    void operator=(const ExceptionTable& e) {
        this->start_pc=e.start_pc;
        this->end_pc=e.end_pc;
        this->handler_pc=e.handler_pc;
        this->local=e.local;
        this->klass.init();
        this->klass=e.klass;
    }

    uint64_t start_pc, end_pc;
    uint64_t handler_pc;
    int64_t local;
    nString klass;
};

struct FinallyTable {
    FinallyTable()
    :
        start_pc(0),
        end_pc(0)
    {
    }

    void operator=(const FinallyTable& ft) {
        start_pc=ft.start_pc;
        end_pc=ft.end_pc;
    }

    uint64_t start_pc, end_pc;
};

enum FinallyCommands {
    EXEC_ALL_FINALLY=0,
    EXEC_SINGLE_FINALLY=1,
    EXEC_PRECEDING_FINALLY=3,
};

#define EXCEPTION_PRINT_MAX 20

#endif //SHARP_EXCEPTION_H
