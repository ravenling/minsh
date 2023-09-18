#include <minsh/minsh.h>
#include <minsh/showp.h>
#include <cmdexec/cmdexec.h>

/* Define MinSH's static members */

std::map<std::string, std::string> MinSH::_alias;

std::map<std::string, std::string> MinSH::_variable;

char MinSH::_buf[CONFIG_BUF_MEM_SIZE];
uint32_t MinSH::_bHead, MinSH::_bTail;

std::shared_ptr<CompleteCommand> MinSH::_cmd;

std::vector<int> MinSH::_fdPool;

std::string MinSH::_history[CONFIG_HISTORY_MEM_SIZE];
uint32_t MinSH::_hHead, MinSH::_hTail;
std::string MinSH::_currentHis;

/* Main loop */
int minsh_main_loop() {

    while(1) { 
        Assert(show_prompt(), "show_prompt failed", 201);

        read_command();

        exec_completecommand(MinSH::get_cmd());

    }


    return 0;
}
