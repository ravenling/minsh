#include "builtin.h"
#include <minsh/minsh.h>
#include <iostream>

void buildin_cmd_init()
{
    p_cd = p_cd_function;
    p_quit = p_quit_function;
    p_history = p_history_function;
    buildin_cmd.insert(pair<std::string, int(*)>("cd", p_cd));
    buildin_cmd.insert(pair<std::string, int(*)>("quit", p_quit));
    buildin_cmd.insert(pair<std::string, int(*)>("history", p_history));
}

void show_buildin_cmd()
{
    for (auto &i : buildin_cmd)
    {
        std::cout << i.first << std::endl;
    }
}

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
        while (cmd[0] == " ")
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
        while (cmd[0] == " ")
        {
            cmd.erase(0, 1);
        }
        int num_history = atoi(cmd.c_str());
        if(num_history <= 0){
            std::cout<<"history number error \n"<<std::endl;
        }
        auto it = cmd_history.end();
        it -= (num_history - 1) ;
        for (; it != cmd_history.end())
        {
            std::cout << *it << std::endl;
        }
    }
}