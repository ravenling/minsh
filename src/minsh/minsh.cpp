#include "minsh.h"
#include "showp.h"

/* Define MinSH's static members */
std::string MinSH::_pwd;
std::map<std::string, std::string> MinSH::_alias;
/* Main loop */
int minsh_main_loop() {

    while(1) { 
        Assert(show_prompt(), "show_prompt failed", 201);


    }


    return 0;
}
