//
// Created by BraxtonN on 1/30/2017.
//
#include <string>
#include <sstream>
#include <cstdio>
#include "runtime.h"
#include "../util/file.h"
#include "../runtime/interp/Opcode.h"
#include "../runtime/interp/register.h"

using namespace std;

options c_options;
size_t succeeded, failed;
_operator string_toop(string op);

void runtime::interpret() {
    if(partial_parse()) {
        succeeded = 0; /* There were no errors to stop the files that succeeded */

        for(parser* p : parsers) {
            errors = new Errors(p->lines, p->sourcefile, true, c_options.aggressive_errors);
            _current = p;

            ast* trunk;
            add_context(context());
            for(size_t i = 0; i < p->treesize(); i++) {
                trunk = p->ast_at(i);

                switch(trunk->gettype()) {
                    case ast_module_decl:
                        break;
                    case ast_import_decl:
                        parse_import_decl(trunk);
                        break;
                    case ast_class_decl:
                        parse_class_decl(trunk);
                        break;
                    case ast_macros_decl:
                        break;
                    default:
                        stringstream err;
                        err << ": unknown ast type: " << trunk->gettype();
                        errors->newerror(INTERNAL_ERROR, trunk->line, trunk->col, err.str());
                        break;
                }
            }

            if(errors->_errs()){
                errs+= errors->error_count();
                uo_errs+= errors->uoerror_count();
                failed++;
            } else
                succeeded++;

            errors->free();
            delete (errors); this->errors = NULL;
        }
    }
}

ClassObject *runtime::parse_base_class(ast *pAst, int startpos) {
    context* Context = get_context();
    ClassObject* klass=NULL;

    if(startpos >= pAst->getentitycount()) {
        return NULL;
    } else {
        ref_ptr ptr = parse_refrence_ptr(pAst->getsubast(0));
        klass = resolve_class_refrence(pAst->getsubast(0), ptr);

        if(klass != NULL) {
            if((Context->super->getHeadClass() != NULL && Context->super->getHeadClass()->curcular(klass)) ||
                    Context->super->match(klass) || klass->match(Context->super->getHeadClass())) {
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col,
                                 "cyclic dependency of class `" + ptr.refname + "` in parent class `" + Context->super->getName() + "`");
            }
        }
    }

    return klass;
}

void runtime::setHeadClass(ClassObject *klass) {
    ClassObject* sup = klass->getSuperClass();
    if(sup == NULL) {
        klass->setHead(klass);
        return;
    }
    while(true) {
        if(sup->getSuperClass() == NULL) {
            klass->setHead(sup);
            return;
        } else
            sup = sup->getSuperClass();
    }
}

/*
 * TODO: add this to parsing macros
 * if(element_has(modifiers, mStatic))
        warning(REDUNDANT_TOKEN, pAst->getentity(element_index(modifiers, mStatic)).getline(),
                pAst->getentity(element_index(modifiers, mStatic)).getcolumn(), " `static`, macros are static by default");
 */
void runtime::parse_class_decl(ast *pAst) {
    context* Context = get_context();
    ast* trunk = pAst->getsubast(pAst->getsubastcount()-1);
    list<AccessModifier> modifiers;
    ClassObject* klass;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);
    string className =  pAst->getentity(startpos).gettoken();

    if(Context->super == NULL) {
        klass = getClass(current_module, className);

        klass->setFullName(current_module + "#" + className);
        setHeadClass(klass);
    }
    else {
        klass = Context->super->getChildClass(className);

        klass->setFullName(Context->super->getFullName() + "." + className);
        klass->setSuperClass(Context->super);
        setHeadClass(klass);
    }

    add_context(context(klass));
    klass->setBaseClass(parse_base_class(pAst, ++startpos));
    assembler->addinjector(klass->getFullName() + init_constructor_postfix);

    for(long i = 0; i < trunk->getsubastcount(); i++) {
        pAst = trunk->getsubast(i);

        switch(pAst->gettype()) {
            case ast_class_decl:
                parse_class_decl(pAst);
                break;
            case ast_var_decl:
                parse_var_decl(pAst);
                break;
            default: {
                stringstream err;
                err << ": unknown ast type: " << trunk->gettype();
                errors->newerror(INTERNAL_ERROR, trunk->line, trunk->col, err.str());
                break;
            }
        }
    }

//    ast* block = pAst->getsubast(pAst->getsubastcount() - 1);
//    for(uint32_t i = 0; i < block->getsubastcount(); i++) {
//        pAst = block->getsubast(i);
//
//        switch(pAst->gettype()) {
//            case ast_class_decl:
//                parse_class_decl(pAst, klass);
//                break;
//            case ast_var_decl:
//                parse_var_decl(pAst);
//
//                injectConstructors();
//                break;
//            case ast_method_decl:
//                rState.fn = NULL;
//                break;
//            case ast_operator_decl:
//                break;
//            case ast_construct_decl:
//                break;
//            case ast_macros_decl:
//                break;
//            default:
//                errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, " unexpected ast type");
//                break;
//        }
//    }
    remove_context();
}

void runtime::parse_var_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=0;

    parse_access_decl(pAst, modifiers, startpos);

    string name =  pAst->getentity(startpos).gettoken();
    Field* field = Context->super->getField(name);

    ResolvedReference ref = parse_utype(pAst->getsubast(0));

    if(pAst->hassubast(ast_value)) {
        Expression expression = parse_value(pAst);
        // TODO: do something based on the assign expression
    }

//    if(parser::isassign_exprsymbol(pAst->getentity(startpos+1).gettoken())) {
//        ExprValue value = parse_value(pAst);
//    }
//
//    if(pAst->hasentity(COMMA)) {
//
//    }
    // check for value assignment and other vars.
    // if it dosent have a var decl init it to default value in injector (if not in method)
}

void runtime::parse_import_decl(ast *pAst) {
    string import = parse_modulename(pAst);
    if(import == current_module) {
        warning(REDUNDANT_IMPORT, pAst->line, pAst->col, " '" + import + "'");
    } else {
        if(!element_has(*modules, import)) {
            errors->newerror(COULD_NOT_RESOLVE, pAst->line, pAst->col,
                             " `" + import + "` ");
        }
    }
}

Expression runtime::parse_value(ast *pAst) {
    return parse_expression(pAst->getsubast(ast_expression));
}

