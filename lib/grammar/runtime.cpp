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
_operator string_toop(string op);

unsigned int runtime::classUID = 0;

void runtime::interpret() {

    if(partial_parse()) {

        resolveAllFields();
        resolveAllMethods();

        for(parser* p : parsers) {
            errors = new Errors(p->lines, p->sourcefile, true, c_options.aggressive_errors);
            _current = p;

            ast* trunk;
            add_scope(Scope(scope_global, NULL));
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
                parse_map.key.addif(p->sourcefile);
                parse_map.value.removefirst(p->sourcefile);
            } else {
                parse_map.value.addif(p->sourcefile);
                parse_map.key.removefirst(p->sourcefile);
            }

            remove_scope();
            errors->free();
            delete (errors); this->errors = NULL;
        }
    }
}

ClassObject *runtime::parse_base_class(ast *pAst, int startpos) {
    Scope* scope = current_scope();
    ClassObject* klass=NULL;

    if(startpos >= pAst->getentitycount()) {
        return NULL;
    } else {
        ref_ptr ptr = parse_refrence_ptr(pAst->getsubast(ast_refrence_pointer));
        klass = resolve_class_refrence(pAst->getsubast(ast_refrence_pointer), ptr);

        if(klass != NULL) {
            if((scope->klass->getHeadClass() != NULL && scope->klass->getHeadClass()->curcular(klass)) ||
                    scope->klass->match(klass) || klass->match(scope->klass->getHeadClass())) {
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col,
                                 "cyclic dependency of class `" + ptr.refname + "` in parent class `" + scope->klass->getName() + "`");
            }
        }
    }

    return klass;
}

void runtime::setHeadClass(ClassObject *klass) {
    ClassObject* sup = klass->getBaseClass();
    if(sup == NULL) {
        klass->setHead(klass);
        return;
    }
    while(true) {
        if(sup->getBaseClass() == NULL) {
            klass->setHead(sup);
            return;
        } else
            sup = sup->getBaseClass();
    }
}

/*
 * TODO: add this to parsing macros
 * if(element_has(modifiers, mStatic))
        warning(REDUNDANT_TOKEN, pAst->getentity(element_index(modifiers, mStatic)).getline(),
                pAst->getentity(element_index(modifiers, mStatic)).getcolumn(), " `static`, macros are static by default");
 */
void runtime::parse_class_decl(ast *pAst) {
    Scope* scope = current_scope();
    ast* trunk = pAst->getsubast(ast_block);
    list<AccessModifier> modifiers;
    ClassObject* klass;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);
    string className =  pAst->getentity(startpos).gettoken();

    if(scope->type == scope_global) {
        klass = getClass(current_module, className);

        klass->setFullName(current_module + "#" + className);
        setHeadClass(klass);
    }
    else {
        klass = scope->klass->getChildClass(className);

        klass->setFullName(scope->klass->getFullName() + "." + className);
        klass->setSuperClass(scope->klass);
        setHeadClass(klass);
    }

    add_scope(Scope(scope_class, klass));
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
                err << ": unknown ast type: " << pAst->gettype();
                errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
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
    remove_scope();
}

void runtime::parse_var_decl(ast *pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    int startpos=0;

    parse_access_decl(pAst, modifiers, startpos);

    string name =  pAst->getentity(startpos).gettoken();
    Field* field = scope->klass->getField(name);

    if(pAst->hassubast(ast_value)) {
        Expression expression = parse_value(pAst->getsubast(ast_value));
        // TODO: do something based on the assign expression
    }

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
    return parseExpression(pAst->getsubast(ast_expression));
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
            expression.type = expression_var;
            parse_hexliteral(pAst->getentity(0).gettoken(), expression.code, pAst);
            return expression;
        case STRING_LITERAL:
            expression.type = expression_string;
            parse_string_literal(pAst->getentity(0).gettoken(), expression.code);
            return expression;
        default:
            break;
    }

    if(pAst->getentity(0).gettoken() == "true" ||
       pAst->getentity(0).gettoken() == "false") {
        expression.type = expression_var;
        parse_boolliteral(pAst->getentity(0).gettoken(), expression.code);
    }

    return expression;
}

void runtime::parse_native_cast(ResolvedReference utype, Expression expression, ast *pAst) {
    int64_t i64;
    if(utype.nf >= fi8 && utype.nf <= fui64) {
        if(expression.type == expression_class) {
            mov_field(expression, pAst);
        }

        switch(utype.nf) {
            case fi8:
                expression.code.push_i64(SET_Ci(i64, MOV8, ecx,0, ebx));
                break;
            case fi16:
                expression.code.push_i64(SET_Ci(i64, MOV16, ecx,0, ebx));
                break;
            case fi32:
                expression.code.push_i64(SET_Ci(i64, MOV32, ecx,0, ebx));
                break;
            case fi64:
                expression.code.push_i64(SET_Ci(i64, MOV64, ecx,0, ebx));
                break;
            case fui8:
                expression.code.push_i64(SET_Ci(i64, MOVU8, ecx,0, ebx));
                break;
            case fui16:
                expression.code.push_i64(SET_Ci(i64, MOVU16, ecx,0, ebx));
                break;
            case fui32:
                expression.code.push_i64(SET_Ci(i64, MOVU32, ecx,0, ebx));
                break;
            case fui64:
                expression.code.push_i64(SET_Ci(i64, MOVU64, ecx,0, ebx));
                break;
        }
    } else if(utype.nf == fvar) {

    } else if(utype.nf == fdynamic) {

    }
}

bool runtime::parse_global_utype(ref_ptr& ptr, Expression& expression, ast* pAst) {
    Scope* scope = current_scope();

    if(getClass(ptr.module, ptr.refname) != NULL) {
        expression.type = expression_class;
        expression.utype.type = ResolvedReference::CLASS;
        expression.utype.klass = getClass(ptr.module, ptr.refname);
        expression.utype.refrenceName = ptr.refname;
        return true;
    }

    return false;
}

