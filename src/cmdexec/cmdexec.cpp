#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

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



    Panic("couldn't find command", false, 406);
    return 1;
}

int redirection_parse(std::shared_ptr<IORedirect> &_presuf, int _fd[10], bool _rd[10]) {
    /* invalid io_number */
    if(_presuf->_ionumber > 9 || _presuf->_ionumber < -1) {
        Panic("invalid IO_NUMBER", false, 409);
        return 1;
    }
    int fd;

    /** TODO: create file **/
    switch(_presuf->_iofile->_redirtype) {
    case RD_GREAT:
        fd = open(_presuf->_iofile->_filename.c_str(), O_WRONLY | O_CREAT);
        MinSH::add_fd(fd);
        if(_presuf->_ionumber == -1) {      // default
            _rd[1] = true; _fd[1] = fd;
        }
        else {
            _rd[_presuf->_ionumber] = true; _fd[_presuf->_ionumber] = fd;
        }
        break; 
    case RD_LESS:
        fd = open(_presuf->_iofile->_filename.c_str(), O_RDONLY);
        if(fd == -1) {
            Panic("no such file", false, 410);
            return 1;
        }
        MinSH::add_fd(fd);
        _rd[0] = true; _fd[0] = fd;
        break;
    case RD_DGREAT:
        fd = open(_presuf->_iofile->_filename.c_str(), O_WRONLY | O_CREAT | O_APPEND);
        MinSH::add_fd(fd);
        if(_presuf->_ionumber == -1) {      // default
            _rd[1] = true; _fd[1] = fd;
        }
        else {
            _rd[_presuf->_ionumber] = true; _fd[_presuf->_ionumber] = fd;
        }
        break;
    default:
        Panic("redirection type not implemented", false, 408);
        return 1;
    }

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
    std::shared_ptr<PrefixSuffixWord> tmpword;
    std::shared_ptr<IORedirect> tmprd;

    for(auto suf : _suffix) {
        switch(suf->_type) {
        case AST_PRESUF_WORD:       // Arguments
            tmpword = std::dynamic_pointer_cast<PrefixSuffixWord>(suf);
            _args.push_back(tmpword->_word);
            break;
        case AST_IOREDIRECT:
            tmprd = std::dynamic_pointer_cast<IORedirect>(suf);
            if(redirection_parse(tmprd, _fd, _rd) != 0) {
                return 1;
            }
            break;
        default:
            Panic("unknown suffix type", false, 404);
            return 1;
        }
    }
    return 0;
}

int exec_simplecommand(std::shared_ptr<SimpleCommand> _cmd, int _pipefd[2]) {
    int fd[10];
    bool rd[10];
    std::vector<std::string> args;

    for(int i = 0; i < 10; i++) {rd[i] = false;}
    rd[0] = true; fd[0] = _pipefd[0];
    rd[1] = true; fd[1] = _pipefd[1];

    /* prefix  */
    if(prefix_parse(_cmd->_prefix, _cmd->_cmdword == "", fd, rd) != 0) {
        return 1;
    }

    /* suffix  */
    if(suffix_parse(_cmd->_suffix, args, fd, rd) != 0) {
        return 1;
    }

    /* no cmdname */
    if(_cmd->_cmdword == "") {
        return 0;
    }

    /* fork & exec */
    pid_t subproc;
    if((subproc = fork()) == -1) {
        Panic("failed to create sub-process", false, 407);
        return 1;
    }
    if(subproc == 0) { 
        exit(exec_cmd(_cmd->_cmdword, args, fd, rd));
    }
    else {
        int retVal;
        waitpid(subproc, &retVal, 0);
        return retVal;
    }
    
    return 1;
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

    /* close redirection files */
    MinSH::close_all_fd();

    return 0;
}

int exec_pipeline(std::shared_ptr<Pipeline> _cmd) {
    int pipefd[2];
    pipefd[0] = STDIN_FILENO;       // init

    // execute command
    int retVal = 0;

    for(size_t i = 0; i < _cmd->_cmdlist.size(); i++) {
        auto cmd = _cmd->_cmdlist[i];

        // new output
        if(i == _cmd->_cmdlist.size() - 1) {
            pipefd[1] = STDOUT_FILENO;
        }
        else {
            // create a tmp file
            pipefd[1] = open("/tmp", O_TMPFILE | O_RDWR);
        }

        retVal = exec_command(cmd, pipefd);

        // output -> next input
        if(i != 0) {
            close(pipefd[0]);
            pipefd[0] = pipefd[1];
        }

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
        if((andor == -1 && retVal != 0) || (andor == 1 && retVal == 0)) {
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


