#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "prs.tab.hh"

typedef enum ASTType{

} node_type;

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

extern std::shared_ptr<MinSHProgram> g_progAST;

/* TODO: design parser functions */

/**
 * @param
 * @return
 **/

#endif