//Expression runtime::parse_utype_expression(ast *pAst) {
//    Scope* scope = current_scope();
//    ref_ptr pointer = parse_type_identifier(pAst->getsubast(ast_type_identifier));
//    Expression expression;
//
//
//    if(pointer.module == "" && pointer.class_heiarchy->size() == 0 && parser::isnative_type(pointer.refname)) {
//        expression.utype.nf = token_tonativefield(pointer.refname);
//        expression.utype.type = ResolvedReference::NATIVE;
//        expression.utype.refrenceName = pointer.refname;
//        expression.type = expression_native;
//
//        if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(RIGHTBRACE)) {
//            expression.utype.array = true;
//        }
//
//        if(pAst->hassubast(ast_mem_access_flag)) {
//            expression.utype.mflag = parse_mem_accessflag(pAst->getsubast(ast_mem_access_flag));
//        }
//        pointer.free();
//        return expression;
//    }
//
//    expression = resolve_refrence_ptr_expression(pointer, pAst);
//
//    ref_ptr ptr=parse_type_identifier(pAst->getsubast(0));
//    ResolvedReference refrence;
//
//    if(ptr.module == "" && parser::isnative_type(ptr.refname)) {
//        refrence.nf = token_tonativefield(ptr.refname);
//        refrence.type = ResolvedReference::NATIVE;
//        refrence.refrenceName = ptr.toString();
//        ptr.free();
//        return refrence;
//    }
//
//    refrence = resolve_refrence_ptr(ptr);
//    refrence.refrenceName = ptr.toString();
//
//    if(refrence.type == ResolvedReference::NOTRESOLVED) {
//        errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(0)->line, pAst->col, " `" + refrence.refrenceName + "` " +
//                                                                                 (ptr.module == "" ? "" : "in module {" + ptr.module + "} "));
//    }
//
//    ptr.free();
//    return refrence;
//}

void runtime::parseCharLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;
    int64_t  i64;
    if(token.gettoken().size() > 1) {
        switch(token.gettoken().at(1)) {
            case 'n':
                expression.code.push_i64(SET_Di(i64, MOVI, '\n'), ebx);
                break;
            case 't':
                expression.code.push_i64(SET_Di(i64, MOVI, '\t'), ebx);
                break;
            case 'b':
                expression.code.push_i64(SET_Di(i64, MOVI, '\b'), ebx);
                break;
            case 'v':
                expression.code.push_i64(SET_Di(i64, MOVI, '\v'), ebx);
                break;
            case 'r':
                expression.code.push_i64(SET_Di(i64, MOVI, '\r'), ebx);
                break;
            case 'f':
                expression.code.push_i64(SET_Di(i64, MOVI, '\f'), ebx);
                break;
            case '\\':
                expression.code.push_i64(SET_Di(i64, MOVI, '\\'), ebx);
                break;
            default:
                expression.code.push_i64(SET_Di(i64, MOVI, token.gettoken().at(1)), ebx);
                break;
        }
    } else {
        expression.code.push_i64(SET_Di(i64, MOVI, token.gettoken().at(0)), ebx);
    }
}

void runtime::parseIntegerLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;
    int64_t i64;
    double var;
    string int_string = invalidate_underscores(token.gettoken());

    if(all_integers(int_string)) {
        var = std::strtod (int_string.c_str(), NULL);
        if(var > DA_MAX || var < DA_MIN) {
            stringstream ss;
            ss << "integral number too large: " + int_string;
            errors->newerror(GENERIC, token.getline(), token.getcolumn(), ss.str());
        }
        expression.code.push_i64(SET_Di(i64, MOVI, var), ebx);
    }else {
        var = std::strtod (int_string.c_str(), NULL);
        if((int64_t )var > DA_MAX || (int64_t )var < DA_MIN) {
            stringstream ss;
            ss << "integral number too large: " + int_string;
            errors->newerror(GENERIC, token.getline(), token.getcolumn(), ss.str());
        }

        expression.code.push_i64(SET_Di(i64, MOVBI, ((int64_t)var)), abs(get_low_bytes(var)));
    }
}

void runtime::parseHexLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;
    int64_t i64;
    double var;
    string hex_string = invalidate_underscores(token.gettoken());

    var = std::strtod (hex_string.c_str(), NULL);
    if(var > DA_MAX || var < DA_MIN) {
        stringstream ss;
        ss << "integral number too large: " + hex_string;
        errors->newerror(GENERIC, token.getline(), token.getcolumn(), ss.str());
    }
    expression.code.push_i64(SET_Di(i64, MOVI, var), ebx);
}

void runtime::parseStringLiteral(token_entity token, Expression& expression) {
    expression.type = expression_string;
    string parsed_string = "";
    bool slash = false;
    for(char c : token.gettoken()) {
        if(slash) {
            slash = false;
            switch(c) {
                case 'n':
                    parsed_string += '\n';
                    break;
                case 't':
                    parsed_string += '\t';
                    break;
                case 'b':
                    parsed_string += '\b';
                    break;
                case 'v':
                    parsed_string += '\v';
                    break;
                case 'r':
                    parsed_string += '\r';
                    break;
                case 'f':
                    parsed_string += '\f';
                    break;
                case '\\':
                    parsed_string += '\\';
                    break;
                default:
                    parsed_string += c;
                    break;
            }
        }

        if(c == '\\') {
            slash = true;
        } else {
            parsed_string += c;
        }
    }

    string_map.push_back(parsed_string);
}

void parseBoolLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;
    int64_t i64;
    expression.code.push_i64(SET_Di(i64, MOVI, (token.gettoken() == "true" ? 1 : 0)), ebx);
}

Expression runtime::parseLiteral(ast* pAst) {
    Expression expression;

    switch(pAst->getentity(0).getid()) {
        case CHAR_LITERAL:
            parseCharLiteral(pAst->getentity(0), expression);
            break;
        case INTEGER_LITERAL:
            parseIntegerLiteral(pAst->getentity(0), expression);
            break;
        case HEX_LITERAL:
            parseHexLiteral(pAst->getentity(0), expression);
            break;
        case STRING_LITERAL:
            parseStringLiteral(pAst->getentity(0), expression);
            break;
        default:
            if(pAst->getentity(0).gettoken() == "true" ||
                    pAst->getentity(0).gettoken() == "true") {
                parseBoolLiteral(pAst->getentity(0), expression);
            }
            break;
    }
    expression.lnk = pAst;
    return expression;
}

ClassObject* runtime::getClassGlobal(string module, string class_name) {
    ClassObject* klass;

    if((klass = getClass(module, class_name)) == NULL) {
        for(keypair<std::string, std::list<string>> &map : *import_map) {
            if(map.key == _current->sourcefile) {

                for(string mod : map.value) {
                    if((klass = getClass(mod, class_name)) != NULL)
                        return klass;
                }

                break;
            }
        }
    }
    return klass;
}

