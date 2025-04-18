#include <common.h>
#include <debug/dbg.h>
#include <init/init.h>
#include <minsh/minsh.h>
#include <map>
#include <string>

/* Instantiate global log file */
#ifdef CONFIG_LOG
std::ofstream g_log_file;
#endif

int main(int argc, char **argv) {

    /* Initiating */
    Assert(minsh_init(), "Init failed", 100);
    
    /* Main Loop */
    minsh_main_loop();

    return 0;
}