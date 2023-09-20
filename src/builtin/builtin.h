#ifndef BUILDIN_H
#define BUILDIN_H

#include <vector>
#include <string>
#include <map>
#include <stdlib.h>

extern std::map<std::string,int(*)(std::vector<std::string>)> builtin_cmd_tab;

/**
 * @brief init builtin command
 **/
void buildin_cmd_init();

/**
 * @brief MinSH help command
 **/
int minsh_help(std::vector<std::string> _args);

/**
 * @brief MinSH alias command
 **/
int minsh_alias(std::vector<std::string> _args);

/**
 * @brief MinSH unalias command
 **/
int minsh_unalias(std::vector<std::string> _args);

/**
 * @brief MinSH echo command
 **/
int minsh_echo(std::vector<std::string> _args);

/**
 * @brief MinSH cd command
 **/
int minsh_change_directory(std::vector<std::string> _args);

/**
 * @brief MinSH exit command
 **/
int minsh_exit(std::vector<std::string> _args);

/**
 * @brief MinSH history command
 **/
int minsh_history(std::vector<std::string> _args);

#endif