void runtime::resolveClassHeiarchy(ClassObject* klass, ref_ptr& refrence, Expression& expression, ast* pAst, bool requireStatic) {
    int64_t i64;
    string object_name = "";
    Field* field = NULL;
    ClassObject* k;
    bool lastRefrence = false;

    for(unsigned int i = 1; i < refrence.class_heiarchy->size()+1; i++) {
        if(i >= refrence.class_heiarchy->size()) {
            // field? if not then class?
            lastRefrence = true;
            object_name = refrence.refname;
        } else
            object_name = refrence.class_heiarchy->at(i);

        if((k = klass->getChildClass(object_name)) == NULL) {

            // field?
            if((field = klass->getField(object_name)) != NULL) {
                // is static?
                if(!lastRefrence && field->array) {
                    errors->newerror(INVALID_ACCESS, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " field array");
                } else if(!lastRefrence && requireStatic && !field->isStatic() && field->type != field_unresolved) {
                    errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "field `" + object_name + "` is an instance variable");
                }



                if(lastRefrence) {
                    expression.utype.type = ResolvedReference::FIELD;
                    expression.utype.field = field;
                    expression.type = expression_field;
                }

                switch(field->type) {
                    case field_unresolved:
                        expression.utype.type = ResolvedReference::NOTRESOLVED;
                        expression.utype.refrenceName = object_name;
                        return;
                    case field_native:
                        if(lastRefrence){}
                        else {
                            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "field `" + object_name + "` is not a class variable");
                            expression.utype.refrenceName = object_name;
                            return;
                        }
                        break;
                    case field_class:
                        klass = field->klass;
                        break;
                }
            } else {
                errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + object_name + "` " +
                                                                                                                                           (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));
                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = object_name;
                expression.type = expression_unknown;
                return;
            }
        } else {
            if(lastRefrence) {
                expression.code.free();
                expression.utype.type = ResolvedReference::CLASS;
                expression.utype.klass = klass;
                expression.type = expression_class;
            }
            klass = k;
        }
    }
}

void runtime::resolveFieldHeiarchy(Field* field, ref_ptr& refrence, Expression& expression, ast* pAst) {
    switch(field->type) {
        case field_unresolved:
            return;
        case field_native:
            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "field `" + field->name + "` is not a class variable");
            expression.utype.type = ResolvedReference::NATIVE;
            expression.utype.nf = field->nf;
            return;
        case field_class:
            resolveClassHeiarchy(field->klass, refrence, expression, pAst, false);
            return;
    }
}

void runtime::resolveUtype(ref_ptr& refrence, Expression& expression, ast* pAst) {
    Scope* scope = current_scope();
    int64_t i64;

    if(refrence.singleRefrence()) {
        ClassObject* klass=NULL;
        Field* field=NULL;

        if(scope->type == scope_global) {
            if((klass = getClassGlobal(refrence.module, refrence.refname)) != NULL) {
                expression.utype.type = ResolvedReference::CLASS;
                expression.utype.klass = klass;
                expression.type = expression_class;
            } else {
                /* Un resolvable */
                errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + refrence.refname + "` " +
                                                                                                                                           (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));

                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = refrence.refname;
                expression.type = expression_unknown;
            }
        } else {
            // scope_class? | scope_instance_block? | scope_static_block?
            if(scope->type != scope_class && (field = scope->getLocalField(refrence.refname)) != NULL) {
                expression.utype.type = ResolvedReference::FIELD;
                expression.utype.field = field;
                expression.code.push_i64(SET_Di(i64, MOVL, scope->getLocalFieldIndex(refrence.refname)));
                expression.type = expression_field;
            }
            else if((field = scope->klass->getField(refrence.refname)) != NULL) {
                // field?
                expression.utype.type = ResolvedReference::FIELD;
                expression.utype.field = field;
                expression.code.push_i64(SET_Di(i64, MOVN, field->vaddr));
                expression.type = expression_field;
            } else {
                if((klass = getClassGlobal(refrence.module, refrence.refname)) != NULL) {
                    // global class ?
                    expression.utype.type = ResolvedReference::CLASS;
                    expression.utype.klass = klass;
                    expression.type = expression_class;
                } else {
                    /* Un resolvable */
                    errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + refrence.refname + "` " +
                                                                                                                                               (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));

                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = refrence.refname;
                    expression.type = expression_unknown;
                }
            }
        }
    } else if(refrence.singleRefrenceModule()){
        /* Must be a class i.e module#globalClass */
        ClassObject* klass=NULL;

        // in this case we ignore scope
        if((klass = getClassGlobal(refrence.module, refrence.refname)) != NULL) {
            expression.utype.type = ResolvedReference::CLASS;
            expression.utype.klass = klass;
            expression.type = expression_class;
        } else {
            /* Un resolvable */
            errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + refrence.refname + "` " +
                                                                                                                                       (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));

            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = refrence.refname;
            expression.type = expression_unknown;
        }
    } else {
        /* field? or class? */
        ClassObject* klass=NULL;
        Field* field=NULL;
        string starter_name = refrence.class_heiarchy->at(0);

        if(scope->type == scope_global) {
            // class?
            if((klass = getClassGlobal(refrence.module, starter_name)) != NULL) {
                resolveClassHeiarchy(klass, refrence, expression, pAst);
                return;
            } else {
                /* un resolvable */
                errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                           (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));
                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = refrence.toString();
                expression.type = expression_unknown;

            }
        } else {
            // scope_class? | scope_instance_block? | scope_static_block?
            if(refrence.module != "") {
                if((klass = getClassGlobal(refrence.module, starter_name)) != NULL) {
                    resolveClassHeiarchy(klass, refrence, expression, pAst);
                    return;
                } else {
                    errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                               (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));
                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = refrence.toString();
                    expression.type = expression_unknown;
                    return;
                }
            }

            if(scope->type != scope_class && (field = scope->getLocalField(refrence.refname)) != NULL) {
                resolveFieldHeiarchy(field, refrence, expression, pAst);
                return;
            }
            else if((field = scope->klass->getField(starter_name)) != NULL) {
                resolveFieldHeiarchy(field, refrence, expression, pAst);
                return;
            } else {
                if((klass = getClassGlobal(refrence.module, starter_name)) != NULL) {
                    resolveClassHeiarchy(klass, refrence, expression, pAst);
                    return;
                } else {
                    errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                               (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));
                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = refrence.toString();
                    expression.type = expression_unknown;
                }
            }
        }

    }
}