Expression runtime::parse_literal(ast *pAst) {
    Expression expression;

    switch(pAst->getentity(0).getid()) {
        case CHAR_LITERAL: {
            expression.type = expression_var;
            parse_charliteral(pAst->getentity(0).gettoken(), expression.code);
            return expression;
        }
        case INTEGER_LITERAL:
            expression.type = expression_var;
            parse_intliteral(pAst->getentity(0).gettoken(), expression.code, pAst);
            return expression;
        case HEX_LITERAL:
            return expression;
        case STRING_LITERAL:
            return expression;
        default:
            break;
    }

    if(pAst->getentity(0).gettoken() == "true" ||
       pAst->getentity(0).gettoken() == "false") {
        // bool literal
    }

    return expression;
}

Expression runtime::parse_primary_expression(ast *pAst) {
    Expression expression;

    if(pAst->hassubast(ast_literal)) {
        expression = parse_literal(pAst->getsubast(ast_literal));
    }

    return expression;
}

Expression runtime::parse_expression(ast *pAst) {
    Expression expression, expr;

    /*
     * Based on this ery expression we know that the value must always evaluate to a var
     * So in that case we can store the value of x in register %egx
     */
    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == _INC || pAst->getentity(0).gettokentype() == _DEC
       || pAst->getentity(0).gettokentype() == PLUS || pAst->getentity(0).gettokentype() == MINUS) {
        expression.type = expression_var;
        expr = parse_expression(pAst->getsubast(ast_expression));
        // TODO: do a check on the expression to make sure the code is correct
        expr.free();
        return expression;
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTPAREN) {
        if(pAst->hassubast(ast_utype)) {
            ResolvedReference reference = parse_utype(pAst->getsubast(ast_utype));
            expr = parse_expression(pAst->getsubast(ast_expression));
            // TODO: check cast on desired expression and that the expression is in face castable
            expr.free();
            return expression;
        }
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == NOT) {
        expr = parse_expression(pAst->getsubast(ast_expression));
        // TODO: evaluate if the expression can be "notted" or inverted

        if(pAst->getsubast_after(ast_expression) == NULL)
            return expression;
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTPAREN) {
        expr = parse_expression(pAst->getsubast(ast_expression));
        // TODO: Take the type of the expression returned to be processed correctly

        if(pAst->hassubast(ast_dotnotation_call_expr)) {
            // TODO: handle
        } else if(pAst->getsubast_after(ast_expression) == NULL)
            return expression;
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTCURLY) {
        if(pAst->hassubast(ast_vector_array)) {
            // TODO: this is not allowed...complain!
        }
    }

    if(pAst->hassubast(ast_primary_expr)) {
        expr = parse_primary_expression(pAst->getsubast(ast_primary_expr));
    }

    return expression;
}

ref_ptr runtime::parse_type_identifier(ast *pAst) {
    if(pAst->getsubastcount() == 0) {
        ref_ptr ptr;
        ptr.refname = pAst->getentity(0).gettoken();
        return ptr;
    } else
        return parse_refrence_ptr(pAst->getsubast(0));
}

ref_ptr runtime::parse_refrence_ptr(ast *pAst) {
    bool hashfound = false, last, hash = pAst->hasentity(HASH);
    string id="";
    ref_ptr ptr;

    for(long i = 0; i < pAst->getentitycount(); i++) {
        id = pAst->getentity(i).gettoken();
        last = i + 1 >= pAst->getentitycount();

        if(id == ".")
            continue;
        else if(id == "#") {
            hashfound = true;
            continue;
        }

        if(hash && !hashfound && !last) {
            ptr.module +=id;
        } else if(!last) {
            ptr.class_heiarchy->push_back(id);
        } else {
            ptr.refname = id;
        }
    }

    return ptr;
}

ClassObject *runtime::parse_base_class(ast *pAst, ClassObject* inheritor) {
    ClassObject* klass=NULL, *creator = NULL;

    for(int i = 0; i < pAst->getentitycount(); i++) {
        if(pAst->getentity(i).gettoken() == "base") {
            int base_ptr = 0;
            ref_ptr ptr = parse_refrence_ptr(pAst->getsubast(base_ptr));

            klass = resolve_class_refrence(pAst->getsubast(base_ptr), ptr);

            if(klass != NULL) {
                if(getSuper(inheritor)->match(klass) || klass->match(inheritor) || inheritor->curcular(klass) || creator->match(klass->getBaseClass())) {
                    errors->newerror(GENERIC, pAst->getsubast(base_ptr)->line, pAst->getsubast(base_ptr)->col,
                                     "cyclic dependency of class `" + ptr.refname + "` in parent class `" + inheritor->getName() + "`");
                }
                else {
                    env->create_class(int_ClassObject(klass));
                }
            }

            break;
        }
        else if(pAst->getentity(i).gettoken() == "class") {
            if(inheritor == NULL) {
                inheritor = getClass(current_module, pAst->getentity(i+1).gettoken());
                creator = inheritor;
            } else
                creator = inheritor->getChildClass(pAst->getentity(i+1).gettoken());
        }
    }

    return klass;
}

ClassObject* runtime::resolve_class_refrence(ast *pAst, ref_ptr &ptr) {
    ResolvedReference resolvedRefrence = resolve_refrence_ptr(ptr);

    if(resolvedRefrence.rt == ResolvedReference::NOTRESOLVED) {
        errors->newerror(COULD_NOT_RESOLVE, pAst->line, pAst->col, " `" + resolvedRefrence.unresolved + "` " +
                            (ptr.module == "" ? "" : "in module {" + ptr.module + "} "));
    } else {

        if(expectReferenceType(resolvedRefrence, ResolvedReference::CLASS, pAst)) {
            return resolvedRefrence.klass;
        }
    }

    return NULL;
}

ResolvedReference runtime::parse_utype(ast *pAst) {
    ref_ptr ptr=parse_type_identifier(pAst->getsubast(0));
    ResolvedReference refrence;

    if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(LEFTBRACE)) {
        refrence.array = true;
    }

    if(pAst->hassubast(ast_mem_access_flag)) {
        refrence.mflag = parse_mem_accessflag(pAst->getsubast(ast_mem_access_flag));
    }

    if(ptr.module == "" && isnative_type(ptr.refname)) {
        refrence.nf = token_tonativefield(ptr.refname);
        refrence.rt = ResolvedReference::NATIVE;
        ptr.free();
        return refrence;
    }

    refrence = resolve_refrence_ptr(ptr);

    if(refrence.rt == ResolvedReference::NOTRESOLVED) {
        errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(0)->line, pAst->col, " `" + refrence.unresolved + "` " +
                                                                   (ptr.module == "" ? "" : "in module {" + ptr.module + "} "));
    }

    ptr.free();
    return refrence;
}

