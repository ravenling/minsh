#ifndef INIT_H
#define INIT_H
#include <map>
#include <string>
#include <pwd.h>
#include <unistd.h>
#include <iostream>
/* TODO: minsh init, init many things. Please list what you have init in @brief*/

/**
 * @return      init success or not
 * @brief       init MinSH, including (TODO: bla1, bla2, bla3...)
 **/
bool minsh_init();
bool init_alias();
bool show_prompt();
#endif