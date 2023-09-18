#include <unistd.h>
#include <vector>
#include <minsh/minsh.h>
#include <debug/dbg.h>
#include "cmdexec.h"

// run this in sub-process
int exec_cmd(std::string _cmd, std::vector<std::string> _args , int _fd[10], bool _rd[10]) {
    /* duplicate fd to redirect */
    for(int i = 0; i < 10; i++) {
        if(!_rd[i]) continue;
        dup2(_fd[i], i);
    }

    /* execute */
    // check if it is builtin
    if(builtin_cmd_tab.count(_cmd) > 0) {
        return builtin_cmd_tab[_cmd](_args);
    }

    /** TODO: find absolute path **/


}

int redirection_parse(std::shared_ptr<IORedirect> &_presuf, int _fd[10], bool _rd[10]) {
    // create file

    


    // 



    return 0;
}

int prefix_parse(std::vector<std::shared_ptr<PrefixSuffix>> &_prefix, bool _onlyPrefix, int _fd[10], bool _rd[10]) {
    size_t index;
    std::string tmpvar, tmpval;
    std::shared_ptr<PrefixSuffixWord> tmpword;
    std::shared_ptr<IORedirect> tmprd;

    for(auto pre : _prefix) {
        switch(pre->_type) {
        case AST_PRESUF_WORD:       // Assignment
            tmpword = std::dynamic_pointer_cast<PrefixSuffixWord>(pre);
            index = tmpword->_word.find_first_of('=');
            if(index == tmpword->_word.size()) {
                Panic("assignment word has no \"=\"", false, 405);
                return 1;
            }
            tmpvar = tmpword->_word.substr(0, index);
            tmpval = tmpword->_word.substr(index+1, tmpword->_word.size());
            if(_onlyPrefix) {
                MinSH::add_var(tmpvar, tmpval);
            }
            else {
                /** TODO: local var **/


            }
            break;
        case AST_IOREDIRECT:
            tmprd = std::dynamic_pointer_cast<IORedirect>(pre);
            if(redirection_parse(tmprd, _fd, _rd) != 0) {
                return 1;
            }
            break;
        default:
            Panic("unknown prefix type", false, 403);
            return 1;
        }
    }
    return 0;
}

int suffix_parse(std::vector<std::shared_ptr<PrefixSuffix>> &_suffix, std::vector<std::string> &_args, int _fd[10], bool _rd[10]) {

}

int exec_simplecommand(std::shared_ptr<SimpleCommand> _cmd, int _pipefd[2]) {
    int fd[10];
    bool rd[10];
    std::vector<std::string> args;

    /** TODO: prefix  **/
    if(prefix_parse(_cmd->_prefix, _cmd->_cmdword == "", fd, rd) != 0) {
        return 1;
    }

    /** TODO: suffix  **/







}

int exec_command(std::shared_ptr<Command> _cmd, int _pipefd[2]) {

    switch (_cmd->_type) {
    case AST_SIMPLE_CMD:
        exec_simplecommand(std::dynamic_pointer_cast<SimpleCommand>(_cmd), _pipefd);
        break;    
    default:
        Panic("unknown command type", false, 402);
        return 1;
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
    /** TODO: pipeline redirect **/
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