Expression runtime::parseUtype(ast* pAst) {
    ref_ptr ptr=parse_type_identifier(pAst->getsubast(0));
    Expression expression;

    if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(LEFTBRACE)) {
        expression.utype.array = true;
    }

    if(ptr.singleRefrence() && parser::isnative_type(ptr.refname)) {
        expression.utype.nf = token_tonativefield(ptr.refname);
        expression.utype.type = ResolvedReference::NATIVE;
        expression.type = expression_native;
        expression.utype.refrenceName = ptr.toString();
        expression.lnk = pAst;
        ptr.free();
        return expression;
    }

    resolveUtype(ptr, expression, pAst);

    expression.lnk = pAst;
    expression.utype.refrenceName = ptr.toString();
    ptr.free();
    return expression;
}

Expression runtime::psrseUtypeClass(ast* pAst) {
    Expression expression = parseUtype(pAst->getsubast(ast_utype));

    if(pAst->hasentity(DOT)) {
        expression.dot = true;
    }

    if(expression.type == expression_class) {
        // TODO: add asm to push class id
    } else {
        errors->newerror(GENERIC, pAst->getsubast(ast_utype)->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_utype)->getsubast(ast_type_identifier)->col, "expected class");
    }
    expression.lnk = pAst;
    return expression;
}

bool runtime::splitMethodUtype(string& name, ref_ptr& ptr) {
    if(ptr.singleRefrence() || ptr.singleRefrenceModule()) {
        return false;
    } else {
        name = ptr.refname;
        ptr.refname = ptr.class_heiarchy->get(ptr.class_heiarchy->size()-1); // assign the last field in the accessor to class
        ptr.class_heiarchy->remove(ptr.class_heiarchy->size()-1);
        return true;
    }
}

List<Expression> runtime::parseValueList(ast* pAst) {
    List<Expression> expressions;

    ast* vAst;
    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        vAst = pAst->getsubast(i);
        expressions.add(parse_value(vAst));
    }

    return expressions;
}

bool runtime::expressionListToParams(List<Param> &params, List<Expression> expressions) {
    bool success = true;
    Expression* expression;
    List<AccessModifier> mods;
    RuntimeNote note;
    Field field;

    for(unsigned int i = 0; i < expressions.size(); i++) {
        expression = &expressions.get(i);
        if(expression->lnk != NULL) {
            note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(expression->lnk->line), expression->lnk->line, expression->lnk->col);
        }

        if(expression->type == expression_var) {
            field = Field(fvar, 0, "", NULL, mods, note);
            field.type = field_native;

            params.add(Param(field));
        } else if(expression->type == expression_string) {
            field = Field(fvar, 0, "", NULL, mods, note);
            field.array = true;

            /* Native string is a char array */
            params.add(Param(field));
        } else if(expression->type == expression_class) {
            success = false;
            errors->newerror(INVALID_PARAM, expression->lnk->line, expression->lnk->col, " `class`, param must be lvalue");
        } else if(expression->type == expression_lclass) {
            field = Field(expression->utype.klass, 0, "", NULL, mods, note);
            field.type = field_class;

            params.add(Param(field));
        } else if(expression->type == expression_field) {
            params.add(*expression->utype.field);
        } else if(expression->type == expression_native) {
            success = false;
            errors->newerror(GENERIC, expression->lnk->line, expression->lnk->col, " unexpected symbol `" + expression->utype.refrenceName + "`");
        } else if(expression->type == expression_dynamicclass) {
            field = Field(fdynamic, 0, "", NULL, mods, note);
            field.type = field_native;
            params.add(field);
        } else {
            /* Unknown expression */
            success = false;
        }
    }

    return success;
}

template<class T>
void runtime::__freeList(List<T> &lst) {
    for(unsigned int i = 0; i < lst.size(); i++) {
        lst.get(i).free();
    }

    lst.free();
}

Method* runtime::resolveMethodUtype(ast* pAst, ast* pAst2) {
    Scope* scope = current_scope();
    Method* fn = NULL;

    /* This is a naked utype so we dont haave to worry about brackets */
    ref_ptr ptr;
    string methodName = "";
    List<Param> params;
    List<Expression> expressions = parseValueList(pAst2);
    Expression expression;

    expressionListToParams(params, expressions);
    ptr = parse_type_identifier(pAst->getsubast(ast_type_identifier));

    if(splitMethodUtype(methodName, ptr)) {
        // accessor
        resolveUtype(ptr, expression, pAst);
        if(expression.type == expression_class || (expression.type == expression_field && expression.utype.field->type == field_class)) {
            ClassObject* klass;
            if(expression.type == expression_class) {
                klass = expression.utype.klass;
            } else {
                klass = expression.utype.field->klass;
            }

            if((fn = klass->getMacros(methodName, params)) != NULL){}
            else if((fn = klass->getFunction(methodName, params)) != NULL){}
            else if((fn = klass->getOverload(string_toop(methodName), params)) != NULL){}
            else if((fn = klass->getConstructor(params)) != NULL) {}
            else {
                if(string_toop(methodName) != op_NO) methodName = "operator" + methodName;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + (klass == NULL ? ptr.refname : methodName) + "`");
            }
        } else if(expression.utype.type == ResolvedReference::NOTRESOLVED) {
        }
        else {
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + (expression.utype.klass == NULL ? ptr.refname : methodName) + "`");
        }
    } else {
        // method or global macros
        if(ptr.singleRefrence()) {
            if(scope->type == scope_global) {
                // must be macros
                if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                    errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname + "`");
                }
            } else {

                if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                    if((fn = scope->klass->getMacros(ptr.refname, params)) != NULL){}
                    else if((fn = scope->klass->getFunction(ptr.refname, params)) != NULL){}
                    else if((fn = scope->klass->getOverload(string_toop(ptr.refname), params)) != NULL){}
                    else if((fn = scope->klass->getConstructor(params)) != NULL) {}
                    else {
                        if(string_toop(methodName) != op_NO) methodName = "operator" + ptr.refname;
                        else methodName = ptr.refname;

                        errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + methodName + "`");
                    }
                }
            }
        } else {
            // must be macros
            if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname + "`");
            }
        }
    }

    __freeList(params);
    __freeList(expressions);
    ptr.free();
    return fn;
}

expression_type runtime::methodReturntypeToExpressionType(Method* fn) {
    if(fn->type == lclass_object)
        return expression_lclass;
    else if(fn->type == lnative_object) {
        if(fn->nobj == fdynamic)
            return expression_dynamicclass;
        else
            return expression_var;
    } else if(fn->type == lvoid)
        return expression_void;
    else
        return expression_unknown;
}

