//
// Created by BraxtonN on 1/30/2017.
//
#include <string.h>
#include <string>
#include <sstream>
#include <cstdio>
#include "runtime.h"
#include "../util/file.h"
#include "../runtime/interp/Opcode.h"
#include "../runtime/interp/register.h"
#include "Asm.h"
#include "../util/List2.h"
#include "../runtime/internal/Exe.h"
#include "../runtime/oo/Object.h"
#include "Optimizer.h"

using namespace std;

options c_options;
_operator string_toop(string op);

unsigned int runtime::classUID = 0;
runtime* runtime::Generator::instance = 0;

void runtime::interpret() {

    if(partial_parse()) {

        resolveAllFields();
        resolveAllMethods();

        if(c_options.magic) {
            List<string> lst;
            lst.addAll(modules);

            for(parser* p : parsers) {
                bool found = false;

                for(unsigned int i = 0; i < import_map.size(); i++) {
                    if(import_map.get(i).key == p->sourcefile) {
                        import_map.get(i).value.addAll(modules);
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    import_map.add(keypair<std::string, List<std::string>>(p->sourcefile, lst));
                }
            }

            lst.free();
        }

        long iter =0;   // TODO: parse new var[] { 1, 2, 3, 4 }; and " hi " + 8 (string concatanation)
        for(parser* p : parsers) {
            errors = new Errors(p->lines, p->sourcefile, true, c_options.aggressive_errors);
            _current = p;
            iter++;

            current_module = "";

            ast* trunk;
            add_scope(Scope(scope_global, NULL));
            for(size_t i = 0; i < p->treesize(); i++) {
                trunk = p->ast_at(i);

                switch(trunk->gettype()) {
                    case ast_module_decl:
                        current_module = parse_modulename(trunk);
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

            if(iter == parsers.size() && errors->error_count() == 0 && errors->uoerror_count() == 0) {
                string starter_classname = "Start";

                ClassObject* StarterClass = getClass("application", starter_classname);
                if(StarterClass != NULL) {
                    List<Param> params;
                    List<AccessModifier> modifiers;
                    RuntimeNote note = RuntimeNote(p->sourcefile, p->geterrors()->getline(1), 1, 0);
                    params.add(Field(fvar, 0, "args", StarterClass, modifiers, note));
                    params.get(0).field.array = true;
                    params.get(0).field.type = field_native;

                    Method* main = StarterClass->getFunction("__init" , params);

                    if(main == NULL) {
                        errors->newerror(GENERIC, 1, 0, "could not locate main method '__init(var[])' in starter class");
                    } else
                        this->main = main;
                } else {
                    errors->newerror(GENERIC, 1, 0, "Could not find starter class '" + starter_classname + "' for application entry point.");
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
            errors->print_errors();
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

        setHeadClass(klass);
    }
    else {
        klass = scope->klass->getChildClass(className);

        klass->setSuperClass(scope->klass);
        setHeadClass(klass);
    }

    add_scope(Scope(scope_class, klass));
    for(long i = 0; i < trunk->getsubastcount(); i++) {
        pAst = trunk->getsubast(i);

        switch(pAst->gettype()) {
            case ast_class_decl:
                parse_class_decl(pAst);
                break;
            case ast_var_decl:
                parse_var_decl(pAst);
                break;
            case ast_method_decl:
                parseMethodDecl(pAst);
                break;
            case ast_operator_decl:
                break;
            case ast_construct_decl:
                parseConstructorDecl(pAst);
                break;
            case ast_macros_decl:
                break;
            default: {
                stringstream err;
                err << ": unknown ast type: " << pAst->gettype();
                errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
                break;
            }
        }
    }

    remove_scope();
}

void runtime::parseReturnStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    Expression returnVal, value = parse_value(pAst->getsubast(ast_value));

    returnVal.type = methodReturntypeToExpressionType(scope->function);
    if(returnVal.type == expression_lclass) {
        returnVal.utype.klass = scope->function->klass;
        returnVal.utype.type = ResolvedReference::CLASS;
    }
    returnVal.utype.array = scope->function->array;
    equals(returnVal, value, ": Returning `" + value.typeToString() + "` from a function returning `" + returnVal.typeToString() + "`");
}

void runtime::parseIfStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    Expression cond = parseExpression(pAst->getsubast(ast_expression)), out;

    string ifEndLabel;
    stringstream ss;
    ss << for_label_end_id << scope->ulid;
    ifEndLabel=ss.str();

    pushExpressionToRegister(cond, out, cmt);
    block.code.inject(block.code.size(), out.code);


    if(pAst->getsubastcount() > 2) {
        int64_t insertAddr, difference;
        block.code.push_i64(SET_Di(i64, op_LOADX, adx));
        insertAddr=block.code.size();
        parseBlock(pAst->getsubast(ast_block), block);
        difference = block.code.size()-insertAddr;

        block.code.__asm64.insert(insertAddr++, SET_Ci(i64, op_IADD, adx,0, (difference+4)));
        block.code.__asm64.insert(insertAddr, SET_Ei(i64, op_IFNE));

        scope->addBranch(ifEndLabel, 1, block.code, pAst->getsubast(ast_expression)->line,
                         pAst->getsubast(ast_expression)->col);

        ast* trunk;
        for(unsigned int i = 2; i < pAst->getsubastcount(); i++) {
            trunk = pAst->getsubast(i);
            switch(trunk->gettype()) {
                case ast_elseif_statement:
                    cond = parseExpression(trunk->getsubast(ast_expression));

                    out.free();
                    pushExpressionToRegister(cond, out, cmt);
                    block.code.inject(block.code.size(), out.code);

                    block.code.push_i64(SET_Di(i64, op_LOADX, adx));
                    insertAddr=block.code.size();
                    parseBlock(trunk->getsubast(ast_block), block);
                    difference = block.code.size()-insertAddr;

                    block.code.__asm64.insert(insertAddr++, SET_Ci(i64, op_IADD, adx,0, (difference+4)));
                    block.code.__asm64.insert(insertAddr, SET_Ei(i64, op_IFNE));

                    scope->addBranch(ifEndLabel, 1, block.code, trunk->getsubast(ast_expression)->line,
                                     trunk->getsubast(ast_expression)->col);
                    break;
                case ast_else_statement:
                    parseBlock(trunk->getsubast(ast_block), block);
                    break;
            }
        }
    } else {
        scope->addStore(ifEndLabel, adx, 1, block.code, pAst->getsubast(ast_expression)->line,
                        pAst->getsubast(ast_expression)->col);
        block.code.push_i64(SET_Ei(i64, op_IFNE));
        parseBlock(pAst->getsubast(ast_block), block);
    }


    scope->label_map.add(keypair<string,int64_t>(ifEndLabel, __init_label_address(block.code)));
}

void runtime::parseAssemblyBlock(Block& block, ast* pAst) {
    string assembly = "";

    if(pAst->getentitycount() == 1) {
        if(file::exists(pAst->getentity(0).gettoken().c_str())) {
            file::buffer __ostream;
            file::read_alltext(pAst->getentity(0).gettoken().c_str(), __ostream);

            assembly = __ostream.to_str();
            __ostream.end();
        } else {
            assembly = pAst->getentity(0).gettoken();
        }
    } else {
        for(unsigned int i = 0; i < pAst->getentitycount(); i++) {
            assembly += pAst->getentity(i).gettoken() + "\n";
        }
    }

    Asm __vasm;
    __vasm.parse(block.code, this, assembly, pAst);
}

void runtime::parseAssemblyStatement(Block& block, ast* pAst) {
    if(c_options.unsafe)
        parseAssemblyBlock(block, pAst->getsubast(ast_assembly_block));
    else
        errors->newerror(GENERIC, pAst, "calling __asm without unsafe mode enabled. try recompiling your code with [-unsafe]");
}

bool runtime::validateLocalField(std::string name, ast* pAst) {
    Scope* scope = current_scope();
    keypair<int, Field>* field;

    if((field = scope->getLocalField(name)) != NULL) {
        if(scope->blocks == field->key) {
            // err redefinition of parameter
            errors->newerror(DUPlICATE_DECLIRATION, pAst, " local variable `" + field->value.name + "`");
            return false;
        } else {
            warning(GENERIC, pAst->line, pAst->col, " local variable `" + field->value.name + "` hides previous declaration in higher scope");
            return true;
        }
    } else {
        return true;
    }
}

Field runtime::utypeArgToField(keypair<string, ResolvedReference> arg) {
    Field field;
    field.name = arg.key;
    field.array = arg.value.array;
    field.fullName = field.name;
    field.klass = arg.value.klass;
    field.modifiers.add(mPublic);
    field.nf = arg.value.nf;
    if(arg.value.type == ResolvedReference::CLASS) {
        field.type = field_class;
    } else if(arg.value.type == ResolvedReference::NATIVE) {
        field.type = field_native;
    } else {
        field.type = field_unresolved;
    }

    return field;
}

void runtime::removeForLabels(Scope* scope) {
    readjust:
    for(long long i = 0; i < scope->label_map.size(); i++) {
        if(scope->label_map.get(i).key == for_label_begin_id) {
            scope->label_map.remove(i);
            goto readjust;
        }
    }
}

void runtime::parseForStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    Expression cond, iter;
    scope->blocks++;
    scope->loops++;
    scope->ulid++;
    stringstream ss;
    string forEndLabel, forBeginLabel;

    parseUtypeArg(pAst, scope, block);

    ss.str("");
    ss << for_label_begin_id << scope->ulid;
    forBeginLabel=ss.str();
    scope->label_map.add(keypair<std::string, int64_t>(forBeginLabel,__init_label_address(block.code)));

    ss.str("");
    ss << for_label_end_id << scope->ulid;
    forEndLabel=ss.str();

    if(pAst->hassubast(ast_for_expresion_cond)) {
        Expression out;
        cond = parseExpression(pAst->getsubast(ast_for_expresion_cond));

        pushExpressionToRegister(cond, out, ebx);
        block.code.inject(block.code.size(), out.code);

        scope->addStore(forEndLabel, adx, 1, block.code, pAst->line, pAst->col);
        block.code.push_i64(SET_Ei(i64, op_IFNE));
    }

    parseBlock(pAst->getsubast(ast_block), block);

    if(pAst->hassubast(ast_for_expresion_iter)) {
        iter = parseExpression(pAst->getsubast(ast_for_expresion_iter));
        block.code.inject(block.code.size(), iter.code);
    }

    block.code.push_i64(SET_Di(i64, op_GOTO, (get_label(forBeginLabel)+1)));
    scope->label_map.add(keypair<std::string, int64_t>(forEndLabel,__init_label_address(block.code)));
    scope->remove_locals(scope->blocks);
    scope->blocks--;
    scope->loops--;
}

void runtime::assignVariable(Field &field, Expression &assignExpr,
                             token_entity assignOper, Expression &outExpr) {
    int64_t i64;
    if(field.nativeInt()) {
        outExpr.code.inject(outExpr.code.__asm64.size(), assignExpr.code); // integer value will be set in ebx

        Generator::setupVariable(outExpr.code, field.vaddr); // get refrence to variable

        outExpr.code.push_i64(SET_Di(i64, op_MOVI, 0), ecx);
        outExpr.code.push_i64(SET_Ci(i64, op_RMOV, ecx, 0, ebx));
    } else if(field.dynamicObject()) {

    } else if(field.klass != NULL) {

    }
}

void runtime::initVariable(Field& field, Expression& outExpr) {
    Generator::setupVariable(outExpr.code, field.vaddr); // get refrence to variable
    int64_t i64;

    if(field.nativeInt()) {
        outExpr.code.push_i64(SET_Di(i64, op_MOVI, 1), ecx);
        outExpr.code.__asm64.add(SET_Di(i64, op_NEWi, ecx));
    } else if(field.dynamicObject()) {
        /* do nothing */
    } else if(field.klass != NULL) {
        outExpr.code.__asm64.add(SET_Di(i64, op_NEW_CLASS, field.klass));
    }
}

void runtime::parseUtypeArg(ast *pAst, Scope *scope, Block &block, Expression* comparator) {
    if(pAst->hassubast(ast_utype_arg)) {
        keypair<string, ResolvedReference> utypeArg = parseUtypeArg(pAst->getsubast(ast_utype_arg));
        Expression value, out;

        if(pAst->hassubast(ast_value)) {
            if(comparator != NULL)
                errors->newerror(UNEXPECTED_SYMBOL, pAst->getsubast(ast_value), " `;` expected");
            value = parse_value(pAst->getsubast(ast_value));
        }

        if(validateLocalField(utypeArg.key, pAst->getsubast(ast_utype_arg))) {
            if(utypeArg.value.type == ResolvedReference::FIELD) {
                errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_utype_arg), " `" + utypeArg.value.field->name + "`");
            }

            keypair<int, Field> local;
            local.set(scope->blocks, utypeArgToField(utypeArg));
            local.value.vaddr = scope->function->local_count++;;
            local.value.local=true;
            scope->locals.push_back(local);

            Expression fieldExpr = fieldToExpression(pAst, local.value);

            if(value.type != expression_unknown) {
                equals(fieldExpr, value);

                token_entity operand("=", SINGLE, 0,0, ASSIGN);
                assignValue(operand, out, fieldExpr, value, pAst);
                block.code.inject(block.code.size(), out.code);
            }
        }
    }
}

void runtime::assignUtypeForeach(ast *pAst, Scope *scope, Block &block, Expression& assignExpr) {
    if(pAst->hassubast(ast_utype_arg)) {
        keypair<string, ResolvedReference> utypeArg = parseUtypeArg(pAst->getsubast(ast_utype_arg));
        Expression out;

        keypair<int, Field>* local = scope->getLocalField(utypeArg.key);
        Expression fieldExpr = fieldToExpression(pAst, local->value);

        token_entity operand("=", SINGLE, 0,0, ASSIGN);
        assignValue(operand, out, fieldExpr, assignExpr, pAst);
        block.code.inject(block.code.size(), out.code);
    }
}

Expression runtime::fieldToExpression(ast *pAst, string name) {
    Scope* scope=current_scope();
    Expression fieldExpr(pAst);
    keypair<int, Field>* field;

    if((field =scope->getLocalField(name)) == NULL)
        return fieldExpr;

    fieldExpr.type = expression_field;
    fieldExpr.utype.field = &field->value;
    fieldExpr.utype.type = ResolvedReference::FIELD;
    fieldExpr.utype.refrenceName = field->value.name;
    return fieldExpr;
}

Expression runtime::fieldToExpression(ast *pAst, Field& field) {
    Scope* scope=current_scope();
    Expression fieldExpr(pAst);

    fieldExpr.type = expression_field;
    fieldExpr.utype.field = &field;
    fieldExpr.utype.type = ResolvedReference::FIELD;
    fieldExpr.utype.refrenceName = field.name;

    if(field.isObjectInMemory()) {
        fieldExpr.code.push_i64(SET_Di(i64, op_MOVL, field_offset(scope, field.vaddr)));
    } else {
        fieldExpr.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
        fieldExpr.code.push_i64(SET_Ci(i64, op_SMOV, ebx, 0, field_offset(scope, field.vaddr)));
    }
    return fieldExpr;
}

void runtime::getArrayValueOfExpression(Expression& expr, Expression& out) {
    switch(expr.type) {
        case expression_var:
            out.type=expression_var;
            out.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));
            break;
        case expression_lclass:
            out.type=expression_lclass;
            out.utype.klass = expr.utype.klass;
            out.code.push_i64(SET_Di(i64, op_MOVND, ebx));
            break;
        case expression_field:
            if(expr.utype.field->nativeInt()) {
                out.type=expression_var;
                out.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));
            }
            else {
                out.type=expression_lclass;
                out.utype.klass = expr.utype.field->klass;
                out.code.push_i64(SET_Di(i64, op_MOVND, ebx));
            }
            break;
        default:
            out=expr;
            out.code.push_i64(SET_Di(i64, op_MOVND, ebx));
            break;
    }
}

void runtime::parseForEachStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    scope->blocks++;
    scope->loops++;
    scope->ulid++;
    string forBeginLabel, forEndLabel;

    Expression arryExpression(parseExpression(pAst->getsubast(ast_expression))), out;
    parseUtypeArg(pAst, scope, block, &arryExpression);

    /*
     * This is stupid but we do this so we dont mess up the refrence with out local array expression variable
     */
    arryExpression = parseExpression(pAst->getsubast(ast_expression));

    block.code.push_i64(SET_Di(i64, op_MOVI, 0), ebx);
    block.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

    if(!arryExpression.arrayObject()) {
        errors->newerror(GENERIC, pAst->getsubast(ast_expression), "expression must evaluate to type array");
    }

    stringstream ss;
    ss << for_label_begin_id << scope->ulid;
    forBeginLabel=ss.str();

    ss.str("");
    ss << for_label_end_id << scope->ulid;
    forEndLabel=ss.str();

    scope->label_map.add(keypair<std::string, int64_t>(forBeginLabel,__init_label_address(block.code)));

    block.code.inject(block.code.size(), arryExpression.code);

    block.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
    block.code.push_i64(SET_Ci(i64, op_SMOV, ebx,0, 0));
    block.code.push_i64(SET_Di(i64, op_SIZEOF, egx));
    block.code.push_i64(SET_Ci(i64, op_LT, ebx,0, egx));
    scope->addStore(forEndLabel, adx, 1, block.code,
                    pAst->getsubast(ast_block)->line, pAst->getsubast(ast_block)->col);
    block.code.push_i64(SET_Ei(i64, op_IFNE));
    getArrayValueOfExpression(arryExpression, out);
    assignUtypeForeach(pAst, scope, block, out);

    parseBlock(pAst->getsubast(ast_block), block);

    block.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
    block.code.push_i64(SET_Ci(i64, op_SMOV, ebx,0, 0));
    block.code.push_i64(SET_Di(i64, op_INC, ebx));
    block.code.push_i64(SET_Ci(i64, op_SMOVR, ebx,0, 0));
    block.code.push_i64(SET_Di(i64, op_GOTO, (get_label(forBeginLabel)+1)));
    scope->label_map.add(keypair<std::string, int64_t>(forEndLabel,__init_label_address(block.code)));
    block.code.push_i64(SET_Ei(i64, op_POP));

    scope->remove_locals(scope->blocks);
    scope->loops--;
    scope->blocks--;
}

void runtime::parseWhileStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    string whileBeginLabel, whileEndLabel;

    Expression cond = parseExpression(pAst->getsubast(ast_expression)), out;

    stringstream ss;
    ss << generic_label_id << ++scope->ulid;
    whileBeginLabel=ss.str();

    ss.str("");
    ss << generic_label_id << ++scope->ulid;
    whileEndLabel=ss.str();

    scope->label_map.add(keypair<std::string, int64_t>(whileBeginLabel,__init_label_address(block.code)));
    pushExpressionToRegister(cond, out, cmt);
    block.code.inject(block.code.size(), out.code);

    scope->addStore(whileEndLabel, adx, 1, block.code,
                    pAst->getsubast(ast_expression)->line, pAst->getsubast(ast_expression)->col);
    block.code.push_i64(SET_Ei(i64, op_IFNE));

    parseBlock(pAst->getsubast(ast_block), block);

    block.code.push_i64(SET_Di(i64, op_GOTO, (get_label(whileBeginLabel)+1)));
    scope->label_map.add(keypair<std::string, int64_t>(whileEndLabel,__init_label_address(block.code)));
}

void runtime::parseDoWhileStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    string whileBeginLabel;

    stringstream ss;
    ss << generic_label_id << ++scope->ulid;
    whileBeginLabel=ss.str();
    scope->label_map.add(keypair<std::string, int64_t>(whileBeginLabel,__init_label_address(block.code)));

    parseBlock(pAst->getsubast(ast_block), block);

    Expression cond = parseExpression(pAst->getsubast(ast_expression)), out;
    pushExpressionToRegister(cond, out, cmt);
    block.code.inject(block.code.size(), out.code);


    scope->addStore(whileBeginLabel, adx, 1, block.code,
                    pAst->getsubast(ast_expression)->line, pAst->getsubast(ast_expression)->col);
    block.code.push_i64(SET_Ei(i64, op_IFE));
}

ClassObject* runtime::parseCatchClause(Block &block, ast *pAst, ExceptionTable et) {
    Scope* scope = current_scope();
    ClassObject* klass = NULL;

    keypair<string, ResolvedReference> catcher = parseUtypeArg(pAst->getsubast(ast_utype_arg_opt));

    string name =  catcher.key;
    keypair<int, Field>* field;
    List<AccessModifier> modCompat;
    modCompat.add(mPublic);

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    Field f = Field(NULL, uid++, name, scope->klass, modCompat, note);

    f.vaddr = scope->function->local_count;
    f.local=true;
    scope->function->local_count++;
    if(catcher.value.type == ResolvedReference::CLASS) {
        f.klass = catcher.value.klass;
        f.type = field_class;
    } else if(catcher.value.type == ResolvedReference::NATIVE) {
        errors->newerror(GENERIC, pAst, " field `" + catcher.value.field->name + "` is not a class");
        f.nf = catcher.value.nf;
        f.type = field_native;
    } else {
        f.type = field_unresolved;
    }

    f.array = catcher.value.array;

    if(validateLocalField(name, pAst)) {
        if(catcher.value.type == ResolvedReference::FIELD) {
            errors->newerror(COULD_NOT_RESOLVE, pAst, " `" + catcher.value.field->name + "`");
        }

        scope->locals.add(keypair<int, Field>(scope->blocks, f));
        field = scope->getLocalField(name);
        et.local = f.vaddr;
        et.klass = f.klass == NULL ? "" : f.klass->getFullName();
        klass=f.klass;
        et.handler_pc = __init_label_address(block.code)+1;
        scope->function->exceptions.push_back(et);
    }

    // TODO: add goto to finally block
    parseBlock(pAst->getsubast(ast_block), block);
    return klass;
}

void runtime::parseFinallyBlock(Block& block, ast* pAst) {
    parseBlock(pAst->getsubast(ast_block), block);
}

void runtime::parseTryCatchStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    ExceptionTable et;
    scope->trys++;
    string catchEndLabel;
    List<ClassObject*> klasses;
    ClassObject* klass;

    et.start_pc = block.code.__asm64.size();
    parseBlock(pAst->getsubast(ast_block), block);
    et.end_pc = block.code.__asm64.size();


    stringstream ss;
    ss << try_label_end_id << ++scope->ulid;
    catchEndLabel = ss.str();

    ast* sub;
    for(unsigned int i = 1; i < pAst->getsubastcount(); i++) {
        sub = pAst->getsubast(i);

        switch(sub->gettype()) {
            case ast_catch_clause:
                scope->blocks++;
                klass = parseCatchClause(block, sub, et);
                scope->addBranch(catchEndLabel, 1, block.code, sub->line, sub->col);

                if(klass != NULL) {
                    if(klasses.find(klass)) {
                        errors->newerror(GENERIC, sub, "exception `" + klass->getName() + "` has already been caught");
                    } else
                        klasses.add(klass);
                }

                scope->remove_locals(scope->blocks);
                scope->blocks--;
                break;
            case ast_finally_block:
                break;
        }
    }

    klasses.free();
    scope->label_map.add(keypair<string,int64_t>(catchEndLabel, __init_label_address(block.code)));

    if(pAst->hassubast(ast_finally_block)) {
        parseFinallyBlock(block, pAst->getsubast(ast_finally_block));
    }
    scope->trys--;
}

void runtime::parseThrowStatement(Block& block, ast* pAst) {
    Expression clause = parseExpression(pAst->getsubast(ast_expression)), out;

    if(clause.type == expression_lclass) {
        ClassObject* throwable = getClass("std.err", "Throwable");

        if(throwable != NULL) {
            if(clause.utype.klass->hasBaseClass(throwable)) {
                pushExpressionToStack(clause, out);

                out.code.push_i64(SET_Ei(i64, op_THROW));
                block.code.inject(block.code.size(), out.code);
            } else {
                errors->newerror(GENERIC, pAst->getsubast(ast_expression), "class `" + clause.utype.klass->getFullName() +
                    "` does not inherit `std.err#Throwable`");
            }
        } else {
            errors->newerror(GENERIC, pAst->getsubast(ast_expression), "missing core class `std.err#Throwable` for exception handling");
        }
    } else if(clause.type == expression_field) {
        if(clause.utype.field->type == field_class) {
            pushExpressionToStack(clause, out);

            out.code.push_i64(SET_Ei(i64, op_THROW));
            block.code.inject(block.code.size(), out.code);
        } else {
            errors->newerror(GENERIC, pAst->getsubast(ast_expression), "field `" + clause.utype.field->name +
                                                                       "` is not a class");
        }
    } else
     {
        errors->newerror(GENERIC, pAst->getsubast(ast_expression), "expression must be of type lclass");
     }
}

int64_t runtime::getLastLoopBeginAddress() {
    Scope* scope = current_scope();

    for(long long i = scope->label_map.size()-1; i < 0; i++) {
        if(scope->label_map.get(i).key == for_label_begin_id) {
            return scope->label_map.get(i).value;
        }
    }

    return -1;
}

void runtime::parseContinueStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();

    if(scope->loops > 0) {
        stringstream name;
        name << for_label_begin_id << scope->loops;
        scope->addBranch(name.str(), 0, block.code, pAst->line, pAst->col);
    } else {
        // error not in loop
        errors->newerror(GENERIC, pAst, "continue statement outside of loop");
    }
}