//void runtime::injectConstructors() {
//    for(int_Method& constr : *rState.instance->constructors) {
//        constr.bytecode.inject(rState.injector, 0);
//    }
//}

void runtime::parse_class_decl(ast *pAst, ClassObject* pObject) {
    // create runtime class
    ClassObject* klass;
    int namepos=1;

    if(isaccess_decl(pAst->getentity(0))) {
        namepos+= this->parse_access_modifier(pAst).size();
    }

    string name =  pAst->getentity(namepos).gettoken();
    if(pObject == NULL) {
        klass = getClass(current_module, name);
    } else {
        klass = pObject->getChildClass(name);
        klass->setSuperClass(getSuper(pObject));
    }

//    rState.fn = NULL;
//    rState.instance = env->create_class(int_ClassObject(klass));
    klass->setBaseClass(parse_base_class(pAst, pObject));



    ast* block = pAst->getsubast(pAst->getsubastcount() - 1);
    for(uint32_t i = 0; i < block->getsubastcount(); i++) {
        pAst = block->getsubast(i);

        switch(pAst->gettype()) {
            case ast_class_decl:
                parse_class_decl(pAst, klass);
                break;
            case ast_var_decl:
                parse_var_decl(pAst);

//                injectConstructors();
                break;
            case ast_method_decl:
//                rState.fn = NULL;
                break;
            case ast_operator_decl:
                break;
            case ast_construct_decl:
                break;
            case ast_macros_decl:
                break;
            default:
                errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, " unexpected ast type");
                break;
        }
    }
}

bool runtime::partial_parse() {
    bool semtekerrors = false;
    for(parser* p : parsers) {
        errors = new Errors(p->lines, p->sourcefile, true, c_options.aggressive_errors);
        _current = p;

        current_module = "$invisible";
        keypair<string, std::list<string>> resolve_map;
        list<string> imports;

        ast* trunk;
        for(int i = 0; i < p->treesize(); i++) {
            trunk = p->ast_at(i);

            if(i==0) {
                if(trunk->gettype() == ast_module_decl) {
                    add_module(current_module = parse_modulename(trunk));
                    imports.push_back(current_module);
                    continue;
                } else {
                    errors->newerror(GENERIC, trunk->line, trunk->col, "module declaration must be "
                        "first in every file");
                }
            }

            add_context(context());
            switch(trunk->gettype()) {
                case ast_class_decl:
                    partial_parse_class_decl(trunk);
                    break;
                case ast_import_decl:
                    imports.push_back(parse_modulename(trunk));
                    break;
                case ast_macros_decl:
                    partial_parse_macros_decl(trunk);
                    break;
                case ast_module_decl: /* fail-safe */
                    errors->newerror(GENERIC, trunk->line, trunk->col, "file module cannot be declared more than once");
                    break;
                default:
                    stringstream err;
                    err << ": unknown ast type: " << trunk->gettype();
                    errors->newerror(INTERNAL_ERROR, trunk->line, trunk->col, err.str());
                    break;
            }
        }

        resolve_map.set(p->sourcefile, imports);
        import_map->push_back(resolve_map);
        if(errors->_errs()){
            errs+= errors->error_count();
            uo_errs+= errors->uoerror_count();

            semtekerrors = true;
            failed++;
        } else
            succeeded++;

        errors->free();
        delete (errors); this->errors = NULL;
        remove_context();
    }

    return !semtekerrors;
}

void runtime::warning(p_errors error, int line, int col, string xcmnts) {
    if(c_options.warnings) {
        if(c_options.werrors){
            errors->newerror(error, line, col, xcmnts);
        } else {
            errors->newwarning(error, line, col, xcmnts);
        }
    }
}

bool runtime::isaccess_decl(token_entity token) {
    return
            token.getid() == IDENTIFIER && token.gettoken() == "protected" ||
            token.getid() == IDENTIFIER && token.gettoken() == "private" ||
            token.getid() == IDENTIFIER && token.gettoken() == "static" ||
            token.getid() == IDENTIFIER && token.gettoken() == "const" ||
            token.getid() == IDENTIFIER && token.gettoken() == "override" ||
            token.getid() == IDENTIFIER && token.gettoken() == "public";
}

int runtime::parse_macro_access_specifiers(list <AccessModifier> &modifiers) {
    int iter=0;
    for(AccessModifier m : modifiers) {
        if(m > mStatic)
            return iter;
        iter++;
    }

    if(element_at(modifiers, 0) <= mProtected) {
        if(modifiers.size() > 2)
            return (int)(modifiers.size() - 1);
        else if(modifiers.size() == 2) {
            if(element_at(modifiers, 1) != mStatic)
                return 1;
        }
    }
    else if(element_at(modifiers, 0) == mStatic) {
        if(modifiers.size() > 1)
            return (int)(modifiers.size() - 1);
    }
    return -1;
}

int runtime::parse_var_accessspecifiers(list <AccessModifier> &modifiers) {
    int iter=0;
    for(AccessModifier m : modifiers) {
        if(m > mStatic)
            return iter;
        iter++;
    }

    if(element_at(modifiers, 0) <= mProtected) {
        if(modifiers.size() > 3)
            return (int)(modifiers.size() - 1);
        else if(modifiers.size() == 2) {
            if((element_at(modifiers, 1) != mConst
                && element_at(modifiers, 1) != mStatic))
                return 1;
        }
        else if(modifiers.size() == 3) {
            if(element_at(modifiers, 1) != mStatic)
                return 1;
            if(element_at(modifiers, 2) != mConst)
                return 2;
        }
    }
    else if(element_at(modifiers, 0) == mStatic) {
        if(modifiers.size() > 2)
            return (int)(modifiers.size() - 1);
        else if(modifiers.size() == 2 && element_at(modifiers, 1) != mConst)
            return 1;
    }
    else if(element_at(modifiers, 0) == mConst) {
        if(modifiers.size() != 1)
            return (int)(modifiers.size() - 1);
    }
    return -1;
}

