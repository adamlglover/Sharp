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
    return numAsts;
}

ast* ast::getsubast(long at)
{
    if(numAsts == 0 || at >= numAsts) return NULL;
    return &sub_asts->get(at);
}

long ast::getentitycount()
{
    return numEntities;
}

token_entity ast::getentity(long at)
{
    if(numEntities == 0) return token_entity();
    return entities->get(at);
}

void ast::add_entity(token_entity entity)
{
    numEntities++;
    entities->add(entity);
}

void ast::add_ast(ast _ast)
{
    numAsts++;
    sub_asts->add(_ast);
}

void ast::free() {
    if(this->entities != NULL) {
        this->entities->free();
        delete (this->entities); this->entities = NULL;
    }

    ast* pAst;
    for(int64_t i = 0; i < this->sub_asts->size(); i++)
    {
        pAst = &this->sub_asts->get(i);
        pAst->free();
    }

    numAsts = 0;
    numEntities = 0;
    this->type = ast_none;
    this->parent = NULL;
    if(this->sub_asts != NULL) {
        this->sub_asts->free();
        delete (this->sub_asts); this->sub_asts = NULL;
    }
}

void ast::freesubs() {
    ast* pAst;
    for(int64_t i = 0; i < this->sub_asts->size(); i++)
    {
        pAst = &this->sub_asts->get(i);
        pAst->free();
    }

    numAsts = 0;
    this->sub_asts->free();
}

void ast::freelastsub() {
    ast* pAst = &this->sub_asts->get(this->sub_asts->size()-1);
    pAst->free();
    numAsts--;
    this->sub_asts->pop_back();
}

void ast::freeentities() {
    numEntities = 0;
    this->entities->free();
}

void ast::freelastentity() {
    numEntities--;
    this->entities->pop_back();
}

bool ast::hassubast(ast_types at) {
    for(unsigned int i = 0; i < sub_asts->size(); i++) {
        if(sub_asts->get(i).gettype() == at)
            return true;
    }
    return false;
}

bool ast::hasentity(token_type t) {

    token_entity e;
    for(unsigned int i = 0; i < entities->size(); i++) {
        e = entities->at(i);
        if(e.gettokentype() == t)
            return true;
    }
    return false;
}

ast *ast::getsubast(ast_types at) {
    ast* pAst;
    for(unsigned int i = 0; i < sub_asts->size(); i++) {
        pAst = &sub_asts->get(i);
        if(pAst->gettype() == at)
            return pAst;
    }
    return NULL;
}

token_entity ast::getentity(token_type t) {
    token_entity e;
    for(unsigned int i = 0; i < entities->size(); i++) {
        e = entities->at(i);
        if(e.gettokentype() == t)
            return e;
    }
    return token_entity();
}

ast *ast::getsubast_after(ast_types at) {
    bool found = false;
    for(unsigned int i = 0; i < sub_asts->size(); i++) {
        if(found)
            return &sub_asts->get(i);
        if(sub_asts->get(i).gettype() == at) {
            found = true;
        }
    }
    return NULL;
}

// TODO: call this method for expressions
// tODO: add param bool override (default true) to override the encapsulation
void ast::encapsulate(ast_types at) {
    unencapsulate();

    add_ast(ast(this, at, this->line, this->col));
    ast* encap = getsubast(getsubastcount()-1);

    for(unsigned int i = 0; i < sub_asts->size(); i++) {
        if(sub_asts->get(i).type != at)
            encap->add_ast(sub_asts->get(i));
    }

    for(unsigned int i = 0; i < entities->size(); i++) {
            encap->add_entity(entities->get(i));
    }
    numAsts = 1;
    numEntities = 0;
    this->entities->free();

    readjust:
        for(unsigned int i = 0; i < sub_asts->size(); i++) {
            if(sub_asts->get(i).type != at) {
                sub_asts->remove(i);
                goto readjust;
            }
        }
}

void ast::unencapsulate() {
    bool unencap = false;
    for(unsigned int i = 0; i < sub_asts->size(); i++) {
        if(sub_asts->get(i).type >= ast_literal_e && sub_asts->get(i).type < ast_none)
            unencap = true;
    }

    if(unencap) {
        ast* encap = getsubast(0);

        for(unsigned int i = 0; i < encap->sub_asts->size(); i++) {
                add_ast(encap->sub_asts->get(i));
        }

        for(unsigned int i = 0; i < encap->entities->size(); i++) {
            add_entity(encap->entities->get(i));
        }
        encap->free();
        sub_asts->remove(0); // remove the encap
    }
}