bool runtime::label_exists(string label) {
    for(unsigned int i = 0; i < current_scope()->label_map.size(); i++) {
        if(current_scope()->label_map.get(i).key == label)
            return true;
    }

    return false;
}

int64_t runtime::get_label(string label) {
    for(unsigned int i = 0; i < current_scope()->label_map.size(); i++) {
        if(current_scope()->label_map.get(i).key == label)
            return current_scope()->label_map.get(i).value;
    }

    return 0;
}

void runtime::parseBreakStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();

    if(scope->loops > 0) {
        stringstream name;
        name << for_label_end_id << scope->loops;
        scope->addBranch(name.str(), 0, block.code, pAst->line, pAst->col);
    } else {
        // error not in loop
        errors->newerror(GENERIC, pAst, "break statement outside of loop");
    }
}

void runtime::parseGotoStatement(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    string label = pAst->getentity(1).gettoken();

    scope->addBranch(label, 0, block.code, pAst->line, pAst->col);
}

void runtime::createLabel(string name, m64Assembler& code, int line, int col) {
    Scope* scope = current_scope();

    if(label_exists(name)) {
        errors->newerror(GENERIC, line, col, "redefinition of label `" + name + "`");
    } else {
        scope->label_map.add(keypair<string, int64_t>(name, __init_label_address(code)));
    }
}

void runtime::parseLabelDecl(Block& block,ast* pAst) {
    string label = pAst->getentity(0).gettoken();

    createLabel(label, block.code, pAst->getentity(0).getline(), pAst->getentity(0).getcolumn());
    parseStatement(block, pAst->getsubast(ast_statement)->getsubast(0));
}

/*
 * if(scope->getLocalField(name) != NULL) {
        // chech scope
        field = scope->getLocalField(name);
        if(field->key == scope->blocks) {
            errors->newerror(GENERIC, pAst->getentity(startpos), " duplicate declaration of local variable `" + field->value.name + "`");
            return;
        } else {
            warning(GENERIC, pAst->line, pAst->col, " local variable `" + field->value.name + "` hides previous declaration in higher scope");
        }
    } else {
        RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                       pAst->line, pAst->col);

        modCompat.addAll(modifiers);
        scope->locals.add(keypair<int, Field>(scope->blocks, Field(NULL, uid++, name, scope->klass, modCompat, note)));
        field = scope->getLocalField(name);
    }

    if(pAst->hassubast(ast_value)) {
        Expression expression = parse_value(pAst->getsubast(ast_value));

        Expression assignee(pAst);
        assignee.type = expression_field;
        assignee.utype.field = &field->value;
        assignee.utype.type = ResolvedReference::FIELD;
        assignee.utype.refrenceName = field->value.name;

        if(equals(assignee, expression)) {
            return;
        }
        // TODO: do something based on the assign expression
    }
 */
void runtime::parseVarDecl(Block& block, ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    List<AccessModifier> modCompat;
    int startpos=0;
    int64_t i64;

    parse_access_decl(pAst, modifiers, startpos);

    string name =  pAst->getentity(startpos).gettoken();
    keypair<int, Field>* field;

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    Field f = Field(NULL, uid++, name, scope->klass, modCompat, note);

    f.vaddr = scope->function->local_count;
    f.local=true;
    scope->function->local_count++;
    Expression utype = parseUtype(pAst->getsubast(ast_utype));
    if(utype.utype.type == ResolvedReference::CLASS) {
        f.klass = utype.utype.klass;
        f.type = field_class;
    } else if(utype.utype.type == ResolvedReference::NATIVE) {
        f.nf = utype.utype.nf;
        f.type = field_native;
    } else {
        f.type = field_unresolved;
    }

    f.array = utype.utype.array;

    if(validateLocalField(name, pAst)) {
        if(utype.utype.type == ResolvedReference::FIELD) {
            errors->newerror(COULD_NOT_RESOLVE, pAst, " `" + utype.utype.field->name + "`");
        }


        modCompat.addAll(modifiers);
        scope->locals.add(keypair<int, Field>(scope->blocks, f));
        field = scope->getLocalField(name);

        if(!(f.nativeInt() && !f.array))
            block.code.__asm64.push_back(SET_Di(i64, op_MOVL, f.vaddr));

        if(pAst->hassubast(ast_value)) {
            Expression expression = parse_value(pAst->getsubast(ast_value));

            if(f.type == field_native) {
                if(f.nativeInt()) {
                    switch(expression.type) {
                        case expression_string:
                            block.code.__asm64.push_back(SET_Di(i64, op_NEWSTR, get_string(expression.value)));
                            break;
                    }
                }
            }

            Expression assignee(pAst);
            assignee.type = expression_field;
            assignee.utype.field = &field->value;
            assignee.utype.type = ResolvedReference::FIELD;
            assignee.utype.refrenceName = field->value.name;

            if(equals(assignee, expression)) {
                return;
            }
            // TODO: do something based on the assign expression
        } else {
            // create variable
        }
    }

}

void runtime::addLine(Block& block, ast *pAst) {
    Scope* scope = current_scope();

    for(unsigned int i = 0; i < scope->function->line_table.size(); i++) {
        if(scope->function->line_table.get(i).value == pAst->line) {
            return;
        }
    }

    scope->function->line_table.add(keypair<int64_t, long>(block.code.__asm64.size(), pAst->line));
}

void runtime::parseStatement(Block& block, ast* pAst) {
    addLine(block, pAst);

    switch(pAst->gettype()) {
        case ast_return_stmnt:
            parseReturnStatement(block, pAst);
            break;
        case ast_if_statement:
            parseIfStatement(block, pAst);
            break;
        case ast_expression: {
            Expression expr =parseExpression(pAst);
            if(expr.func && expr.type != expression_void) {
                expr.code.push_i64(SET_Ei(i64, op_POP));
            }

            block.code.inject(block.code.size(), expr.code); // done
        }
            break;
        case ast_assembly_statement:
            parseAssemblyStatement(block, pAst); // done
            break;
        case ast_for_statement:
            parseForStatement(block, pAst); // done
            break;
        case ast_foreach_statement:
            parseForEachStatement(block, pAst); // done
            break;
        case ast_while_statement:
            parseWhileStatement(block, pAst); // done
            break;
        case ast_do_while_statement:
            parseDoWhileStatement(block, pAst); // done
            break;
        case ast_trycatch_statement:
            parseTryCatchStatement(block, pAst);
            break;
        case ast_throw_statement:
            parseThrowStatement(block, pAst); // done
            break;
        case ast_continue_statement:
            parseContinueStatement(block, pAst); // done
            break;
        case ast_break_statement:
            parseBreakStatement(block, pAst); // done
            break;
        case ast_goto_statement:
            parseGotoStatement(block, pAst); // done
            break;
        case ast_label_decl:
            parseLabelDecl(block, pAst); // done
            break;
        case ast_var_decl:
            parseVarDecl(block, pAst);
            break;
        default: {
            stringstream err;
            err << ": unknown ast type: " << pAst->gettype();
            errors->newerror(INTERNAL_ERROR, pAst->line, pAst->col, err.str());
            break;
        }
    }
}

void runtime::parseBlock(ast* pAst, Block& block) {
    Scope* scope = current_scope();
    scope->blocks++;

    ast* trunk;
    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        trunk = pAst->getsubast(i);

        if(trunk->gettype() == ast_block) {
            parseBlock(trunk, block);
            continue;
        } else
            trunk = trunk->getsubast(0);

        parseStatement(block, trunk);
    }

    scope->remove_locals(scope->blocks);

    scope->blocks--;
}

void runtime::parseConstructorDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);

    List<Param> params;
    parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));

    Method* method = scope->klass->getConstructor(params);

    if(method != NULL) {
        add_scope(Scope(scope_instance_block, scope->klass, method));

        keypair<int, Field> local;
        Scope* curr = current_scope();
        for(unsigned int i = 0; i < params.size(); i++) {

            params.get(i).field.vaddr=i;
            params.get(i).field.local=true;

            local.set(curr->blocks, params.get(i).field);
            curr->locals.add(local);
        }

        Block fblock;
        parseBlock(pAst->getsubast(ast_block), fblock);

        resolveAllBranches(fblock);
        method->code.__asm64.addAll(fblock.code.__asm64);
        remove_scope();
    }
}

void runtime::resolveAllBranches(Block& block) {
    Scope *scope = current_scope();

    int64_t address, i64;
    BranchTable* bt;
    for(unsigned int i = 0; i < scope->branches.size(); i++)
    {
        bt = &scope->branches.get(i);

        if((address = scope->getLabel(bt->label.str())) != -1) {

            if(bt->store) {
                scope->function->unique_address_table.add(bt->branch_pc); // add indirect address store for optimizer

                block.code.__asm64.replace(bt->branch_pc, SET_Di(i64, op_MOVI, (bt->__offset+address)));
                block.code.__asm64.replace(bt->branch_pc+1, bt->_register);
            } else {
                block.code.__asm64.replace(bt->branch_pc, SET_Di(i64, op_GOTO, (bt->__offset+address)));
            }
        } else
            errors->newerror(COULD_NOT_RESOLVE, bt->line, bt->col, " `" + bt->label.str() + "`");
    }


    if(block.code.size() == 0 || GET_OP(block.code.__asm64.get(block.code.size() -1)) != op_RET) {
        block.code.push_i64(SET_Ei(i64, op_RET));
    }
    __freeList(scope->branches);
}

void runtime::parseMethodDecl(ast* pAst) {
    Scope* scope = current_scope();
    list<AccessModifier> modifiers;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);

    List<Param> params;
    string name =  pAst->getentity(startpos).gettoken();
    parseMethodParams(params, parseUtypeArgList(pAst->getsubast(ast_utype_arg_list)), pAst->getsubast(ast_utype_arg_list));

    Method* method = scope->klass->getFunction(name, params);

    if(method != NULL) {

        if(method->isStatic()) {
            add_scope(Scope(scope_static_block, scope->klass, method));
        } else
            add_scope(Scope(scope_instance_block, scope->klass, method));

        keypair<int, Field> local;
        Scope* curr = current_scope();
        for(unsigned int i = 0; i < params.size(); i++) {

            params.get(i).field.vaddr=i;
            params.get(i).field.local=true;
            local.set(curr->blocks, params.get(i).field);
            curr->locals.add(local);
        }

        Block fblock;
        parseBlock(pAst->getsubast(ast_block), fblock);

        resolveAllBranches(fblock);
        method->code.__asm64.addAll(fblock.code.__asm64);
        remove_scope();
    }
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

        Expression assignee(pAst);
        assignee.type = expression_field;
        assignee.utype.field = field;
        assignee.utype.type = ResolvedReference::FIELD;
        assignee.utype.refrenceName = field->name;

        if(equals(assignee, expression)) {
            return;
        }
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
        if(!modules.find(import)) {
            errors->newerror(COULD_NOT_RESOLVE, pAst->line, pAst->col,
                             " `" + import + "` ");
        }
    }
}

Expression runtime::parse_value(ast *pAst) {
    return parseExpression(pAst->getsubast(ast_expression));
}

void runtime::parseCharLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;

    int64_t  i64;
    if(token.gettoken().size() > 1) {
        switch(token.gettoken().at(1)) {
            case 'n':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\n'), ebx);
                expression.value = '\n';
                break;
            case 't':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\t'), ebx);
                expression.intValue = '\t';
                break;
            case 'b':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\b'), ebx);
                expression.intValue = '\b';
                break;
            case 'v':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\v'), ebx);
                expression.intValue = '\v';
                break;
            case 'r':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\r'), ebx);
                expression.intValue = '\r';
                break;
            case 'f':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\f'), ebx);
                expression.intValue = '\f';
                break;
            case '\\':
                expression.code.push_i64(SET_Di(i64, op_MOVI, '\\'), ebx);
                expression.intValue = '\\';
                break;
            default:
                expression.code.push_i64(SET_Di(i64, op_MOVI, token.gettoken().at(1)), ebx);
                expression.intValue = token.gettoken().at(1);
                break;
        }
    } else {
        expression.code.push_i64(SET_Di(i64, op_MOVI, token.gettoken().at(0)), ebx);
        expression.intValue =token.gettoken().at(0);
    }

    expression.literal = true;
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
        expression.code.push_i64(SET_Di(i64, op_MOVI, var), ebx);
    }else {
        var = std::strtod (int_string.c_str(), NULL);
        if((int64_t )var > DA_MAX || (int64_t )var < DA_MIN) {
            stringstream ss;
            ss << "integral number too large: " + int_string;
            errors->newerror(GENERIC, token.getline(), token.getcolumn(), ss.str());
        }

        expression.code.push_i64(SET_Di(i64, op_MOVBI, ((int64_t)var)), abs(get_low_bytes(var)));
    }

    expression.intValue = var;
    expression.literal = true;
}

void runtime::parseHexLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;

    int64_t i64;
    double var;
    string hex_string = invalidate_underscores(token.gettoken());

    var = strtoll(hex_string.c_str(), NULL, 16);
    if(var > DA_MAX || var < DA_MIN) {
        stringstream ss;
        ss << "integral number too large: " + hex_string;
        errors->newerror(GENERIC, token.getline(), token.getcolumn(), ss.str());
    }
    expression.code.push_i64(SET_Di(i64, op_MOVI, var), ebx);

    expression.intValue = var;
    expression.literal = true;
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

    expression.value = parsed_string;
    string_map.addif(parsed_string);
    expression.intValue = string_map.indexof(parsed_string);
}

void runtime::parseBoolLiteral(token_entity token, Expression& expression) {
    expression.type = expression_var;
    expression.code.push_i64(SET_Di(i64, op_MOVI, (token.gettoken() == "true" ? 1 : 0)), ebx);

    expression.intValue = (token.gettoken() == "true" ? 1 : 0);
    expression.literal = true;
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
                    pAst->getentity(0).gettoken() == "false") {
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
        for(unsigned int i = 0; i < import_map.size(); i++) {
            if(import_map.get(i).key == _current->sourcefile) {

                List<string>& lst = import_map.get(i).value;
                for(unsigned int x = 0; x < lst.size(); x++) {
                    if((klass = getClass(lst.get(x), class_name)) != NULL)
                        return klass;
                }
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
            if((field = klass->getField(object_name, true)) != NULL) {
                // is static?
                if(!lastRefrence && field->array) {
                    errors->newerror(INVALID_ACCESS, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " field array");
                } else if(requireStatic && !field->isStatic() && field->type != field_unresolved) {
                    errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "static access on instance field `" + object_name + "`");
                }

                // for now we are just generating code for x.x.f not Main.x...thats static access
                expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));

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
                            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "invalid access to non-class field `" + object_name + "`");
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
            if(field != NULL) {
                field = NULL;
                errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " ecpected class or module before `" + object_name + "` ");
            }
            expression.code.push_i64(SET_Di(i64, op_MOVG, klass->vaddr));
            if(lastRefrence) {
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


ResolvedReference runtime::getBaseClassOrField(string name, ClassObject* start) {
    ClassObject* base;
    Field* field;
    ResolvedReference reference;

    for(;;) {
        base = start->getBaseClass();

        if(base == NULL) {
            // could not resolve
            return reference;
        }

        if(name == base->getName()) {
            reference.klass = base;
            reference.type = ResolvedReference::CLASS;
            return reference;
            // base class
        } else if((field = base->getField(name))) {
            reference.field = field;
            reference.type = ResolvedReference::FIELD;
            return reference;
        } else {
            start = base; // recursivley assign klass to new base
        }
    }
}

void runtime::resolveBaseUtype(Scope* scope, ref_ptr& reference, Expression& expression, ast* pAst) {
    ClassObject* klass = scope->klass, *base;
    Field* field;
    ResolvedReference ref;

    if(scope->klass->getBaseClass() == NULL) {
        errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + scope->klass->getFullName() + "` does not inherit a base class");
        expression.utype.type = ResolvedReference::NOTRESOLVED;
        expression.utype.refrenceName = reference.toString();
        expression.type = expression_unresolved;
        return;
    }

    if(reference.singleRefrence()) {
        ref = getBaseClassOrField(reference.refname, klass);
        if(ref.type != ResolvedReference::NOTRESOLVED) {
            if(ref.type == ResolvedReference::FIELD) {
                // field!
            } else {
                if(scope->type == scope_static_block) {
                    errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + ref.klass->getName() + "` from self in static context");
                }

                if(scope->klass->hasBaseClass(ref.klass)) {
                    // base class
                    expression.utype.type = ResolvedReference::CLASS;
                    expression.utype.klass = ref.klass;
                    expression.type = expression_class;

                    expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                } else {
                    // klass provided is not a base
                    errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + reference.refname + "`" +
                                                                                                                                     (reference.module == "" ? "" : " in module {" + reference.module + "}")
                                                                                                                                     + " is not a base class of class " + scope->klass->getName());
                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = reference.toString();
                    expression.type = expression_unresolved;
                }
            }
        } else {
            errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + reference.refname + "` " +
                                                                                                                                       (reference.module == "" ? "" : "in module {" + reference.module + "} "));
            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = reference.toString();
            expression.type = expression_unresolved;
        }
    } else if(reference.singleRefrenceModule()) {
        base = getClass(reference.module, reference.refname);

        if(base != NULL) {
            if(scope->klass->hasBaseClass(base)) {
                // base class

                expression.utype.type = ResolvedReference::CLASS;
                expression.utype.klass = base;
                expression.type = expression_class;
                expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                return;
            } else {
                // klass provided is not a base
                errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + reference.refname + "`" +
                                                                                                                                 (reference.module == "" ? "" : " in module {" + reference.module + "}")
                                                                                                                                 + " is not a base class of class " + scope->klass->getName());
                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = reference.toString();
                expression.type = expression_unresolved;
            }
        }

        /* Un resolvable */

        errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + reference.refname + "` " +
                                                                                                                                   (reference.module == "" ? "" : "in module {" + reference.module + "} "));
        expression.utype.type = ResolvedReference::NOTRESOLVED;
        expression.utype.refrenceName = reference.refname;
        expression.type = expression_unresolved;
        return;
    } else {
        base = klass->getBaseClass();
        string starter_name = reference.class_heiarchy->at(0);

        if(base != NULL) {

            if(reference.module != "") {
                // first must be class
                if((klass = getClassGlobal(reference.module, starter_name)) != NULL) {
                    if(scope->klass->hasBaseClass(klass)) {
                        expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                        resolveClassHeiarchy(klass, reference, expression, pAst);
                    } else {
                        // klass provided is not a base
                        errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + starter_name + "`" +
                                                                                                                                                   (reference.module == "" ? "" : " in module {" + reference.module + "}")
                                                                                                                                         + " is not a base class of class " + scope->klass->getName());
                        expression.utype.type = ResolvedReference::NOTRESOLVED;
                        expression.utype.refrenceName = reference.toString();
                        expression.type = expression_unresolved;
                    }
                    return;
                } else {
                    errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                               (reference.module == "" ? "" : "in module {" + reference.module + "} "));
                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = reference.toString();
                    expression.type = expression_unresolved;
                }
            } else {
                ref = getBaseClassOrField(starter_name, klass);
                if(ref.type != ResolvedReference::NOTRESOLVED) {
                    if(ref.type == ResolvedReference::FIELD) {
                        expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                        expression.code.push_i64(SET_Di(i64, op_MOVN, ref.field->vaddr)); // gain access to field in object

                        resolveFieldHeiarchy(ref.field, reference, expression, pAst);
                    } else {
                        expression.code.push_i64(SET_Di(i64, op_MOVL, 0));

                        resolveClassHeiarchy(ref.klass, reference, expression, pAst);
                    }
                } else {
                    if((klass = getClassGlobal(reference.module, starter_name)) != NULL) {
                        if(scope->klass->hasBaseClass(klass)) {
                            expression.code.push_i64(SET_Di(i64, op_MOVL, 0));

                            resolveClassHeiarchy(klass, reference, expression, pAst);
                        } else {
                            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + starter_name + "`" +
                                                                                                                                             (reference.module == "" ? "" : " in module {" + reference.module + "}")
                                                                                                                                             + " is not a base class of class " + scope->klass->getName());
                            expression.utype.type = ResolvedReference::NOTRESOLVED;
                            expression.utype.refrenceName = reference.toString();
                            expression.type = expression_unresolved;
                        }
                    } else {
                        errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                                   (reference.module == "" ? "" : "in module {" + reference.module + "} "));
                        expression.utype.type = ResolvedReference::NOTRESOLVED;
                        expression.utype.refrenceName = reference.toString();
                        expression.type = expression_unresolved;
                    }
                }
            }
        }else {
            errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + starter_name + "` " +
                                                                                                                                       (reference.module == "" ? "" : "in module {" + reference.module + "} "));
            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = reference.toString();
            expression.type = expression_unresolved;
        }
    }
}

void runtime::resolveSelfUtype(Scope* scope, ref_ptr& reference, Expression& expression, ast* pAst) {
    if(reference.singleRefrence()) {
        ClassObject* klass=NULL;
        Field* field=NULL;

        if(scope->type == scope_global) {
            /* cannot get self from global refrence */
            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line,
                             pAst->getsubast(ast_type_identifier)->col,
                             "cannot get object `" + reference.refname + "` from self at global scope");

            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = reference.refname;
            expression.type = expression_unresolved;
        } else {
            if(scope->type == scope_static_block) {
                errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + reference.refname + "` from self in static context");
            }

            if((field = scope->klass->getField(reference.refname)) != NULL) {
                // field?
                expression.utype.type = ResolvedReference::FIELD;
                expression.utype.field = field;
                expression.type = expression_field;

                expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));
            } else {
                /* Un resolvable */
                errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + reference.refname + "` " +
                                                                                                                                           (reference.module == "" ? "" : "in module {" + reference.module + "} "));

                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = reference.refname;
                expression.type = expression_unresolved;
            }
        }
    } else if(reference.singleRefrenceModule()) {
        /* Un resolvable */
        errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " use of module {" + reference.module + "} in expression signifies global access of object");

        expression.utype.type = ResolvedReference::NOTRESOLVED;
        expression.utype.refrenceName = reference.refname;
        expression.type = expression_unresolved;
    } else {
        /* field? or class? */
        ClassObject* klass=NULL;
        Field* field=NULL;
        string starter_name = reference.class_heiarchy->at(0);

        if(scope->type == scope_global) {
            /* cannot get self from global refrence */
            errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + starter_name + "` from self at global scope");

            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = reference.refname;
            expression.type = expression_unresolved;
        } else {
            if(reference.module != "") {
                /* Un resolvable */
                errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " use of module {" + reference.module + "} in expression signifies global access of object");

                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = reference.refname;
                expression.type = expression_unresolved;
                return;
            }

            if(scope->type == scope_static_block) {
                errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + starter_name + "` from self in static context");

                expression.utype.type = ResolvedReference::NOTRESOLVED;
                expression.utype.refrenceName = reference.refname;
                expression.type = expression_unresolved;
            } else {

                if((field = scope->klass->getField(starter_name)) != NULL) {
                    expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                    expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));

                    resolveFieldHeiarchy(field, reference, expression, pAst);
                } else {
                    /* Un resolvable */
                    errors->newerror(COULD_NOT_RESOLVE, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, " `" + reference.refname + "` " +
                                                                                                                                               (reference.module == "" ? "" : "in module {" + reference.module + "} "));

                    expression.utype.type = ResolvedReference::NOTRESOLVED;
                    expression.utype.refrenceName = reference.refname;
                    expression.type = expression_unresolved;
                }
            }
        }
    }
}

void runtime::resolveUtype(ref_ptr& refrence, Expression& expression, ast* pAst) {
    Scope* scope = current_scope();
    int64_t i64;

    expression.lnk = pAst;
    if(scope->self) {
        resolveSelfUtype(scope, refrence, expression, pAst);
    } else if(scope->base) {
        resolveBaseUtype(scope, refrence, expression, pAst);
    } else {
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
                    expression.type = expression_unresolved;
                }
            } else {

                // scope_class? | scope_instance_block? | scope_static_block?
                if(scope->type != scope_class && scope->getLocalField(refrence.refname) != NULL) {
                    field = &scope->getLocalField(refrence.refname)->value;
                    expression.utype.type = ResolvedReference::FIELD;
                    expression.utype.field = field;
                    expression.type = expression_field;

                    if(field->nativeInt()) {
                        if(field->isObjectInMemory()) {
                            expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(scope, field->vaddr)));
                        } else {
                            expression.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                            expression.code.push_i64(SET_Ci(i64, op_SMOV, ebx, 0, field_offset(scope, field->vaddr)));
                        }
                    }
                    else
                        expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(scope, field->vaddr)));
                }
                else if((field = scope->klass->getField(refrence.refname)) != NULL) {
                    // field?
                    if(scope->type == scope_static_block) {
                        errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + refrence.refname + "` from self in static context");
                    }

                    expression.utype.type = ResolvedReference::FIELD;
                    expression.utype.field = field;
                    expression.type = expression_field;

                    expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                    expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));
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
                        expression.type = expression_unresolved;
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
                expression.type = expression_unresolved;
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
                    expression.type = expression_unresolved;

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
                        expression.type = expression_unresolved;
                        return;
                    }
                }

                if(scope->type != scope_class && scope->getLocalField(starter_name) != NULL) {
                    field = &scope->getLocalField(starter_name)->value;

                    if(field->nativeInt()) {
                        if(field->isObjectInMemory()) {
                            expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(scope, field->vaddr)));
                        } else {
                            expression.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                            expression.code.push_i64(SET_Ci(i64, op_SMOV, ebx, 0, field_offset(scope, field->vaddr)));
                        }
                    }
                    else
                        expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(scope, field->vaddr)));
                    resolveFieldHeiarchy(field, refrence, expression, pAst);
                    return;
                }
                else if((field = scope->klass->getField(starter_name)) != NULL) {
                    if(scope->type == scope_static_block) {
                        errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "cannot get object `" + starter_name + "` from self in static context");
                    }

                    expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
                    expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));
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
                        expression.type = expression_unresolved;
                    }
                }
            }
        }
    }
}

