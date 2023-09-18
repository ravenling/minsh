#include "init.h"
#include <minsh/minsh.h>
#include <builtin/builtin.h>
#include <debug/dbg.h>
#include <common.h>
#include <string>

bool init_alias();
bool init_variable();

/* TODO: implement minsh_init */

bool minsh_init() {

    /* core */
    MinSH::init_var();
    MinSH::init_buf();
    MinSH::init_current_his();
    MinSH::init_history();

    /* builtin */
    buildin_cmd_init();
    
    /* alias */
    Assert(init_alias(), "Init_alias failed", 101);

    /* variable */
    init_variable();

    return true;
}

/* init alias */
bool init_alias(){
    std::string ali, cmd;

    /* add more */
    ali.clear(); cmd.clear();
    ali.append("ll"); cmd.append("ls -l");
    MinSH::set_alias(ali, cmd);

    return true;
}

/* init variable */
bool init_variable() {
    // $PATH
    std::string var, val;

    var.assign("PATH");
    val.assign("/usr/bin:"); val.append(get_home_dir());

    MinSH::_envVar[var] = val;


    return true;
}