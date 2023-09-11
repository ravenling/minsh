#ifndef MINSH_H
#define MINSH_H

#include <string>

/* TODO: complete the MinSH class */
class MinSH{
private:
    static std::string _pwd;
public:
    static void set_pwd(std::string _newpwd) {_pwd = _newpwd;};
    static std::string get_pwd() {return _pwd;}
};

/* TODO: main loop frame work, use functions from parser.h */

/**
 * @return      MinSH exit code
 * @brief       MinSH main loop
 **/
int minsh_main_loop();

#endif