int runtime::parse_method_access_specifiers(list <AccessModifier> &modifiers) {
    int iter=0, acess=0;
    for(AccessModifier m : modifiers) {
        if(m == mConst)
            return iter;
        iter++;
    }

    if(element_at(modifiers, 0) <= mProtected) {
        if(modifiers.size() > 3)
            return (int)(modifiers.size() - 1);
        else if(modifiers.size() == 2) {
            if(element_at(modifiers, 1) != mStatic && element_at(modifiers, 1) != mOverride)
                return 1;
        }
        else if(modifiers.size() == 3) {
            if(element_at(modifiers, 1) != mStatic)
                return 1;
            if(element_at(modifiers, 2) != mOverride)
                return 2;
        }
    }
    else if(element_at(modifiers, 0) == mStatic) {
        if(modifiers.size() > 2)
            return (int)(modifiers.size() - 1);
        else if(modifiers.size() == 2 && element_at(modifiers, 1) != mOverride)
            return 1;
    }
    else if(element_at(modifiers, 0) == mOverride) {
        if(modifiers.size() != 1)
            return (int)(modifiers.size() - 1);
    }
    return -1;
}

list<AccessModifier> runtime::parse_access_modifier(ast *trunk) {
    int iter=0;
    list<AccessModifier> modifiers;

    do {
        modifiers.push_back(entity_tomodifier(trunk->getentity(iter++)));
    }while(isaccess_decl(trunk->getentity(iter)));

    return modifiers;
}

void runtime::cleanup() {
    for(parser* p2 : parsers) {
        p2->free();
    }
    parsers.clear();
    delete(errors); errors = NULL;
    modules->clear();
    delete(modules); modules = NULL;

    for(ClassObject& klass : *classes) {
        klass.free();
    }
    classes->clear();
    delete (classes); classes = NULL;

    for(keypair<string, std::list<string>>& map : *import_map) {
        map.value.clear();
    }
    import_map->clear();
    delete (import_map); import_map = NULL;

    for(Method& macro : *macros) {
        macro.clear();
    }
    macros->clear();
    delete (macros); macros = NULL;

    contexts->clear();
    delete (contexts); contexts = NULL;
}

void rt_error(string message) {
    cout << "bootstrap:  error: " << message << endl;
    exit(1);
}

void help() {
    std::cerr << "Usage: bootstrap" << " {OPTIONS} SOURCE FILE(S)" << std::endl;
    cout << "Source file must have a .sharp extension to be compiled\n" << endl;
    cout << "[-options]\n\n    -V                print the bootstrap version number and exit" << endl;
    cout <<               "    -showversion      print the bootstrap version number and continue." << endl;
    cout <<               "    -o<file>          set the output object file." << endl;
    cout <<               "    -c                compile only and do not generate object file." << endl;
    cout <<               "    -a                enable aggressive error reporting." << endl;
    cout <<               "    -s                strip debugging info." << endl;
    cout <<               "    -O                optimize code." << endl;
    cout <<               "    -w                disable warnings." << endl;
    cout <<               "    -unsafe -u        allow unsafe code." << endl;
    cout <<               "    -werror           enable warnings as errors." << endl;
    cout <<               "    -release -r       disable debugging on application." << endl;
    cout <<               "    --h -?            display this help message." << endl;
}

#define opt(v) strcmp(argv[i], v) == 0
void _srt_start(list<string> files);

void print_vers();

int _bootstrap(int argc, const char* argv[]) {
    int_errs();

    if (argc < 2) { // We expect 2 arguments: the source file(s), and program options
        help();
        return 1;
    }

    list<string> files;
    for (int i = 1; i < argc; ++i) {
        args_:
            if(opt("-a")){
                c_options.aggressive_errors = true;
            }
            else if(opt("-c")){
                c_options.compile = true;
            }
            else if(opt("-o")){
                if(i+1 >= argc)
                    rt_error("output file required after option `-o`");
                else
                    c_options.out = string(argv[++i]);
            }
            else if(opt("-V")){
                print_vers();
                exit(0);
            }
            else if(opt("-O")){
                c_options.optimize = true;
            }
            else if(opt("-h") || opt("-?")){
                help();
                exit(0);
            }
            else if(opt("-R") || opt("-release")){
                c_options.optimize = true;
                c_options.debug = false;
                c_options.strip = true;
            }
            else if(opt("-s")){
                c_options.strip = true;
            }
            else if(opt("-showversion")){
                print_vers();
                cout << endl;
            }
            else if(opt("-w")){
                c_options.warnings = false;
            }
            else if(opt("-u")){
                c_options.unsafe = true;
            }
            else if(opt("-werror")){
                c_options.werrors = true;
                c_options.warnings = true;
            }
            else if(string(argv[i]).at(0) == '-'){
                rt_error("invalid option `" + string(argv[i]) + "`, try bootstrap -h");
            }
            else {
                // add the source files
                string f;
                do {
                    if(string(argv[i]).at(0) == '-')
                        goto args_;
                    f =string(argv[i++]);

                    if(!element_has(files, f))
                        files.push_back(f);
                }while(i<argc);
                break;
            }
    }

    if(files.size() == 0){
        help();
        return 1;
    }

    for(string file : files){
        if(!file::exists(file.c_str())){
            rt_error("file `" + file + "` doesnt exist!");
        }
        if(!file::endswith(".sharp", file)){
            rt_error("file `" + file + "` is not a sharp file!");
        }
    }

    _srt_start(files);
    return 0;
}

void print_vers() {
    cout << progname << " " << progvers;
}

void _srt_start(list<string> files)
{
    std::list<parser*> parsers;
    parser* p = NULL;
    tokenizer* t;
    file::stream source;
    size_t errors=0, uo_errors=0;
    succeeded=0, failed=0;

    for(string file : files) {
        source.begin();

        file::read_alltext(file.c_str(), source);
        if(source.empty()) {
            for(parser* p2 : parsers) {
                p2->free();
                delete(p2);
            }

            rt_error("file `" + file + "` is empty.");
        }

        t = new tokenizer(source.to_str(), file);
        if(t->geterrors()->_errs())
        {
            t->geterrors()->print_errors();

            errors+= t->geterrors()->error_count();
            uo_errors+= t->geterrors()->uoerror_count();
            failed++;
        }
        else {
            p = new parser(t);

            if(p->geterrors()->_errs())
            {
                p->geterrors()->print_errors();

                errors+= p->geterrors()->error_count();
                uo_errors+= p->geterrors()->uoerror_count();
                failed++;
            } else {
                parsers.push_back(p);
                succeeded++;
            }
        }

        t->free();
        delete (t);
        source.end();
    }

    if(errors == 0 && uo_errors == 0) {
        failed = 0, succeeded=0;
        runtime rt(c_options.out, parsers);

        errors+=rt.errs;
        uo_errors+=rt.uo_errs;
        rt.cleanup();
    }
    else {
        for(parser* p2 : parsers) {
            p2->free();
        }
        parsers.clear();
    }

    cout << endl << "==========================================================\n" ;
    cout << "Errors: " << (c_options.aggressive_errors ? uo_errors : errors) << " Succeeded: "
         << succeeded << " Failed: " << failed << endl;

}

