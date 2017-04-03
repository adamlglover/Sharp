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

class ref_ptr;
class ResolvedRefrence;

struct RunState {
    int_ClassObject* instance;
    int_Method* fn;
    BytecodeStream injector;
};

class ResolvedRefrence {
public:
    ResolvedRefrence()
            :
            rt(NOTRESOLVED),
            field(NULL),
            method(NULL),
            klass(NULL),
            oo(NULL),
            unresolved("")
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
        ref = ResolvedRefrence();
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
                return "refrence {" + ResolvedRefrence::toString(ref.rt) + "}";
            case UNKNOWN:
                return "string literal";

        }
    }


    ExprType et;
    ResolvedRefrence ref;
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

        rState.fn = NULL;
        rState.instance = NULL;

        env = new Environment();
        macros = new list<Method>();
        modules = new list<string>();
        classes = new list<ClassObject>();
        import_map = new list<keypair<string, list<string>>>();

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
    RunState rState;
    parser* _current;
    list<parser*> parsers;
    string out;
    list<string>* modules;
    list<Method>* macros;
    string current_module;
    list<ClassObject>* classes;
    list<keypair<string, std::list<string>>>*  import_map;
    list<context>* contexts;
    int64_t ctp;
    uint64_t uid;

    /* One off variables */
    RuntimeNote last_note;
    string last_notemsg;

    void interpret();

    bool preprocess();

    string parse_modulename(ast *pAst);

    list<AccessModifier> parse_access_modifier(ast *trunk);

    AccessModifier entity_tomodifier(token_entity entity);

    void preprocc_class_decl(ast *trunk, ClassObject* parent);

    void preprocc_var_decl(ast *pAst, ClassObject *pObject);

    bool isaccess_decl(token_entity token);

    int parse_var_accessspecifiers(list <AccessModifier> &modifiers);

    NativeField token_tonativefield(string entity);

    void preprocc_method_decl(ast *pAst, ClassObject *pObject);

    int ismethod_access_specifiers(list<AccessModifier> &modifiers);

    list<Param> ast_toparams(ast *pAst, ClassObject* parent);

    void preprocc_operator_decl(ast *pAst, ClassObject *pObject);

    void preprocc_constructor_decl(ast *pAst, ClassObject *pObject);

    void preprocc_macros_decl(ast *pAst, ClassObject *pObject);

    int ismacro_access_specifiers(list<AccessModifier> &modifiers);

    void warning(p_errors error, int line, int col, string xcmnts);

    void printnote(RuntimeNote& note, string msg);

    void parse_class_decl(ast *pAst, ClassObject* pObject);

    ClassObject *parse_base_class(ast *pAst, ClassObject* pObject);

    ref_ptr parse_refrence_ptr(ast *pAst);

    ResolvedRefrence resolve_refrence_ptr(ref_ptr &ref_ptr);

    ClassObject* resolve_class_refrence(ast *pAst, ref_ptr &ptr);

    CXX11_INLINE
    ClassObject *try_class_resolve(string intmodule, string name);

    ResolvedRefrence parse_utype(ast *pAst);

    void parse_import_decl(ast *pAst);

    ClassObject *getSuper(ClassObject *pObject);

    void parse_var_decl(ast *pAst);

    void injectConstructors();

    bool expectReferenceType(ResolvedRefrence refrence, ResolvedRefrence::RefrenceType type, ast *pAst);

    ref_ptr parse_type_identifier(ast *pAst);

    ExprValue parse_value(ast *pAst);

    ExprValue parse_expression(ast *pAst);

    void checkCast(ast* pAst, ExprValue value, ResolvedRefrence cast);

    string nativefield_tostr(NativeField nf);

    bool nativeFieldCompare(NativeField field, ExprValue::ExprType type);

    void addInstruction(Opcode opcode, double *pInt, int n);

    void partial_parse_class_decl(ast *pAst);

    context *get_context();

    bool parse_access_decl(ast *pAst, list <AccessModifier> &list, int &startpos);

    void parse_class_access_modifiers(list <AccessModifier> &list, ast* pAst);

    ClassObject *addGlobalClassObject(string basic_string, ast *pAst);

    ClassObject *addChildClassObject(string name, ast *pAst, ClassObject* super);

    context *add_context(context ctx);

    void remove_context();

    void partial_parse_var_decl(ast *pAst);

    void parse_var_access_modifiers(list <AccessModifier> &list, ast *pAst);

    void partial_parse_fn_decl(ast *pAst);

    void parse_fn_access_modifiers(list <AccessModifier> &list, ast *pAst);

    list <Param> partial_parse_utype_arglist(ast *pAst);

    string partial_parse_utypearg(ast *pAst);

    bool partial_contains_param(list <Param> &list, string name);

    void partial_parse_operator_decl(ast *pAst);
};

#define progname "bootstrap"
#define progvers "0.1.6"

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
        class_heiarchy = new list<nString>();
        module = "";
        refname = "";
    }

    void operator=(list<nString>* ch) {
        if(ch != NULL) {
            clear();
            class_heiarchy->clear();

            for(unsigned int i = 0; i < ch->size(); i++) {
                class_heiarchy->push_back(element_at(*ch, i).str());
            }
        }
    }

    void clear() {
        for(unsigned int i = 0; i < class_heiarchy->size(); i++) {
            element_at(*class_heiarchy, i).free();
        }
    }

    ~ref_ptr() {
        delete (class_heiarchy);
        class_heiarchy = NULL;
    }

    void print() {
        // dev code
        cout << "refrence pointer -----" << endl;
        cout << "id: " << refname << endl;
        cout << "mod: " << module << endl;
        cout << "class: ";
        for(nString n : *class_heiarchy)
            cout << n.str() << ".";
        cout << endl;
    }

    string module;
    list<nString>* class_heiarchy;
    string refname;
};

#endif //SHARP_RUNTIME_H
