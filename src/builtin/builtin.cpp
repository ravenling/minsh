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
        int chdirRet = chdir(_args[0].c_str());
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


/*
int p_cd_function(std::string cmd)
{
    if (cmd == "cd")
    {
        int chdir_ret = chdir(home_dir.c_str());
        return chdir_ret;
    }
    else
    {
        cmd = cmd.substr(2, std::string::npos);
        while (cmd.at(0) == ' ')
        {
            cmd.erase(0, 1);
        }
        int chdir_ret = chdir(cmd.c_str());
        return chdir_ret;
    }
}

int p_quit_function()
{
    std::cout << "quit MinSH." << std::endl;
    exit(0);
}

int p_history_function(std::string cmd)
{
    if (cmd == "history")
    {
        for (auto &it : cmd_history)
        {
            std::cout << it << std::endl;
        }
    }
    else
    {
        cmd = cmd.substr(7, std::string::npos);
        while (cmd.at(0) == ' ')
        {
            cmd.erase(0, 1);
        }
        int num_history = atoi(cmd.c_str());
        if(num_history <= 0){
            std::cout<<"history number error \n"<<std::endl;
        }
        auto it = cmd_history.end();
        it -= (num_history - 1) ;
        for (; it != cmd_history.end(); it++)
        {
            std::cout << *it << std::endl;
        }
    }
}
*/