Expression runtime::parseUtype(ast* pAst) {
    ref_ptr ptr=parse_type_identifier(pAst->getsubast(0));
    Expression expression;

    if(ptr.singleRefrence() && parser::isnative_type(ptr.refname)) {
        expression.utype.nf = token_tonativefield(ptr.refname);
        expression.utype.type = ResolvedReference::NATIVE;
        expression.type = expression_native;
        expression.utype.refrenceName = ptr.toString();

        if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(RIGHTBRACE)) {
            expression.utype.array = true;
        }

        expression.lnk = pAst;
        ptr.free();
        return expression;
    }

    resolveUtype(ptr, expression, pAst);

    if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(RIGHTBRACE)) {
        expression.utype.array = true;
    }

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
        expression.code.push_i64(SET_Di(i64, op_MOVI, expression.utype.klass->vaddr), ebx);
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

bool runtime::expressionListToParams(List<Param> &params, List<Expression>& expressions) {
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
            field.array = expression->utype.array;

            params.add(Param(field));
        } else if(expression->type == expression_string) {
            field = Field(fvar, 0, "", NULL, mods, note);
            field.type = field_native;
            field.array = true;

            /* Native string is a char array */
            params.add(Param(field));
        } else if(expression->type == expression_class) {
            success = false;
            errors->newerror(INVALID_PARAM, expression->lnk->line, expression->lnk->col, " `class`, param must be lvalue");
        } else if(expression->type == expression_lclass) {
            field = Field(expression->utype.klass, 0, "", NULL, mods, note);
            field.type = field_class;
            field.array = expression->utype.array;

            params.add(Param(field));
        } else if(expression->type == expression_field) {
            params.add(*expression->utype.field);
        } else if(expression->type == expression_native) {
            success = false;
            errors->newerror(GENERIC, expression->lnk->line, expression->lnk->col, " unexpected symbol `" + expression->utype.refrenceName + "`");
        } else if(expression->type == expression_dynamicclass) {
            field = Field(fdynamic, 0, "", NULL, mods, note);
            field.type = field_native;
            field.array = expression->utype.array;

            params.add(field);
        } else if(expression->type == expression_void) {
            field = Field(fvoid, 0, "", NULL, mods, note);
            field.type = field_native;

            params.add(Param(field));
        }
        else if(expression->type == expression_null) {
            field = Field(fnof, 0, "", NULL, mods, note);
            field.type = field_class;
            field.null = true;

            params.add(Param(field));
        }
        else if(expression->type == expression_lclass) {
            field = Field(expression->utype.klass, 0, "", NULL, mods, note);
            field.type = field_class;
            field.array = expression->utype.array;

            params.add(Param(field));
        } else {
            /* Unknown expression */
            field = Field(fnof, 0, "", NULL, mods, note);
            field.type = field_unresolved;

            params.add(Param(field));
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

string runtime::paramsToString(List<Param> &param) {
    string message = "(";
    for(unsigned int i = 0; i < param.size(); i++) {
        switch(param.get(i).field.type) {
            case field_native:
                message += nativefield_tostr(param.get(i).field.nf);
                break;
            case field_class:
                if(param.get(i).field.null) {
                    if(param.get(i).field.klass == NULL)
                        message += "<class-type>";
                    else
                        message += param.get(i).field.klass->getName();
                } else message += param.get(i).field.klass->getFullName();
                break;
            case field_unresolved:
                message += "?";
                break;
            default:
                message += "?";
                break;
        }

        if(param.get(i).field.array)
            message += "[]";
        if((i+1) < param.size()) {
            message += ",";
        }
    }

    message += ")";
    return message;
}

void runtime::pushExpressionToStack(Expression& expression, Expression& out) {
    if(!expression._new)
        out.code.inject(out.code.__asm64.size(), expression.code);

    switch(expression.type) {
        case expression_var:
            if(expression._new) {
                out.code.push_i64(SET_Di(i64, op_INC, sp));
                out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                out.code.inject(out.code.__asm64.size(), expression.code);
            } else {
                if (expression.func) {
                } else
                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
            }
            break;
        case expression_field:
            if(expression.utype.field->nativeInt() && !expression.utype.field->array) {
                if(expression.utype.field->local) {
                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                } else {
                    out.code.push_i64(SET_Di(i64, op_MOVI, 0), adx);
                    out.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, adx));
                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                }
            } else if(expression.utype.field->nativeInt() && expression.utype.field->array) {
                out.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else if(expression.utype.field->dynamicObject() || expression.utype.field->type == field_class) {
                out.code.push_i64(SET_Ei(i64, op_PUSHREF));
            }
            break;
        case expression_lclass:
            if(expression._new) {
                out.code.push_i64(SET_Di(i64, op_INC, sp));
                out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                out.code.inject(out.code.__asm64.size(), expression.code);
            } else {
                if(expression.func) {
                    /* I think we do nothing? */
                } else {
                    out.code.push_i64(SET_Ei(i64, op_PUSHREF));
                }
            }
            break;
        case expression_string:
            out.code.push_i64(SET_Di(i64, op_INC, sp));
            out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            out.code.push_i64(SET_Di(i64, op_NEWSTR, expression.intValue));
            break;
        case expression_null:
            out.code.push_i64(SET_Di(i64, op_INC, sp));
            out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            out.code.push_i64(SET_Ei(i64, op_DEL));
            break;
        case expression_dynamicclass:
            if(expression._new) {
                out.code.push_i64(SET_Di(i64, op_INC, sp));
                out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                out.code.inject(out.code.__asm64.size(), expression.code);
            } else {
                if(expression.func) {
                    /* I think we do nothing? */
                } else {
                    out.code.push_i64(SET_Ei(i64, op_PUSHREF));
                }
            }
            break;
    }
}

void runtime::pushExpressionToStackNoInject(Expression& expression, Expression& out) {

    switch(expression.type) {
        case expression_var:
            out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
            break;
        case expression_field:
            if(expression.utype.field->nativeInt() && !expression.utype.field->array) {
                if(expression.utype.field->local) {
                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                } else {
                    out.code.push_i64(SET_Di(i64, op_MOVI, 0), adx);
                    out.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, adx));
                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                }
            } else if(expression.utype.field->nativeInt() && expression.utype.field->array) {
                out.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else if(expression.utype.field->dynamicObject() || expression.utype.field->type == field_class) {
                out.code.push_i64(SET_Ei(i64, op_PUSHREF));
            }
            break;
        case expression_lclass:
            if(expression._new) {
                out.code.push_i64(SET_Di(i64, op_INC, sp));
                out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else {
                if(expression.func) {
                    /* I think we do nothing? */
                } else {
                    out.code.push_i64(SET_Ei(i64, op_PUSHREF));
                }
            }
            break;
        case expression_string:
            out.code.push_i64(SET_Di(i64, op_INC, sp));
            out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            out.code.push_i64(SET_Di(i64, op_NEWSTR, expression.intValue));
            break;
        case expression_null:
            out.code.push_i64(SET_Di(i64, op_INC, sp));
            out.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            out.code.push_i64(SET_Ei(i64, op_DEL));
            break;
        case expression_dynamicclass:
            // ToDO: implement
            break;
    }
}

Method* runtime::resolveMethodUtype(ast* pAst, ast* pAst2, Expression &out) {
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

            if((fn = klass->getMacros(methodName, params, true)) != NULL){}
            else if((fn = klass->getFunction(methodName, params, true)) != NULL){}
            else if((fn = klass->getOverload(string_toop(methodName), params, true)) != NULL){}
            else if((fn = klass->getConstructor(params)) != NULL) {}
            else if(klass->getField(methodName, true) != NULL) {
                errors->newerror(GENERIC, pAst2->line, pAst2->col, " symbol `" + methodName + "` is a field");
            }
            else {
                if(string_toop(methodName) != oper_NO) methodName = "operator" + methodName;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + methodName + paramsToString(params) + "`");
            }
        } else if(expression.type == expression_field && expression.utype.field->type != field_class) {
            errors->newerror(GENERIC, expression.lnk->line, expression.lnk->col, " field `" +  expression.utype.field->name + "` is not a class");

        } else if(expression.utype.type == ResolvedReference::NOTRESOLVED) {
        }
        else {
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" +  methodName + paramsToString(params) +  "`");
        }
    } else {
        // method or global macros
        if(ptr.singleRefrence()) {
            if(scope->type == scope_global) {
                // must be macros
                if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                    errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname +  paramsToString(params) + "`");
                }
            } else {

                if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                    if((fn = scope->klass->getMacros(ptr.refname, params, true)) != NULL){}
                    else if((fn = scope->klass->getFunction(ptr.refname, params, true)) != NULL){}
                    else if((fn = scope->klass->getOverload(string_toop(ptr.refname), params, true)) != NULL){}
                    else if(ptr.refname == scope->klass->getName() && (fn = scope->klass->getConstructor(params)) != NULL) {}
                    else if(scope->klass->getField(methodName, true) != NULL) {
                        errors->newerror(GENERIC, pAst2->line, pAst2->col, " symbol `" + methodName + "` is a field");
                    }
                    else {
                        if(string_toop(methodName) != oper_NO) methodName = "operator" + ptr.refname;
                        else methodName = ptr.refname;

                        errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname +  paramsToString(params) + "`");
                    }
                }
            }
        } else {
            // must be macros
            if((fn = getmacros(ptr.module, ptr.refname, params)) == NULL) {
                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname +  paramsToString(params) + "`");
            }
        }
    }


    if(fn != NULL) {
        if(scope->type == scope_static_block && !fn->isStatic())
            errors->newerror(GENERIC, pAst->line, pAst->col, "illegal call to instance function in static context");

        if(fn->type != lvoid)
            out.code.push_i64(SET_Di(i64, op_INC, sp));

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        for(unsigned int i = 0; i < expressions.size(); i++) {
            pushExpressionToStack(expressions.get(i), out);
        }
        out.code.push_i64(SET_Di(i64, op_CALL, fn->vaddr));
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
    Expression expression, interm;
    Method* fn;
    ast* pAst2;

    if(pAst->hassubast(ast_dot_fn_e)) {
        pAst2 = pAst->getsubast(ast_dot_fn_e);
        fn = resolveMethodUtype(pAst2->getsubast(ast_utype), pAst2->getsubast(ast_value_list), expression);
        if(fn != NULL) {

            expression.type = methodReturntypeToExpressionType(fn);
            if(expression.type == expression_lclass) {
                expression.utype.klass = fn->klass;
                expression.utype.type = ResolvedReference::CLASS;
            }

            expression.func = true;
            expression.utype.array = fn->array;

            if(pAst->hasentity(_INC) || pAst->hasentity(_DEC)) {
                token_entity entity = pAst->hasentity(_INC) ? pAst->getentity(_INC) : pAst->getentity(_DEC);
                OperatorOverload* overload;
                List<Param> emptyParams;

                expression.type = expression_var;
                if(fn->array) {
                    errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "call to function `" + fn->getName() + paramsToString(*fn->getParams()) + "` must return an int to use `" + entity.gettoken() + "` operator");
                } else {
                    switch(fn->type) {
                        case lvoid:
                            errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "cannot use `" + entity.gettoken() + "` operator on function that returns void ");
                            break;
                        case lnative_object:
                            if(fn->nobj == fdynamic)
                                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "function returning dynamic_object must be casted before using `" + entity.gettoken() + "` operator");
                            else {
                                if(pAst->hasentity(_INC))
                                    expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                                else
                                    expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                            }
                            break;
                        case lclass_object:
                            if((overload = fn->klass->getOverload(string_toop(entity.gettoken()), emptyParams)) != NULL) {
                                // add code to call overload
                                expression.type = methodReturntypeToExpressionType(overload);
                                if(expression.type == expression_lclass) {
                                    expression.utype.klass = overload->klass;
                                    expression.utype.type = ResolvedReference::CLASS;
                                }

                                if(overload->type != lvoid)
                                    expression.code.push_i64(SET_Di(i64, op_INC, sp));
                                expression.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
                                expression.code.push_i64(SET_Di(i64, op_CALL, overload->vaddr));
                            } else if(fn->klass->hasOverload(string_toop(entity.gettoken()))) {
                                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "call to function `" + fn->getName() + paramsToString(*fn->getParams()) + "`; missing overload params for operator `"
                                                                                                + fn->klass->getFullName() + ".operator" + entity.gettoken() + "`");
                            } else {
                                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "call to function `" + fn->getName() + paramsToString(*fn->getParams()) + "` must return an int to use `" + entity.gettoken() + "` operator");
                            }
                            break;
                        case lundefined:
                            // do nothing
                            break;
                    }
                }
            }

            // TODO: check for ++ and --
        } else
            expression.type = expression_unresolved;
    } else {
        expression = parseUtype(pAst->getsubast(ast_utype));
    }

    if(pAst->getsubastcount() > 1) {
        // chained calls
        if(expression.type == expression_void) {
            errors->newerror(GENERIC, pAst->getsubast(1)->line, pAst->getsubast(1)->col, "illegal acces to function of return type `void`");
        } else
            parseDotNotationChain(pAst, expression, 1);
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

Method* runtime::resolveSelfMethodUtype(ast* pAst, ast* pAst2, Expression& out) {
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
        if(scope->type == scope_global) {

            errors->newerror(GENERIC, pAst2->line, pAst2->col,
                             "cannot get object `" + methodName + paramsToString(params) + "` from self at global scope");
        }

        scope->self = true;
        resolveUtype(ptr, expression, pAst);
        scope->self = false;
        if(expression.type == expression_class || (expression.type == expression_field && expression.utype.field->type == field_class)) {
            ClassObject* klass;
            if(expression.type == expression_class) {
                klass = expression.utype.klass;
            } else {
                klass = expression.utype.field->klass;
            }

            if((fn = klass->getMacros(methodName, params, true)) != NULL){}
            else if((fn = klass->getFunction(methodName, params, true)) != NULL){}
            else if((fn = klass->getOverload(string_toop(methodName), params, true)) != NULL){}
            else if(methodName == klass->getName() && (fn = klass->getConstructor(params)) != NULL) {}
            else {
                if(string_toop(methodName) != oper_NO) methodName = "operator" + methodName;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + methodName + paramsToString(params) + "`");
            }
        } else if(expression.utype.type == ResolvedReference::NOTRESOLVED) {
        }
        else {
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + methodName + paramsToString(params) +  "`");
        }
    } else {
        // method or macros
        if(ptr.singleRefrence()) {
            if(scope->type == scope_global) {
                // must be macros
                errors->newerror(GENERIC, pAst2->line, pAst2->col,
                                 "cannot get object `" + ptr.refname + paramsToString(params) + "` from self at global scope");
            } else {

                if((fn = scope->klass->getMacros(ptr.refname, params, true)) != NULL){}
                else if((fn = scope->klass->getFunction(ptr.refname, params, true)) != NULL){}
                else if((fn = scope->klass->getOverload(string_toop(ptr.refname), params, true)) != NULL){}
                else if(ptr.refname == scope->klass->getName() && (fn = scope->klass->getConstructor(params)) != NULL) {}
                else {
                    if(string_toop(methodName) != oper_NO) methodName = "operator" + ptr.refname;
                    else methodName = ptr.refname;

                    errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname +  paramsToString(params) + "`");
                }
            }
        } else {
            // must be macros
            errors->newerror(GENERIC, pAst2->line, pAst2->col, " use of module {" + ptr.refname + "} in expression signifies global access of object");

        }
    }

    if(fn != NULL) {
        if(!fn->isStatic() && scope->type == scope_static_block) {
            errors->newerror(GENERIC, pAst->line, pAst->col, "call to instance function in static context");
        }

        if(fn->type != lvoid) {
            out.code.push_i64(SET_Di(i64, op_INC, sp));
        }

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        if(!fn->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        for(unsigned int i = 0; i < expressions.size(); i++) {
            pushExpressionToStack(expressions.get(i), out);
        }
        out.code.push_i64(SET_Di(i64, op_CALL, fn->vaddr));
    }

    __freeList(params);
    __freeList(expressions);
    ptr.free();
    return fn;
}

Expression runtime::parseSelfDotNotationCall(ast* pAst) {
    Scope* scope = current_scope();
    string method_name="";
    Expression expression;
    Method* fn;
    ast* pAst2;

    if(pAst->hassubast(ast_dot_fn_e)) {
        pAst2 = pAst->getsubast(ast_dot_fn_e);
        fn = resolveSelfMethodUtype(pAst2->getsubast(ast_utype), pAst2->getsubast(ast_value_list), expression);
        if(fn != NULL) {
            expression.type = methodReturntypeToExpressionType(fn);
            if(expression.type == expression_lclass) {
                expression.utype.klass = fn->klass;
                expression.utype.type = ResolvedReference::CLASS;
            }

            expression.utype.array = fn->array;
        } else
            expression.type = expression_unresolved;

    } else {
        scope->self = true;
        expression = parseUtype(pAst->getsubast(ast_utype));
        scope->self = false;
    }

    if(pAst->hasentity(DOT)) {
        expression.dot = true;
    }


    if(pAst->getsubastcount() > 1) {
        // chained calls
        parseDotNotationChain(pAst, expression, 1);
    }

    expression.lnk = pAst;
    return expression;
}

Method* runtime::resolveBaseMethodUtype(ast* pAst, ast* pAst2, Expression& out) {
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
        if(scope->type == scope_global) {

            errors->newerror(GENERIC, pAst2->line, pAst2->col,
                             "cannot get object `" + methodName + paramsToString(params) + "` from self at global scope");
        }

        scope->base = true;
        resolveUtype(ptr, expression, pAst);
        scope->base = false;
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
            else if(methodName == klass->getName() && (fn = klass->getConstructor(params)) != NULL) {}
            else {
                if(string_toop(methodName) != oper_NO) methodName = "operator" + methodName;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + (klass == NULL ? ptr.refname : methodName) + paramsToString(params) + "`");
            }
        } else if(expression.utype.type == ResolvedReference::NOTRESOLVED) {
        }
        else {
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + (expression.utype.klass == NULL ? ptr.refname : methodName) + paramsToString(params) +  "`");
        }
    } else {
        // method or macros
        if(ptr.singleRefrence()) {
            if(scope->type == scope_global) {
                // must be macros
                errors->newerror(GENERIC, pAst2->line, pAst2->col,
                                 "cannot get object `" + ptr.refname + paramsToString(params) + "` from self at global scope");
            } else {
                ClassObject* base, *start = scope->klass->getBaseClass();

                if(start == NULL) {
                    errors->newerror(GENERIC, pAst->getsubast(ast_type_identifier)->line, pAst->getsubast(ast_type_identifier)->col, "class `" + scope->klass->getFullName() + "` does not inherit a base class");
                    return NULL;
                }

                for(;;) {
                    base = start;

                    if(base == NULL) {
                        // could not resolve
                        if(string_toop(methodName) != oper_NO) methodName = "operator" + ptr.refname;
                        else methodName = ptr.refname;

                        errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname +  paramsToString(params) + "`");
                        return NULL;
                    }

                    if((fn = base->getMacros(ptr.refname, params)) != NULL){ break; }
                    else if((fn = base->getFunction(ptr.refname, params)) != NULL){ break; }
                    else if((fn = base->getOverload(string_toop(ptr.refname), params)) != NULL){ break; }
                    else if(ptr.refname == base->getName() && (fn = base->getConstructor(params)) != NULL) { break; }
                    else {
                        start = base->getBaseClass(); // recursivley assign klass to new base
                    }
                }
            }
        } else {
            // must be macros
            errors->newerror(GENERIC, pAst2->line, pAst2->col, " use of module {" + ptr.refname + "} in expression signifies global access of object");

        }
    }


    if(fn != NULL) {
        if(!fn->isStatic() && scope->type == scope_static_block) {
            errors->newerror(GENERIC, pAst->line, pAst->col, "call to instance function in static context");
        }

        if(fn->type != lvoid) {
            out.code.push_i64(SET_Di(i64, op_INC, sp));
        }

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        if(!fn->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        for(unsigned int i = 0; i < expressions.size(); i++) {
            pushExpressionToStack(expressions.get(i), out);
        }
        out.code.push_i64(SET_Di(i64, op_CALL, fn->vaddr));
    }

    __freeList(params);
    __freeList(expressions);
    ptr.free();
    return fn;
}

Expression runtime::parseBaseDotNotationCall(ast* pAst) {
    Scope* scope = current_scope();
    Expression expression;
    string method_name="";
    Method* fn;
    ast* pAst2;

    if(pAst->hassubast(ast_dot_fn_e)) {
        pAst2 = pAst->getsubast(ast_dot_fn_e);
        fn = resolveBaseMethodUtype(pAst2->getsubast(ast_utype), pAst2->getsubast(ast_value_list), expression);
        if(fn != NULL) {
            expression.type = methodReturntypeToExpressionType(fn);
            if(expression.type == expression_lclass) {
                expression.utype.klass = fn->klass;
                expression.utype.type = ResolvedReference::CLASS;
            }

            expression.utype.array = fn->array;
        } else
            expression.type = expression_unresolved;

    } else {
        scope->base = true;
        expression = parseUtype(pAst->getsubast(ast_utype));
        scope->base = false;
    }

    if(pAst->hasentity(DOT)) {
        expression.dot = true;
    }

    if(pAst->getsubastcount() > 1) {
        // chained calls
        parseDotNotationChain(pAst, expression, 1);
    }

    expression.lnk = pAst;
    return expression;
}

Expression runtime::parseSelfExpression(ast* pAst) {
    Scope* scope = current_scope();
    Expression expression;

    if(pAst->hasentity(PTR)) {
        // self-><expression>
        return parseSelfDotNotationCall(pAst->getsubast(ast_dotnotation_call_expr));
    } else {
        // self
        expression.type = expression_lclass;
        expression.utype.klass = scope->klass;
        expression.code.push_i64(SET_Di(i64, op_MOVL, 0));
    }

    if(scope->type == scope_global || scope->type == scope_static_block) {
        errors->newerror(GENERIC, pAst->line, pAst->col, "illegal reference to self in static context");
        expression.type = expression_unknown;
    }

    expression.lnk = pAst;
    return expression;
}

Expression runtime::parseBaseExpression(ast* pAst) {
    Scope* scope = current_scope();
    Expression expression;

    expression = parseBaseDotNotationCall(pAst->getsubast(ast_dotnotation_call_expr));

    expression.lnk = pAst;
    return expression;
}

Expression runtime::parseNullExpression(ast* pAst) {
    Scope* scope = current_scope();
    Expression expression(pAst);

    expression.utype.refrenceName = "null";
    expression.type = expression_null;
    return expression;
}

List<Expression> runtime::parseVectorArray(ast* pAst) {
    List<Expression> vecArry;

    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        vecArry.add(parseExpression(pAst->getsubast(i)));
    }

    return vecArry;
}

void runtime::checkVectorArray(Expression& utype, List<Expression>& vecArry) {
    if(utype.utype.type == ResolvedReference::NOTRESOLVED) return;

    for(unsigned int i = 0; i < vecArry.size(); i++) {
        if(vecArry.get(i).type != expression_unresolved) {
            switch(vecArry.get(i).type) {
                case expression_native:
                    errors->newerror(UNEXPECTED_SYMBOL, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, " `" + nativefield_tostr(utype.utype.nf) + "`");
                    break;
                case expression_lclass:
                    if(utype.utype.type == ResolvedReference::CLASS) {
                        if(!utype.utype.klass->match(vecArry.get(i).utype.klass)){
                            errors->newerror(INCOMPATIBLE_TYPES, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, ": initalization of class `" +
                                                                                                                    utype.utype.klass->getName() + "` is not compatible with class `" + vecArry.get(i).utype.klass->getName() + "`");
                        }
                    } else {
                        errors->newerror(INCOMPATIBLE_TYPES, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, ": type `" + utype.utype.toString() + "` is not compatible with type `"
                                                                                                                + vecArry.get(i).utype.toString() + "`");
                    }
                    break;
                case expression_var:
                    if(utype.utype.type != ResolvedReference::NATIVE) {
                        errors->newerror(INCOMPATIBLE_TYPES, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, ": type `" + utype.utype.toString() + "` is not compatible with type `"
                                                                                                                + vecArry.get(i).utype.toString() + "`");
                    }else {
                        if(utype.utype.nativeInt() && vecArry.get(i).utype.nativeInt()) {}
                        else if(utype.utype.dynamicObject() && vecArry.get(i).utype.dynamicObject()) {}
                        else {
                            errors->newerror(INCOMPATIBLE_TYPES, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, ": type `" + utype.utype.toString() + "` is not compatible with type `"
                                                                                                                    + vecArry.get(i).utype.toString() + "`");
                        }
                    }
                    break;
                case expression_null:
                    if(utype.utype.type != ResolvedReference::CLASS) {
                        errors->newerror(GENERIC, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, "cannot assign null to type `" + utype.utype.toString() + "1");
                    }
                    break;
                case expression_string:
                    errors->newerror(GENERIC, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, "multi dimentional array are not supported yet, use string() class instead");
                    break;
                case expression_dynamicclass:
                    if(utype.utype.type != ResolvedReference::CLASS) {
                        errors->newerror(GENERIC, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, "cannot assign null to type `" + utype.utype.toString() + "`");
                    }
                    break;
                default:
                    errors->newerror(GENERIC, vecArry.get(i).lnk->line, vecArry.get(i).lnk->col, "error processing vector array");
                    break;
            }
        }
    }
}

