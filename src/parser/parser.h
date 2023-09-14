#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "prs.tab.hh"

typedef enum RedirectionType {
    RD_LESS,            // <
    RD_GREAT,           // >
    RD_DLESS,           // <<
    RD_DGREAT,          // >>
    RD_LESSAND,         // <&
    RD_GREATAND,        // >&
    RD_LESSGREAT,       // <>
    RD_DLESSDASH,       // <<-
    RD_CLOBBER,         // >|
} rd_type;

extern std::shared_ptr<CompleteCommand> g_cmdAST;

namespace yy {
    parser::symbol_type yylex();
};

/* TODO: design parser functions */

/**
 * @param
 * @return      0: success ; 1: error ; -1: read more 
 * @brief       
 **/



#endif