bool runtime::module_exists(string name) {
    for(string& mname : *modules) {
        if(mname == name)
            return true;
    }

    return false;
}

bool runtime::class_exists(string module, string name) {
    for(ClassObject& klass : *classes) {
        if(klass.getName() == name) {
            if(module != "")
                return klass.getModuleName() == module;
            return true;
        }
    }

    return false;
}

Method *runtime::getmacros(string module, string name, list<Param> params) {
    for(Method& macro : *macros) {
        if(Param::match(*macro.getParams(), params) && name == macro.getName()) {
            if(module != "")
                return module == macro.getModule() ? &macro : NULL;

            return &macro;
        }
    }

    return NULL;
}

bool runtime::add_macros(Method macro) {
    if(getmacros(macro.getModule(), macro.getName(), *macro.getParams()) == NULL) {
        macros->push_back(macro);
        return true;
    }
    return false;
}

bool runtime::add_class(ClassObject klass) {
    if(!class_exists(klass.getModuleName(), klass.getName())) {
        classes->push_back(klass);
        return true;
    }
    return false;
}

void runtime::add_module(string name) {
    if(!module_exists(name)) {
        modules->push_back(name);
    }
}

ClassObject *runtime::getClass(string module, string name) {
    for(ClassObject& klass : *classes) {
        if(klass.getName() == name) {
            if(module != "" && klass.getModuleName() == module)
                return &klass;
            else if(module == "")
                return &klass;
        }
    }

    return NULL;
}

string runtime::parse_modulename(ast *pAst) {
    if(pAst == NULL) return "";
    pAst = pAst->getsubast(0); // module_list

    stringstream str;
    for(long i = 0; i < pAst->getentitycount(); i++) {
        str << pAst->getentity(i).gettoken();
    }
    return str.str();
}

NativeField runtime::token_tonativefield(string entity) {
    if(entity == "var")
        return fvar;
    else if(entity == "dynamic_object")
        return fdynamic;
    return fnof;
}

AccessModifier runtime::entity_tomodifier(token_entity entity) {
    if(entity.gettoken() == "public")
        return mPublic;
    else if(entity.gettoken() == "private")
            return mPrivate;
    else if(entity.gettoken() == "protected")
        return mProtected;
    else if(entity.gettoken() == "const")
        return mConst;
    else if(entity.gettoken() == "static")
        return mStatic;
    else if(entity.gettoken() == "override")
        return mOverride;
    return mStatic;
}

void runtime::printnote(RuntimeNote& note, string msg) {
    if(last_notemsg != msg && last_note.getLine() != note.getLine())
    {
        cout << note.getNote(msg);
        last_notemsg = msg;
    }
}

ClassObject* runtime::try_class_resolve(string intmodule, string name) {
    ClassObject* ref = NULL;

    if((ref = getClass(intmodule, name)) == NULL) {
        for(keypair<string, std::list<string>> &map : *import_map) {
            if(map.key == _current->sourcefile) {

                for(string mod : map.value) {
                    if((ref = getClass(mod, name)) != NULL)
                        return ref;
                }

                break;
            }
        }
    }

    return ref;
}

Method* runtime::try_macro_resolve(string intmodule, string name, list<Param> params) {
    Method* ref = NULL;

    if((ref = getmacros(intmodule, name, params)) == NULL) {
        for(keypair<string, std::list<string>> &map : *import_map) {
            if(map.key == _current->sourcefile) {

                for(string mod : map.value) {
                    if((ref = getmacros(mod, name, params)) != NULL)
                        return ref;
                }

                break;
            }
        }
    }

    return ref;
}

ResolvedReference runtime::resolve_refrence_ptr(ref_ptr &ref_ptr) {
    ResolvedReference refrence;

    if(ref_ptr.class_heiarchy->size() == 0) {
        ClassObject* klass = try_class_resolve(ref_ptr.module, ref_ptr.refname);
        if(klass == NULL) {
            refrence.rt = ResolvedReference::NOTRESOLVED;
            refrence.unresolved = ref_ptr.refname;
        } else {
            refrence.rt = ResolvedReference::CLASS;
            refrence.klass = klass;
        }
    } else {
        ClassObject* klass = try_class_resolve(ref_ptr.module, ref_ptr.class_heiarchy->get(0));
        if(klass == NULL) {
            refrence.rt = ResolvedReference::NOTRESOLVED;
            refrence.unresolved = ref_ptr.class_heiarchy->get(0);
        } else {
            ClassObject* childClass = NULL;
            string className;
            for(size_t i = 1; i < ref_ptr.class_heiarchy->size(); i++) {
                className = ref_ptr.class_heiarchy->get(i);

                if((childClass = klass->getChildClass(className)) == NULL) {
                    refrence.rt = ResolvedReference::NOTRESOLVED;
                    refrence.unresolved = className;
                    return refrence;
                } else {
                    klass = childClass;
                }

            }

            if(childClass != NULL) {
                if(childClass->getField(ref_ptr.refname) != NULL) {
                    refrence.rt = ResolvedReference::FIELD;
                    refrence.field = childClass->getField(ref_ptr.refname);
                }  else if(childClass->getChildClass(ref_ptr.refname) != NULL) {
                    refrence.rt = ResolvedReference::CLASS;
                    refrence.klass = klass->getChildClass(ref_ptr.refname);
                } else {
                    refrence.rt = ResolvedReference::NOTRESOLVED;
                    refrence.unresolved = ref_ptr.refname;
                }
            } else {
                if(klass->getField(ref_ptr.refname) != NULL) {
                    refrence.rt = ResolvedReference::FIELD;
                    refrence.field = klass->getField(ref_ptr.refname);
                } else if(klass->getChildClass(ref_ptr.refname) != NULL) {
                    refrence.rt = ResolvedReference::CLASS;
                    refrence.klass = klass->getChildClass(ref_ptr.refname);
                } else {
                    refrence.rt = ResolvedReference::NOTRESOLVED;
                    refrence.unresolved = ref_ptr.refname;
                }
            }
        }
    }

    return refrence;
}

ClassObject *runtime::getSuper(ClassObject *pObject) {
    ClassObject* super = pObject;
    for( ;; ) {
        if(pObject->getSuperClass() == NULL)
            return super;
        else {
            super = pObject->getSuperClass();
            pObject = super;
        }
    }
    return super;
}