Expression runtime::parseDotNotationCall(ast* pAst) {
    string method_name="";
    Expression expression;
    Method* fn;

    if(pAst->hassubast(ast_value_list)) {
        fn = resolveMethodUtype(pAst->getsubast(ast_utype), pAst->getsubast(ast_value_list));
        if(fn != NULL) {
            expression.type = methodReturntypeToExpressionType(fn);
            if(expression.type == expression_lclass)
                expression.utype.klass = fn->klass;
        } else
            expression.type = expression_unknown;
    } else {
        expression = parseUtype(pAst->getsubast(ast_utype));
    }

    if(pAst->hasentity(DOT)) {
        expression.dot = true;
    }

    expression.lnk = pAst;
    return expression;
}

Expression runtime::parsePrimaryExpression(ast* pAst) {
    pAst = pAst->getsubast(0);

    switch(pAst->gettype()) {
        case ast_literal_e:
            return parseLiteral(pAst->getsubast(ast_literal));
        case ast_utype_class_e:
            return psrseUtypeClass(pAst);
        case ast_dot_not_e:
            return parseDotNotationCall(pAst->getsubast(ast_dotnotation_call_expr));
        default:
            stringstream err;
            err << ": unknown ast type: " << pAst->gettype();
            errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
            return Expression(pAst); // not an expression!
    }
}

int recursive_expressions = 0; // TODO: use this to figure out sneaky user errors
Expression runtime::parseExpression(ast *pAst) {
    ast* encap = pAst->getsubast(0);

    switch(encap->gettype()) {
        case ast_primary_expr:
            return parsePrimaryExpression(encap);
            break;
        default:
            stringstream err;
            err << ": unknown ast type: " << pAst->gettype();
            errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
            return Expression(encap); // not an expression!
    }

    /*
     * Based on this very expression we know that the value must always evaluate to a var
     * So in that case we can store the value of x in register %egx
     *
    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == _INC || pAst->getentity(0).gettokentype() == _DEC) {
        expression = parse_expression(pAst->getsubast(ast_expression));
        pre_incdec_expression(expression, pAst->getsubast(ast_expression));

        remove_context();
        expression.type = expression_var;
        return expression;
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTPAREN) {
        if(pAst->hassubast(ast_utype)) {
            expression = parse_cast_expression(Context, pAst);
            remove_context();
            return expression;
        }
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == NOT) {
        expression = parse_expression(pAst->getsubast(ast_expression));
        // TODO: evaluate if the expression can be "notted" or inverted

        if (pAst->getsubast_after(ast_expression) == NULL) {
            remove_context();
            return expression;
        }
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTPAREN) {
        expression = parse_expression(pAst->getsubast(ast_expression));
        // TODO: Take the type of the expression returned to be processed correctly

        if (pAst->hassubast(ast_dotnotation_call_expr)) {
            // TODO: handle
        } else if (pAst->getsubast_after(ast_expression) == NULL){
            remove_context();
            return expression;
        }
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettokentype() == LEFTCURLY) {
        if(pAst->hassubast(ast_vector_array)) {
            // TODO: this is not allowed...complain!
        }
    }

    if(pAst->hassubast(ast_primary_expr)) {
        expression = parse_primary_expression(pAst->getsubast(ast_primary_expr));
        if(pAst->getsubast_after(ast_primary_expr) == NULL){
            remove_context();
            return expression;
        }
    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettoken() == "self") {

    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettoken() == "base") {

    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettoken() == "null") {

    }

    if(pAst->getentitycount() != 0 && pAst->getentity(0).gettoken() == "new") {

    }
    */
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
            if(ptr.module == "")
                ptr.module =id;
            else
                ptr.module += "." + id;
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

    if(resolvedRefrence.type == ResolvedReference::NOTRESOLVED) {
        errors->newerror(COULD_NOT_RESOLVE, pAst->line, pAst->col, " `" + resolvedRefrence.refrenceName + "` " +
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


    if(ptr.module == "" && parser::isnative_type(ptr.refname)) {
        refrence.nf = token_tonativefield(ptr.refname);
        refrence.type = ResolvedReference::NATIVE;
        refrence.refrenceName = ptr.toString();
    }else {
        refrence = resolve_refrence_ptr(ptr);
        refrence.refrenceName = ptr.toString();

        if(refrence.type == ResolvedReference::NOTRESOLVED) {
            errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(0)->line, pAst->col, " `" + refrence.refrenceName + "` " +
                                                                                     (ptr.module == "" ? "" : "in module {" + ptr.module + "} "));
        }
    }

    if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(RIGHTBRACE)) {
        refrence.array = true;
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

void runtime::resolveVarDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    int startpos=0;

    parse_access_decl(pAst, modifiers, startpos);
    string name =  pAst->getentity(startpos).gettoken();
    Field* field = scope->klass->getField(name);
    Expression expression = parseUtype(pAst->getsubast(ast_utype));
    if(expression.utype.type == ResolvedReference::CLASS) {
        field->klass = expression.utype.klass;
        field->type = field_class;
    } else if(expression.utype.type == ResolvedReference::NATIVE) {
        field->nf = expression.utype.nf;
        field->type = field_native;
    } else {
        field->type = field_unresolved;
    }

    field->array = expression.utype.array;
}

Field runtime::fieldMapToField(string param_name, ResolvedReference utype, ast* pAst) {
    Field field;

    if(utype.type == ResolvedReference::FIELD) {
        errors->newerror(COULD_NOT_RESOLVE, utype.field->note.getLine(), utype.field->note.getCol(), " `" + utype.field->name + "`");
        field.type = field_unresolved;
        field.note = utype.field->note;
        field.modifiers.addAll(utype.field->modifiers);
        field.nf = utype.field->nf;
    } else if(utype.type == ResolvedReference::CLASS) {
        field.type = field_class;
        field.note = utype.klass->note;
        field.klass = utype.klass;
        field.modifiers.add(utype.klass->getAccessModifier());
    } else if(utype.type == ResolvedReference::NATIVE) {
        field.type = field_native;
        field.modifiers.add(mPublic);
        field.nf = utype.nf;
    }
    else {
        field.type = field_unresolved;
    }

    field.fullName = param_name;
    field.uid = uid++;
    field.name = param_name;
    field.array = utype.array;
    field.note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line), pAst->line, pAst->col);

    if(utype.type == ResolvedReference::FIELD)
        return *utype.field;

    return field;
}

