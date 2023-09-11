#include <fstream>

#include <config.h>

/* Global log file*/
#ifdef CONFIG_LOG
extern std::ofstream g_log_file;
#endif