void runtime::pushExpressionToRegister(Expression& expr, Expression& out, int reg) {
    out.code.inject(out.code.__asm64.size(), expr.code);
    pushExpressionToRegisterNoInject(expr, out, reg);
}


void runtime::pushExpressionToRegisterNoInject(Expression& expr, Expression& out, int reg) {
    switch(expr.type) {
        case expression_var:
            if(expr.utype.array) {
                errors->newerror(GENERIC, expr.lnk, "cannot get integer value from var[] expression");
            }

            if(expr.func) {
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
                out.code.push_i64(SET_Ci(i64, op_SMOV, reg,0, 0));
                out.code.push_i64(SET_Ei(i64, op_POP));

            } else if(reg != ebx) {
                out.code.push_i64(SET_Ci(i64, op_MOVR, reg,0, ebx));
            }
            break;
        case expression_field:
            if(expr.utype.field->nativeInt() && !expr.utype.field->array) {
                if(expr.utype.field->local) {
                    if(reg != ebx) {
                        out.code.push_i64(SET_Ci(i64, op_MOVR, reg,0, ebx));
                    }
                } else {
                    out.code.push_i64(SET_Di(i64, op_MOVI, 0), adx);
                    out.code.push_i64(SET_Ci(i64, op_MOVX, reg,0, adx));
                }
            } else if(expr.utype.field->nativeInt() && expr.utype.field->array) {
                errors->newerror(GENERIC, expr.lnk, "cannot get integer value from " + expr.utype.field->name + "[] expression");
            } else if(expr.utype.field->dynamicObject() || expr.utype.field->type == field_class) {
                errors->newerror(GENERIC, expr.lnk, "cannot get integer value from non integer type `dynamic_object`");
            }
            break;
        case expression_lclass:
            errors->newerror(GENERIC, expr.lnk, "cannot get integer value from non integer type `" + expr.utype.klass->getFullName() +"`");
            break;
        case expression_string:
            errors->newerror(GENERIC, expr.lnk, "cannot get integer value from non integer type `var[]`");
            break;
        case expression_null:
            errors->newerror(GENERIC, expr.lnk, "cannot get integer value from type of null");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, expr.lnk, "cannot get integer value from non integer type `dynamic_object`");
            break;
        default:
            errors->newerror(GENERIC, expr.lnk, "cannot get integer value from non integer expression");
            break;
    }
}

void runtime::parseNewArrayExpression(Expression& out, Expression& utype, ast* pAst) {
    Expression sizeExpr = parseExpression(pAst->getsubast(ast_expression));

    pushExpressionToRegister(sizeExpr, out, ebx);
    if(out.type == expression_var)
        out.code.push_i64(SET_Di(i64, op_NEWi, ebx));
    else
        out.code.push_i64(SET_Di(i64, op_NEW_OBJ_ARRY, ebx));
}

Expression runtime::parseNewExpression(ast* pAst) {
    Scope* scope = current_scope();
    Expression expression, utype;
    List<Expression> expressions;
    List<Param> params;

    utype = parseUtype(pAst->getsubast(ast_utype));
    if(pAst->hassubast(ast_vector_array)) {
        // vec array
        if(!utype.utype.array) {
            errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "expected '[]' before array initalizer '{}'");
        }

        expressions = parseVectorArray(pAst->getsubast(ast_vector_array));
        checkVectorArray(utype, expressions);

        expression.type = utype.type;
        expression.utype = utype.utype;
        if(expression.type == expression_class) {
            expression.type = expression_lclass;
        } else if(expression.type == expression_native) {
            if(utype.utype.nativeInt())
                expression.type = expression_var;
            else if(utype.utype.dynamicObject())
                expression.type = expression_dynamicclass;
        }
        expression.utype.array = true;
    }
    else if(pAst->hassubast(ast_array_expression)) {
        expression.type = utype.type;
        expression.utype = utype.utype;
        if(expression.type == expression_class) {
            expression.type = expression_lclass;
        } else if(expression.type == expression_native) {
            if(utype.utype.nativeInt())
                expression.type = expression_var;
            else if(utype.utype.dynamicObject())
                expression.type = expression_dynamicclass;
        }
        expression.utype.array = true;
        parseNewArrayExpression(expression, utype, pAst->getsubast(ast_array_expression));
    }
    else {
        // ast_value_list
        if(pAst->hassubast(ast_value_list))
            expressions = parseValueList(pAst->getsubast(ast_value_list));
        if(utype.type == expression_class) {
            Method* fn=NULL;
            if(!pAst->hassubast(ast_value_list)) {
                errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "expected '()' after class " + utype.utype.klass->getName());
            } else {
                expressionListToParams(params, expressions);
                if((fn=utype.utype.klass->getConstructor(params))==NULL) {
                    errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "class `" + utype.utype.klass->getFullName() +
                            "` does not contain constructor `" + utype.utype.klass->getName() + paramsToString(params) + "`");
                }
            }
            expression.type = expression_lclass;
            expression.utype = utype.utype;

            if(fn!= NULL) {

                expression.code.push_i64(SET_Di(i64, op_NEW_CLASS, utype.utype.klass->vaddr));
                expression.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
                if(!fn->isStatic())
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

                for(unsigned int i = 0; i < expressions.size(); i++) {
                    pushExpressionToStack(expressions.get(i), expression);
                }
                expression.code.push_i64(SET_Di(i64, op_CALL, fn->vaddr));
            }
        } else if(utype.type == expression_native) {
            // native creation
            if(pAst->hassubast(ast_value_list)) {
                errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, " native type `" + nativefield_tostr(utype.utype.nf) + "` does not contain a constructor");
            }
            expression.type = expression_native;
            expression.utype = utype.utype;
        } else {
            if(utype.utype.type != ResolvedReference::NOTRESOLVED)
                errors->newerror(COULD_NOT_RESOLVE, utype.lnk->line, utype.lnk->col, " `" + utype.utype.toString() + "`");
        }
    }

    __freeList(params);
    __freeList(expressions);
    expression._new = true;
    expression.lnk = pAst;
    return expression;
}

void runtime::postIncClass(Expression& out, token_entity op, ClassObject* klass) {
    OperatorOverload* overload;
    List<Param> emptyParams;

    if(op.gettokentype() == _INC) {
        overload = klass->getPostIncOverload();
    } else {
        overload = klass->getPostDecOverload();
    }

    if(overload != NULL) {
        // add code to call overload
        if(overload->type != lvoid)
            out.code.push_i64(SET_Di(i64, op_INC, sp));

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        out.code.push_i64(SET_Di(i64, op_MOVI, 1), ebx);
        out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

        if(!overload->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        out.code.push_i64(SET_Di(i64, op_CALL, overload->vaddr));

        out.type = methodReturntypeToExpressionType(overload);
        if(out.type == expression_lclass) {
            out.utype.klass = overload->klass;
            out.utype.type = ResolvedReference::CLASS;
        } else {

        }
    } else if(klass->hasOverload(string_toop(op.gettoken()))) {
        errors->newerror(GENERIC, op.getline(), op.getcolumn(), "use of class `" + klass->getName() + "`; missing overload params for operator `"
                                                                        + klass->getFullName() + ".operator" + op.gettoken() + "`");
        out.type = expression_var;
    } else {
        errors->newerror(GENERIC, op.getline(), op.getcolumn(), "use of class `" + klass->getName() + "` must return an int to use `" + op.gettoken() + "` operator");
        out.type = expression_var;
    }
}

Expression runtime::parsePostInc(ast* pAst) {
    Expression interm;
    token_entity entity = pAst->hasentity(_INC) ? pAst->getentity(_INC) : pAst->getentity(_DEC);

    interm = parseIntermExpression(pAst->getsubast(0));
    Expression expression(interm);

    if(interm.utype.array){
        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "expression must evaluate to an int to use `" + entity.gettoken() + "` operator");
    } else {
        switch(interm.type) {
            case expression_var:
                if(interm.func) {
                    pushExpressionToRegister(interm, expression, ebx);
                    if(entity.gettokentype() == _INC)
                        expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                    else
                        expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                } else {
                    if(entity.gettokentype() == _INC)
                        expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                    else
                        expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                }
                break;
            case expression_field:
                if(interm.utype.field->type == field_class) {
                    if(expression.utype.field->local)
                        expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(current_scope(), expression.utype.field->vaddr)));
                    postIncClass(expression, entity, interm.utype.field->klass);
                    return expression;
                } else if(interm.utype.field->type == field_native) {
                    if(interm.utype.field->nf == fdynamic) {
                        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "use of `" + entity.gettoken() + "` operator on field of type `dynamic_object` without a cast. Try ((SomeClass)dynamic_class)++");
                    } else if(interm.utype.field->nativeInt()) {
                        // increment the field
                        pushExpressionToRegisterNoInject(interm, expression, ebx);
                        expression.code.push_i64(SET_Di(i64, op_MOVI, 1), ecx);

                        if(entity.gettokentype() == _INC)
                            expression.code.push_i64(
                                    SET_Ci(i64, op_ADDL, ecx,0 , field_offset(current_scope(), interm.utype.field->vaddr)));
                        else
                            expression.code.push_i64(
                                    SET_Ci(i64, op_SUBL, ecx,0 , field_offset(current_scope(), interm.utype.field->vaddr)));
                    } else {
                        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "expression must evaluate to an int to use `" + entity.gettoken() + "` operator");
                    }
                } else {
                    // do nothing field is unresolved
                }
                break;
            case expression_lclass:
                if(interm.func)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                postIncClass(expression, entity, interm.utype.klass);
                return expression;
                break;
            case expression_dynamicclass:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "use of `" + entity.gettoken() + "` operator on type `dynamic_object` without a cast. Try ((SomeClass)dynamic_class)++");
                break;
            case expression_null:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "value `null` cannot be used as var");
                break;
            case expression_native:
                errors->newerror(UNEXPECTED_SYMBOL, entity.getline(), entity.getcolumn(), " `" + nativefield_tostr(interm.utype.nf) + "`");
                break;
            case expression_string:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "increment on immutable string");
                break;
            case expression_unresolved:
                // do nothing
                break;
            default:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "expression must evaluate to an int to use `" + entity.gettoken() + "` operator");
                break;
        }
    }

    expression.type = expression_var;
    return expression;
}

void runtime::resolveUtypeContext(ClassObject* classContext, ref_ptr& refrence, Expression& expression, ast* pAst) {
    int64_t i64;
    Field* field;

    if(refrence.singleRefrence()) {
        if((field = classContext->getField(refrence.refname)) != NULL) {

            expression.utype.type = ResolvedReference::FIELD;
            expression.utype.field = field;
            expression.code.push_i64(SET_Di(i64, op_MOVN, field->vaddr));
            expression.type = expression_field;
        }else {
            /* Un resolvable */
            errors->newerror(COULD_NOT_RESOLVE, pAst->line, pAst->col, " `" + refrence.refname + "` " + (refrence.module == "" ? "" : "in module {" + refrence.module + "} "));

            expression.utype.type = ResolvedReference::NOTRESOLVED;
            expression.utype.refrenceName = refrence.refname;
            expression.type = expression_unresolved;
        }
    } else {
        resolveClassHeiarchy(classContext, refrence, expression, pAst);
    }
}

Expression runtime::parseUtypeContext(ClassObject* classContext, ast* pAst) {
    ref_ptr ptr=parse_type_identifier(pAst->getsubast(0));
    Expression expression;

    if(ptr.singleRefrence() && parser::isnative_type(ptr.refname)) {
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + ptr.refname + "`");

        expression.type = expression_unresolved;
        expression.lnk = pAst;
        ptr.free();
        return expression;
    }

    resolveUtypeContext(classContext, ptr, expression, pAst);

    if(pAst->hasentity(LEFTBRACE) && pAst->hasentity(RIGHTBRACE)) {
        expression.utype.array = true;
    }

    expression.lnk = pAst;
    expression.utype.refrenceName = ptr.toString();
    ptr.free();
    return expression;
}

Method* runtime::resolveContextMethodUtype(ClassObject* classContext, ast* pAst, ast* pAst2, Expression& out, Expression& contextExpression) {
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

    if(ptr.module != "") {
        errors->newerror(GENERIC, pAst->line, pAst->col, "module name not allowed in nested method call");
        return NULL;
    }

    if(splitMethodUtype(methodName, ptr)) {
        // accessor
        resolveUtypeContext(classContext, ptr, expression, pAst);
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
            else if(classContext->getField(methodName) != NULL) {
                errors->newerror(GENERIC, pAst2->line, pAst2->col, " symbol `" + classContext->getFullName() + methodName + "` is a field");
            }
            else {
                if(string_toop(methodName) != oper_NO) methodName = "operator" + methodName;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + classContext->getFullName() + "." + (klass == NULL ? ptr.refname : methodName) + paramsToString(params) + "`");
            }
        }
        else if(expression.type == expression_field && expression.utype.field->type != field_class) {
            errors->newerror(GENERIC, pAst2->line, pAst2->col, " field `" + (expression.utype.klass == NULL ? ptr.refname : methodName) + "` is not a class");

        }
        else if(expression.utype.type == ResolvedReference::NOTRESOLVED) {
        }
        else {
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + classContext->getFullName() + "." + (expression.utype.klass == NULL ? ptr.refname : methodName) + "`");
        }
    } else {
        // method or global macros
        if(ptr.singleRefrence()) {
            if((fn = classContext->getMacros(ptr.refname, params)) != NULL){}
            else if((fn = classContext->getFunction(ptr.refname, params)) != NULL){}
            else if((fn = classContext->getOverload(string_toop(ptr.refname), params)) != NULL){}
            else if(classContext->getField(ptr.refname) != NULL) {
                errors->newerror(GENERIC, pAst2->line, pAst2->col, " symbol `" + ptr.refname + "` is a field");
            }
            else {
                if(string_toop(ptr.refname) != oper_NO) methodName = "operator" + ptr.refname;
                else methodName = ptr.refname;

                errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + classContext->getFullName() + "." + methodName +  paramsToString(params) + "`");
            }
        } else {
            // must be macros but it can be
            errors->newerror(COULD_NOT_RESOLVE, pAst2->line, pAst2->col, " `" + ptr.refname + "`");
        }
    }


    if(fn != NULL) {
        if(fn->isStatic()) {
            warning(GENERIC, pAst->line, pAst->col, "instance access to static function");
        }

        if(contextExpression.func && !fn->isStatic())
            out.code.push_i64(SET_Di(i64, op_MOVSL, 0));

        if(fn->type != lvoid) {
            out.code.push_i64(SET_Di(i64, op_INC, sp));
        }

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        if(contextExpression.func && !fn->isStatic()) {
            out.code.push_i64(SET_Di(i64, op_INC, sp));
            out.code.push_i64(SET_Di(i64, op_SMOVOBJ, 0)); // mutate object to the stack move self
        }
        else if(!fn->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        for(unsigned int i = 0; i < expressions.size(); i++) {
            pushExpressionToStack(expressions.get(i), out);
        }
        out.code.push_i64(SET_Di(i64, op_CALL, fn->vaddr));
    }

    __freeList(params);
    __freeList(expressions);
    ptr.free();
    return fn;
}

Expression runtime::parseDotNotationCallContext(Expression& contextExpression, ast* pAst) {
    string method_name="";
    Expression expression, interm;
    Method* fn;

    if(contextExpression.type == expression_dynamicclass) {
        errors->newerror(GENERIC, pAst->line, pAst->col, "cannot infer function from dynamic class, did you forget to ass a cast? i.e. ((SomeClass)dynamic_class)");
        expression.type = expression_unresolved;
        return expression;
    } else if(contextExpression.type == expression_unresolved) {
        expression.type= expression_unresolved;
        return expression;
    } else if(contextExpression.type != expression_lclass) {
        errors->newerror(GENERIC, pAst->line, pAst->col, "expression does not return a class");
        expression.type = expression_unresolved;
        return expression;
    }

    ClassObject* klass = contextExpression.utype.klass;

    if(pAst->gettype() == ast_dot_fn_e) {
        fn = resolveContextMethodUtype(klass, pAst->getsubast(ast_utype),
                                       pAst->getsubast(ast_value_list), expression, contextExpression);
        if(fn != NULL) {
            expression.type = methodReturntypeToExpressionType(fn);
            if(expression.type == expression_lclass)
                expression.utype.klass = fn->klass;
            expression.utype.array = fn->array;

            // TODO: check for ++ and --
            // TODO: parse dot_notation_chain expression
        } else
            expression.type = expression_unresolved;
        expression.func=true;
    } else {
        if(contextExpression.func)
            expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

        /*
         * Nasty way to check which ast to pass
         * can we do this a better way?
         */
        if(pAst->gettype() == ast_utype)
            expression = parseUtypeContext(klass, pAst);
        else
            expression = parseUtypeContext(klass, pAst->getsubast(ast_utype));
    }

    if(pAst->hasentity(DOT)) {
        expression.dot = true;
    }

    expression.lnk = pAst;
    return expression;
}

Expression runtime::parseArrayExpression(Expression& interm, ast* pAst) {
    Expression expression(interm), indexExpr;

    indexExpr = parseExpression(pAst);

    expression._new=false;
    expression.utype.array = false;
    expression.utype.array = false;
    expression.arrayElement=true;
    expression.lnk = pAst;
    bool referenceAffected = currentRefrenceAffected(indexExpr);

    switch(interm.type) {
        case expression_field:
            if(!interm.utype.field->array) {
                // error not an array
                errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            expression.code.inject(expression.code.__asm64.size(), indexExpr.code);

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));

            if(interm.utype.field->type == field_class) {
                expression.utype.klass = interm.utype.field->klass;
                expression.type = expression_lclass;

                expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));
            } else if(interm.utype.field->type == field_native) {
                expression.type = expression_var;
                expression.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));
            }else {
                expression.type = expression_unknown;
            }

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_string:
            errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            break;
        case expression_var:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            expression.code.inject(expression.code.__asm64.size(), indexExpr.code);
            if(indexExpr.func || indexExpr.type != expression_var) {
                expression.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
                expression.code.push_i64(SET_Ci(i64, op_SMOV, ebx,0, 0));
                expression.code.push_i64(SET_Ei(i64, op_POP));
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_lclass:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            expression.code.inject(expression.code.__asm64.size(), indexExpr.code);
            if(indexExpr.func || indexExpr.type != expression_var) {
                expression.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
                expression.code.push_i64(SET_Ci(i64, op_SMOV, ebx,0, 0));
                expression.code.push_i64(SET_Ei(i64, op_POP));
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "null cannot be used as an array");
            break;
        case expression_dynamicclass:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            expression.code.inject(expression.code.__asm64.size(), indexExpr.code);
            if(indexExpr.func || indexExpr.type != expression_var) {
                expression.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, sp));
                expression.code.push_i64(SET_Ci(i64, op_SMOV, ebx,0, 0));
                expression.code.push_i64(SET_Ei(i64, op_POP));
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_void:
            errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "void cannot be used as an array");
            break;
        case expression_unresolved:
            /* do nothing */
            break;
        default:
            errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "invalid array expression before `[`");
            break;
    }

    if(indexExpr.type == expression_var) {
        // we have an integer!
    } else if(indexExpr.type == expression_field) {
        if(!indexExpr.utype.field->nativeInt()) {
            errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "array index is not an integer");
        }
    } else if(indexExpr.type == expression_unresolved) {
    } else {
        errors->newerror(GENERIC, indexExpr.lnk->line, indexExpr.lnk->col, "array index is not an integer");
    }

    return expression;
}

bool runtime::currentRefrenceAffected(Expression& expr) {
    int opcode;
    for(unsigned int i = 0; i < expr.code.size(); i++) {
        opcode = GET_OP(expr.code.__asm64.get(i));

        switch(opcode) {
            case op_MOVL:
            case op_MOVSL:
            case op_MOVN:
            case op_MOVG:
            case op_MOVND:
                return true;
            default:
                break;
        }
    }

    return false;
}

Expression runtime::parseArrayExpression(ast* pAst) {
    Expression expression, interm, indexExpr, rightExpr;
    Field* field;

    interm = parseIntermExpression(pAst->getsubast(0));
    indexExpr = parseExpression(pAst->getsubast(1));

    if(!interm._new)
        expression.code.inject(expression.code.__asm64.size(), interm.code);
    expression.type = interm.type;
    expression.utype = interm.utype;
    expression.utype.array = false;
    expression._new=false;
    expression.arrayElement=true;
    expression.lnk = pAst;
    bool referenceAffected = currentRefrenceAffected(indexExpr);

    switch(interm.type) {
        case expression_field:
            if(!interm.utype.field->array) {
                // error not an array
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            expression.code.inject(expression.code.__asm64.size(), indexExpr.code);

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));

            if(interm.utype.field->type == field_class) {
                expression.utype.klass = interm.utype.field->klass;
                expression.type = expression_lclass;

                expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));
            } else if(interm.utype.field->type == field_native) {
                expression.type = expression_var;
                expression.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));
            }else {
                expression.type = expression_unknown;
            }

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }

            break;
        case expression_string:
            errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            break;
        case expression_var:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            pushExpressionToRegister(indexExpr, expression, ebx);

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, ebx));

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_lclass:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(interm._new) {
                expression.code.push_i64(SET_Di(i64, op_INC, sp));
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                expression.code.inject(expression.code.__asm64.size(), interm.code);
            }

            if(!interm._new) {
                if(c_options.optimize) {
                    if(referenceAffected)
                        expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
                } else
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            }


            pushExpressionToRegister(indexExpr, expression, ebx);

            if(!interm._new) {
                if (c_options.optimize) {
                    if (referenceAffected)
                        expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                } else
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            }

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));

            if(!interm._new) {
                if (c_options.optimize) {
                    if (referenceAffected) {
                        expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                    }
                } else {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_POP));
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "null cannot be used as an array");
            break;
        case expression_dynamicclass:
            if(!interm.utype.array) {
                // error not an array
                errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "expression of type `" + interm.typeToString() + "` must evaluate to array");
            }

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Ei(i64, op_PUSHREF));
            } else
                expression.code.push_i64(SET_Ei(i64, op_PUSHREF));

            pushExpressionToRegister(indexExpr, expression, ebx);

            if(c_options.optimize) {
                if(referenceAffected)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            } else
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));

            expression.code.push_i64(SET_Di(i64, op_CHECKLEN, ebx));
            expression.code.push_i64(SET_Di(i64, op_MOVND, ebx));

            if(c_options.optimize) {
                if(referenceAffected) {
                    expression.code.push_i64(SET_Ei(i64, op_SDELREF));
                }
            } else {
                expression.code.push_i64(SET_Ei(i64, op_SDELREF));
            }
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "void cannot be used as an array");
            break;
        case expression_unresolved:
            /* do nothing */
            break;
        default:
            errors->newerror(GENERIC, pAst->getsubast(0)->line, pAst->getsubast(0)->col, "invalid array expression before `[`");
            break;
    }

    if(indexExpr.type == expression_var) {
        // we have an integer!
    } else if(indexExpr.type == expression_field) {
        if(!indexExpr.utype.field->nativeInt()) {
            errors->newerror(GENERIC, pAst->getsubast(1)->line, pAst->getsubast(1)->col, "array index is not an integer");
        }
    } else if(indexExpr.type == expression_unresolved) {
    } else {
        errors->newerror(GENERIC, pAst->getsubast(1)->line, pAst->getsubast(1)->col, "array index is not an integer");
    }

    if(pAst->getsubastcount() > 2) {
        // expression after (we may not need this?)
        parseDotNotationChain(pAst->getsubast(2), expression, 0);
    }

    return expression;
}

Expression &runtime::parseDotNotationChain(ast *pAst, Expression &expression, unsigned int startpos) {

    ast* utype;
    Expression rightExpr(expression);
    for(unsigned int i = startpos; i < pAst->getsubastcount(); i++) {
            utype = pAst->getsubast(i);

            if(rightExpr.utype.array && utype->gettype() != ast_expression) {
                errors->newerror(GENERIC, utype->line, utype->col, "expected array index `[]`");
            }

            if(utype->gettype() == ast_dotnotation_call_expr) {
                rightExpr = parseDotNotationChain(utype, rightExpr, 0);
            }
            else if(utype->gettype() == ast_expression){ /* array expression */
                rightExpr = parseArrayExpression(rightExpr, utype);
            }
            else {
                rightExpr = parseDotNotationCallContext(rightExpr, utype);
            }

            rightExpr.lnk = utype;
            if(rightExpr.type == expression_unresolved || rightExpr.type == expression_unknown)
                break;

            expression.code.inject(expression.code.size(), rightExpr.code);
            expression.type = rightExpr.type;
            expression.utype = rightExpr.utype;
            expression.utype.array = rightExpr.utype.array;
    }
    return expression;
}

