#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <memory.h>

#include <vector>
#include <minsh/minsh.h>
#include <debug/dbg.h>
#include "cmdexec.h"

// run external binary
int exec_external(std::string _cmd, std::vector<std::string> _args, int _fd[10], bool _rd[10]) {
    /* redirection */
    for(int i = 0; i < 10; i++) {
        if(!_rd[i]) continue;
        if(dup2(_fd[i], i) == -1) {
            Panic("failed to duplicate file descriptor", false, 401);
            return 1;
        }
    }

    /* execl */
    char **argv = new char*[_args.size()+2];
    argv[0] = new char[_cmd.size() + 1];
    strcpy(argv[0], _cmd.c_str());                  // file path
    for(size_t i = 0; i < _args.size(); i++) {
        argv[i+1] = new char[_args[i].size() + 1];
        strcpy(argv[i+1], _args[i].c_str());
    }
    argv[_args.size()+1] = NULL;
    
    if(execv(_cmd.c_str(), argv) == -1) {
        Panic("failed to execute external program", false, 414);
        exit(1);
    }
 
    return 0;
}

int exec_cmd(std::string _cmd, std::vector<std::string> _args, int _fd[10], bool _rd[10]) {
    int retVal = 1;
    /* execute */

    // check if it is builtin, if it is, execute in the same process
    if(builtin_cmd_tab.count(_cmd) > 0) {
        /* backup fd table */
        int fdBckup[10];
        for(int i = 0; i < 10; i++) {
            if(!_rd[i]) continue;
            fdBckup[i] = open("/tmp", O_TMPFILE | O_RDWR);
            if(fdBckup[i] == -1) {
                Panic("failed to create tmp file", false, 401);
                return 1;
            }
            if(dup2(i, fdBckup[i]) == -1) {
                Panic("failed to duplicate file descriptor", false, 401);
                return 1;
            }
        }

        /* duplicate fd to redirect */
        for(int i = 0; i < 10; i++) {
            if(!_rd[i]) continue;
            if(dup2(_fd[i], i) == -1) {
                Panic("failed to duplicate file descriptor", false, 401);
                return 1;
            }
        }

        retVal = builtin_cmd_tab[_cmd](_args);
        
        /* recover fd table */
        for(int i = 0; i < 10; i++) {
            if(!_rd[i]) continue;
            if(dup2(fdBckup[i], i) == -1) {
                Panic("failed to duplicate file descriptor", false, 401);
                return 1;
            }
        }

        return retVal;
    }

    // find absolute path
    std::string absPath = find_external_command(_cmd);

    if(absPath.size() == 0) {
        Panic("couldn't find command", false, 406);
        return 1;
    }

    // fork & exec
    pid_t subProc = fork();
    if(subProc == -1) {
        Panic("couldn't find command", false, 407);
        return 1;
    }
    if(subProc == 0) {      // child
        exec_external(absPath, _args, _fd, _rd);
    } else {                // parent
        if(waitpid(subProc, &retVal, 0) == -1) {
            Panic("couldn't find command", false, 413);
            return 1;
        }
    }

    return retVal;
}

int redirection_parse(std::shared_ptr<IORedirect> &_presuf, int _fd[10], bool _rd[10]) {
    /* invalid io_number */
    if(_presuf->_ionumber > 9 || _presuf->_ionumber < -1) {
        Panic("invalid IO_NUMBER", false, 409);
        return 1;
    }
    int fd;

    /* create file */
    switch(_presuf->_iofile->_redirtype) {
    case RD_GREAT:
        if((fd = open(_presuf->_iofile->_filename.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666)) == -1) {
            if(remove(_presuf->_iofile->_filename.c_str()) == 0) {
                fd = open(_presuf->_iofile->_filename.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
            }
        }
        if(fd == -1) {
            Panic("failed to create file", false, 412);
            return 1;
        }
        MinSH::add_fd(fd);
        if(_presuf->_ionumber == -1) {      // default
            _rd[1] = true; _fd[1] = fd;
        }
        else {
            _rd[_presuf->_ionumber] = true; _fd[_presuf->_ionumber] = fd;
        }
        break; 
    case RD_LESS:
        fd = open(_presuf->_iofile->_filename.c_str(), O_RDONLY, 0666);
        if(fd == -1) {
            Panic("no such file", false, 410);
            return 1;
        }
        MinSH::add_fd(fd);
        _rd[0] = true; _fd[0] = fd;
        break;
    case RD_DGREAT:
        fd = open(_presuf->_iofile->_filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
        if(fd == -1) {
            Panic("failed to create file", false, 412);
            return 1;
        }
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
                if(MinSH::_envVar.count(tmpvar) > 0) {
                    MinSH::_envVar[tmpvar] = tmpval;
                } else {
                    MinSH::add_var(tmpvar, tmpval);
                }
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

    /* exec */
    return exec_cmd(_cmd->_cmdword, args, fd, rd);    
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
            if(pipefd[1] == -1) {
                Panic("failed to create tmp file", false, 401);
                return 1;
            }
        }

        retVal = exec_command(cmd, pipefd);

        // output -> next input
        if(i != 0) {
            close(pipefd[0]);
        }
        pipefd[0] = pipefd[1];

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


