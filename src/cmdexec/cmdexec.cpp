#include <unistd.h>
#include <vector>
#include <debug/dbg.h>
#include "cmdexec.h"





int exec_simplecommand(std::shared_ptr<SimpleCommand> _cmd, int _pipefd[2]) {


}

int exec_command(std::shared_ptr<Command> _cmd, int _pipefd[2]) {

    switch (_cmd->_type) {
    case AST_SIMPLE_CMD:

        break;    
    default:
        Panic("unknown command type", DEBUG_ERR, 402);
        break;
    }

    return 0;
}

int exec_pipeline(std::shared_ptr<Pipeline> _cmd) {
    int pipefd[2];
    // create pipe
    if(pipe(pipefd) == -1) {
        Panic("failed to create pipe", DEBUG_ERR, 401);
        return 1;
    }

    // execute command
    int retVal = 0;
    for(auto cmd : _cmd->_cmdlist) {
        retVal = exec_command(cmd, pipefd);
    }

    return  (_cmd->_bangpref) ?
            (retVal == 0) :         // retVal  = 0 -> 1
            (retVal != 0) ;         // retVal != 0 -> 1
}

int exec_andorcommand(std::shared_ptr<AndOrCommand> _cmd) {
    /* Init */
    int andor = -1;     // &&
    int retVal = 0;     // 0

    for(auto pipeline : _cmd->_pipelinelist) {
        if(andor == -1 && retVal != 0 || andor == 1 && retVal == 0) {
            andor = pipeline->_andorsuf;
            continue;
        }
        retVal = exec_pipeline(pipeline);
        andor = pipeline->_andorsuf;
    }

    return 0;
}

int exec_completecommand(std::shared_ptr<CompleteCommand> _cmd) {

    for(auto andor : _cmd->_andorlist) {
        if(andor->_isasync) {   /** TODO: Async **/

        } else {
            exec_andorcommand(andor);
        }
    }

    return 0;
}