bool runtime::expectReferenceType(ResolvedReference refrence, ResolvedReference::RefrenceType expectedType, ast *pAst) {
    if(refrence.rt == expectedType)
        return true;

    errors->newerror(EXPECTED_REFRENCE_OF_TYPE, pAst->line, pAst->col, " '" + ResolvedReference::toString(expectedType) + "' instead of '" +
            ResolvedReference::toString(refrence.rt) + "'");
    return false;
}

//void runtime::checkCast(ast* pAst, ExprValue value, ResolvedReference cast) {
//
//    if(value.et == ExprValue::UNKNOWN) {
//
//    }
//
//    switch (cast.rt) {
//        case ResolvedReference::NATIVE:
//            if(nativeFieldCompare(cast.nf, value.et))
//                warning(REDUNDANT_CAST, pAst->line, pAst->col, " `" + ResolvedReference::toString(cast.rt)
//                    + "` and `" + value.typeToString() + "`");
//
///*            if(cast.nf <= fchar) {
//                if(value.et == ExprValue::STR_LITERAL || value.et == ExprValue::REFRENCE
//                        || value.et == ExprValue::UNKNOWN) {
//                    errors->newerror(INVALID_CAST, pAst->col, pAst->line, " `" + nativefield_tostr(cast.nf) +
//                            "` and `" + value.typeToString() + "`");
//                }
//            }
//
//            if(cast.nf == fstring) {
//                if(value.et != ExprValue::STR_LITERAL) {
//                    errors->newerror(INVALID_CAST, pAst->col, pAst->line, " `string` and `" + value.typeToString() + "`");
//                }
//            }
//
//            if(cast.nf == fdynamic) {
//                if(value.et != ExprValue::REFRENCE) {
//                    errors->newerror(INVALID_CAST, pAst->col, pAst->line, " `dynamic object` and `" + value.typeToString() + "`");
//                }
//            }*/
//            break;
//        case ResolvedReference::FIELD:
//            errors->newerror(INVALID_CAST, pAst->col, pAst->line, " cannot cast field field");
//            break;
//        case ResolvedReference::METHOD:
//        case ResolvedReference::MACROS:
//        case ResolvedReference::OO:
//            errors->newerror(INVALID_CAST, pAst->col, pAst->line, " cannot cast a method");
//            break;
//
//        case ResolvedReference::CLASS:
//            if(value.et != ExprValue::REFRENCE || (value.ref.rt != ResolvedReference::CLASS && value.ref.rt != ResolvedReference::FIELD)) {
//                errors->newerror(INVALID_CAST, pAst->col, pAst->line, " `class` and `" + value.typeToString() + "`");
//                return;
//            }
//
//            if(!cast.klass->match(value.ref.klass)) {
//                errors->newerror(INVALID_CAST, pAst->col, pAst->line, " `class` and `" + value.typeToString() + "`");
//            }
//            break;
//
//        default: // not resolved already complained about
//            break;
//    }
//}

string runtime::nativefield_tostr(NativeField nf) {
    switch (nf) {
        case fvar:
            return "var";
        case fdynamic:
            return "dynamic object";
        case fvoid:
            return "void";
        case fnof:
            return "not a native field";
    }
}

//bool runtime::nativeFieldCompare(NativeField field, ExprValue::ExprType type) {
//    switch (field) {
//        default:
//            return false;
//    }
//}

//void runtime::addInstruction(Opcode opcode, double *pInt, int n) {
//    if(rState.fn == NULL) {
//        rState.injector.add((int)opcode);
//
//        for(int i = 0; i < n; i++)
//            rState.injector.add(pInt[i]);
//    } else {
//        rState.fn->bytecode.add((int)opcode);
//
//        for(int i = 0; i < n; i++)
//            rState.fn->bytecode.add(pInt[i]);
//    }
//}

context *runtime::get_context() {
    return &element_at(*contexts, ctp);
}

context *runtime::add_context(context ctx) {
    contexts->push_back(ctx);
    ctp++;
    return get_context();
}

void runtime::remove_context() {
    get_context()->clear();
    contexts->pop_back();
    ctp--;
}

void runtime::partial_parse_class_decl(ast *pAst) {
    context* Context = get_context();
    ast* astBlock = pAst->getsubast(pAst->getsubastcount()-1);
    list<AccessModifier> modifiers;
    ClassObject* klass;
    int startpos=1;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_class_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    string className =  pAst->getentity(startpos).gettoken();

    if(Context->super == NULL)
        klass = addGlobalClassObject(className, modifiers, pAst);
    else
        klass = addChildClassObject(className, modifiers, pAst, Context->super);


    add_context(context(klass));
     for(long i = 0; i < astBlock->getsubastcount(); i++) {
        pAst = astBlock->getsubast(i);

         switch(pAst->gettype()) {
             case ast_class_decl:
                 partial_parse_class_decl(pAst);
                 break;
             case ast_var_decl:
                 partial_parse_var_decl(pAst);
                 break;
             case ast_method_decl:
                 partial_parse_fn_decl(pAst);
                 break;
             case ast_operator_decl:
                 partial_parse_operator_decl(pAst);
                 break;
             case ast_construct_decl:
                 partial_parse_constructor_decl(pAst);
                 break;
             case ast_macros_decl:
                 partial_parse_macros_decl(pAst);
                 break;
             default:
                 stringstream err;
                 err << ": unknown ast type: " << pAst->gettype();
                 errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
                 break;
         }
    }
    remove_context();
}

void runtime::partial_parse_macros_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=1;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_macros_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    list<Param> params;
    string name =  pAst->getentity(startpos).gettoken();

    params = partial_parse_utype_arglist(pAst->getsubast(0));
    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    Method macro = Method(name, current_module, NULL, params, modifiers, NULL, note);

    if(Context->super == NULL) {
        addGlobalMacros(macro, pAst);
    } else {
        addChildMacros(macro, pAst, Context->super);
    }
}

void runtime::partial_parse_constructor_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=0;


    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_constructor_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    list<Param> params;
    string name = pAst->getentity(startpos).gettoken();

    if(name == Context->super->getName()) {
        params = partial_parse_utype_arglist(pAst->getsubast(0));
        RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                       pAst->line, pAst->col);

        if(!Context->super->addConstructor(Method(name, current_module, Context->super, params, modifiers, NULL, note))) {
            this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                                   "constructor `" + name + "` is already defined in the scope");
            printnote(Context->super->getConstructor(params)->note, "constructor `" + name + "` previously defined here");
        }
    } else
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "constructor `" + name + "` must be the same name as its parent");
}

