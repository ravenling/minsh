#include "init.h"
#include <minsh/minsh.h>
#include <debug/dbg.h>
#include <common.h>
#include <string>

bool init_alias();

/* TODO: implement minsh_init */

bool minsh_init() {
    
    MinSH::init_buf();
    MinSH::init_current_his();
    MinSH::init_history();

    Assert(init_alias(), "Init_alias failed", 101);


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
