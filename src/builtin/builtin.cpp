#include "builtin.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <minsh/minsh.h>
#include <stdint.h>

std::map<std::string, int(*)(std::vector<std::string>)> builtin_cmd_tab;

std::map<std::string, std::string> help_info = {
    {"help",        "help [cmd]\n\tDisplay information about builtin commands.\n"},
    {"alias",       "alias -p | name=[value]\n\tDefine or display aliases.\n"},
    {"cd",          "cd [dir]\n\tChange the shell working directory.\n"},
    {"echo",        "echo [arg ...]\n\tWrite arguments to the standard output.\n"},
    {"exit",        "exit\n\tExit the shell.\n"},
    {"history",     "history [n]\n\tDisplay history list.\n"},
    {"unalias",     "unalias [-a] name [name ...]\n\tRemove each NAME from the list of defined aliases.\n"},
};


void buildin_cmd_init() {

    builtin_cmd_tab["help"] = minsh_help;
    builtin_cmd_tab["alias"] = minsh_alias;
    builtin_cmd_tab["unalias"] = minsh_unalias;
    builtin_cmd_tab["cd"] = minsh_change_directory;
    builtin_cmd_tab["echo"] = minsh_echo;
    builtin_cmd_tab["history"] = minsh_history;
    builtin_cmd_tab["exit"] = minsh_exit;

    return;
}

int minsh_echo(std::vector<std::string> _args) {
    if(_args.size() != 0) {
        for(size_t index = 0; index < _args.size() - 1; index++) {
            std::cout << _args[index] << " ";
        }
        std::cout << _args.back();
    }
    std::cout << std::endl;
    return 0;
}

int minsh_help(std::vector<std::string> _args) {
    if(_args.size() > 1){
        std::cerr << "too many arguments" << std::endl;
        return 1;
    }

    if(_args.size() == 0) {
        for(auto info : help_info) {
            std::cout << info.first+": " << info.second << std::endl;
        }
    } else {
        if(help_info.count(_args[0]) <= 0) {
            std::cerr << "unknown built-in command '" + _args[0] + "'" << std::endl;
            return 1;
        }
        std::cout << _args[0] + ": " << help_info[_args[0]];
    }
    return 0;
}

int minsh_alias(std::vector<std::string> _args) {
    if(_args.size() > 1){
        std::cerr << "too many arguments" << std::endl;
        return 1;
    }
    if(_args[0] == "-p") {
        MinSH::print_alias();
        return 0;
    }
    if(_args[0].find_first_of('=') == _args[0].size() || _args[0][0] == '=') {
        std::cerr << "invalid expression" << std::endl;
        return 1;
    }
    std::string &assign = _args[0];
    std::string ali = assign.substr(0, assign.find_first_of('='));
    std::string val = assign.substr(assign.find_first_of('=')+1, assign.size()-assign.find_first_of('=')-1);

    MinSH::set_alias(ali, val);

    return 0;
}

int minsh_unalias(std::vector<std::string> _args) {
    if(_args.size() <= 0){
        std::cerr << "need 1 argument at least" << std::endl;
        return 1;
    }
    if(_args[0] == "-a") {
        std::string tmp;
        return MinSH::del_alias(tmp, true);
    }
    int retVal = 0;
    for(auto ali : _args) {
        if(MinSH::del_alias(ali, false) != 0) {
            std::cerr << "undefined alias" << std::endl;
            retVal = 1;
        }
    }
    return retVal;
}

int minsh_change_directory(std::vector<std::string> _args){
    if(_args.size() > 2){
        std::cerr << "too many arguments" << std::endl;
        return 1;
    }
    if(_args.size() == 0){
        // cd
        int chdirRet = chdir(get_home_dir().c_str());
        return chdirRet;
    }
    else{
        // cd /home/linux
        if(_args[0].front() == '~') {
            _args[0] = get_home_dir() + _args[0].substr(1,_args[0].size()-1);
        }
        int chdirRet = chdir(_args[0].c_str());
        if(chdirRet != 0) {
            std::cerr << "invalid path" << std::endl;
        }
        return chdirRet;
    }
}

int minsh_exit(std::vector<std::string> _args){
    std::cout << "Exit MinSH. Byebye :)" << std::endl;
    exit(0);
}

int minsh_history(std::vector<std::string> _args){
    if(_args.size() > 1){
        std::cerr << "too many arguments" << std::endl;
        return 1;
    }

    uint32_t historyNum = UINT32_MAX;

    if(_args.size() == 1) {
        historyNum = strtoul(_args[0].c_str(), nullptr, 10);
    }

    for(uint32_t i = 1; i <= historyNum && i <= MinSH::get_his_count(); i++){
        std::cout << i << "\t" << MinSH::get_history(i) << std::endl;
    }

    return 0;
}
