#include "minsh.h"
#include "showp.h"

/* Define MinSH's static members */
std::string MinSH::_pwd;
std::map<std::string, std::string> MinSH::_alias;
char MinSH::_buf[CONFIG_BUF_MEM_SIZE];
uint32_t MinSH::_bHead, MinSH::_bTail;
std::vector<std::shared_ptr<CompleteCommand>> MinSH::_cmds;

/* Main loop */
int minsh_main_loop() {

    while(1) { 
        Assert(show_prompt(), "show_prompt failed", 201);


    }


    return 0;
}