Expression runtime::parseIntermExpression(ast* pAst) {
    Expression expression;

    switch(pAst->gettype()) {
        case ast_primary_expr:
            return parsePrimaryExpression(pAst);
        case ast_self_e:
            return parseSelfExpression(pAst);
        case ast_base_e:
            return parseBaseExpression(pAst);
        case ast_null_e:
            return parseNullExpression(pAst);
        case ast_new_e:
            return parseNewExpression(pAst);
        case ast_post_inc_e:
            return parsePostInc(pAst);
        case ast_arry_e:
            return parseArrayExpression(pAst);
        case ast_cast_e:
            return parseCastExpression(pAst);
        case ast_pre_inc_e:
            return parsePreInc(pAst);
        case ast_paren_e:
            return parseParenExpression(pAst);
        case ast_not_e:
            return parseNotExpression(pAst);
        case ast_vect_e:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unexpected vector array expression. Did you mean 'new type { <data>, .. }'?");
            expression.lnk = pAst;
            return expression;
        case ast_add_e:
        case ast_mult_e:
        case ast_shift_e:
        case ast_less_e:
        case ast_equal_e:
        case ast_and_e:
            return parseBinaryExpression(pAst);
        case ast_ques_e:
            return parseQuesExpression(pAst);
        default:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unexpected expression format");
            return Expression(pAst); // not an expression!
    }
}

void runtime::parseNativeCast(Expression& utype, Expression& arg, Expression& out) {
    Scope* scope = current_scope();
    if(arg.type != expression_unresolved) {
        if(arg.utype.array != utype.utype.array) {
            errors->newerror(INCOMPATIBLE_TYPES, utype.lnk->line, utype.lnk->col, "; cannot cast `" + arg.typeToString() + "` to `" + utype.typeToString() + "`");
            out.type = expression_unresolved;
            return;
        }
    }

    if(arg.utype.type != ResolvedReference::FIELD && utype.utype.nf == arg.utype.nf)
        warning(GENERIC, utype.lnk->line, utype.lnk->col, "redundant cast of type `" + arg.typeToString() + "` to `" + utype.typeToString() + "`");
    else if(arg.utype.type == ResolvedReference::FIELD && utype.utype.nf == arg.utype.field->nf)
        warning(GENERIC, utype.lnk->line, utype.lnk->col, "redundant cast of type `" + arg.typeToString() + "` to `" + utype.typeToString() + "`");

    out.type = expression_var;
    out.utype = utype.utype;
    switch(utype.utype.nf) {
        case fi8:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOV8, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOV8, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fi16:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOV16, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOV16, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fi32:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOV32, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOV32, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fi64:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOV8, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOV8, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fui8:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOVU8, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOV8, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fui16:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOVU16, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOVU16, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fui32:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOVU32, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOVU32, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fui64:
            if(arg.type == expression_var) {
                out.code.push_i64(SET_Ci(i64, op_MOVU64, ebx,0, ebx));
                return;
            } else if(arg.type == expression_field) {
                pushExpressionToRegisterNoInject(arg, out, ebx);
                out.code.push_i64(SET_Ci(i64, op_MOVU64, ebx, 0, ebx));
                out.code.push_i64(SET_Ci(i64, op_MOVR, adx, 0, fp));
                out.code.push_i64(SET_Ci(i64, op_SMOVR, ebx, 0, field_offset(scope, arg.utype.field->vaddr)));
                return;
            }
            break;
        case fvar:
            if(arg.type == expression_var || arg.type==expression_field) {
                return;
            } // TODO: do we even have to do anything here?
            break;
        case fvoid:
            errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "type `void` cannot be used as a cast");
            return;
        case fdynamic:
            out.type = expression_lclass;
            if(arg.type == expression_lclass) {
                return;
            }
            break;
    }

    if(arg.type != expression_unresolved)
        errors->newerror(INCOMPATIBLE_TYPES, utype.lnk->line, utype.lnk->col, "; cannot cast `" + arg.utype.typeToString() + "` to `" + utype.utype.typeToString() + "`");
    return;
}

void runtime::parseClassCast(Expression& utype, Expression& arg, Expression& out) {
    if(arg.type != expression_unresolved) {
        if(arg.utype.array != utype.utype.array) {
            errors->newerror(INCOMPATIBLE_TYPES, utype.lnk->line, utype.lnk->col, "; cannot cast `" + arg.utype.typeToString() + "` to `" + utype.utype.typeToString() + "`");
            out.type = expression_unresolved;
            return;
        }
    }

    switch(arg.type) {
        case expression_lclass:
            if(utype.utype.klass->hasBaseClass(arg.utype.klass)) {
                out.type = utype.type;
                out.utype = utype.utype;
                return;
            }
            break;
        case expression_dynamicclass:
            // TODO: put runtime code to evaluate at runtime
            out.code.push_i64(SET_Di(i64, op_CHECK_CAST, utype.utype.klass->vaddr));
            out.type = utype.type;
            out.utype = utype.utype;
            return;
        case expression_field:
            if(arg.utype.field->type == field_class) {
                if(utype.utype.klass->match(arg.utype.klass)) {
                    out.type = expression_lclass;
                    out.utype = utype.utype;
                    return;
                }
            } else if(arg.utype.field->type == field_native && arg.utype.field->nf == fdynamic) {
                out.code.push_i64(SET_Di(i64, op_CHECK_CAST, utype.utype.klass->vaddr));
                out.type = expression_lclass;
                out.utype = utype.utype;
                return;
            } else if(arg.utype.field->type == field_native) {
                errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "field `" + arg.utype.field->name + "` is not a class; "
                                                                          "cannot cast `" + arg.utype.typeToString() + "` to `" + utype.utype.typeToString() + "`");
                return;
            } else {
                // field is unresolved
                out.type = expression_unresolved;
                out.utype = utype.utype;
                return;
            }
            break;
    }

    if(arg.type != expression_unresolved)
        errors->newerror(INCOMPATIBLE_TYPES, utype.lnk->line, utype.lnk->col, "; cannot cast `" + arg.utype.typeToString() + "` to `" + utype.utype.typeToString() + "`");
    return;
}

Expression runtime::parseCastExpression(ast* pAst) {
    Expression expression, utype, arg;

    utype = parseUtype(pAst->getsubast(ast_utype));
    arg = parseExpression(pAst->getsubast(ast_expression));
    expression.code.inject(expression.code.size(), arg.code);

    if(utype.type != expression_unknown &&
            utype.type != expression_unresolved) {
        switch(utype.type) {
            case expression_native:
                parseNativeCast(utype, arg, expression);
                break;
            case expression_class:
                parseClassCast(utype, arg, expression);
                break;
            case expression_field:
                errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "cast expression of type `field` not allowed, must be of type `class`");
                break;
            default:
                errors->newerror(GENERIC, utype.lnk->line, utype.lnk->col, "cast expression of type `" + utype.typeToString() + "` not allowed, must be of type `class`");
                break;
        }
    }

    return expression;
}

void runtime::preIncClass(Expression& out, token_entity op, ClassObject* klass) {
    OperatorOverload* overload;
    List<Param> emptyParams;

    if(op.gettokentype() == _INC) {
        overload = klass->getPreIncOverload();
    } else {
        overload = klass->getPreDecOverload();
    }

    if(overload != NULL) {
        // add code to call overload
        if(overload->type != lvoid)
            out.code.push_i64(SET_Di(i64, op_INC, sp));

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        out.code.push_i64(SET_Di(i64, op_MOVI, 0), ebx);
        out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

        if(!overload->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        out.code.push_i64(SET_Di(i64, op_CALL, overload->vaddr));

        out.type = methodReturntypeToExpressionType(overload);
        if(out.type == expression_lclass) {
            out.utype.klass = overload->klass;
            out.utype.type = ResolvedReference::CLASS;
        }
    } else if(klass->hasOverload(string_toop(op.gettoken()))) {
        errors->newerror(GENERIC, op.getline(), op.getcolumn(), "use of class `" + klass->getName() + "`; missing overload params for operator `"
                                                                + klass->getFullName() + ".operator" + op.gettoken() + "`");
        out.type = expression_var;
    } else {
        errors->newerror(GENERIC, op.getline(), op.getcolumn(), "use of class `" + klass->getName() + "` must return an int to use `" + op.gettoken() + "` operator");
        out.type = expression_var;
    }
}

Expression runtime::parsePreInc(ast* pAst) {
    Expression interm;
    token_entity entity = pAst->hasentity(_INC) ? pAst->getentity(_INC) : pAst->getentity(_DEC);

    interm = parseExpression(pAst->getsubast(0));
    Expression expression(interm);

    if(expression.utype.array){
        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "expression must evaluate to an int to use `" + entity.gettoken() + "` operator");
    } else {
        switch(expression.type) {
            case expression_var:
                if(interm.func) {
                    pushExpressionToRegister(interm, expression, ebx);
                    if(entity.gettokentype() == _INC)
                        expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                    else
                        expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                } else {
                    if(entity.gettokentype() == _INC)
                        expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                    else
                        expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                }
                break;
            case expression_field:
                if(interm.utype.field->type == field_class) {
                    if(expression.utype.field->local)
                        expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(current_scope(), expression.utype.field->vaddr)));

                    preIncClass(expression, entity, expression.utype.field->klass);
                    return expression;
                } else if(interm.utype.field->type == field_native) {
                    if (interm.utype.field->nf == fdynamic) {
                        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "use of `" + entity.gettoken() +
                                                                                        "` operator on field of type `dynamic_object` without a cast. Try ((SomeClass)dynamic_class)++");
                    } else if (interm.utype.field->nativeInt()) {
                        // increment the field
                        expression.code.push_i64(SET_Di(i64, op_MOVI, 1), ecx);

                        if(entity.gettokentype() == _INC)
                            expression.code.push_i64(
                                    SET_Ci(i64, op_ADDL, ecx, 0, field_offset(current_scope(), interm.utype.field->vaddr)));
                        else
                            expression.code.push_i64(
                                    SET_Ci(i64, op_SUBL, ecx, 0, field_offset(current_scope(), interm.utype.field->vaddr)));

                        if(entity.gettokentype() == _INC)
                            expression.code.push_i64(SET_Di(i64, op_INC, ebx));
                        else
                            expression.code.push_i64(SET_Di(i64, op_DEC, ebx));
                    } else {
                        errors->newerror(GENERIC, entity.getline(), entity.getcolumn(),
                                         "expression must evaluate to an int to use `" + entity.gettoken() +
                                         "` operator");
                    }

                } else {
                    // do nothing field is unresolved
                }
                break;
            case expression_lclass:
                if(interm.func)
                    expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
                preIncClass(expression, entity, expression.utype.klass);
                return expression;
                break;
            case expression_dynamicclass:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "use of `" + entity.gettoken() + "` operator on type `dynamic_object` without a cast. Try ((SomeClass)dynamic_class)++");
                break;
            case expression_null:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "value `null` cannot be used as var");
                break;
            case expression_native:
                errors->newerror(UNEXPECTED_SYMBOL, entity.getline(), entity.getcolumn(), " `" + nativefield_tostr(expression.utype.nf) + "`");
                break;
            case expression_string:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "increment on immutable string");
                break;
            case expression_unresolved:
                // do nothing
                break;
            default:
                errors->newerror(GENERIC, entity.getline(), entity.getcolumn(), "expression must evaluate to an int to use `" + entity.gettoken() + "` operator");
                break;
        }
    }

    expression.type = expression_var;
    return expression;
}

Expression runtime::parseParenExpression(ast* pAst) {
    Expression expression;

    expression = parseExpression(pAst->getsubast(ast_expression));

    if(pAst->hassubast(ast_dotnotation_call_expr)) {
        return parseDotNotationChain(pAst->getsubast(ast_dotnotation_call_expr), expression, 0);
    }

    return expression;
}

void runtime::notClass(Expression& out, ClassObject* klass, ast* pAst) {
    List<Param> emptyParams;
    Method* overload;
    if((overload = klass->getOverload(oper_NOT, emptyParams)) != NULL) {
        // call overload operator
        if(overload->type != lvoid)
            out.code.push_i64(SET_Di(i64, op_INC, sp));

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        if(!overload->isStatic())
            out.code.push_i64(SET_Ei(i64, op_PUSHREF));

        out.code.push_i64(SET_Di(i64, op_CALL, overload->vaddr));

        out.type = methodReturntypeToExpressionType(overload);
        if(out.type == expression_lclass) {
            out.utype.klass = overload->klass;
            out.utype.type = ResolvedReference::CLASS;
        }
    } else if(klass->hasOverload(oper_NOT)) {
        errors->newerror(GENERIC, pAst->line, pAst->col, "use of unary operator '!' on class `" + klass->getName() + "`; missing overload params for operator `"
                                                                        + klass->getFullName() + ".operator!`");
    } else {
        // error cannot apply not to expression of type class
        errors->newerror(GENERIC, pAst->line, pAst->col, "unary operator '!' cannot be applied to expression of type `" + klass->getFullName() + "`");
    }
}

Expression runtime::parseNotExpression(ast* pAst) {
    Expression expression;

    expression = parseExpression(pAst->getsubast(ast_expression));
    switch(expression.type) {
        case expression_var:
            // negate value
            if(expression.func)
                pushExpressionToRegisterNoInject(expression, expression, ebx);

            expression.code.push_i64(SET_Ci(i64, op_NOT, ebx,0, ebx));
            break;
        case expression_lclass:
            // check for !operator
            if(expression.func)
                expression.code.push_i64(SET_Di(i64, op_MOVSL, 0));
            notClass(expression, expression.utype.klass, pAst);
            break;
        case expression_unresolved:
            // do nothing
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + expression.utype.toString() + "`");
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + expression.utype.typeToString() + "`");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unary operator '!' cannot be applied to dynamic_object, did you forget to add a cast?  i.e !((SomeClass)dynamic_class)");
            break;
        case expression_field:
            expression.type = expression_var;
            if(expression.utype.field->type == field_native) {
                if(expression.utype.field->dynamicObject()) {
                    errors->newerror(GENERIC, pAst->line, pAst->col, "use of `!` operator on field of type `dynamic_object` without a cast. Try !((SomeClass)dynamic_class)");
                } else if(expression.utype.field->nativeInt()) {
                    pushExpressionToRegisterNoInject(expression, expression, ebx);
                    expression.code.push_i64(SET_Ci(i64, op_NOT, ebx,0, ebx));
                }
            } else if(expression.utype.field->type == field_class) {
                if(expression.utype.field->local)
                    expression.code.push_i64(SET_Di(i64, op_MOVL, field_offset(current_scope(), expression.utype.field->vaddr)));
                notClass(expression, expression.utype.field->klass, pAst);
            } else {
                errors->newerror(GENERIC, pAst->line, pAst->col, "field must evaluate to an int to use `!` operator");
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unary operator '!' cannot be applied to null");
            break;
        case expression_string:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unary operator '!' cannot be applied to immutable string");
            break;
        default:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unary operator '!' cannot be applied to given expression");
            break;

    }

    return expression;
}

void runtime::addClass(token_entity operand, ClassObject* klass, Expression& out, Expression& left, Expression &right, ast* pAst) {
    List<Param> params;
    List<Expression> eList;
    eList.push_back(right);
    OperatorOverload* overload;
    right.literal = false;

    expressionListToParams(params, eList);

    if((overload = klass->getOverload(string_toop(operand.gettoken()), params)) != NULL) {
        // call operand
        if(overload->type != lvoid)
            out.code.push_i64(SET_Di(i64, op_INC, sp));

        out.code.push_i64(SET_Ei(i64, op_INIT_FRAME));
        pushExpressionToStack(right, out);

        if(!overload->isStatic())
            pushExpressionToStack(left, out);

        out.type = methodReturntypeToExpressionType(overload);
        right.free();
        right.type=out.type;
        right.func=true;
        if(out.type == expression_lclass) {
            out.utype.klass = overload->klass;
            out.utype.type = ResolvedReference::CLASS;
            right.utype.klass = overload->klass;
            right.utype.type = ResolvedReference::CLASS;
        }

        out.code.push_i64(SET_Di(i64, op_CALL, overload->vaddr));
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `"
                                                          + left.typeToString() + "` and `" + right.typeToString() + "`");
    }

    __freeList(params);
    __freeList(eList);
}

OPCODE runtime::operandToLocalMathOp(token_entity operand)
{
    if(operand == "+")
        return op_ADDL;
    else if(operand == "-")
        return op_SUBL;
    else if(operand == "*")
        return op_MULL;
    else if(operand == "/")
        return op_DIVL;
    else if(operand == "%")
        return op_MODL;
    else
        return op_NOP;
}

void runtime::addNative(token_entity operand, NativeField nf, Expression& out, Expression& left, Expression& right, ast* pAst) {
    out.type = expression_var;
    right.type = expression_var;
    right.func=false;
    right.literal = false;
    Expression expr;

    if(left.type == expression_var) {
        equals(left, right);

        pushExpressionToRegister(right, out, egx);
        pushExpressionToRegister(left, out, ebx);
        out.code.push_i64(SET_Ci(i64, operandToOp(operand), ebx,0, egx), ebx);
        right.code.free();
    } else if(left.type == expression_field) {
        if(left.utype.field->nativeInt()) {
            equals(left, right);

            pushExpressionToRegister(right, out, egx); // no inject?
            pushExpressionToRegister(left, out, ebx);
            out.code.push_i64(SET_Ci(i64, operandToOp(operand), ebx,0, egx), ebx);
            right.code.free();
        }
        else {
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
        }
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
    }
}

Expression runtime::parseUnary(token_entity operand, Expression& right, ast* pAst) {
    Expression expression;

    switch(right.type) {
        case expression_var:
            if(c_options.optimize && right.literal) {
                double var = 0;

                if(operand == "+")
                    var = +right.intValue;
                else
                    var = -right.intValue;

                if((((int64_t)abs(right.intValue)) - abs(right.intValue)) > 0) {
                    // movbi
                    if((int64_t )var > DA_MAX || (int64_t )var < DA_MIN) {
                        stringstream ss;
                        ss << "integral number too large: " << var;
                        errors->newerror(GENERIC, operand.getline(), operand.getcolumn(), ss.str());
                    }

                    expression.code.push_i64(SET_Di(i64, op_MOVBI, ((int64_t)var)), abs(get_low_bytes(var)));
                } else {
                    // movi
                    if(var > DA_MAX || var < DA_MIN) {
                        stringstream ss;
                        ss << "integral number too large: " << var;
                        errors->newerror(GENERIC, operand.getline(), operand.getcolumn(), ss.str());
                    }
                    expression.code.push_i64(SET_Di(i64, op_MOVI, var), ebx);
                }
            } else {
                expression.code.inject(expression.code.size(), right.code);

                if(operand == "+")
                    expression.code.push_i64(SET_Ci(i64, op_IMUL, ebx,0, 1));
                else
                    expression.code.push_i64(SET_Ci(i64, op_IMUL, ebx,0, -1));
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Unary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + right.typeToString() + "`");
            break;
        case expression_field:
            if(right.utype.field->type == field_native) {
                // add var
                if(right.utype.field->nativeInt()) {
                    expression.code.inject(expression.code.size(), right.code);

                    if(operand == "+") {
                        expression.code.push_i64(SET_Ci(i64, op_IMUL, ebx,0, 1));
                    }
                    else {
                        expression.code.push_i64(SET_Ci(i64, op_IMUL, ebx,0, -1));
                    }
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Unary operator `" + operand.gettoken() +
                                                                      "` cannot be applied to expression of type `" + right.typeToString() + "`");
                }
            } else if(right.utype.field->type == field_class) {
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Expression of type `" + right.typeToString() + "` is non numeric");
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + right.typeToString() + "`");
            break;
        case expression_lclass:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Expression of type `" + right.typeToString() + "` is non numeric");
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + right.typeToString() + "`");
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Unary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + right.typeToString() + "`");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Unary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + right.typeToString() + "`");
            break;
        case expression_string:
            // TODO: construct new string(<string>) and use that to concatonate strings
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Expression of type `" + right.typeToString() + "` is non numeric");
            break;
        default:
            break;
    }

    expression.type = expression_var;
    expression.lnk = pAst;
    return expression;
}

OPCODE runtime::operandToOp(token_entity operand)
{
    if(operand == "+")
        return op_ADD;
    else if(operand == "-")
        return op_SUB;
    else if(operand == "*")
        return op_MUL;
    else if(operand == "/")
        return op_DIV;
    else if(operand == "%")
        return op_MOD;
    else
        return op_MOD;
}

OPCODE runtime::operandToShftOp(token_entity operand)
{
    if(operand == "<<")
        return op_SHL;
    else
        return op_SHR;
}

OPCODE runtime::operandToLessOp(token_entity operand)
{
    if(operand == "<")
        return op_LT;
    else if(operand == "<=")
        return op_LTE;
    else if(operand == ">")
        return op_GT;
    else if(operand == ">=")
        return op_GTE;
    else
        return op_LT;
}

OPCODE runtime::operandToCompareOp(token_entity operand)
{
    if(operand == "!=")
        return op_TNE;
    else if(operand == "==")
        return op_TEST;
    else
        return op_TEST;
}

bool runtime::addExpressions(Expression &out, Expression &leftExpr, Expression &rightExpr, token_entity operand, double* varout) {
    if(operand == "+")
        *varout = leftExpr.intValue + rightExpr.intValue;
    else if(operand == "-")
        *varout = leftExpr.intValue - rightExpr.intValue;
    else if(operand == "*")
        *varout = leftExpr.intValue * rightExpr.intValue;
    else if(operand == "/")
        *varout = leftExpr.intValue / rightExpr.intValue;
    else if(operand == "%")
        *varout = (int64_t)leftExpr.intValue % (int64_t)rightExpr.intValue;

    if(isDClassNumberEncodable(*varout)) {
        return false;
    } else {
        if(out.code.size() >= 2 && (GET_OP(out.code.__asm64.get(out.code.size()-2)) == op_MOVI
                                    || GET_OP(out.code.__asm64.get(out.code.size()-2)) == op_MOVBI)){
            out.code.__asm64.pop_back();
            out.code.__asm64.pop_back();
        }

        if((((int64_t)abs(*varout)) - abs(*varout)) > 0) {
            // movbi
            out.code.push_i64(SET_Di(i64, op_MOVBI, ((int64_t)*varout)), abs(get_low_bytes(*varout)));
        } else {
            // movi
            out.code.push_i64(SET_Di(i64, op_MOVI, *varout), ebx);
        }

        rightExpr.literal = true;
        rightExpr.intValue = *varout;
        rightExpr.code.free();
    }

    out.literal = true;
    out.intValue = *varout;
    return true;
}

bool runtime::shiftLiteralExpressions(Expression &out, Expression &leftExpr, Expression &rightExpr,
                                      token_entity operand) {
    double var=0;
    if(operand == "<<")
        var = (int64_t)leftExpr.intValue << (int64_t)rightExpr.intValue;
    else if(operand == ">>")
        var = (int64_t)leftExpr.intValue >> (int64_t)rightExpr.intValue;

    if(isDClassNumberEncodable(var)) {
        return false;
    } else {
        if((((int64_t)abs(var)) - abs(var)) > 0) {
            // movbi
            out.code.push_i64(SET_Di(i64, op_MOVBI, ((int64_t)var)), abs(get_low_bytes(var)));
        } else {
            // movi
            out.code.push_i64(SET_Di(i64, op_MOVI, var), ebx);
        }

        rightExpr.literal = true;
        rightExpr.intValue = var;
        rightExpr.code.free();
    }

    out.literal = true;
    out.intValue = var;
    return true;
}


void runtime::lessThanLiteralExpressions(Expression &out, Expression &leftExpr, Expression &rightExpr,
                                      token_entity operand) {
    double var=0;
    if(operand == "<")
        var = leftExpr.intValue < rightExpr.intValue;
    else if(operand == ">")
        var = leftExpr.intValue > rightExpr.intValue;
    else if(operand == ">=")
        var = leftExpr.intValue >= rightExpr.intValue;
    else if(operand == "<=")
        var = leftExpr.intValue <= rightExpr.intValue;

    out.code.push_i64(SET_Di(i64, op_MOVI, var), ebx);

    rightExpr.literal = true;
    rightExpr.intValue = var;
    rightExpr.code.free();

    out.literal = true;
    out.intValue = var;
}

bool runtime::isMathOp(token_entity entity)
{
    return entity == "+" || entity == "-"
            || entity == "*" || entity == "/"
            || entity == "%";
}

