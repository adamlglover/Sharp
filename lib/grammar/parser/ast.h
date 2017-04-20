//
// Created by bknun on 1/7/2017.
//

#ifndef SHARP_AST_H
#define SHARP_AST_H

#include <list>
#include "tokenizer/tokenentity.h"
#include "../../util/List2.h"

enum ast_types
{
    ast_class_decl,
    ast_import_decl,
    ast_module_decl,
    ast_macros_decl,
    ast_method_decl,
    ast_construct_decl,
    ast_label_decl,
    ast_operator_decl,
    ast_var_decl,
    ast_value,
    ast_value_list,
    ast_utype_arg_list,
    ast_utype_arg_list_opt,
    ast_vector_array,
    ast_utype_arg,
    ast_utype_arg_opt,
    ast_expression,
    ast_primary_expr,
    ast_dotnotation_call_expr,
    ast_utype,
    ast_block,
    ast_finally_block,
    ast_assembly_block,
    ast_catch_clause,
    ast_method_return_type,
    ast_return_stmnt,
    ast_statement,
    ast_if_statement,
    ast_elseif_statement,
    ast_else_statement,
    ast_trycatch_statement,
    ast_throw_statement,
    ast_continue_statement,
    ast_break_statement,
    ast_goto_statement,
    ast_while_statement,
    ast_assembly_statement,
    ast_for_statement,
    ast_foreach_statement,
    ast_type_identifier,
    ast_refrence_pointer,
    ast_modulename,
    ast_literal,

    /**
     * Encapsulated ast's to make processing expressions easier
     */
    ast_literal_e,
    ast_utype_class_e,
    ast_dot_not_e,
    ast_self_e,
    ast_base_e,
    ast_null_e,
    ast_new_e,
    ast_not_e,
    ast_post_inc_e,
    ast_arry_e,
    ast_dot_fn_e,
    ast_cast_e,
    ast_pre_inc_e,
    ast_paren_e,
    ast_vect_e,
    ast_add_e,

    ast_none
};

class ast
{
public:
    ast(ast* parent, ast_types type, int line, int col)
            :
            type(type),
            parent(parent),
            line(line),
            col(col),
            numEntities(0),
            numAsts(0)
    {
        sub_asts = new List<ast>();
        entities = new List<token_entity>();
        sub_asts->init();
        entities->init();
    }

    ast()
            :
            type(ast_none),
            parent(NULL),
            line(0),
            col(0),
            numEntities(0),
            numAsts(0)
    {
        sub_asts = new List<ast>();
        entities = new List<token_entity>();
        sub_asts->init();
        entities->init();
    }

    void encapsulate(ast_types at);

    ast_types gettype();
    ast* getparent();
    long getsubastcount();
    ast *getsubast(long at);
    ast *getsubast(ast_types at);
    ast *getsubast_after(ast_types at);
    bool hassubast(ast_types at);
    bool hasentity(token_type t);
    void freesubs();
    long getentitycount();
    token_entity getentity(long at);
    token_entity getentity(token_type t);

    void add_entity(token_entity entity);
    void add_ast(ast _ast);
    void free();

    void freeentities();
    void freelastsub();
    void freelastentity();

    int line, col;
    long numEntities, numAsts;

private:
    ast_types type;
    ast *parent;
    List<ast> *sub_asts;
    List<token_entity> *entities;

};

#endif //SHARP_AST_H
