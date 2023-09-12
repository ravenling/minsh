#include <common.h>
#include <debug/dbg.h>
#include <init/init.h>
#include <map>
#include <string>

/* Instantiate global log file */
#ifdef CONFIG_LOG
std::ofstream g_log_file;
#endif



int main(int argc, char **argv) {
    /* TODO: Config File */


    /* Initiating */
    Assert(minsh_init(), "Init failed", 100);
    
    /* TODO: Main Loop */

    return 0;
}