void runtime::parseAddExpressionChain(Expression &out, ast *pAst) {
    Expression leftExpr, rightExpr, peekExpr;
    int operandPtr = 0;
    double value=0;
    bool firstEval=true;
    token_entity operand;
    List<token_entity> operands;
    operands.add(pAst->getentity(0));
    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        if(pAst->getsubast(i)->getentitycount() > 0 && isMathOp(pAst->getsubast(i)->getentity(0)))
            operands.add(pAst->getsubast(i)->getentity(0));
    }

    operandPtr=operands.size()-1;
    for(long int i = pAst->getsubastcount()-1; i >= 0; i--) {
        if(firstEval) {
            rightExpr = pAst->getsubast(i)->gettype() == ast_expression ? parseExpression(pAst->getsubast(i))
                                                                        : parseIntermExpression(pAst->getsubast(i));
            i--;
            firstEval= false;
        }
        leftExpr = pAst->getsubast(i)->gettype() == ast_expression ? parseExpression(pAst->getsubast(i)) : parseIntermExpression(pAst->getsubast(i));
        operand = operands.get(operandPtr--);
        double var = 0;

        switch(leftExpr.type) {
            case expression_var:
                if(rightExpr.type == expression_var) {
                    if(leftExpr.literal && rightExpr.literal) {

                        if(!addExpressions(out, leftExpr, rightExpr, operand, &var)) {
                            goto calculate;
                        }
                    } else {
                        calculate:
                        // is left leftexpr a literal?
                        pushExpressionToStack(rightExpr, out);
                        pushExpressionToRegister(leftExpr, out, ebx);
                        out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                        out.code.push_i64(SET_Ci(i64, operandToOp(operand), ebx,0, ecx), ebx);

                        rightExpr.func=false;
                        rightExpr.literal = false;
                        rightExpr.code.free();
                        var=0;
                    }

                    out.type = expression_var;
                }else if(rightExpr.type == expression_field) {
                    if(rightExpr.utype.field->type == field_native) {
                        // add var
                        leftEval:
                        if(leftExpr.literal && (i-1) >= 0) {
                            peekExpr = pAst->getsubast(i-1)->gettype() == ast_expression ?
                                       parseExpression(pAst->getsubast(i-1)) : parseIntermExpression(pAst->getsubast(i-1));
                            Expression outtmp;

                            if(peekExpr.literal) {
                                if(addExpressions(outtmp, peekExpr, leftExpr, operand, &var)) {
                                    i--;
                                    leftExpr=outtmp;
                                    leftExpr.type=expression_var;
                                    goto leftEval;
                                }
                            }
                        }

                        addNative(operand, rightExpr.utype.field->nf, out, leftExpr, rightExpr, pAst);
                    } else if(rightExpr.utype.field->type == field_class) {
                        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                          "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                    } else {
                        // do nothing field unresolved
                    }
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                      "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                }
                break;
            case expression_null:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                break;
            case expression_field:
                if(leftExpr.utype.field->type == field_native) {
                    // add var
                    addNative(operand, leftExpr.utype.field->nf, out, leftExpr, rightExpr, pAst);
                } else if(leftExpr.utype.field->type == field_class) {
                    addClass(operand, leftExpr.utype.field->klass, out, leftExpr, rightExpr, pAst);
                } else {
                    // do nothing field unresolved
                }
                break;
            case expression_native:
                errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + leftExpr.typeToString() + "`");
                break;
            case expression_lclass:
                addClass(operand, out.utype.klass, out, out, rightExpr, pAst);
                break;
            case expression_class:
                errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + leftExpr.typeToString() + "`");
                break;
            case expression_void:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                break;
            case expression_dynamicclass:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`. Did you forget to apply a cast? "
                                                                          "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
                break;
            case expression_string:
                // TODO: construct new string(<string>) and use that to concatonate strings
                break;
            default:
                break;
        }
    }

    /*
     * So we dont missinterpret the value returned from the expr :)
     */
    out.func=rightExpr.func;
    out.literal=rightExpr.literal=false;
}

Expression runtime::parseAddExpression(ast* pAst) {
    Expression expression, right;
    token_entity operand = pAst->hasentity(PLUS) ? pAst->getentity(PLUS) : pAst->getentity(MINUS);

    if(operand.gettokentype() == PLUS && pAst->getsubastcount() == 1) {
        // left is right then add 1 to data
        right = parseExpression(pAst->getsubast(0));
        return parseUnary(operand, right, pAst);
    } else if(operand.gettokentype() == MINUS && pAst->getsubastcount() == 1) {
        // left is right multiply expression by -1
        right = parseExpression(pAst->getsubast(0));
        return parseUnary(operand, right, pAst);
    }

    parseAddExpressionChain(expression, pAst);

    expression.lnk = pAst;
    return expression;
}

bool runtime::isDClassNumberEncodable(double var) { return (int64_t )var > DA_MAX || (int64_t )var < DA_MIN; }

Expression runtime::parseMultExpression(ast* pAst) {
    Expression expression, left, right;
    token_entity operand = pAst->getentity(0);

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary *<expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    parseAddExpressionChain(expression, pAst);

    expression.lnk = pAst;
    return expression;
}

void runtime::shiftNative(token_entity operand, Expression& out, Expression &left, Expression &right, ast* pAst) {
    out.type = expression_var;
    right.type = expression_var;
    right.func=false;
    right.literal = false;

    if(left.type == expression_var) {
        equals(left, right);

        pushExpressionToRegister(right, out, egx);
        pushExpressionToRegister(left, out, ebx);
        out.code.push_i64(SET_Ci(i64, operandToShftOp(operand), ebx,0, egx), ebx);
        right.code.free();
    } else if(left.type == expression_field) {
        if(left.utype.field->nativeInt()) {
            equals(left, right);

            pushExpressionToRegister(right, out, egx); // no inject?
            pushExpressionToRegister(left, out, ebx);
            out.code.push_i64(SET_Ci(i64, operandToShftOp(operand), ebx,0, egx), ebx);
            right.code.free();
        }
        else {
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
        }
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
    }
}


void runtime::lessThanNative(token_entity operand, Expression& out, Expression &left, Expression &right, ast* pAst) {
    out.type = expression_var;
    right.type = expression_var;
    right.func=false;
    right.literal = false;

    if(left.type == expression_var) {
        equals(left, right);

        pushExpressionToRegister(right, out, egx);
        pushExpressionToRegister(left, out, ebx);
        out.code.push_i64(SET_Ci(i64, operandToLessOp(operand), ebx,0, egx));
        out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
        right.code.free();
    } else if(left.type == expression_field) {
        if(left.utype.field->nativeInt()) {
            equals(left, right);

            pushExpressionToRegister(right, out, egx);
            pushExpressionToRegister(left, out, ebx);
            out.code.push_i64(SET_Ci(i64, operandToLessOp(operand), ebx,0, egx));
            out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
            right.code.free();
        }
        else {
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
        }
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
    }
}

void runtime::assignNative(token_entity operand, Expression& out, Expression &left, Expression &right, ast* pAst) {
    out.type = expression_var;

    if(left.type == expression_var) {
        equals(left, right);

        pushExpressionToRegister(right, out, egx);
        pushExpressionToRegister(left, out, ebx);
        out.code.push_i64(SET_Ci(i64, operandToCompareOp(operand), ebx,0, egx));
        out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
        right.code.free();
    } else if(left.type == expression_field) {
        if(left.utype.field->nativeInt()) {
            equals(left, right);

            pushExpressionToRegister(right, out, egx);
            pushExpressionToRegister(left, out, ebx);
            out.code.push_i64(SET_Ci(i64, operandToCompareOp(operand), ebx,0, egx));
            out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
            right.code.free();
        }
        else {
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
        }
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
    }
}

void runtime::assignValue(token_entity operand, Expression& out, Expression &left, Expression &right, ast* pAst) {
    out.type=expression_var;
    out.literal=false;

    /*
     * We know when this is called the operand is going to be =
     */
    if((left.type == expression_var && !left.arrayElement) || left.literal) {
        errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable, expression must be of lvalue");
    } else if(left.type == expression_field) {
        if(left.utype.field->isObjectInMemory()) {
            memassign:
            if(left.utype.field->array && operand != "=") {
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken()
                                                                  + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            }

            if(equalsNoErr(left, right)) {
                if(left.utype.field->nativeInt()) {}
                else if(left.utype.field->dynamicObject()) {
                    out.type=expression_dynamicclass;
                }else if(left.utype.field->type == field_class) {
                    out.type=expression_lclass;
                    out.utype.klass=left.utype.field->klass;
                }

                if(c_options.optimize && right.utype.type == ResolvedReference::FIELD
                        && right.utype.field->local) {
                    out.inject(left);
                    out.code.push_i64(SET_Di(i64, op_MUTL, field_offset(current_scope(), right.utype.field->vaddr)));
                } else {
                    pushExpressionToStack(right, out);
                    out.inject(left);
                    out.code.push_i64(SET_Ei(i64, op_POPREF));
                }
            } else {
                if(left.type == expression_lclass) {
                    addClass(operand, left.utype.klass, out, left, right, pAst);
                } else if(left.utype.type == ResolvedReference::FIELD && left.utype.field->type == field_class) {
                    addClass(operand, left.utype.field->klass, out, left, right, pAst);
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken()
                                                                      + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
                }
            }
        } else {
            // this will b easy...
            if(left.utype.field->local) {
                pushExpressionToRegister(right, out, ecx);

                if(operand == "=") {
                    out.code.push_i64(SET_Ci(i64, op_MOVR, adx,0, fp));
                    out.code.push_i64(SET_Ci(i64, op_SMOVR, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "+=") {
                    out.code.push_i64(SET_Ci(i64, op_ADDL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "-=") {
                    out.code.push_i64(SET_Ci(i64, op_SUBL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "*=") {
                    out.code.push_i64(SET_Ci(i64, op_MULL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "/=") {
                    out.code.push_i64(SET_Ci(i64, op_DIVL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "%=") {
                    out.code.push_i64(SET_Ci(i64, op_MODL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "&=") {
                    out.code.push_i64(SET_Ci(i64, op_ANDL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "|=") {
                    out.code.push_i64(SET_Ci(i64, op_ORL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                } else if(operand == "^=") {
                    out.code.push_i64(SET_Ci(i64, op_NOTL, ecx,0, field_offset(current_scope(), left.utype.field->vaddr)));
                }

            } else {
                pushExpressionToStack(right, out);
                if(operand == "=")
                    out.inject(left);
                else
                    pushExpressionToRegister(left, out, egx);


                out.code.push_i64(SET_Di(i64, op_MOVI, 0), adx);
                out.code.push_i64(SET_Di(i64, op_POPR, ecx));

                if(operand == "=") {
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                } else if(operand == "+=") {
                    out.code.push_i64(SET_Ci(i64, op_ADD, egx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                } else if(operand == "-=") {
                    out.code.push_i64(SET_Ci(i64, op_SUB, egx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                } else if(operand == "*=") {
                    out.code.push_i64(SET_Ci(i64, op_MUL, egx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                } else if(operand == "/=") {
                    out.code.push_i64(SET_Ci(i64, op_DIV, egx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                } else if(operand == "%=") {
                    out.code.push_i64(SET_Ci(i64, op_MOD, egx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_RMOV, adx,0, ecx));
                }
            }
        }
    } else if(left.type == expression_lclass) {
        goto memassign;
    } else if(left.type == expression_var) {
        // this must be an array element
        if(left.arrayElement) {
            Expression e;
            for(unsigned int i = 0; i < left.code.size(); i++)
            {
                if(GET_OP(left.code.__asm64.get(i)) == op_MOVX) {
                    left.code.__asm64.remove(i);
                    pushExpressionToRegister(right, e, ebx);

                    left.code.__asm64.insert(i++, SET_Di(i64, op_PUSHR, ebx));
                                                                                                            left.code.inject(i, e.code); // add the code to get the value to set
                    i+=e.code.size();

                    left.code.__asm64.insert(i++, SET_Di(i64, op_POPR, egx));
                    left.code.__asm64.insert(i, SET_Ci(i64, op_RMOV, egx,0, ebx));
                    break;
                }
            }
            out.inject(left);
        }
    } else {
        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken()
                                                          + "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
    }

}

Expression runtime::parseShiftExpression(ast* pAst) {
    Expression out, left, right;
    token_entity operand = pAst->getentity(0);

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary << <expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    left = parseIntermExpression(pAst->getsubast(0));
    right = parseExpression(pAst->getsubast(1));

    out.type = expression_var;
    switch(left.type) {
        case expression_var:
            if(right.type == expression_var) {
                if(left.literal && right.literal) {

                    if(!shiftLiteralExpressions(out, left, right, operand)) {
                        goto calculate;
                    }
                } else {
                    calculate:
                    // is left left expr a literal?
                    pushExpressionToStack(right, out);
                    pushExpressionToRegister(left, out, ebx);
                    out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                    out.code.push_i64(SET_Ci(i64, operandToShftOp(operand), ebx,0, ecx), ebx);
                }
            }
            else if(right.type == expression_field) {
                if(right.utype.field->type == field_native) {
                    // add var
                    shiftNative(operand, out, left, right, pAst);
                    //addNative(operand, right.utype.field->nf, expression, left, right, pAst);
                } else if(right.utype.field->type == field_class) {
                    addClass(operand, right.utype.field->klass, out, left, right, pAst);
                } else {
                    // do nothing field unresolved
                }
            } else if(right.type == expression_lclass) {
                addClass(operand, left.utype.klass, out, left, right, pAst);
            } else {
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                shiftNative(operand, out, left, right, pAst);
            } else if(left.utype.field->type == field_class) {
                addClass(operand, left.utype.field->klass, out, left, right, pAst);
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_lclass:
            addClass(operand, left.utype.klass, out, left, right, pAst);
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`. Did you forget to apply a cast? "
                                                                      "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
            break;
        case expression_string:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        default:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
    }

    out.lnk = pAst;
    return out;
}

Expression runtime::parseLessExpression(ast* pAst) {
    Expression out, left, right;
    token_entity operand = pAst->getentity(0);

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary < <expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    left = parseIntermExpression(pAst->getsubast(0));
    right = parseExpression(pAst->getsubast(1));

    out.type = expression_var;
    switch(left.type) {
        case expression_var:
            if(right.type == expression_var) {
                if(left.literal && right.literal) {
                    lessThanLiteralExpressions(out, left, right, operand);
                } else {
                    calculate:
                    // is left left expr a literal?
                    pushExpressionToStack(right, out);
                    pushExpressionToRegister(left, out, ebx);
                    out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                    out.code.push_i64(SET_Ci(i64, operandToLessOp(operand), ebx,0, ecx));
                    out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
                }
            }
            else if(right.type == expression_field) {
                if(right.utype.field->type == field_native) {
                    // add var
                    lessThanNative(operand, out, left, right, pAst);
                } else if(right.utype.field->type == field_class) {
                    addClass(operand, right.utype.field->klass, out, left, right, pAst);
                } else {
                    // do nothing field unresolved
                }
            } else if(right.type == expression_lclass) {
                addClass(operand, left.utype.klass, out, left, right, pAst);
            } else {
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                lessThanNative(operand, out, left, right, pAst);
            } else if(left.utype.field->type == field_class) {
                addClass(operand, left.utype.field->klass, out, left, right, pAst);
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_lclass:
            addClass(operand, left.utype.klass, out, left, right, pAst);
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`. Did you forget to apply a cast? "
                                                                      "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
            break;
        case expression_string:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        default:
            break;
    }

    out.lnk = pAst;
    return out;
}

Expression runtime::parseEqualExpression(ast* pAst) {
    Expression out, left, right;
    token_entity operand = pAst->getentity(0);

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary = <expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    left = parseIntermExpression(pAst->getsubast(0));
    right = parseExpression(pAst->getsubast(1));

    out.type = expression_var;
    switch(left.type) {
        case expression_var:
            if(operand.gettokentype() == ASSIGN) {
                assignValue(operand, out, left, right, pAst);
            } else {
                if(right.type == expression_var) {
                    assignNative(operand, out, left, right, pAst);
                }
                else if(right.type == expression_field) {
                    if(right.utype.field->type == field_native) {
                        // add var
                        assignNative(operand, out, left, right, pAst);
                    } else if(right.utype.field->type == field_class) {
                        addClass(operand, right.utype.field->klass, out, left, right, pAst);
                    } else {
                        // do nothing field unresolved
                    }
                } else if(right.type == expression_lclass) {
                    addClass(operand, left.utype.klass, out, left, right, pAst);
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                      "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
                }
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable");
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                if(operand.gettokentype() == ASSIGN) {
                    assignValue(operand, out, left, right, pAst);
                } else {
                    assignNative(operand, out, left, right, pAst);
                }
            } else if(left.utype.field->type == field_class) {
                if(operand.gettokentype() == ASSIGN) {
                    assignValue(operand, out, left, right, pAst);
                } else {
                    addClass(operand, left.utype.field->klass, out, left, right, pAst);
                }
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_lclass:
            if(operand.gettokentype() == ASSIGN) {
                assignValue(operand, out, left, right, pAst);
            } else {
                addClass(operand, left.utype.field->klass, out, left, right, pAst);
            }
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable/comparable");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable/comparable. Did you forget to apply a cast? "
                    "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
            break;
        case expression_string:
            if(operand.gettokentype() == ASSIGN) {
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            } else {
                // TODO: construct new string(<string>) and use that to concatonate strings
                // right expr must be of type expression_string or std#string
                out.type = expression_string;
            }
            break;
        default:
            break;
    }

    out.lnk = pAst;
    return out;
}

long long andExprs=0;
void runtime::parseAndExpressionChain(Expression& out, ast* pAst) {
    Expression leftExpr, rightExpr, peekExpr;
    int operandPtr = 0;
    bool firstEval=true;
    token_entity operand;
    List<token_entity> operands;
    operands.add(pAst->getentity(0));
    bool value=0;
    for(unsigned int i = 0; i < pAst->getsubastcount(); i++) {
        if(pAst->getsubast(i)->getentitycount() > 0 && isMathOp(pAst->getsubast(i)->getentity(0)))
            operands.add(pAst->getsubast(i)->getentity(0));
    }

    operandPtr=operands.size()-1;
    for(long int i = pAst->getsubastcount()-1; i >= 0; i--) {
        if(firstEval) {
            rightExpr = pAst->getsubast(i)->gettype() == ast_expression ? parseExpression(pAst->getsubast(i))
                                                                        : parseIntermExpression(pAst->getsubast(i));
            i--;
            firstEval= false;
        }
        leftExpr = pAst->getsubast(i)->gettype() == ast_expression ? parseExpression(pAst->getsubast(i)) : parseIntermExpression(pAst->getsubast(i));
        operand = operands.get(operandPtr--);

        out.boolExpressions.appendAll(rightExpr.boolExpressions);
        out.boolExpressions.appendAll(leftExpr.boolExpressions);

        out.type=expression_var;
        switch(leftExpr.type) {
            case expression_var:
                if(rightExpr.type == expression_var) {
                    if(leftExpr.literal && rightExpr.literal) {
                        if(operand == "&&") {
                            value=leftExpr.intValue&&rightExpr.intValue;
                            out.code.push_i64(SET_Di(i64, op_MOVI, value), ebx);
                        } else if(operand == "||") {
                            value=leftExpr.intValue||rightExpr.intValue;
                            out.code.push_i64(SET_Di(i64, op_MOVI, value), ebx);
                        } else if(operand == "&") {
                            out.code.push_i64(SET_Di(i64, op_MOVI, leftExpr.intValue&(int64_t)rightExpr.intValue), ebx);
                        } else if(operand == "|") {
                            out.code.push_i64(SET_Di(i64, op_MOVI, leftExpr.intValue|(int64_t)rightExpr.intValue), ebx);
                        } else if(operand == "^") {
                            out.code.push_i64(SET_Di(i64, op_MOVI, leftExpr.intValue^(int64_t)rightExpr.intValue), ebx);
                        }

                    } else {
                        evaluate:
                        if((leftExpr.literal || rightExpr.literal) && operand == "||") {
                            if(leftExpr.literal) {
                                out.code.push_i64(SET_Di(i64, op_MOVI, leftExpr.intValue==0), ebx);
                            } else {
                                out.code.push_i64(SET_Di(i64, op_MOVI, rightExpr.intValue==0), ebx);
                            }
                        } else {
                            // is left leftexpr a literal?
                            if(operand == "&&") {
                                pushExpressionToRegister(leftExpr, out, ebx);

                                if(andExprs==1) {
                                    out.code.push_i64(SET_Di(i64, op_SKPNE, 8));
                                    out.boolExpressions.add(out.code.size()-1);
                                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                                } else {
                                    out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));
                                }

                                pushExpressionToRegister(rightExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                                out.code.push_i64(SET_Ci(i64, op_AND, ebx,0, ecx)); // the oder in which we eval each side dosent freakn matter!

                                if(andExprs==1) {
                                    out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));

                                    if((i-1) >= 0) {
                                        out.code.push_i64(SET_Di(i64, op_SKPNE, 8));
                                        out.boolExpressions.add(out.code.size()-1);
                                    }
                                } else {
                                    out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
                                }
                            } else if(operand == "||") {
                                pushExpressionToRegister(leftExpr, out, ebx);

                                if(andExprs==1) {
                                    out.code.push_i64(SET_Di(i64, op_SKPNE, 8));
                                    out.boolExpressions.add(out.code.size()-1);
                                } else {
                                    out.code.push_i64(SET_Di(i64, op_SKPE, rightExpr.code.size()));
                                    pushExpressionToRegister(rightExpr, out, ebx);
                                    if(andExprs==1) {
                                        out.code.push_i64(SET_Di(i64, op_SKPNE, 8));
                                        out.boolExpressions.add(out.code.size()-1);
                                    }
                                }
                            } else if(operand == "|") {
                                pushExpressionToRegister(leftExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

                                pushExpressionToRegister(rightExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                                out.code.push_i64(SET_Ci(i64, op_OR, ecx,0, ebx));
                                out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
                            } else if(operand == "&") {
                                pushExpressionToRegister(leftExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

                                pushExpressionToRegister(rightExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                                out.code.push_i64(SET_Ci(i64, op_uAND, ecx,0, ebx));
                                out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
                            } else if(operand == "^") {
                                pushExpressionToRegister(leftExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_PUSHR, ebx));

                                pushExpressionToRegister(rightExpr, out, ebx);
                                out.code.push_i64(SET_Di(i64, op_POPR, ecx));
                                out.code.push_i64(SET_Ci(i64, op_uNOT, ecx,0, ebx));
                                out.code.push_i64(SET_Ci(i64, op_MOVR, ebx,0, cmt));
                            }
                        }

                        rightExpr.func=false;
                        rightExpr.literal = false;
                        rightExpr.code.free();
                        rightExpr.type=expression_var;
                    }

                }else if(rightExpr.type == expression_field) {
                    if(rightExpr.utype.field->type == field_native) {
                        // add var
                        goto evaluate;
                    } else if(rightExpr.utype.field->type == field_class) {
                        errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                          "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                    } else {
                        // do nothing field unresolved
                    }
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                      "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                }
                break;
            case expression_null:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                break;
            case expression_field:
                if(leftExpr.utype.field->type == field_native) {
                    // add var
                    goto evaluate;
                } else if(leftExpr.utype.field->type == field_class) {
                    addClass(operand, leftExpr.utype.field->klass, out, leftExpr, rightExpr, pAst);
                } else {
                    // do nothing field unresolved
                }
                break;
            case expression_native:
                errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + leftExpr.typeToString() + "`");
                break;
            case expression_lclass:
                addClass(operand, out.utype.klass, out, out, rightExpr, pAst);
                break;
            case expression_class:
                errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + leftExpr.typeToString() + "`");
                break;
            case expression_void:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`");
                break;
            case expression_dynamicclass:
                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                  "` cannot be applied to expression of type `" + leftExpr.typeToString() + "` and `" + rightExpr.typeToString() + "`. Did you forget to apply a cast? "
                                                                          "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
                break;
            case expression_string:
                // TODO: construct new string(<string>) and use that to concatonate strings
                break;
            default:
                break;
        }
    }


    /*
     * So we dont missinterpret the value returned from the expr :)
     */
    out.func=rightExpr.func;
    out.literal=rightExpr.literal=false;
    if(--andExprs == 0) {
        for(unsigned int i = 0; i < out.boolExpressions.size(); i++) {
            long index=out.boolExpressions.get(i);
            int op = GET_OP(out.code.__asm64.get(index));

            out.code.__asm64.replace(index, SET_Di(i64, op, ((out.code.size()-1)-index)-1));
        }
    }
}

Expression runtime::parseAndExpression(ast* pAst) {
    Expression out, left, right;
    token_entity operand = pAst->getentity(0);
    andExprs++;

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary << <expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    parseAndExpressionChain(out, pAst);

//    left = parseIntermExpression(pAst->getsubast(0));
//    right = parseExpression(pAst->getsubast(1));
//
//    out.type = expression_var;
//    switch(left.type) {
//        case expression_var:
//            if(right.type == expression_var) {
//                compareValue(operand, out, left, right, pAst);
//            }
//            else if(right.type == expression_field) {
//                if(right.utype.field->type == field_native) {
//                    // add var
//
//                    compareValue(operand, out, left, right, pAst);
//                } else if(right.utype.field->type == field_class) {
//                    addClass(operand, right.utype.field->klass, out, left, right, pAst);
//                } else {
//                    // do nothing field unresolved
//                }
//            } else if(right.type == expression_lclass) {
//                addClass(operand, left.utype.klass, out, left, right, pAst);
//            } else {
//                errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
//                                                                  "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
//            }
//            break;
//        case expression_null:
//            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
//                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
//            break;
//        case expression_field:
//            if(left.utype.field->type == field_native) {
//                // add var
//                compareValue(operand, out, left, right, pAst);
//            } else if(left.utype.field->type == field_class) {
//                addClass(operand, left.utype.field->klass, out, left, right, pAst);
//            } else {
//                // do nothing field unresolved
//            }
//            break;
//        case expression_native:
//            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
//            break;
//        case expression_lclass:
//            addClass(operand, left.utype.klass, out, left, right, pAst);
//            break;
//        case expression_class:
//            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
//            break;
//        case expression_void:
//            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
//                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
//            break;
//        case expression_dynamicclass:
//            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
//                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`. Did you forget to apply a cast? "
//                                                                      "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
//            break;
//        case expression_string:
//            // TODO: construct new string(<string>) and use that to concatonate strings
//            out.type = expression_string;
//            break;
//        default:
//            break;
//    }

    out.lnk = pAst;
    return out;
}

