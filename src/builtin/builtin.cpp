#include "builtin.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <minsh/minsh.h>
#include <stdint.h>

std::map<std::string, int(*)(std::vector<std::string>)> builtin_cmd_tab;

void buildin_cmd_init() {

    builtin_cmd_tab["echo"] = minsh_echo;
    builtin_cmd_tab["cd"] = minsh_change_directory;
    builtin_cmd_tab["history"] = minsh_history;
    builtin_cmd_tab["quit"] = minsh_quit;

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

int minsh_help(std::vector<std::string> _args);

int minsh_change_directory(std::vector<std::string> _args){
    if(_args.size() > 2){
        std::cout << "too many arguments" << std::endl;
        return 0;
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
            std::cout << "invalid path" << std::endl;
        }
        return chdirRet;
    }
}

int minsh_quit(std::vector<std::string> _args){
    std::cout << "quit MinSH." << std::endl;
    exit(0);
}

int minsh_history(std::vector<std::string> _args){
    if(_args.size() > 1){
        std::cout << "too many arguments" << std::endl;
        return 0;
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
