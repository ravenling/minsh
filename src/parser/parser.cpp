#include <minsh/minsh.h>
#include "parser.h"

std::shared_ptr<CompleteCommand> cmdAST;

bool read_command() {
    // reset current history
    MinSH::init_current_his();

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

    // add history
    MinSH::add_history();

    // debug print
    //MinSH::get_cmpcmd()->debug_print();

    return true;
}