Expression runtime::parseAssignExpression(ast* pAst) {
    Expression out, left, right;
    token_entity operand = pAst->getentity(0);

    if(pAst->getsubastcount() == 1) {
        // cannot compute unary = <expression>
        errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + operand.gettoken() + "`");
        return Expression(pAst);
    }

    left = parseIntermExpression(pAst->getsubast(0));
    right = parseExpression(pAst->getsubast(1));

    out.type = expression_var;
    switch(left.type) {
        case expression_var:
            if(operand.gettokentype() == ASSIGN) {
                errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable");
            } else {
                if(right.type == expression_var) {
                    assignNative(operand, out, left, right, pAst);
                }
                else if(right.type == expression_field) {
                    if(right.utype.field->type == field_native) {
                        // add var
                        assignValue(operand, out, left, right, pAst);
                    } else if(right.utype.field->type == field_class) {
                        addClass(operand, right.utype.field->klass, out, left, right, pAst);
                    } else {
                        // do nothing field unresolved
                    }
                } else if(right.type == expression_lclass) {
                    addClass(operand, left.utype.klass, out, left, right, pAst);
                } else {
                    errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                                      "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
                }
            }
            break;
        case expression_null:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable");
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                assignValue(operand, out, left, right, pAst);
            } else if(left.utype.field->type == field_class) {
                addClass(operand, left.utype.field->klass, out, left, right, pAst);
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_native:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_lclass:
            addClass(operand, left.utype.klass, out, left, right, pAst);
            break;
        case expression_class:
            errors->newerror(UNEXPECTED_SYMBOL, pAst->line, pAst->col, " `" + left.typeToString() + "`");
            break;
        case expression_void:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable");
            break;
        case expression_dynamicclass:
            errors->newerror(GENERIC, pAst->line, pAst->col, "expression is not assignable. Did you forget to apply a cast? "
                                                                     "i.e ((SomeClass)dynamic_class) " + operand.gettoken() + " <data>");
            break;
        case expression_string:
            errors->newerror(GENERIC, pAst->line,  pAst->col, "Binary operator `" + operand.gettoken() +
                                                              "` cannot be applied to expression of type `" + left.typeToString() + "` and `" + right.typeToString() + "`");
            break;
        default:
            break;
    }

    out.lnk = pAst;
    return out;
}

Expression runtime::parseBinaryExpression(ast* pAst) {
    switch(pAst->gettype()) {
        case ast_add_e:
            return parseAddExpression(pAst);
        case ast_mult_e:
            return parseMultExpression(pAst);
        case ast_shift_e:
            return parseShiftExpression(pAst);
        case ast_less_e:
            return parseLessExpression(pAst);
        case ast_equal_e:
            return parseEqualExpression(pAst);
        case ast_and_e:
            return parseAndExpression(pAst);
        case ast_assign_e:
            return parseAssignExpression(pAst);
        default:
            return Expression(pAst);
    }
}

bool runtime::equals(Expression& left, Expression& right, string msg) {

    if(left.type == expression_native) {
        errors->newerror(UNEXPECTED_SYMBOL, left.lnk->line, left.lnk->col, " `" + left.typeToString() + "`" + msg);
        return false;
    }
    if(right.type == expression_native) {
        errors->newerror(UNEXPECTED_SYMBOL, right.lnk->line, right.lnk->col, " `" + right.typeToString() + "`" + msg);
        return false;
    }

    switch(left.type) {
        case expression_var:
            if(right.type == expression_var) {
                // add 2 vars
                return true;
            }
            else if(right.type == expression_field) {
                if(right.utype.field->type == field_native) {
                    if(right.utype.field->nativeInt()) {
                        return true;
                    }
                }
            }
            break;
        case expression_null:
            if(right.type == expression_lclass || right.type == expression_dynamicclass) {
                return true;
            } else if(right.type == expression_class) {
                errors->newerror(GENERIC, right.lnk->line,  right.lnk->col, "Class `" + right.typeToString() + "` must be lvalue" + msg);
                return false;
            }
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                if(right.type == expression_var) {
                    if(left.utype.field->nativeInt()) {
                        return true;
                    }
                } else if(right.type == expression_dynamicclass) {
                    if(left.utype.field->dynamicObject()) {
                        return true;
                    }
                } else if(right.type == expression_field) {
                    if(right.utype.field->nativeInt()) {
                        return left.utype.field->nativeInt();
                    }
                    else if(right.utype.field->dynamicObject()) {
                        return left.utype.field->dynamicObject();
                    }
                } else if(right.type == expression_string) {
                    return left.utype.field->array;
                } else if(right.type == expression_lclass) {
                    return left.utype.field->dynamicObject();
                }
            } else if(left.utype.field->type == field_class) {
                if(right.type == expression_lclass) {
                    if(left.utype.field->klass->match(right.utype.klass)) {
                        return true;
                    }
                } else if(right.type == expression_class) {
                    if(left.utype.field->klass->match(right.utype.klass)) {
                        errors->newerror(GENERIC, right.lnk->line,  right.lnk->col, "Class `" + right.typeToString() + "` must be lvalue" + msg);
                        return false;
                    }
                } else if(right.type == expression_field && right.utype.field->type == field_class) {
                    if(right.utype.field->klass->match(left.utype.field->klass)) {
                        return true;
                    }
                } else {
                    List<Param> params;
                    List<Expression> exprs;
                    exprs.push_back(right);

                    expressionListToParams(params, exprs);
                    return left.utype.field->klass->getOverload(oper_EQUALS, params) != NULL;
                }
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_lclass:
            if(right.type == expression_lclass) {
                if(left.utype.klass->match(right.utype.klass)) {
                    return true;
                }
            } else if(right.type == expression_field) {
                if(left.utype.klass->match(right.utype.field->klass)) {
                    return true;
                }
            } else if(right.type == expression_class) {
                if(left.utype.klass->match(right.utype.klass)) {
                    errors->newerror(GENERIC, right.lnk->line,  right.lnk->col, "Class `" + right.typeToString() + "` must be lvalue" + msg);
                    return false;
                }
            }
            break;
        case expression_class:
            if(right.type == expression_class) {
                if(left.utype.klass->match(right.utype.klass)) {
                    return true;
                }
            }
            break;
        case expression_void:
            if(right.type == expression_void) {
                return true;
            }
            break;
        case expression_dynamicclass:
            if(right.type == expression_dynamicclass || right.type == expression_lclass) {
                return true;
            } else if(right.type == expression_field) {
                if(right.utype.field->nf == fdynamic || right.utype.field->type == field_class) {
                    return true;
                }
            }
            break;
        case expression_string:
            if(right.type == expression_field) {
                if(right.utype.field->nativeInt() && right.utype.field->array) {
                    return true;
                }
            }
            else if(right.type == expression_var) {
                if(right.utype.array) {
                    return true;
                }
            }
            else if(right.type == expression_string) {
                return true;
            }
            break;
        default:
            break;
    }

    errors->newerror(GENERIC, right.lnk->line,  right.lnk->col, "Expressions of type `" + left.typeToString() + "` and `" + right.typeToString() + "` are not compatible" + msg);
    return false;
}

bool runtime::equalsNoErr(Expression& left, Expression& right) {

    if(left.type == expression_native) {
        return false;
    }
    if(right.type == expression_native) {
        return false;
    }

    switch(left.type) {
        case expression_var:
            if(right.type == expression_var) {
                // add 2 vars
                return true;
            }
            else if(right.type == expression_field) {
                if(right.utype.field->type == field_native) {
                    if(right.utype.field->nativeInt()) {
                        return !right.utype.field->array;
                    }
                }
            }
            break;
        case expression_null:
            if(right.type == expression_lclass || right.type == expression_dynamicclass) {
                return true;
            } else if(right.type == expression_class) {
                return false;
            }
            break;
        case expression_field:
            if(left.utype.field->type == field_native) {
                // add var
                if(right.type == expression_var) {
                    if(left.utype.field->nativeInt()) {
                        return !left.utype.array;
                    }
                } else if(right.type == expression_dynamicclass) {
                    if(left.utype.field->dynamicObject()) {
                        return true;
                    }
                } else if(right.type == expression_field) {
                    if(right.utype.field->nativeInt()) {
                        return left.utype.field->nativeInt() && left.utype.field->array==right.utype.field->array;
                    }
                    else if(right.utype.field->dynamicObject()) {
                        return left.utype.field->dynamicObject();
                    }
                } else if(right.type == expression_string) {
                    return right.utype.field->nativeInt() && left.utype.field->array;
                } else if(right.type == expression_lclass) {
                    return left.utype.field->dynamicObject();
                }
            } else if(left.utype.field->type == field_class) {
                if(right.type == expression_lclass) {
                    if(left.utype.field->klass->match(right.utype.klass)) {
                        return true;
                    }
                } else if(right.type == expression_class) {
                    if(left.utype.field->klass->match(right.utype.klass)) {
                        return false;
                    }
                } else if(right.type == expression_field && right.utype.field->type == field_class) {
                    if(right.utype.field->klass->match(left.utype.field->klass)) {
                        return true;
                    }
                }
            } else {
                // do nothing field unresolved
            }
            break;
        case expression_lclass:
            if(right.type == expression_lclass) {
                if(left.utype.klass->match(right.utype.klass)) {
                    return true;
                }
            } else if(right.type == expression_field) {
                if(left.utype.klass->match(right.utype.field->klass)) {
                    return true;
                }
            } else if(right.type == expression_class) {
                if(left.utype.klass->match(right.utype.klass)) {
                    return false;
                }
            }
            break;
        case expression_class:
            if(right.type == expression_class) {
                if(left.utype.klass->match(right.utype.klass)) {
                    return true;
                }
            }
            break;
        case expression_void:
            if(right.type == expression_void) {
                return true;
            }
            break;
        case expression_dynamicclass:
            if(right.type == expression_dynamicclass || right.type == expression_lclass) {
                return true;
            } else if(right.type == expression_field) {
                if(right.utype.field->nf == fdynamic || right.utype.field->type == field_class) {
                    return true;
                }
            }
            break;
        case expression_string:
            if(right.type == expression_field) {
                if(right.utype.field->nativeInt() && right.utype.field->array) {
                    return true;
                }
            }
            else if(right.type == expression_var) {
                if(right.utype.array) {
                    return true;
                }
            }
            else if(right.type == expression_string) {
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}

Expression runtime::parseQuesExpression(ast* pAst) {
    Expression condition,condIfTrue, condIfFalse, expression;

    condition = parseIntermExpression(pAst->getsubast(0));
    condIfTrue = parseExpression(pAst->getsubast(1));
    condIfFalse = parseExpression(pAst->getsubast(1));
    expression = condIfTrue;

    expression.code.__asm64.free();
    pushExpressionToRegister(condition, expression, cmt);

    expression.code.push_i64(SET_Ci(i64, op_LOADF, adx,0, (condIfTrue.code.size() + 3)));
    expression.code.push_i64(SET_Ei(i64, op_IFNE));

    expression.code.inject(expression.code.size(), condIfTrue.code);
    expression.code.push_i64(SET_Di(i64, op_SKP, condIfFalse.code.size()));
    expression.code.inject(expression.code.size(), condIfFalse.code);


    if(equals(condIfTrue, condIfFalse)) {
        if(condIfTrue.type == expression_class) {
            errors->newerror(GENERIC, condIfTrue.lnk->line,  condIfTrue.lnk->col, "Class `" + condIfTrue.typeToString() + "` must be lvalue");
            return condIfTrue;
        }
    }

    return expression;
}

int recursive_expressions = 0; // TODO: use this to figure out sneaky user errors
Expression runtime::parseExpression(ast *pAst) {
    Expression expression;
    ast* encap  = pAst->getsubast(0);

    switch(encap->gettype()) {
        case ast_primary_expr:
            return parsePrimaryExpression(encap);
        case ast_self_e:
            return parseSelfExpression(encap);
        case ast_base_e:
            return parseBaseExpression(encap);
        case ast_null_e:
            return parseNullExpression(encap);
        case ast_new_e:
            return parseNewExpression(encap);
        case ast_post_inc_e:
            return parsePostInc(encap);
        case ast_arry_e:
            return parseArrayExpression(encap);
        case ast_cast_e:
            return parseCastExpression(encap);
        case ast_pre_inc_e:
            return parsePreInc(encap);
        case ast_paren_e:
            return parseParenExpression(encap);
        case ast_not_e:
            return parseNotExpression(encap);
        case ast_vect_e:
            errors->newerror(GENERIC, pAst->line, pAst->col, "unexpected vector array expression. Did you mean 'new type { <data>, .. }'?");
            expression.lnk = encap;
            return expression;
        case ast_add_e:
        case ast_mult_e:
        case ast_shift_e:
        case ast_less_e:
        case ast_equal_e:
        case ast_and_e:
        case ast_assign_e:
            return parseBinaryExpression(encap);
        case ast_ques_e:
            return parseQuesExpression(encap);
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
    field->vaddr = scope->klass->getFieldIndex(name);
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
    if(pAst->getentitycount() != 0)
        utype_arg.key = pAst->getentity(0).gettoken();
    else
        utype_arg.key = "";

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
    Method method = Method(name, current_module, scope->klass, params, modCompat, NULL, note, sourceFiles.indexof(_current->sourcefile));
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, method);
    } else
        method.type = lvoid;

    method.vaddr = address_spaces++;
    method.local_count = params.size();

    if(!method.isStatic())
        method.local_count++; // hold spot for self
    if(!scope->klass->addFunction(method)) {
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "function `" + name + "` is already defined in the scope");
        printnote(scope->klass->getFunction(name, params)->note, "function `" + name + "` previously defined here");
    }
}

void runtime::parseMethodReturnType(Expression& expression, Method& method) {
    method.array = expression.utype.array;
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
    Method macro = Method(name, current_module, scope->klass, params, modCompat, NULL, note, sourceFiles.indexof(_current->sourcefile));
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, macro);
    } else
        macro.type = lvoid;


    macro.vaddr = address_spaces++;
    macro.local_count = params.size();
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
    OperatorOverload operatorOverload = OperatorOverload(note, scope->klass, params, modCompat, NULL, string_toop(op), sourceFiles.indexof(_current->sourcefile), op);
    if(pAst->hassubast(ast_method_return_type)) {
        utype = parseUtype(pAst->getsubast(ast_method_return_type)->getsubast(ast_utype));
        parseMethodReturnType(utype, operatorOverload);
    } else
        operatorOverload.type = lvoid;

    operatorOverload.vaddr = address_spaces++;
    operatorOverload.local_count = params.size();

    if(!operatorOverload.isStatic())
        operatorOverload.local_count++; // hold spot for self
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

        Method method = Method(name, current_module, scope->klass, params, modCompat, NULL, note, sourceFiles.indexof(_current->sourcefile));
        method.type = lvoid;

        method.vaddr = address_spaces++;
        method.local_count = params.size();
        method.local_count++; // hold spot for self
        if(!scope->klass->addConstructor(method)) {
            this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                                   "constructor `" + name + "` is already defined in the scope");
            printnote(scope->klass->getConstructor(params)->note, "constructor `" + name + "` previously defined here");
        }
    } else
        this->errors->newerror(PREVIOUSLY_DEFINED, pAst->line, pAst->col,
                               "constructor `" + name + "` must be the same name as its parent");
}

