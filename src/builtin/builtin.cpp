#include "builtin.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <minsh/showp.h>
std::map<std::string,int(*)(std::vector<std::string>)> builtin_cmd_tab;
void buildin_cmd_init() {
    builtin_cmd_tab["echo"] = minsh_echo;
    //buildin_cmd_tab["cd"] = minsh_change_directory;



    return;
}

int minsh_echo(std::vector<std::string> _args) {
    for(auto s = _args.begin(); s != _args.end(); s++) {
        if(s != _args.end()){
            std::cout << *s << " ";
        }
        else{
            std::cout << *s <<std::endl;
        }
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
    if(_args.size() == 1){
        //cd
        int chdir_ret = chdir(home_dir.c_str());
        return chdir_ret;
    }
    else{
        //cd /home/linux
        int chdir_ret = chdir(_args.at(1).c_str());
        return chdir_ret;
    }
    // if(_args.at(0) == "cd")
    // {
    //     int chdir_ret = chdir(home_dir.c_str());
    //     return chdir_ret;
    // }
    // else
    // {
    //     _args.at(0) = cmd.substr(2, std::string::npos);
    //     while (cmd.at(0) == ' ')
    //     {
    //         cmd.erase(0, 1);
    //     }
    //     int chdir_ret = chdir(cmd.c_str());
    //     return chdir_ret;
    // }
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
    auto s =_args.at(0);
    int history_num = stoi(s);
    for(int i = 0; i <= history_num; i++){
        MinSH::get_history(i);
    }
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