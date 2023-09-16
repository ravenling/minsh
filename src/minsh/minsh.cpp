#include "minsh.h"
#include "showp.h"

/* Define MinSH's static members */
std::string MinSH::_pwd;

std::map<std::string, std::string> MinSH::_alias;

char MinSH::_buf[CONFIG_BUF_MEM_SIZE];
uint32_t MinSH::_bHead, MinSH::_bTail;

std::shared_ptr<CompleteCommand> MinSH::_cmd;

std::string MinSH::_history[CONFIG_HISTORY_MEM_SIZE];
uint32_t MinSH::_hHead, MinSH::_hTail;
std::string MinSH::_currentHis;

/* Main loop */
int minsh_main_loop() {

    while(1) { 
        Assert(show_prompt(), "show_prompt failed", 201);

        read_command();

        std::cout << "History count: " << MinSH::get_his_count() << std::endl;
        for(uint32_t i = 1; i <= MinSH::get_his_count(); i++) {
            std::cout << "#" << i << " " << MinSH::get_history(i) << std::endl;
        }

    }


    return 0;
}
