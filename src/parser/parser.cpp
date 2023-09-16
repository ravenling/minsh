#include <minsh/minsh.h>
#include "parser.h"

std::shared_ptr<CompleteCommand> cmdAST;

bool read_command() {
    // reset location for yy::parser::error
    ploc.initialize();

    // create parser
    yy::parser cmdParser;
    
    if(cmdParser() != 0) {
        MinSH::reset_buf();
        return false;
    }

    // set to MinSH
    MinSH::set_cmpcmd(cmdAST);

    // debug print
    MinSH::get_cmpcmd()->debug_print();

    return true;
}