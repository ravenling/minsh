#ifndef PARSER_H
#define PARSER_H

#include <debug/dbg.h>
#include "ast.h"
#include "prs.tab.hh"

enum TokenType {
    TK_NEW,
    TK_WORD,
    TK_ASSIGNMENT_WORD,
    TK_NAME,
    TK_NEWLINE,
    TK_IO_NUMBER,
    TK_OPERATOR,

    /* reserve words */
    TK_CASE,
    TK_DO, 
    TK_DONE, 
    TK_ELIF, 
    TK_ELSE, 
    TK_ESAC, 
    TK_FI,
    TK_FOR,
    TK_IF, 
    TK_IN, 
    TK_THEN, 
    TK_UNTIL, 
    TK_WHILE,
};

struct Token {
    uint8_t _type;
    std::string _val;
    char _delim;
};

typedef enum OperatorType {

    /* redirection */
    RD_LESS,            // <
    RD_GREAT,           // >
    RD_DLESS,           // <<
    RD_DGREAT,          // >>
    RD_LESSAND,         // <&
    RD_GREATAND,        // >&
    RD_LESSGREAT,       // <>
    RD_DLESSDASH,       // <<-
    RD_CLOBBER,         // >|

    /* other operator */
    OP_DAND,            // &&
    OP_DPIPE,           // ||
    OP_PIPE,            // |
    OP_BANG,            // !
    OP_AND,             // &
    OP_SEMICOLON,       // ;
    OP_LBRACE,          // {
    OP_RBRACE,          // }

} op_type;

extern std::shared_ptr<CompleteCommand> cmdAST;

namespace yy {
    parser::symbol_type yylex();
};

extern yy::parser::location_type ploc;

namespace yy {
/**
 * @return      symbol
 * @brief       token to symbol
 * @warning     DO NOT use this function out of *test* or *parser*
 **/
parser::symbol_type get_symbol(Token &_token);
};

/**
 * @return      true: success ; false: error 
 * @brief       TK_WORD to TK_WORD/TK_ASSIGNWORD/...
 * @warning     DO NOT use this function out of *test* or *parser*
 **/
bool grammer_conv(Token &_token);

/**
 * @return      token
 * @brief       get new token
 * @warning     DO NOT use this function out of *test* or *parser*
 **/
Token get_token();

/**
 * @return      true: success ; false: error 
 * @brief       read newline to MinSH::_buf
 * @warning     DO NOT use this function out of *test* or *parser*
 **/
bool read_newline();

/**
 * @return      true: success ; false: error 
 * @brief       read commands and send to MinSH::_cmd
 **/
bool read_command();

#endif