keypair<string, ResolvedReference> runtime::parseUtypeArg(ast* pAst) {
    keypair<string, ResolvedReference> utype_arg;
    utype_arg.value = parseUtype(pAst->getsubast(ast_utype)).utype;
    utype_arg.key = pAst->getentity(0).gettoken();

    return utype_arg;
}

keypair<List<string>, List<ResolvedReference>> runtime::parseUtypeArgList(ast* pAst) {
    keypair<List<string>, List<ResolvedReference>> utype_argmap;
    keypair<string, ResolvedReference> utype_arg;
    utype_argmap.key.init();
    utype_argmap.value.init();

    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        utype_arg = parseUtypeArg(pAst->getsubast(i));
        utype_argmap.key.push_back(utype_arg.key);
        utype_argmap.value.push_back(utype_arg.value);
    }

    return utype_argmap;
}

bool runtime::containsParam(List<Param> params, string param_name) {
    for(unsigned int i = 0; i < params.size(); i++) {
        if(params.get(i).field.name == param_name)
            return true;
    }
    return false;
}

void runtime::parseMethodParams(List<Param>& params, keypair<List<string>, List<ResolvedReference>> fields, ast* pAst) {
    for(unsigned int i = 0; i < fields.key.size(); i++) {
        if(containsParam(params, fields.key.get(i))) {
            errors->newerror(SYMBOL_ALREADY_DEFINED, pAst->line, pAst->col, "symbol `" + fields.key.get(i) + "` already defined in the scope");
        } else
            params.add(Param(fieldMapToField(fields.key.get(i), fields.value.get(i), pAst)));
    }
}

void runtime::resolveMethodDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=1;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_fn_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    List<Param> params;
    string name =  pAst->getentity(startpos).gettoken();
    modCompat.addAll(modifiers);
    parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    Expression utype;
    Method method = Method(name, current_module, scope->klass, params, modCompat, NULL, note);
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, method);
    } else
        method.type = lvoid;

    if(!scope->klass->addFunction(method)) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "function `" + name + "` is already defined in the scope");
        printnote(scope->klass->getFunction(name, params)->note, "function `" + name + "` previously defined here");
    }
}

void runtime::parseMethodReturnType(Expression& expression, Method& method) {
    if(expression.type == expression_class) {
        method.type = lclass_object;
        method.klass = expression.utype.klass;
    } else if(expression.type == expression_native) {
        method.type = lnative_object;
        method.nobj = expression.utype.nf;
    } else {
        method.type = lundefined;
        errors->newerror(GENERIC, expression.lnk->line, expression.lnk->col, "expected class or native type for method's return value");
    }
}

void runtime::resolveMacrosDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=1;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_macros_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    if(element_has(modifiers, mStatic))
        warning(REDUNDANT_TOKEN, pAst->getentity(element_index(modifiers, mStatic)).getline(),
                pAst->getentity(element_index(modifiers, mStatic)).getcolumn(), " `static`, macros are static by default");

    List<Param> params;
    string name =  pAst->getentity(startpos).gettoken();
    modCompat.addAll(modifiers);
    parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    // TODO: parse return type
    Expression utype;
    Method macro = Method(name, current_module, scope->klass, params, modCompat, NULL, note);
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, macro);
    } else
        macro.type = lvoid;

    if(scope->type == scope_global) {
        addGlobalMacros(macro, pAst);
    } else {
        addChildMacros(macro, pAst, scope->klass);
    }
}

void runtime::resolveOperatorDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=2;

    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_fn_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    List<Param> params;
    string op =  pAst->getentity(startpos).gettoken();
    modCompat.addAll(modifiers);
    parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    Expression utype;
    OperatorOverload operatorOverload = OperatorOverload(note, scope->klass, params, modCompat, NULL, string_toop(op));
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, operatorOverload);
    } else
        operatorOverload.type = lvoid;

    if(!scope->klass->addOperatorOverload(operatorOverload)) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "function `" + op + "` is already defined in the scope");
        printnote(scope->klass->getOverload(string_toop(op), params)->note, "function `" + op + "` previously defined here");
    }
}

void runtime::resolveConstructorDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=0;


    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_constructor_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    List<Param> params;
    string name = pAst->getentity(startpos).gettoken();
    modCompat.addAll(modifiers);

    if(name == scope->klass->getName()) {
        parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));
        RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                       pAst->line, pAst->col);

        Method method = Method(name, current_module, scope->klass, params, modCompat, NULL, note);
        method.type = lvoid;

        if(!scope->klass->addConstructor(method)) {
            this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                                   "constructor `" + name + "` is already defined in the scope");
            printnote(scope->klass->getConstructor(params)->note, "constructor `" + name + "` previously defined here");
        }
    } else
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "constructor `" + name + "` must be the same name as its parent");
}

void runtime::resolveClassDecl(ast* pAst) {
    Scope* scope = current_scope();
    ast* astBlock = pAst->getsubast(ast_block);
    list<AccessModifier> modifiers;
    ClassObject* klass;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);
    string name =  pAst->getentity(startpos).gettoken();

    if(scope->type == scope_global)
        klass = getClass(current_module, name);
    else
        klass = scope->klass->getChildClass(name);

    add_scope(Scope(scope_class, klass));
    for(long i = 0; i < astBlock->getsubastcount(); i++) {
        pAst = astBlock->getsubast(i);

        switch(pAst->gettype()) {
            case ast_class_decl:
                resolveClassDecl(pAst);
                break;
            case ast_var_decl:
                if(!resolvedFields)
                    resolveVarDecl(pAst);
                break;
            case ast_method_decl:
                if(resolvedFields)
                    resolveMethodDecl(pAst);
                break;
            case ast_macros_decl:
                if(resolvedFields)
                    resolveMacrosDecl(pAst);
                break;
            case ast_operator_decl:
                if(resolvedFields)
                    resolveOperatorDecl(pAst);
                break;
            case ast_construct_decl:
                if(resolvedFields)
                    resolveConstructorDecl(pAst);
                break;
            default:
                stringstream err;
                err << ": unknown ast type: " << pAst->gettype();
                errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
                break;
        }
    }

    remove_scope();
}

void runtime::resolveAllMethods() {
    resolvedFields = true;
    /*
     * All fields have been processed, so we are good to fully parse utypes for method params
     */
    resolveAllFields();
}

