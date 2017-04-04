//
// Created by BraxtonN on 1/30/2017.
//

#ifndef SHARP_RUNTIME_H
#define SHARP_RUNTIME_H

#include "../../stdimports.h"
#include "parser/parser.h"
#include "ClassObject.h"
#include "Environment.h"
#include "BytecodeStream.h"
#include "Opcode.h"
#include "../runtime/oo/string.h"
#include "m64Assembler.h"

class ref_ptr;
class ResolvedReference;

struct mem_access_flag {
    mem_access_flag()
    :
            ref(0),
            ptr(0)
    {
    }
    int ref;
    int ptr;
};

class ResolvedReference {
public:
    ResolvedReference()
            :
            rt(NOTRESOLVED),
            field(NULL),
            method(NULL),
            klass(NULL),
            oo(NULL),
            unresolved(""),
            array(false)
    {
    }

    enum RefrenceType {
        FIELD,
        CLASS,
        METHOD,
        MACROS,
        OO,
        NATIVE,
        NOTRESOLVED
    };

    static string toString(RefrenceType type) {
        switch(type) {
            case FIELD:
                return "field";
            case CLASS:
                return "class";
            case METHOD:
                return "method";
            case MACROS:
                return "macros";
            case OO:
                return "operator overload";
            case NATIVE:
                return "native type";
            default:
                return "unknown";
        }
    }

    string unresolved;
    bool array;
    mem_access_flag mflag;
    RefrenceType rt;
    NativeField nf;
    ClassObject* klass;
    Field* field;
    Method* method;
    OperatorOverload* oo;
};

class ExprValue {

public:
    ExprValue()
    :
            et(UNKNOWN),
            str_val(""),
            int_val(0),
            char_val(0)
    {
        ref = ResolvedReference();
    }


    enum ExprType {
        STR_LITERAL,
        CHAR_LITERAL,
        INT_LITERAL,
        HEX_LITERAL,
        BOOL_LITERAL,
        REFRENCE,
        UNKNOWN
    };

    string typeToString() {
        switch (et) {
            case STR_LITERAL:
                return "string literal";
            case CHAR_LITERAL:
                return "character literal";
            case INT_LITERAL:
                return "integer literal";
            case BOOL_LITERAL:
                return "bool literal";
            case HEX_LITERAL:
                return "hex literal";
            case REFRENCE:
                return "refrence {" + ResolvedReference::toString(ref.rt) + "}";
            case UNKNOWN:
                return "string literal";

        }
    }


    ExprType et;
    ResolvedReference ref;
    string str_val;
    int64_t int_val;
    char char_val;
    bool bool_val;
};

struct context {
    context()
    :
            super(NULL)
    {
    }

    context(ClassObject* super)
    :
            super(super)
    {
    }

    ClassObject* super;

    void clear() {
        super = NULL;
    }
};

#define init_constructor_postfix "()<init>"

class runtime
{
public:
    runtime(string out, list<parser*> parsers)
    :
            parsers(parsers),
            out(out),
            uid(0),
            errs(0),
            uo_errs(0),
            current_module(""),
            last_note("","",0,0),
            last_notemsg(""),
            ctp(-1)
    {
        for(parser* p : parsers) {
            if(!p->parsed)
                return;
        }

        env = new Environment();
        macros = new list<Method>();
        modules = new list<string>();
        classes = new list<ClassObject>();
        import_map = new list<keypair<string, list<string>>>();
        assembler = new m64Assembler();

        contexts = new list<context>();
        interpret();
    }


    bool module_exists(string name);
    bool class_exists(string module, string name);
    bool add_class(ClassObject klass);
    bool add_macros(Method macro);
    void add_module(string name);
    ClassObject* getClass(string module, string name);
    Method* getmacros(string module, string name, list<Param> params);
    void cleanup();

    Errors* errors;
    size_t errs, uo_errs;
private:
    Environment* env;
    parser* _current;
    list<parser*> parsers;
    string out;
    list<string>* modules;
    list<Method>* macros;
    string current_module;
    list<ClassObject>* classes;
    list<keypair<string, std::list<string>>>*  import_map;
    list<context>* contexts;
    m64Assembler* assembler;
    int64_t ctp;
    uint64_t uid;

    /* One off variables */
    RuntimeNote last_note;
    string last_notemsg;

    void interpret();

    bool partial_parse();

    string parse_modulename(ast *pAst);

    list<AccessModifier> parse_access_modifier(ast *trunk);

    AccessModifier entity_tomodifier(token_entity entity);

    bool isaccess_decl(token_entity token);

    int parse_var_accessspecifiers(list <AccessModifier> &modifiers);

    NativeField token_tonativefield(string entity);

    int parse_method_access_specifiers(list <AccessModifier> &modifiers);

    int parse_macro_access_specifiers(list <AccessModifier> &modifiers);

    void warning(p_errors error, int line, int col, string xcmnts);

    void printnote(RuntimeNote& note, string msg);

    void parse_class_decl(ast *pAst, ClassObject* pObject);

    ClassObject *parse_base_class(ast *pAst, ClassObject* pObject);

    ref_ptr parse_refrence_ptr(ast *pAst);

    ResolvedReference resolve_refrence_ptr(ref_ptr &ref_ptr);

