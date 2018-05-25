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

void tokenizer::parse_lines() {
    stringstream line;

    for(long i = 0; i < len; i++) {
        if(toks[i] == '\n') {
            lines.add(line.str());
            line.str("");
        } else
            line << toks[i];
    }

    if(!line.str().empty())
        lines.add(line.str());
}

static inline bool comment_start(const char c0, const char c1, int& mode)
{
    mode = 0;
    if ('/' == c0)
    {
        if ('/' == c1) { mode = 1;}
        else if ('*' == c1) { mode = 2;}
    }
    return (mode != 0);
}

static inline bool comment_end(const char c0, const char c1, const int mode)
{
    return ((1 == mode) && ('\n' == c0)) ||
           ((2 == mode) && ( '*' == c0) && ('/' == c1));
}

void tokenizer::parse() {
    if(len == 0)
        return;

    parse_lines();
    errors = new ErrorManager(lines, file, false, c_options.aggressive_errors);

    for(;;) {

        start:
        invalidate_whitespace:
        while(!isend() && iswhitespace(current))
        {
            if(current == '\n')
            {
                newline();
            }

            advance();
        }

        // invalidate_comments
        int mode = 0;
        if(isend() || peekend(1))
            goto scan;
        else if (!comment_start(current,peek(1),mode))
            goto scan;

        col+=2; cursor+=2;
        while (!isend() && !comment_end(current,peek(1),mode))
        {
            if(current == '\n'){
                newline();
            }

            advance();
        }

        if(current == '\n'){
            newline();
        }

        if (!isend())
        {
            cursor += mode;
            goto invalidate_whitespace;
        } else if(isend() && !comment_end(current,peek(1),mode))
        {
            errors->createNewError(GENERIC, line, col, "expected closed comment before end of file");
        }

        scan:
        if(isend())
        {
            goto end;
        } else if(issymbol(current)) // <, >, <<, etc.
        {
            if(!peekend(1)) // += -= /=
            {
                token_type  type = NONE;

                char chs[] = {current, peek(1)};

                if ((chs[0] == '<') && (chs[1] == '=')) type = _LTE;
                else if ((chs[0] == '>') && (chs[1] == '=')) type = _GTE;
                else if ((chs[0] == '!') && (chs[1] == '=')) type = NOTEQ;
                else if ((chs[0] == '=') && (chs[1] == '=')) type = EQEQ;
                else if ((chs[0] == '<') && (chs[1] == '<')) type = SHL;
                else if ((chs[0] == '>') && (chs[1] == '>')) type = SHR;
                else if ((chs[0] == '&') && (chs[1] == '&')) type = ANDAND;
                else if ((chs[0] == '|') && (chs[1] == '|')) type = OROR;
                else if ((chs[0] == '-') && (chs[1] == '>')) type = PTR;
                else if ((chs[0] == '+') && (chs[1] == '+')) type = _INC;
                else if ((chs[0] == '-') && (chs[1] == '-')) type = _DEC;
                else if ((chs[0] == '+') && (chs[1] == '=')) type = PLUSEQ;
                else if ((chs[0] == '*') && (chs[1] == '=')) type = MULTEQ;
                else if ((chs[0] == '-') && (chs[1] == '=')) type = MINUSEQ;
                else if ((chs[0] == '/') && (chs[1] == '=')) type = DIVEQ;
                else if ((chs[0] == '&') && (chs[1] == '=')) type = ANDEQ;
                else if ((chs[0] == '|') && (chs[1] == '=')) type = OREQ;
                else if ((chs[0] == '^') && (chs[1] == '=')) type = XOREQ;
                else if ((chs[0] == '%') && (chs[1] == '=')) type = MODEQ;

                if (NONE != type)
                {
                    string s;
                    s.append(1,chs[0]);
                    s.append(1,chs[1]);

                    entites.add(token_entity(s, SINGLE, col, line, type));
                    cursor += 2;
                    goto start;
                }
            }

            if ('<' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, LESSTHAN));
            else if ('>' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, GREATERTHAN));
            else if (';' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, SEMICOLON));
            else if (':' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, COLON));
            else if ('+' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, PLUS));
            else if ('-' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, MINUS));
            else if ('*' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, MULT));
            else if (',' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, COMMA));
            else if ('=' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, ASSIGN));
            else if ('#' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, HASH));
            else if ('!' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, NOT));
            else if ('/' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, _DIV));
            else if ('%' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, _MOD));
            else if ('(' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, LEFTPAREN));
            else if (')' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, RIGHTPAREN));
            else if ('{' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, LEFTCURLY));
            else if ('}' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, RIGHTCURLY));
            else if ('.' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, DOT));
            else if ('[' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, LEFTBRACE));
            else if (']' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, RIGHTBRACE));
            else if ('&' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, AND));
            else if ('|' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, OR));
            else if ('^' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, XOR));
            else if ('?' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, QUESMK));
            else if ('$' == current)
                entites.add(token_entity(string(1, current), SINGLE, col, line, DOLLAR));
            else
            {
                errors->createNewError(UNEXPECTED_SYMBOL, line, col, " `" + string(1, current) + "`");
            }

            advance();
            goto start;
        } else if(isletter(current) || current == '_') // int, dave, etc. _992
        {
            stringstream var;
            bool hasletter = false;

            while(!isend() && (isalnum(current) || current == '_'))
            {
                if(isletter(current))
                    hasletter = true;

                var << current;
                advance();
            }

            if(!hasletter)
                errors->createNewError(GENERIC, line, col, " expected at least 1 letter in identifier `" + var.str() + "`");
            else
                entites.add(token_entity(var.str(), IDENTIFIER, col, line));

            goto start;
        } else if(isnumber(current)) // 0xff2ea, 3.14159, 773_721_3376, etc 0x32f
        {
            if(current == '0' && peek(1) == 'x') {
                stringstream num;
                bool underscore_ok = false;

                num << "0x";
                col+=2; cursor+=2;
                while(!isend() && (ishexnum(current) || current == '_'))
                {
                    if(ishexnum(current))
                        underscore_ok = true;
                    else {
                        if(!underscore_ok) {
                            errors->createNewError(ILLEGAL_NUMBER_FORMAT, line, col, ", unexpected or illegally placed underscore");
                            break;
                        }

                        advance();
                        continue;
                    }

                    num << current;
                    advance();
                }

                entites.add(token_entity(num.str(), HEX_LITERAL, col, line));
                goto start;
            } else {
                /*
                Attempt to match a valid numeric value in one of the following formats:
                1. 123456
                2. 123.456
                3. 1_23.456e3
                4. 123.456E3
                5. 123.456e+3
                6. 123.456E+3
                7. 123.456e-3
                8. 123.456E-3
                9. 12345e5
             */
            }
        }

    }

    end:
    entites.push_back(*EOF_token);
}