void runtime::partial_parse_operator_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=2;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_fn_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    list<Param> params;
    string op =  pAst->getentity(startpos).gettoken();
    params = partial_parse_utype_arglist(pAst->getsubast(0));

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    if(!Context->super->addOperatorOverload(OperatorOverload(note, Context->super, params, modifiers, NULL, string_toop(op)))) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "function `" + op + "` is already defined in the scope");
        printnote(Context->super->getOverload(string_toop(op), params)->note, "function `" + op + "` previously defined here");
    }
}

void runtime::partial_parse_fn_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=1;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_fn_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    list<Param> params;
    string name =  pAst->getentity(startpos).gettoken();
    params = partial_parse_utype_arglist(pAst->getsubast(0));

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    if(!Context->super->addFunction(Method(name, current_module, Context->super, params, modifiers, NULL, note))) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "function `" + name + "` is already defined in the scope");
        printnote(Context->super->getFunction(name, params)->note, "function `" + name + "` previously defined here");
    }
}

void runtime::partial_parse_var_decl(ast *pAst) {
    context* Context = get_context();
    list<AccessModifier> modifiers;
    int startpos=0;


    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_var_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    string name =  pAst->getentity(startpos).gettoken();
    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(!Context->super->addField(Field(NULL, uid++, name, Context->super, &modifiers, note))) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "field `" + name + "` is already defined in the scope");
        printnote(Context->super->getField(name)->note, "field `" + name + "` previously defined here");
    }
}

void runtime::parse_macros_access_modifiers(std::list<AccessModifier> &modifiers, ast *pAst) {
    if (modifiers.size() > 2)
        this->errors->newerror(GENERIC, pAst->line, pAst->col, "too many access specifiers");
    else {
        int m = parse_macro_access_specifiers(modifiers);
        switch (m) {
            case -1:
                break;
            default:
                this->errors->newerror(INVALID_ACCESS_SPECIFIER, pAst->getentity(m).getline(),
                                       pAst->getentity(m).getcolumn(), " `" + pAst->getentity(m).gettoken() + "`");
                break;
        }
    }

    if(!element_has(modifiers, mPublic) && !element_has(modifiers, mPrivate)
       && element_has(modifiers, mProtected)) {
        modifiers.push_back(mPublic);
    }
}

void runtime::parse_constructor_access_modifiers(std::list < AccessModifier > &modifiers, ast * pAst) {
    if(modifiers.size() > 1)
        this->errors->newerror(GENERIC, pAst->line, pAst->col, "too many access specifiers");
    else {
        AccessModifier mod = element_at(modifiers, 0);

        if(mod != mPublic && mod != mPrivate && mod != mProtected)
            this->errors->newerror(INVALID_ACCESS_SPECIFIER, pAst->line, pAst->col,
                                   " `" + pAst->getentity(0).gettoken() + "`");
    }
}

void runtime::parse_fn_access_modifiers(std::list <AccessModifier> &modifiers, ast *pAst) {
    if(modifiers.size() > 3)
        this->errors->newerror(GENERIC, pAst->line, pAst->col, "too many access specifiers");
    else {
        int m= parse_method_access_specifiers(modifiers);
        switch(m) {
            case -1:
                break;
            default:
                this->errors->newerror(INVALID_ACCESS_SPECIFIER, pAst->getentity(m).getline(),
                                       pAst->getentity(m).getcolumn(), " `" + pAst->getentity(m).gettoken() + "`");
                break;
        }
    }

    if(!element_has(modifiers, mPublic) && !element_has(modifiers, mPrivate)
       && element_has(modifiers, mProtected)) {
        modifiers.push_back(mPublic);
    }
}

bool runtime::parse_access_decl(ast *pAst, list <AccessModifier> &modifiers, int &startpos) {
    if(pAst == NULL) return false;

    if(isaccess_decl(pAst->getentity(0))) {
        modifiers = parse_access_modifier(pAst);
        startpos+=modifiers.size();
        return true;
    }
    return false;
}

void runtime::parse_class_access_modifiers(std::list <AccessModifier> &modifiers, ast* pAst) {
    if(modifiers.size() > 1)
        this->errors->newerror(GENERIC, pAst->line, pAst->col, "too many access specifiers");
    else {
        AccessModifier mod = element_at(modifiers, 0);

        if(mod != mPublic && mod != mPrivate && mod != mProtected)
            this->errors->newerror(INVALID_ACCESS_SPECIFIER, pAst->line, pAst->col,
                                   " `" + pAst->getentity(0).gettoken() + "`");
    }
}

void runtime::addChildMacros(Method method, ast *pAst, ClassObject *super) {
    if(!super->addMacros(method)) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "macro `" + method.getName() + "` is already defined in the scope");

        Method* macro= super->getMacros(method.getName(), *method.getParams());
        printnote(macro->note, "macro `" + method.getName() + "` previously defined here");
    }
}

void runtime::addGlobalMacros(Method method, ast* pAst) {
    if(!add_macros(method)) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "macro `" + method.getName() + "` is already defined in the scope");

        Method* macro= getmacros(current_module, method.getName(), *method.getParams());
        printnote(macro->note, "macro `" + macro->getName() + "` previously defined here");
    }
}

ClassObject *runtime::addGlobalClassObject(string name, std::list <AccessModifier>& modifiers, ast *pAst) {
    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(!this->add_class(ClassObject(name, current_module, this->uid++, element_at(modifiers, 0), note))){

        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col, "class `" + name +
                                   "` is already defined in module {" + current_module + "}");

        printnote(this->getClass(current_module, name)->note, "class `" + name + "` previously defined here");
        return getClass(current_module, name);;
    } else
        return getClass(current_module, name);
}

ClassObject *runtime::addChildClassObject(string name, std::list <AccessModifier>& modifiers, ast *pAst, ClassObject* super) {
    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(!super->addChildClass(ClassObject(name,
                                          current_module, this->uid++, element_at(modifiers, 0),
                                          note, super))) {
        this->errors->newerror(DUPLICATE_CLASS, pAst->line, pAst->col, " '" + name + "'");

        printnote(super->getChildClass(name)->note, "class `" + name + "` previously defined here");
        return super->getChildClass(name);
    } else
        return super->getChildClass(name);
}