    ClassObject* resolve_class_refrence(ast *pAst, ref_ptr &ptr);

    CXX11_INLINE
    ClassObject *try_class_resolve(string intmodule, string name);

    ResolvedReference parse_utype(ast *pAst);

    void parse_import_decl(ast *pAst);

    ClassObject *getSuper(ClassObject *pObject);

    void parse_var_decl(ast *pAst);

    void injectConstructors();

    bool expectReferenceType(ResolvedReference refrence, ResolvedReference::RefrenceType type, ast *pAst);

    ref_ptr parse_type_identifier(ast *pAst);

    ExprValue parse_value(ast *pAst);

    ExprValue parse_expression(ast *pAst);

    void checkCast(ast* pAst, ExprValue value, ResolvedReference cast);

    string nativefield_tostr(NativeField nf);

    bool nativeFieldCompare(NativeField field, ExprValue::ExprType type);

    void addInstruction(Opcode opcode, double *pInt, int n);

    void partial_parse_class_decl(ast *pAst);

    context *get_context();

    bool parse_access_decl(ast *pAst, list <AccessModifier> &list, int &startpos);

    void parse_class_access_modifiers(list <AccessModifier> &list, ast* pAst);

    ClassObject *addGlobalClassObject(string basic_string, std::list <AccessModifier>& modifiers, ast *pAst);

    ClassObject *addChildClassObject(string name, std::list <AccessModifier>& modifiers, ast *pAst, ClassObject* super);

    context *add_context(context ctx);

    void remove_context();

    void partial_parse_var_decl(ast *pAst);

    void parse_var_access_modifiers(std::list <AccessModifier> &list, ast *pAst);

    void partial_parse_fn_decl(ast *pAst);

    void parse_fn_access_modifiers(list <AccessModifier> &list, ast *pAst);

    list <Param> partial_parse_utype_arglist(ast *pAst);

    string partial_parse_utypearg(ast *pAst);

    bool partial_contains_param(std::list <Param> &list, string name);

    void partial_parse_operator_decl(ast *pAst);

    void partial_parse_constructor_decl(ast *pAst);

    void parse_constructor_access_modifiers(list <AccessModifier> &list, ast *pAst);

    void partial_parse_macros_decl(ast *pAst);

    void parse_macros_access_modifiers(std::list<AccessModifier> &list, ast *pAst);

    void addGlobalMacros(Method method, ast* pAst);

    void addChildMacros(Method method, ast *pAst, ClassObject *pObject);

    void parse_class_decl(ast *pAst);

    ClassObject *parse_base_class(ast *pAst, int startpos);

    void setHeadClass(ClassObject *pObject);

    mem_access_flag parse_mem_accessflag(ast *pAst);

    bool isnative_type(string type);

    Method *try_macro_resolve(string intmodule, string name, list <Param> params);
};

#define progname "bootstrap"
#define progvers "0.1.15"

struct options {
    /*
     * Activate aggressive error reporting for the bootstrapper.
     */
    bool aggressive_errors = false;

    /*
     * Only compile all the files
     */
    bool compile = false;

    /*
     * Output file to write to
     */
    string out = "out";

    /*
     * Disable warnings
     */
    bool warnings = true;

    /*
     * Optimize code
     */
    bool optimize = false;

    /*
     * Set code to be debuggable (flag only used in manifest creation)
     */
    bool debug = true;

    /*
     * Strip debugging info (if-applicable)
     */
    bool strip = false;

    /*
     * Enable warnings as errors
     */
    bool werrors = false;

    /*
     * Allow unsafe code
     */
    bool unsafe = false;
};

int _bootstrap(int argc, const char* argv[]);

extern options c_options;

template <class T>
inline T& element_at(list<T>& l, size_t x) {
    return *std::next(l.begin(), x);
}

template <class T>
inline T& element_at(list<T>& l, T search) {
    for(T var : l) {
        if(search == var)
            return var;
    }

    return *std::next(l.begin(), 0);
}

template <class T>
inline long element_index(list<T>& l, T search) {
    long iter=0;
    for(T var : l) {
        if(search == var)
            return iter;
        iter++;
    }

    return -1;
}

template <class T>
inline bool element_has(list<T>& l, T search) {
    for(T var : l) {
        if(search == var)
            return true;
    }
    return false;
}

class ref_ptr {
public:
    ref_ptr() {
        class_heiarchy = new List<string>();
        module = "";
        refname = "";
    }

    void operator=(ref_ptr ptr) {
        this->module = ptr.module;
        this->refname = ptr.refname;

        delete (class_heiarchy);
        this->class_heiarchy = ptr.class_heiarchy;
        ptr.class_heiarchy = NULL;
    }

    void free() {
        if(class_heiarchy != NULL) {
            class_heiarchy->free();
            delete (class_heiarchy);
        }
        class_heiarchy = NULL;
    }

    ~ref_ptr() {
        free();
    }

    void print() {
        // dev code
        cout << "refrence pointer -----" << endl;
        cout << "id: " << refname << endl;
        cout << "mod: " << module << endl;
        cout << "class: ";
        for(int i = 0; i < class_heiarchy->size(); i++)
            cout << class_heiarchy->at(i) << ".";
        cout << endl;
    }

    string module;
    List<string>* class_heiarchy;
    string refname;
};

#endif //SHARP_RUNTIME_H