void runtime::addDefaultConstructor(ClassObject* klass, ast* pAst) {
    List<Param> emptyParams;
    Scope* scope = current_scope();
    List<AccessModifier> modCompat;
    modCompat.add(mPublic);

    RuntimeNote note = RuntimeNote(_current->sourcefile, _current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(klass->getConstructor(emptyParams) == NULL) {
        Method method = Method(klass->getName(), current_module, scope->klass, emptyParams, modCompat, NULL, note, sourceFiles.indexof(_current->sourcefile));

        method.vaddr = address_spaces++;
        klass->addConstructor(method);
    }
}

void runtime::resolveClassDecl(ast* pAst) {
    Scope* scope = current_scope();
    ast* astBlock = pAst->getsubast(ast_block), *trunk;
    list<AccessModifier> modifiers;
    ClassObject* klass;
    int startpos=1;

    parse_access_decl(pAst, modifiers, startpos);
    string name =  pAst->getentity(startpos).gettoken();

    if(scope->type == scope_global) {

        klass = getClass(current_module, name);
        klass->setFullName(current_module + "#" + name);
    }
    else {
        klass = scope->klass->getChildClass(name);
        klass->setFullName(scope->klass->getFullName() + "." + name);
    }

    if(resolvedFields)
        klass->vaddr = class_size++;

    add_scope(Scope(scope_class, klass));
    klass->setBaseClass(parse_base_class(pAst, ++startpos));
    for(long i = 0; i < astBlock->getsubastcount(); i++) {
        trunk = astBlock->getsubast(i);

        switch(trunk->gettype()) {
            case ast_class_decl:
                resolveClassDecl(trunk);
                break;
            case ast_var_decl:
                if(!resolvedFields)
                    resolveVarDecl(trunk);
                break;
            case ast_method_decl:
                if(resolvedFields)
                    resolveMethodDecl(trunk);
                break;
            case ast_macros_decl:
                if(resolvedFields)
                    resolveMacrosDecl(trunk);
                break;
            case ast_operator_decl:
                if(resolvedFields)
                    resolveOperatorDecl(trunk);
                break;
            case ast_construct_decl:
                if(resolvedFields)
                    resolveConstructorDecl(trunk);
                break;
            default:
                stringstream err;
                err << ": unknown ast type: " << trunk->gettype();
                errors->newerror(INTERNAL_ERROR, trunk->line, trunk->col, err.str());
                break;
        }
    }

    if(resolvedFields)
        addDefaultConstructor(klass, pAst);
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
        sourceFiles.addif(p->sourcefile);

        current_module = "$invisible";
        keypair<string, List<string>> resolve_map;
        List<string> imports;

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
                    imports.add(parse_modulename(trunk));
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
        import_map.push_back(resolve_map);
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
    modules.free();

    __freeList(classes);

    for(unsigned int i = 0; i < import_map.size(); i++) {
        import_map.get(i).value.free();
    }

    import_map.free();

    __freeList(macros);

    scope_map.free();
    sourceFiles.free();
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
    cout <<               "    -v<version>       set application version." << endl;
    cout <<               "    -unsafe -u        allow unsafe code." << endl;
    cout <<               "    -objdmp           create a dump file of the entire generated assembly." << endl;
    cout <<               "    -target           specify target platform to run on. i.e sharp:" << endl;
    cout <<               "    -werror           enable warnings as errors." << endl;
    cout <<               "    -release -r       disable debugging on application." << endl;
    cout <<               "    --h -?            display this help message." << endl;
}

#define opt(v) strcmp(argv[i], v) == 0
void _srt_start(List<string> files);

void print_vers();

std::string to_lower(string s) {
    string newstr = "";
    for(char c : s) {
        newstr += tolower(c);
    }
    return newstr;
}

int _bootstrap(int argc, const char* argv[]) {
    int_errs();

    if (argc < 2) { // We expect 2 arguments: the source file(s), and program options
        help();
        return 1;
    }

    List<string> files;
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
            else if(opt("-magic")){
                c_options.magic = true;
            }
            else if(opt("-showversion")){
                print_vers();
                cout << endl;
            }
            else if(opt("-target")){
                if(i+1 >= argc)
                    rt_error("file version required after option `-target`");
                else {
                    std::string x = std::string(argv[++i]);
                    if(runtime::all_integers(x))
                        c_options.target = strtol(x.c_str(), NULL, 0);
                    else {
                        if(to_lower(x) == "base") {
                            c_options.target = versions.BASE;
                        } else {
                            rt_error("unknown target " + x);
                        }
                    }
                }
            }
            else if(opt("-w")){
                c_options.warnings = false;
            }
            else if(opt("-v")){
                if(i+1 >= argc)
                    rt_error("file version required after option `-v`");
                else
                    c_options.vers = string(argv[++i]);
            }
            else if(opt("-u") || opt("-unsafe")){
                c_options.unsafe = true;
            }
            else if(opt("-werror")){
                c_options.werrors = true;
                c_options.warnings = true;
            }
            else if(opt("-objdmp")){
                c_options.objDump = true;
            }
            else if(string(argv[i]).at(0) == '-'){
                rt_error("invalid option `" + string(argv[i]) + "`, try bootstrap -h");
            }
            else {
                // add the source files
                do {
                    if(string(argv[i]).at(0) == '-')
                        goto args_;

                    files.addif(string(argv[i++]));
                }while(i<argc);
                break;
            }
    }

    if(files.size() == 0){
        help();
        return 1;
    }

    for(unsigned int i = 0; i < files.size(); i++) {
        string& file = files.get(i);

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

void _srt_start(List<string> files)
{
    std::list<parser*> parsers;
    parser* p = NULL;
    tokenizer* t;
    file::buffer source;
    size_t errors=0, uo_errors=0;
    int succeeded=0, failed=0, panic=0;

    for(unsigned int i = 0; i < files.size(); i++) {
        string& file = files.get(i);
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

                if(p->panic) {
                    panic = 1;
                }

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

    if(panic) {
        cout << "Detected more than 9999+ errors, fix your freakin code!";
    }

    if(!panic && errors == 0 && uo_errors == 0) {
        failed = 0, succeeded=0;
        runtime rt(c_options.out, parsers);

        failed = rt.parse_map.key.size();
        succeeded = rt.parse_map.value.size();

        errors+=rt.errs;
        uo_errors+=rt.uo_errs;
        if(errors == 0 && uo_errors == 0) {
            if(!c_options.compile)
                rt.generate();
        }

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
    return modules.find(name);
}

bool runtime::class_exists(string module, string name) {
    ClassObject* klass = NULL;
    for(unsigned int i = 0; i < classes.size(); i++) {
        klass = &classes.get(i);
        if(klass->getName() == name) {
            if(module != "")
                return klass->getModuleName() == module;
            return true;
        }
    }

    return false;
}

Method *runtime::getmacros(string module, string name, List<Param>& params) {
    Method* macro = NULL;
    for(unsigned int i = 0; i < macros.size(); i++) {
        macro = &macros.get(i);
        if(Param::match(*macro->getParams(), params) && name == macro->getName()) {
            if(module != "")
                return module == macro->getModule() ? macro : NULL;

            return macro;
        }
    }

    return NULL;
}

Method *runtime::getmacros(string module, string name, int64_t _offset) {
    Method* macro = NULL;
    for(unsigned int i = 0; i < macros.size(); i++) {
        macro = &macros.get(i);
        if(name == macro->getName()) {
            if(module != "") {
                if(module == macro->getModule()) {
                    if(_offset == 0)
                        return macro;
                    else
                        _offset--;
                }
            }

            if(_offset == 0)
                return macro;
            else
                _offset--;
        }
    }

    return NULL;
}

bool runtime::add_macros(Method macro) {
    if(getmacros(macro.getModule(), macro.getName(), *macro.getParams()) == NULL) {
        macros.add(macro);
        return true;
    }
    return false;
}

bool runtime::add_class(ClassObject klass) {
    if(!class_exists(klass.getModuleName(), klass.getName())) {

        classes.add(klass);
        return true;
    }
    return false;
}

void runtime::add_module(string name) {
    if(!module_exists(name)) {
        modules.push_back(name);
    }
}

ClassObject *runtime::getClass(string module, string name) {
    ClassObject* klass = NULL;
    for(unsigned int i = 0; i < classes.size(); i++) {
        klass = &classes.get(i);
        if(klass->getName() == name) {
            if(module != "" && klass->getModuleName() == module)
                return klass;
            else if(module == "")
                return klass;
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
        for(unsigned int i = 0; i < import_map.size(); i++) {
            if(import_map.get(i).key == _current->sourcefile) {

                List<string>& lst = import_map.get(i).value;
                for(unsigned int x = 0; x < lst.size(); x++) {
                    if((ref = getClass(lst.get(i), name)) != NULL)
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
        expression.code.push_i64(SET_Di(i64, op_MOVI, 0), egx);
        expression.code.push_i64(SET_Ci(i64, op_MOVX, ebx,0, egx));
    } else {
        errors->newerror(GENERIC, pAst->line, pAst->col, "variable expected");
    }
}

Scope *runtime::current_scope() {
    if(scope_map.size() == 0)
        return NULL;
    return &scope_map.get(scope_map.size()-1);
}

Scope* runtime::add_scope(Scope scope) {
    scope_map.push_back(scope);
    return current_scope();
}

void runtime::remove_scope() {
    Scope* scope = current_scope();
    scope->free();

    scope_map.pop_back();
}

_operator string_toop(string op) {
    if(op=="+")
        return oper_PLUS;
    if(op=="-")
        return oper_MINUS;
    if(op=="*")
        return oper_MULT;
    if(op=="/")
        return oper_DIV;
    if(op=="%")
        return oper_MOD;
    if(op=="++")
        return oper_INC;
    if(op=="--")
        return oper_DEC;
    if(op=="=")
        return oper_EQUALS;
    if(op=="==")
        return oper_EQUALS_EQ;
    if(op=="+=")
        return oper_PLUS_EQ;
    if(op=="-=")
        return oper_MIN_EQ;
    if(op=="*=")
        return oper_MULT_EQ;
    if(op=="/=")
        return oper_DIV_EQ;
    if(op=="&=")
        return oper_AND_EQ;
    if(op=="|=")
        return oper_OR_EQ;
    if(op=="!=")
        return oper_NOT_EQ;
    if(op=="%=")
        return oper_MOD_EQ;
    if(op=="!")
        return oper_NOT;
    if(op=="<<")
        return oper_SHL;
    if(op==">>")
        return oper_SHR;
    if(op=="<")
        return oper_LESSTHAN;
    if(op==">")
        return oper_GREATERTHAN;
    if(op=="<=")
        return oper_LTEQ;
    if(op==">=")
        return oper_GTEQ;
    return oper_NO;
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

string Expression::typeToString() {
    switch(type) {
        case expression_string:
            return "var[]";
        case expression_unresolved:
            return "?";
        case expression_var:
            return string("var") + (utype.array ? "[]" : "");
        case expression_lclass:
            return utype.typeToString();
        case expression_native:
            return utype.typeToString();
        case expression_unknown:
            return "?";
        case expression_class:
            return utype.typeToString();
        case expression_void:
            return "void";
        case expression_dynamicclass:
            return "dynamic object";
        case expression_field:
            return utype.typeToString();
        case expression_null:
            return "null";
    }
    return utype.typeToString();
}

void Expression::operator=(Expression expression) {
    this->type=expression.type;
    this->_new=expression._new;

    this->code.free();
    this->code.inject(0, expression.code);
    this->dot=expression.dot;
    this->func=expression.func;
    this->intValue=expression.intValue;
    this->literal=expression.literal;
    this->lnk = expression.lnk;
    this->utype  = expression.utype;
    this->value = expression.value;
    this->arrayElement = expression.arrayElement;
    this->boolExpressions.addAll(expression.boolExpressions);
}

void Expression::inject(Expression &expression) {
    this->code.inject(this->code.size(), expression.code);
}

string ResolvedReference::toString() {
    if(type == FIELD || type == CLASS) {
        return refrenceName;
    } else if(type == NATIVE) {
        return runtime::nativefield_tostr(nf);
    }
    return refrenceName;
}

bool ResolvedReference::nativeInt() {
    return nf >= fi8 && nf <= fvar;
}

bool ResolvedReference::dynamicObject() {
    return nf == fdynamic;
}

string ResolvedReference::typeToString() {
    switch(type) {
        case FIELD:
            if(field != NULL) {
                if(field->type == field_native) {
                    return runtime::nativefield_tostr(field->nf) + (array || field->array ? "[]" : "");
                } else if(field->type == field_class) {
                    return field->klass->getFullName() + (array || field->array ? "[]" : "");
                } else
                    return toString() + (array ? "[]" : "");
            } else
                return toString() + (array ? "[]" : "");
            break;
        case CLASS:
            if(klass != NULL) {
                return klass->getFullName() + (array ? "[]" : "");
            } else
                return toString();
            break;
        case NATIVE:
            return runtime::nativefield_tostr(nf) + (array ? "[]" : "");
        case NOTRESOLVED:
            return toString() + (array ? "[]" : "");
    }
    return toString() + (array ? "[]" : "");
}

string mi64_tostr(int64_t i64)
{
    string str;
    mi64_t mi;
    SET_mi64(mi, i64);

    str+=(uint8_t)GET_mi32w(mi.A);
    str+=(uint8_t)GET_mi32x(mi.A);
    str+=(uint8_t)GET_mi32y(mi.A);
    str+=(uint8_t)GET_mi32z(mi.A);

    str+=(uint8_t)GET_mi32w(mi.B);
    str+=(uint8_t)GET_mi32x(mi.B);
    str+=(uint8_t)GET_mi32y(mi.B);
    str+=(uint8_t)GET_mi32z(mi.B);
    return str;
}

string copychars(char c, int t) {
    nString s;
    int it = 0;

    while (it++ < t)
        s += c;

    return s.str();
}

std::string runtime::generate_manifest() {
    stringstream manifest;

    manifest << (char)manif;
    manifest << ((char)0x02); manifest << c_options.out << ((char)nil);
    manifest << ((char)0x4); manifest << c_options.vers << ((char)nil);
    manifest << ((char)0x5); manifest << c_options.debug ? ((char)1) : ((char)nil);
    manifest << ((char)0x6); manifest << mi64_tostr(main->vaddr) << ((char)nil);
    manifest << ((char)0x7); manifest << mi64_tostr(address_spaces) << ((char)nil);
    manifest << ((char)0x8); manifest << mi64_tostr(class_size) << ((char)nil);
    manifest << ((char)0x9 ); manifest << 1 << ((char)nil);
    manifest << ((char)0x0c); manifest << mi64_tostr(string_map.size()) << ((char)nil);
    manifest << ((char)0x0e); manifest << c_options.target << ((char)nil);
    manifest << ((char)0x0f); manifest << sourceFiles.size() << ((char)nil);
    manifest << '\n' << (char)eoh;

    return manifest.str();
}

std::string runtime::generate_header() {
    stringstream header;
    header << (char)file_sig << "SEF"; header << copychars(nil, offset);
    header << (char)digi_sig1 << (char)digi_sig2 << (char)digi_sig3;

    header << generate_manifest();
    return header.str();
}

long long field_tovirtual_type(Field& field) {
    if(field.type == field_native) {
        if(field.nf == fdynamic)
            return dynamicobject;
        else
            return nativeint;
    } else {
        return field.klass->vaddr;
    }
}

std::string runtime::field_to_stream(Field& field) {
    stringstream fstream;

    fstream << ((char)data_field);
    fstream << field.name << ((char)nil);
    fstream << field.vaddr << ((char)nil);
    fstream << field_tovirtual_type(field) << ((char)nil);
    fstream << (field.modifiers.find(mStatic) ? 1 : 0) << ((char)nil);
    fstream << (field.array ? 1 : 0) << ((char)nil);
    fstream << (field.type == field_class ? field.klass->vaddr : -1) << ((char)nil);
    fstream << endl;

    return fstream.str();
}


std::string runtime::class_to_stream(ClassObject& klass) {
    stringstream kstream;

    kstream << (char)data_class;
    kstream << (klass.getSuperClass() == NULL ? -1 : klass.getSuperClass()->vaddr) << ((char)nil);
    kstream << mi64_tostr(klass.vaddr);
    kstream << klass.getFullName() << ((char)nil);
    kstream << klass.getTotalFieldCount() << ((char)nil);
    kstream << klass.getTotalFunctionCount() << ((char)nil);

    for(long long i = 0; i < klass.fieldCount(); i++) {
        kstream << field_to_stream(*klass.getField(i));
    }

    ClassObject* base = klass.getBaseClass();
    while(base != NULL) {
        for(long long i = 0; i < base->fieldCount(); i++) {
            kstream << field_to_stream(*base->getField(i));
        }

        base = base->getBaseClass();
    }

    /* Constructors */
    for(long long i = 0; i < klass.constructorCount(); i++) {
        kstream << (char)data_method;
        kstream << mi64_tostr(klass.getConstructor(i)->vaddr) << ((char)nil);
        allMethods.add(klass.getConstructor(i));
    }

    base = klass.getBaseClass();
    while(base != NULL) {
        for(long long i = 0; i < base->constructorCount(); i++) {
            kstream << (char)data_method;
            kstream << mi64_tostr(base->getConstructor(i)->vaddr) << ((char)nil);
        }

        base = base->getBaseClass();
    }

    /* Methods */
    for(long long i = 0; i < klass.functionCount(); i++) {
        kstream << (char)data_method;
        kstream << mi64_tostr(klass.getFunction(i)->vaddr) << ((char)nil);
        allMethods.add(klass.getFunction(i));
    }

    base = klass.getBaseClass();
    while(base != NULL) {
        for(long long i = 0; i < base->functionCount(); i++) {
            kstream << (char)data_method;
            kstream << mi64_tostr(base->getFunction(i)->vaddr) << ((char)nil);
        }

        base = base->getBaseClass();
    }

    /* Overloads */
    for(long long i = 0; i < klass.overloadCount(); i++) {
        kstream << (char)data_method;
        kstream << mi64_tostr(klass.getOverload(i)->vaddr) << ((char)nil);
        allMethods.add(klass.getOverload(i));
    }

    base = klass.getBaseClass();
    while(base != NULL) {
        for(long long i = 0; i < base->overloadCount(); i++) {
            kstream << (char)data_method;
            kstream << mi64_tostr(base->getOverload(i)->vaddr) << ((char)nil);
        }

        base = base->getBaseClass();
    }

    /* Macros */
    for(long long i = 0; i < klass.macrosCount(); i++) {
        kstream << (char)data_method;
        kstream << mi64_tostr(klass.getMacros(i)->vaddr) << ((char)nil);
        allMethods.add(klass.getMacros(i));
    }

    base = klass.getBaseClass();
    while(base != NULL) {
        for(long long i = 0; i < base->macrosCount(); i++) {
            kstream << (char)data_method;
            kstream << mi64_tostr(base->getMacros(i)->vaddr) << ((char)nil);
        }

        base = base->getBaseClass();
    }

    for(long long i = 0; i < klass.childClassCount(); i++) {
        kstream << class_to_stream(*klass.getChildClass(i)) << endl;
    }

    return kstream.str();
}

std::string runtime::generate_data_section() {
    stringstream data_sec;
    for(int64_t i = 0; i < classes.size(); i++) {
        data_sec << class_to_stream(classes.get(i)) << endl;
    }

    for(int64_t i = 0; i < sourceFiles.size(); i++) {
        data_sec << (char)data_file;
        data_sec << sourceFiles.get(i) << ((char)nil);
    }

    data_sec << "\n"<< "\n" << (char)eos;

    for(int64_t i = 0; i < macros.size(); i++) {
        allMethods.add(&macros.get(i));
    }

    List<Method*> reorderedList;
    int64_t iter=0;

    readjust:
    for(int64_t i = 0; i < allMethods.size(); i++) {
        if(allMethods.get(i)->vaddr == iter) {
            iter++;
            reorderedList.add(allMethods.get(i));
            if(iter < allMethods.size())
                goto readjust;
        }
    }

    allMethods.addAll(reorderedList);
    reorderedList.free();

    return data_sec.str();
}

std::string runtime::generate_string_section() {
    stringstream strings;

    for(int64_t i = 0; i < string_map.size(); i++) {
        strings << (char)data_string;
        strings << mi64_tostr(i) << ((char)nil) << string_map.get(i) << ((char)nil);
    }

    strings << "\n"<< "\n" << (char)eos;

    return strings.str();
}

std::string runtime::method_to_stream(Method* method) {
    stringstream func;

    for(unsigned int i = 0; i < method->paramCount(); i++) {
        func << field_tovirtual_type(method->getParam(i).field) << ((char)nil);
        func << (method->getParam(i).field.array ? 1 : 0) << ((char)nil);
    }

    for(long i = 0; i < method->code.__asm64.size(); i++) {
        func << mi64_tostr(method->code.__asm64.get(i));
    }
    return func.str();
}

std::string runtime::generate_text_section() {
    stringstream text;

    text << (char)stext;

    for(long i = 0; i < allMethods.size(); i++) {
        Method* f = allMethods.get(i);
        text << (char)data_method;
        text << mi64_tostr(allMethods.get(i)->vaddr);
        text << allMethods.get(i)->getName() << ((char)nil);
        text << allMethods.get(i)->sourceFile << ((char)nil);
        text << mi64_tostr(allMethods.get(i)->pklass->vaddr);
        text << mi64_tostr(allMethods.get(i)->paramCount());
        text << mi64_tostr(allMethods.get(i)->local_count);
        text << mi64_tostr(allMethods.get(i)->code.__asm64.size());
        text << (allMethods.get(i)->isStatic() ? 0 : 1) << ((char)nil);

        text << allMethods.get(i)->line_table.size() << ((char)nil);
        for(unsigned int x = 0; x < allMethods.get(i)->line_table.size(); x++) {
            text << mi64_tostr(allMethods.get(i)->line_table.get(x).key);
            text << mi64_tostr(allMethods.get(i)->line_table.get(x).value);
        }

        text << allMethods.get(i)->exceptions.size() << ((char)nil);
        for(unsigned int x = 0; x < allMethods.get(i)->exceptions.size(); x++) {
            ExceptionTable &et=allMethods.get(i)->exceptions.get(x);
            text << mi64_tostr(allMethods.get(i)->exceptions.get(x).handler_pc);
            text << mi64_tostr(allMethods.get(i)->exceptions.get(x).end_pc);
            text << allMethods.get(i)->exceptions.get(x).klass.str() << ((char)nil);
            text << mi64_tostr(allMethods.get(i)->exceptions.get(x).local);
            text << mi64_tostr(allMethods.get(i)->exceptions.get(x).start_pc);
        }
    }

    for(long i = 0; i < allMethods.size(); i++) {
        text << (char)data_byte;
        text << method_to_stream(allMethods.get(i)) << endl;
    }
    text << "\n" << (char)eos;
    return text.str();
}

std::string runtime::generate_meta_section() {
    stringstream meta;

    meta << (char)smeta;

    for(parser* p : parsers) {
        meta << (char)data_file;
        meta << p->getData() << (char)0x0;
        meta << endl;
    }

    meta << "\n" << (char)eos;
    return meta.str();
}

void runtime::generate() {
    file::buffer _ostream;
    _ostream.begin();

    _ostream << generate_header() ;
    _ostream << (char)sdata;
    _ostream << generate_data_section();

    if(c_options.optimize) {
        Optimizer optimizer; // ToDo: make struct OptimizerStat { } to create a total view of how many instructions optimized out in total
        for(unsigned int i = 0; i < allMethods.size(); i++)
        {
            Method* method = allMethods.get(i);
            optimizer.optimize(method);
        }
    }

    _ostream << generate_string_section();
    _ostream << generate_text_section();

    if(c_options.debug && !c_options.strip) {
        _ostream << generate_meta_section();
    }

    // ToDo: create line tabel and meta data

    if(c_options.objDump)
        createDumpFile();

    if(file::write(c_options.out.c_str(), _ostream)) {
        cout << progname << ": error: failed to write out to executable " << c_options.out << endl;
    }
    _ostream.end();
    allMethods.free();
}

string runtime::find_method(int64_t id) {
    for(unsigned int i = 0; i < allMethods.size(); i++) {
        if(allMethods.get(i)->vaddr == id)
            return allMethods.get(i)->getFullName();
    }
    return "";
}

string runtime::find_class(int64_t id) {
    for(unsigned int i = 0; i < classes.size(); i++) {
        if(classes.get(i).vaddr == id)
            return classes.get(i).getFullName();
        else {
            ClassObject &klass = classes.get(i);
            for(unsigned int x = 0; x < klass.childClassCount(); x++) {
                if(klass.getChildClass(x)->vaddr == id)
                    return klass.getChildClass(x)->getFullName();
            }
        }
    }
    return "";
}

void runtime::createDumpFile() {
    file::buffer _ostream;
    _ostream.begin();

    _ostream << "Object Dump file:\n" << "################################\n\n";
    for(unsigned int i =0; i < allMethods.size(); i++) {
        Method* method = allMethods.get(i);
        stringstream tmp;
        tmp << "func:@" << method->vaddr;

        _ostream << tmp.str() << " [" << method->getFullName() << "] "
                 << method->note.getNote("") << "\n\n";
        _ostream << method->getName() << ":\n";
        for(unsigned int x = 0; x < method->code.size(); x++) {
            stringstream ss;
            int64_t x64=method->code.__asm64.get(x);
            ss <<std::hex << "[0x" << x << std::dec << "] " << x << ":" << '\t';

            switch(GET_OP(x64)) {
                case op_NOP:
                {
                    ss<<"nop";
                    _ostream << ss.str();
                    break;
                }
                case op_INT:
                {
                    ss<<"int 0x" << std::hex << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MOVI:
                {
                    ss<<"movi #" << GET_Da(x64) << ", ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x)) ;
                    _ostream << ss.str();
                    break;
                }
                case op_RET:
                {
                    ss<<"ret";
                    _ostream << ss.str();
                    break;
                }
                case op_HLT:
                {
                    ss<<"hlt";
                    _ostream << ss.str();
                    break;
                }
                case op_NEWi:
                {
                    ss<<"newi ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_CHECK_CAST:
                {
                    ss<<"check_cast ";
                    ss<< GET_Da(x64);
                    ss << " // " << find_class(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOV8:
                {
                    ss<<"mov8 ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOV16:
                {
                    ss<<"mov16 ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOV32:
                {
                    ss<<"mov32 ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOV64:
                {
                    ss<<"mov64 ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_PUSHR:
                {
                    ss<<"pushr ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_ADD:
                {
                    ss<<"add ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_SUB:
                {
                    ss<<"sub ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_MUL:
                {
                    ss<<"mul ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_DIV:
                {
                    ss<<"div ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_MOD:
                {
                    ss<<"mod ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_POP:
                {
                    ss<<"pop";
                    _ostream << ss.str();
                    break;
                }
                case op_INC:
                {
                    ss<<"inc ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_DEC:
                {
                    ss<<"dec ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOVR:
                {
                    ss<<"movr ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOVX:
                {
                    ss<<"movx ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_LT:
                {
                    ss<<"lt ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_BRH:
                {
                    ss<<"brh";
                    _ostream << ss.str();
                    break;
                }
                case op_BRE:
                {
                    ss<<"bre";
                    _ostream << ss.str();
                    break;
                }
                case op_IFE:
                {
                    ss<<"ife";
                    _ostream << ss.str();
                    break;
                }
                case op_IFNE:
                {
                    ss<<"ifne";
                    _ostream << ss.str();
                    break;
                }
                case op_GT:
                {
                    ss<<"gt ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_GTE:
                {
                    ss<<"ge ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_LTE:
                {
                    ss<<"le ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOVL:
                {
                    ss<<"movl " << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_RMOV:
                {
                    ss<<"rmov ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOV:
                {
                    ss<<"mov ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MOVD:
                {
                    ss<<"movd ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MOVBI:
                {
                    ss<<"movbi #" << GET_Da(x64) << ", #";
                    ss<< method->code.__asm64.get(++x);
                    _ostream << ss.str();
                    break;
                }
                case op_SIZEOF:
                {
                    ss<<"sizeof ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_PUT:
                {
                    ss<<"put ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_PUTC:
                {
                    ss<<"_putc ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_CHECKLEN:
                {
                    ss<<"chklen ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_MOVU8:
                {
                    ss<<"";
                    _ostream << ss.str();
                    break;
                }
                case op_MOVU16:
                {
                    ss<<"";
                    _ostream << ss.str();
                    break;
                }
                case op_MOVU32:
                {
                    ss<<"";
                    _ostream << ss.str();
                    break;
                }
                case op_MOVU64:
                {
                    ss<<"";
                    _ostream << ss.str();
                    break;
                }
                case op_MOVN:
                {
                    ss<<"movn #" << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_GOTO:
                {
                    ss<<"goto @" << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MOVG:
                {
                    ss<<"movg @"<< GET_Da(x64);
                    ss << " // @"; ss << find_class(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_LOADX:
                {
                    ss<<"loadx ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_NEWSTR:
                {
                    ss<<"newstr @" << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_PUSHREF:
                {
                    ss<<"pushref";
                    _ostream << ss.str();
                    break;
                }
                case op_DELREF:
                {
                    ss<<"del_ref";
                    _ostream << ss.str();
                    break;
                }
                case op_INIT_FRAME:
                {
                    ss<<"iframe";
                    _ostream << ss.str();
                    break;
                }
                case op_CALL:
                {
                    ss<<"call @" << GET_Da(x64) << " // <";
                    ss << find_method(GET_Da(x64)) << ">";
                    _ostream << ss.str();
                    break;
                }
                case op_NEW_CLASS:
                {
                    ss<<"new_class @" << GET_Da(x64);
                    ss << " // "; ss << find_class(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_SMOV:
                {
                    ss<<"smov ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << '+';
                    if(GET_Cb(x64)<0) ss<<"[";
                    ss<<GET_Cb(x64);
                    if(GET_Cb(x64)<0) ss<<"]";
                    _ostream << ss.str();
                    break;
                }
                case op_SMOVR:
                {
                    ss<<"smovr ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << '+';
                    if(GET_Cb(x64)<0) ss<<"[";
                    ss<<GET_Cb(x64);
                    if(GET_Cb(x64)<0) ss<<"]";
                    _ostream << ss.str();
                    break;
                }
                case op_SMOVOBJ:
                {
                    ss<<"smovobj @" << GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IADD:
                {
                    ss<<"iadd ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_ISUB:
                {
                    ss<<"isub ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IMUL:
                {
                    ss<<"imul ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IDIV:
                {
                    ss<<"idiv ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IMOD:
                {
                    ss<<"imod ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", #";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_SLEEP:
                {
                    ss<<"sleep ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_TEST:
                {
                    ss<<"test ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_LOCK:
                {
                    ss<<"_lck ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_ULOCK:
                {
                    ss<<"ulck";
                    _ostream << ss.str();
                    break;
                }
                case op_EXP:
                {
                    ss<<"exp ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_ADDL:
                {
                    ss<<"addl ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_SUBL:
                {
                    ss<<"subl ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MULL:
                {
                    ss<<"mull ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_DIVL:
                {
                    ss<<"divl ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MODL:
                {
                    ss<<"modl #";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_MOVSL:
                {
                    ss<<"movsl #";
                    ss<< GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_DEL:
                {
                    ss<<"del";
                    _ostream << ss.str();
                    break;
                }
                case op_MOVND:
                {
                    ss<<"movnd ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_SDELREF:
                {
                    ss<<"sdelref";
                    _ostream << ss.str();
                    break;
                }
                case op_NEW_OBJ_ARRY:
                {
                    ss<<"new_objarry ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_NOT: //c
                {
                    ss<<"not ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_SKP:// d
                {
                    ss<<"skp @";
                    ss<< GET_Da(x64);
                    ss << " // pc = " << (x + GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_LOADF: //c
                {
                    ss<<"loadf ";
                    ss<< Asm::registrerToString(GET_Ca(x64)) << ',';
                    ss<<GET_Cb(x64);
                    ss << " // store pc at <@" << (x + GET_Cb(x64)) << ">";
                    _ostream << ss.str();
                    break;
                }case op_IADDL:
                {
                    ss<<"iaddl ";
                    ss<< GET_Ca(x64) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_ISUBL:
                {
                    ss<<"isubl #";
                    ss<< GET_Ca(x64) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IMULL:
                {
                    ss<<"imull #";
                    ss<< GET_Ca(x64) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IDIVL:
                {
                    ss<<"idivl #";
                    ss<< GET_Ca(x64) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_IMODL:
                {
                    ss<<"imodl #";
                    ss<< GET_Ca(x64) << ", @";
                    ss<<GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_POPR:// d
                {
                    ss<<"popr ";
                    ss<< Asm::registrerToString(GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_SHL:
                {
                    ss<<"shl ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_SHR:
                {
                    ss<<"shr ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    ss<< " -> ";
                    ss<< Asm::registrerToString(method->code.__asm64.get(++x));
                    _ostream << ss.str();
                    break;
                }
                case op_TNE:
                {
                    ss<<"tne ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_POPREF:
                {
                    ss<<"popref";
                    _ostream << ss.str();
                    break;
                }
                case op_MUTL:
                {
                    ss<<"mutl ";
                    ss<<GET_Da(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_SKPE:
                {
                    ss<<"skpe ";
                    ss<<GET_Da(x64);
                    ss << " // pc = " << (x + GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_SKPNE:
                {
                    ss<<"skpne ";
                    ss<<GET_Da(x64);
                    ss << " // pc = " << (x + GET_Da(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_AND:
                {
                    ss<<"and ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_uAND:
                {
                    ss<<"uand ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_OR:
                {
                    ss<<"or ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_uNOT:
                {
                    ss<<"unot ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< Asm::registrerToString(GET_Cb(x64));
                    _ostream << ss.str();
                    break;
                }
                case op_ANDL:
                {
                    ss<<"andl ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_ORL:
                {
                    ss<<"orl ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_NOTL:
                {
                    ss<<"notl ";
                    ss<< Asm::registrerToString(GET_Ca(x64));
                    ss<< ", ";
                    ss<< GET_Cb(x64);
                    _ostream << ss.str();
                    break;
                }
                case op_THROW:
                {
                    ss<<"throw ";
                    _ostream << ss.str();
                    break;
                }
                default:
                    ss << "? (" << GET_OP(x64) << ")";
                    _ostream << ss.str();
                    break;
            }

            _ostream << "\n";
        }

        _ostream << '\n';
    }

    if(file::write((c_options.out + ".asm").c_str(), _ostream)) {
        cout << progname << ": error: failed to write out to dump file " << c_options.out << endl;
    }
    _ostream.end();
}

void runtime::Generator::setupVariable(m64Assembler &assembler, int64_t address) {
    int64_t i64;

    assembler.push_i64(SET_Di(i64, op_MOVL, variableOffset(address)));
}

int64_t runtime::Generator::variableOffset(int64_t address) {
    if(runtime::Generator::instance->current_scope()->type == scope_instance_block) {
        /*
         * We add 1 to the local address because self will be refrence 0
         * on the stack
         */
        address++;
    }
    return address;
}

void runtime::Generator::assignValue(m64Assembler &assembler, Expression &expression, token_entity entity) {

}
