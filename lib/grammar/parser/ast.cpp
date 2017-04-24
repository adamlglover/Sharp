//
// Created by bknun on 1/7/2017.
//
#include "ast.h"
#include "../runtime.h"


ast_types ast::gettype()
{
    return type;
}

ast* ast::getparent()
{
    return parent;
}

long ast::getsubastcount()
{
    return sub_asts.size();
}

ast* ast::getsubast(long at)
{
    if(numAsts == 0 || at >= numAsts) {
        stringstream ss;
        ss << "internal error, ast not found at index " << at;
        throw runtime_error(ss.str());
    }
    return &sub_asts.get(at);
}

long ast::getentitycount()
{
    return numEntities;
}

token_entity ast::getentity(long at)
{
    if(numEntities == 0) return token_entity();
    return entities.get(at);
}

void ast::add_entity(token_entity entity)
{
    numEntities++;
    entities.add(entity);
}

void ast::add_ast(ast _ast)
{
    numAsts++;
    sub_asts.add(_ast);
}

void ast::free() {
    this->entities.free();

    ast* pAst;
    for(int64_t i = 0; i < this->sub_asts.size(); i++)
    {
        pAst = &this->sub_asts.get(i);
        pAst->free();
    }

    numAsts = 0;
    numEntities = 0;
    this->type = ast_none;
    this->parent = NULL;
    this->sub_asts.free();
}

void ast::freesubs() {
    ast* pAst;
    for(int64_t i = 0; i < this->sub_asts.size(); i++)
    {
        pAst = &this->sub_asts.get(i);
        pAst->free();
    }

    numAsts = 0;
    this->sub_asts.free();
}

void ast::freelastsub() {
    ast* pAst = &this->sub_asts.get(this->sub_asts.size()-1);
    pAst->free();
    numAsts--;
    this->sub_asts.pop_back();
}

void ast::freeentities() {
    numEntities = 0;
    this->entities.free();
}

void ast::freelastentity() {
    numEntities--;
    this->entities.pop_back();
}

bool ast::hassubast(ast_types at) {
    for(unsigned int i = 0; i < sub_asts.size(); i++) {
        if(sub_asts.get(i).gettype() == at)
            return true;
    }
    return false;
}

bool ast::hasentity(token_type t) {

    token_entity e;
    for(unsigned int i = 0; i < entities.size(); i++) {
        e = entities.at(i);
        if(e.gettokentype() == t)
            return true;
    }
    return false;
}

ast *ast::getsubast(ast_types at) {
    ast* pAst;
    for(unsigned int i = 0; i < sub_asts.size(); i++) {
        pAst = &sub_asts.get(i);
        if(pAst->gettype() == at)
            return pAst;
    }
    return NULL;
}

token_entity ast::getentity(token_type t) {
    token_entity e;
    for(unsigned int i = 0; i < entities.size(); i++) {
        e = entities.at(i);
        if(e.gettokentype() == t)
            return e;
    }
    return token_entity();
}

ast *ast::getsubast_after(ast_types at) {
    bool found = false;
    for(unsigned int i = 0; i < sub_asts.size(); i++) {
        if(found)
            return &sub_asts.get(i);
        if(sub_asts.get(i).gettype() == at) {
            found = true;
        }
    }
    return NULL;
}

// TODO: call this method for expressions
// tODO: add param bool override (default true) to override the encapsulation
void ast::encapsulate(ast_types at) {

    add_ast(ast(this, at, this->line, this->col));
    ast* encap = getsubast(getsubastcount()-1);

    for(unsigned int i = 0; i < sub_asts.size(); i++) {
        if(sub_asts.get(i).type != at)
            encap->add_ast(sub_asts.get(i));
    }

    for(unsigned int i = 0; i < entities.size(); i++) {
            encap->add_entity(entities.get(i));
    }

    readjust:
        for(unsigned int i = 0; i < sub_asts.size(); i++) {
            if(sub_asts.get(i).type != at) {
                sub_asts.remove(i);
                goto readjust;
            }
        }

    numAsts = 1;
    numEntities = 0;
    entities.free();
}

void ast::settype(ast_types t) {
    type = t;
}

