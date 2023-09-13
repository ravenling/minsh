#ifndef MINSH_H
#define MINSH_H

#include <map>
#include <string>
#include <debug/dbg.h>
#include <vector>
/* TODO: complete the MinSH class */

class MinSH{

private:

    static std::string _pwd;
    static std::map<std::string, std::string> _alias;
public:

    static void set_pwd(std::string & _newpwd) {_pwd = std::string(_newpwd);}
    static std::string get_pwd() {return _pwd;}
    static std::string get_alias(std::string & _cmd) {return _alias[_cmd];}
    static void set_alias(std::string & _ali, std::string & _cmd) {
        Assert(_ali.size() > 0 && _cmd.size() > 0, "Parameter \"_ali\" and \"_cmd\" in \"set_alias\" cannot be empty string", 201);
        _alias[_ali] = std::string(_cmd);
    }
    static void del_alias(std::string & _ali, bool _isall) {
        if(_isall) {
            _alias.clear();
        }
        else {
            if(_alias.count(_ali) <= 0) {
                Log("Trying to delete an unavalible alias", DEBUG_WARN);
            }
            else {
                _alias.erase(_ali);
            }
        }
    }

};

/* TODO: main loop frame work, use functions from parser.h */

/**
 * @return      MinSH exit code
 * @brief       MinSH main loop
 **/
int minsh_main_loop();

#endif