void runtime::parse_var_access_modifiers(std::list <AccessModifier> &modifiers, ast *pAst) {
    if(modifiers.size() > 3)
        this->errors->newerror(GENERIC, pAst->line, pAst->col, "too many access specifiers");
    else {
        int m= parse_var_accessspecifiers(modifiers);
        switch(m) {
            case -1:
                break;
            default:
                this->errors->newerror(INVALID_ACCESS_SPECIFIER, pAst->getentity(m).getline(),
                                       pAst->getentity(m).getcolumn(), " `" + pAst->getentity(m).gettoken() + "`");
                break;
        }
    }

    if(!element_has(modifiers, mPublic) && !element_has(modifiers, mPrivate)
       && element_has(modifiers, mProtected)) {
        modifiers.push_back(mPublic);
    }
}

list <Param> runtime::partial_parse_utype_arglist(ast *pAst) {
    context* Context = get_context();
    list<Param> params;

    if(pAst->gettype() != ast_utype_arg_list || pAst->getsubastcount() == 0)
        return params;

    string name;
    for(int i = 0; i < pAst->getsubastcount(); i++) {
        name = partial_parse_utypearg(pAst->getsubast(i));

        if(!partial_contains_param(params, name)) {
            RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                        pAst->line, pAst->col);

            params.push_back(Param(Field(
                    NULL, uid++,name, Context->super, NULL, note)));
        } else {
            this->errors->newerror(GENERIC, pAst->line, pAst->col, "function parameter with name '" + name + "' already exists.");
        }
    }

    return params;
}

string runtime::partial_parse_utypearg(ast *pAst) {
    return pAst->getentity(0).gettoken();
}

bool runtime::partial_contains_param(list <Param> &list, string name) {
    for(Param &param : list) {
        if(param.field->name == name)
            return true;
    }
    return false;
}

mem_access_flag runtime::parse_mem_accessflag(ast *pAst) {
    mem_access_flag flag;
    if(pAst->hasentity(AND)) {
        flag.ptr = 0;
        flag.ref = 1;
    } else if(pAst->hasentity(MULT)) {
        if(!c_options.unsafe) {
            errors->newerror(GENERIC, pAst->getentity(MULT),
                             "using explicit reference pointer requires unsafe mode (-unsafe) to be turned on");
        }

        flag.ptr = 1;
        flag.ref = 0;
    } else {
        flag.ptr = 0;
        flag.ref = 0;
    }

    return flag;
}

bool runtime::isnative_type(string type) {
    return type == "var"
           /*
            * This is not a native type but we want this to be
            * able to be set as a variable
            */
           || type == "dynamic_object"
            ;
}

/*
 * Convert a string that contains a representation of a character liter
 * i.e 'c' or '\n' to its real representation
 */
void runtime::parse_charliteral(string char_string, m64Assembler &assembler) {
    int64_t  i64;
    if(char_string.size() > 1) {
        switch(char_string.at(1)) {
            case 'n':
                assembler.push_i64(SET_Di(i64, MOVI, '\n'), ebx);
                break;
            case 't':
                assembler.push_i64(SET_Di(i64, MOVI, '\t'), ebx);
                break;
            case 'b':
                assembler.push_i64(SET_Di(i64, MOVI, '\b'), ebx);
                break;
            case 'v':
                assembler.push_i64(SET_Di(i64, MOVI, '\v'), ebx);
                break;
            case 'r':
                assembler.push_i64(SET_Di(i64, MOVI, '\r'), ebx);
                break;
            case 'f':
                assembler.push_i64(SET_Di(i64, MOVI, '\f'), ebx);
                break;
            default:
                assembler.push_i64(SET_Di(i64, MOVI, char_string.at(1)), ebx);
                break;
        }
    } else {
        assembler.push_i64(SET_Di(i64, MOVI, char_string.at(0)), ebx);
    }
}

void runtime::parse_intliteral(string int_string, m64Assembler &assembler, ast* pAst) {
    int64_t i64;
    double var;
    int_string = invalidate_underscores(int_string);

    if(all_integers(int_string)) {
        var = std::strtod (int_string.c_str(), NULL);
        if(var > DA_MAX || var < DA_MIN) {
            stringstream ss;
            ss << "integral number too large: " + int_string;
            errors->newerror(GENERIC, pAst->line, pAst->col, ss.str());
        }
        assembler.push_i64(SET_Di(i64, MOVI, var), ebx);
    }else {
        var = std::strtod (int_string.c_str(), NULL);
        if((int64_t )var > DA_MAX || (int64_t )get_low_bytes(var) < DA_MIN) {
            stringstream ss;
            ss << "integral number too large: " + int_string;
            errors->newerror(GENERIC, pAst->line, pAst->col, ss.str());
        }

        assembler.push_i64(SET_Di(i64, MOVBI, ((int64_t)var)), get_low_bytes(var));
    }
}

bool runtime::all_integers(string int_string) {
    for(char c : int_string) {
        if(!isdigit(c))
            return false;
    }
    return true;
}

string runtime::invalidate_underscores(string basic_string) {
    stringstream newstring;
    for(char c : basic_string) {
        if(c != '_')
            newstring << c;
    }
    return newstring.str();
}

int64_t runtime::get_low_bytes(double var) {
    stringstream ss;
    ss.precision(16);
    ss << var;
    string num = ss.str(), result = "";
    for(unsigned int i = 0; i < num.size(); i++) {
        if(num.at(i) == '.') {
            for(unsigned int x = i+1; x < num.size(); x++) {
                result += num.at(x);
            }
            return strtoll(result.c_str(), NULL, 0);
        }
    }
    return 0;
}

_operator string_toop(string op) {
    if(op=="+")
        return op_PLUS;
    if(op=="-")
        return op_MINUS;
    if(op=="*")
        return op_MULT;
    if(op=="/")
        return op_DIV;
    if(op=="%")
        return op_MOD;
    if(op=="++")
        return op_INC;
    if(op=="--")
        return op_DEC;
    if(op=="=")
        return op_EQUALS;
    if(op=="==")
        return op_EQUALS_EQ;
    if(op=="+=")
        return op_PLUS_EQ;
    if(op=="-=")
        return op_MIN_EQ;
    if(op=="*=")
        return op_MULT_EQ;
    if(op=="/=")
        return op_DIV_EQ;
    if(op=="&=")
        return op_AND_EQ;
    if(op=="|=")
        return op_OR_EQ;
    if(op=="!=")
        return op_NOT_EQ;
    if(op=="%=")
        return op_MOD_EQ;
    return op_NO;
}