void runtime::resolveAllFields() {
    for(parser* p : parsers) {
        errors = new Errors(p->lines, p->sourcefile, true, c_options.aggressive_errors);
        _current = p;
        current_module = "$invisible";

        ast* trunk;
        add_scope(Scope(scope_global, NULL));
        for(int i = 0; i < p->treesize(); i++) {
            trunk = p->ast_at(i);

            if(i==0) {
                if(trunk->gettype() == ast_module_decl) {
                    add_module(current_module = parse_modulename(trunk));
                    continue;
                }
            }

            switch(trunk->gettype()) {
                case ast_class_decl:
                    resolveClassDecl(trunk);
                    break;
                case ast_macros_decl:
                    if(resolvedFields)
                        resolveMacrosDecl(trunk);
                    break;
                default:
                    /* ignore */
                    break;
            }
        }
        remove_scope();

        if(errors->_errs()){
            errs+= errors->error_count();
            uo_errs+= errors->uoerror_count();

            parse_map.key.addif(p->sourcefile);
            parse_map.value.removefirst(p->sourcefile);
        } else {
            parse_map.value.addif(p->sourcefile);
            parse_map.key.removefirst(p->sourcefile);
        }

        errors->free();
        delete (errors); this->errors = NULL;
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
        add_scope(Scope(scope_global, NULL));
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

            switch(trunk->gettype()) {
                case ast_class_decl:
                    partial_parse_class_decl(trunk);
                    break;
                case ast_import_decl:
                    imports.push_back(parse_modulename(trunk));
                    break;
                case ast_macros_decl: /* Will be parsed later */
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
            parse_map.key.addif(p->sourcefile);
            parse_map.value.removefirst(p->sourcefile);
        } else {
            parse_map.value.addif(p->sourcefile);
            parse_map.key.removefirst(p->sourcefile);
        }

        errors->free();
        delete (errors); this->errors = NULL;
        remove_scope();
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

    scope_map->free();
    delete (scope_map); scope_map = NULL;
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
    int succeeded=0, failed=0;

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

        failed = rt.parse_map.key.size();
        succeeded = rt.parse_map.value.size();

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

Method *runtime::getmacros(string module, string name, List<Param>& params) {
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

        klass.vaddr = classUID++;
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
    else if(entity == "__int8")
        return fi8;
    else if(entity == "__int16")
        return fi16;
    else if(entity == "__int32")
        return fi32;
    else if(entity == "__int64")
        return fi64;
    else if(entity == "__uint8")
        return fui8;
    else if(entity == "__uint16")
        return fui16;
    else if(entity == "__uint32")
        return fui32;
    else if(entity == "__uint64")
        return fui64;
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

ResolvedReference runtime::resolve_refrence_ptr(ref_ptr &ref_ptr) {
    ResolvedReference refrence;

    if(ref_ptr.class_heiarchy->size() == 0) {
        ClassObject* klass = try_class_resolve(ref_ptr.module, ref_ptr.refname);
        if(klass == NULL) {
            refrence.type = ResolvedReference::NOTRESOLVED;
            refrence.refrenceName = ref_ptr.refname;
        } else {
            refrence.type = ResolvedReference::CLASS;
            refrence.klass = klass;
        }
    } else {
        ClassObject* klass = try_class_resolve(ref_ptr.module, ref_ptr.class_heiarchy->get(0));
        if(klass == NULL) {
            refrence.type = ResolvedReference::NOTRESOLVED;
            refrence.refrenceName = ref_ptr.class_heiarchy->get(0);
        } else {
            ClassObject* childClass = NULL;
            string className;
            for(size_t i = 1; i < ref_ptr.class_heiarchy->size(); i++) {
                className = ref_ptr.class_heiarchy->get(i);

                if((childClass = klass->getChildClass(className)) == NULL) {
                    refrence.type = ResolvedReference::NOTRESOLVED;
                    refrence.refrenceName = className;
                    return refrence;
                } else {
                    klass = childClass;
                }

            }

            if(childClass != NULL) {
                if(childClass->getChildClass(ref_ptr.refname) != NULL) {
                    refrence.type = ResolvedReference::CLASS;
                    refrence.klass = klass->getChildClass(ref_ptr.refname);
                } else {
                    refrence.type = ResolvedReference::NOTRESOLVED;
                    refrence.refrenceName = ref_ptr.refname;
                }
            } else {
                if(klass->getChildClass(ref_ptr.refname) != NULL) {
                    refrence.type = ResolvedReference::CLASS;
                    refrence.klass = klass->getChildClass(ref_ptr.refname);
                } else {
                    refrence.type = ResolvedReference::NOTRESOLVED;
                    refrence.refrenceName = ref_ptr.refname;
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
    if(refrence.type == expectedType)
        return true;

    errors->newerror(EXPECTED_REFRENCE_OF_TYPE, pAst->line, pAst->col, " '" + ResolvedReference::toString(expectedType) + "' instead of '" +
            ResolvedReference::toString(refrence.type) + "'");
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
        case fi8:
            return "__int8";
        case fi16:
            return "__int16";
        case fi32:
            return "__int32";
        case fi64:
            return "__int64";
        case fui8:
            return "__uint8";
        case fui16:
            return "__uint16";
        case fui32:
            return "__uint32";
        case fui64:
            return "__uint64";
        case fdynamic:
            return "dynamic object";
        case fvoid:
            return "void";
        case fnof:
            return "not a native field";
        default:
            return "unknown";
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

void runtime::partial_parse_class_decl(ast *pAst) {
    Scope* scope = current_scope();
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

    if(scope->klass == NULL)
        klass = addGlobalClassObject(className, modifiers, pAst);
    else
        klass = addChildClassObject(className, modifiers, pAst, scope->klass);


    add_scope(Scope(scope_class, klass));
     for(long i = 0; i < astBlock->getsubastcount(); i++) {
        pAst = astBlock->getsubast(i);

         switch(pAst->gettype()) {
             case ast_class_decl:
                 partial_parse_class_decl(pAst);
                 break;
             case ast_var_decl:
                 partial_parse_var_decl(pAst);
                 break;
             case ast_method_decl: /* Will be parsed later */
                 break;
             case ast_operator_decl: /* Will be parsed later */
                 break;
             case ast_construct_decl: /* Will be parsed later */
                 break;
             case ast_macros_decl: /* Will be parsed later */
                 break;
             default:
                 stringstream err;
                 err << ": unknown ast type: " << pAst->gettype();
                 errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
                 break;
         }
    }
    remove_scope();
}

void runtime::partial_parse_var_decl(ast *pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=0;


    if(parse_access_decl(pAst, modifiers, startpos)){
        parse_var_access_modifiers(modifiers, pAst);
    } else {
        modifiers.push_back(mPublic);
    }

    string name =  pAst->getentity(startpos).gettoken();
    modCompat.addAll(modifiers);
    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(!scope->klass->addField(Field(NULL, uid++, name, scope->klass, modCompat, note))) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "field `" + name + "` is already defined in the scope");
        printnote(scope->klass->getField(name)->note, "field `" + name + "` previously defined here");
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
            case '\\':
                assembler.push_i64(SET_Di(i64, MOVI, '\\'), ebx);
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

void runtime::parse_hexliteral(string hex_string, m64Assembler &assembler, ast *pAst) {
    int64_t i64;
    double var;
    hex_string = invalidate_underscores(hex_string);

    var = std::strtod (hex_string.c_str(), NULL);
    if(var > DA_MAX || var < DA_MIN) {
        stringstream ss;
        ss << "integral number too large: " + hex_string;
        errors->newerror(GENERIC, pAst->line, pAst->col, ss.str());
    }
    assembler.push_i64(SET_Di(i64, MOVI, var), ebx);
}

void runtime::parse_string_literal(string basic_string, m64Assembler &assembler) {
    int64_t i64;
    string new_string = "";
    for(unsigned int i = 0; i < basic_string.size(); i++) {
        if(basic_string.at(i) == '\\') {
            if((i+1) >= basic_string.size()) {
                break;
            } else {
                i++;
                switch(basic_string.at(i)) {
                    case 'n':
                        new_string +='\n';
                        break;
                    case 't':
                        new_string +='\t';
                        break;
                    case 'b':
                        new_string +='\b';
                        break;
                    case 'v':
                        new_string +='\v';
                        break;
                    case 'r':
                        new_string +='\r';
                        break;
                    case 'f':
                        new_string +='\f';
                        break;
                    case '\\':
                        new_string +='\\';
                        break;
                    default:
                        new_string +=basic_string.at(i);
                        break;
                }
            }
        } else {
            new_string += basic_string.at(i);
        }
    }


    assembler.push_i64(SET_Di(i64, MOVI, add_string(new_string)), ebx);
}

void runtime::parse_boolliteral(string bool_string, m64Assembler &assembler) {
    int64_t i64;
    assembler.push_i64(SET_Di(i64, MOVI, (bool_string == "true" ? 1 : 0)), ebx);
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
    int iter=0;
    for(unsigned int i = 0; i < num.size(); i++) {
        if(num.at(i) == '.') {
            for(unsigned int x = i+1; x < num.size(); x++) {
                if(iter++ > 16)
                    break;
                result += num.at(x);
            }
            return strtoll(result.c_str(), NULL, 0);
        }
    }
    return 0;
}

int64_t runtime::add_string(string basic_string) {
    if(!has_string(basic_string)) {
        string_map.push_back(basic_string);
        return string_map.size()-1;
    }
    return get_string(basic_string);
}

bool runtime::has_string(string basic_string) {
    for(unsigned int i = 0; i < string_map.size(); i++) {
        if(string_map.at(i) == basic_string)
            return true;
    }
    return false;
}

int64_t runtime::get_string(string basic_string) {
    for(unsigned int i = 0; i < string_map.size(); i++) {
        if(string_map.at(i) == basic_string)
            return i;
    }
    return -1;
}

void runtime::mov_field(Expression &expression, ast* pAst) {
    int64_t i64;
    if(expression.utype.type == ResolvedReference::FIELD) {
        expression.code.push_i64(SET_Di(i64, OBJECT_NXT, expression.utype.field->vaddr));
        expression.code.push_i64(SET_Di(i64, MOVI, 0), egx);
        expression.code.push_i64(SET_Ci(i64, MOVX, ebx,0, egx));
    } else {
        errors->newerror(GENERIC, pAst->line, pAst->col, "variable expected");
    }
}

void runtime::parse_class_cast(ResolvedReference &utype, Expression &expression, ast *pAst) {
    if(utype.type != ResolvedReference::NOTRESOLVED) {
        if(expression.type == expression_var) {
            errors->newerror(INVALID_CAST, pAst->getsubast(ast_expression)->line,
                             pAst->getsubast(ast_expression)->col, " '" + expression.utype.toString() + "' and '" + utype.toString() + "'");
        } else if(expression.type == expression_class) {
            if(utype.klass->match(expression.utype.klass)) {
                errors->newerror(REDUNDANT_CAST, pAst->getsubast(ast_expression)->line,
                                 pAst->getsubast(ast_expression)->col, " '" + expression.utype.toString() + "' and '" + utype.toString() + "'");
            } else if(utype.klass->getHeadClass()->curcular(expression.utype.klass)) {

            }

        }
    }
}

Scope *runtime::current_scope() {
    if(scope_map->size() == 0)
        return NULL;
    return &scope_map->get(scope_map->size()-1);
}

Scope* runtime::add_scope(Scope scope) {
    scope_map->push_back(scope);
    return current_scope();
}

void runtime::remove_scope() {
    Scope* scope = current_scope();
    scope->locals.free();

    scope_map->pop_back();
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

void Expression::utype_refrence_toexpression(ResolvedReference ref) {
    if(ref.type == ResolvedReference::RefrenceType::NOTRESOLVED) {
        this->type = expression_unknown;
    } else {
        switch(ref.type) {
//            case ResolvedReference::RefrenceType::FIELD:
//                this->type = ref.array ? expression_classarray : expression_class;
//                break;
//            case ResolvedReference::RefrenceType::CLASS:
//                this->type = ref.array ? expression_classarray : expression_class;
//                break;
//            case ResolvedReference::RefrenceType::METHOD:
//                break;
//            case ResolvedReference::RefrenceType::MACROS:
//                break;
//            case ResolvedReference::RefrenceType::OO:
//                break;
//            case ResolvedReference::RefrenceType::NATIVE:
//                if(ref.nf == fdynamic)
//                    this->type = ref.array ? expression_dynamicclass_array : expression_dynamicclass;
//                else
//                this->type = ref.array ? expression_vararray : expression_var;
//                break;
            default:
                break;
        }
    }
}

string ResolvedReference::toString() {
    if(type == FIELD || type == CLASS) {
        return refrenceName;
    } else if(type == NATIVE) {
        return runtime::nativefield_tostr(nf);
    }
    return "";
}
