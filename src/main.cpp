#include <common.h>
#include <debug/dbg.h>
#include <init/init.h>
#include <map>
#include <string>

/* Instantiate global log file */
#ifdef CONFIG_LOG
std::ofstream g_log_file;
#endif

/*alias别名用map实现*/
std::map <std::string, std::string> g_alias;



int main(int argc, char **argv) {
    /* TODO: Config File */


    /* Initiating */
    Assert(minsh_init(), "Init failed", 1);
    
    Assert(init_alias(), "Init_alias failed", 2);
    /* TODO: Main Loop */
    Assert(show_prompt(), "show_prompt failed", 3);

    return 0;
}