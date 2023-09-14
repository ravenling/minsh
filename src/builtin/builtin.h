#ifndef BUILDIN_H
#define BUILDIN_H
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
std::map<std::string,int(*)> buildin_cmd;
std::vector<std::string> cmd_history;
void show_buildin_cmd();
int (*p_cd)(std::string);
int (*p_quit)();
int (*p_history)(std::string);
#endif