//
// Created by bknun on 11/13/2017.
//

#include "tokenizer.h"
#include "../../runtime.h"

#define iswhitespace(c) \
    ((' '  == c) || ('\n' == c) || \
    ('\r' == c) || ('\t' == c) || \
    ('\b' == c) || ('\v' == c) || \
    ('\f' == c)) \

#define current \
    ((cursor < len) ? \
        toks.at(cursor) \
    : toks.at(len-1)) \

#define newline() \
    col = 0, line++;

#define isend() \
    (cursor>=len)

#define peekend(forward) \
    ((cursor+forward)>=len)

#define issymbol(c) \
    (('+' == c) || ('-' == c) || \
    ('*' == c) || ('/' == c) || \
    ('^' == c) || ('<' == c) || \
    ('>' == c) || ('=' == c) || \
    (',' == c) || ('!' == c) || \
    ('(' == c) || (')' == c) || \
    ('[' == c) || (']' == c) || \
    ('{' == c) || ('}' == c) || \
    ('%' == c) || (':' == c) || \
    ('?' == c) || ('&' == c) || \
    ('|' == c) || (';' == c) || \
    ('!' == c) || ('.' == c) || \
    ('#' == c) || ('$' == c))

#define peek(forward) \
    (((cursor+forward) >= len || (cursor+forward) < 0) ? \
        toks.at(len-1) : toks.at(cursor+forward))

#define isnumber(c) \
    isdigit(c)

#define isletter(c) \
    isalpha(c)

#define ishexnum(c) \
    (isdigit(c) || ((c >= 65) && (c <= 72)) || \
        ((c >= 97) && (c <= 104)))

#define advance() \
    col++; cursor++;

#define tokensLeft() \
    (len - cursor)

#define issign(s) \
    ((s == '+') || (s == '-'))

unsigned long tokenizer::getEntityCount()
{
    return entites.size();
}

List<token_entity>& tokenizer::getEntities()
{
    return entites;
}

void tokenizer::parse() {

}
