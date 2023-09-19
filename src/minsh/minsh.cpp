#include <pwd.h>
#include <minsh/minsh.h>
#include <cmdexec/cmdexec.h>

/* Define MinSH's static members */
std::map<std::string, std::string> MinSH::_envVar;

std::map<std::string, std::string> MinSH::_alias;

std::map<std::string, std::string> MinSH::_variable;

char MinSH::_buf[CONFIG_BUF_MEM_SIZE];
uint32_t MinSH::_bHead, MinSH::_bTail;

std::shared_ptr<CompleteCommand> MinSH::_cmd;

std::vector<int> MinSH::_fdPool;

std::string MinSH::_history[CONFIG_HISTORY_MEM_SIZE];
uint32_t MinSH::_hHead, MinSH::_hTail;
std::string MinSH::_currentHis;

int minsh_main_loop() {

    while(1) { 

        Assert(show_prompt(), "show_prompt failed", 201);

        if(read_command()) {
            exec_completecommand(MinSH::get_cmd());
        }

    }

    return 0;
}

/* get home dir */
std::string get_home_dir() {
    /* get user information */
    passwd *pwd = getpwuid(getuid());
    std::string userName = pwd->pw_name;

    /* get cwd */
    char cwdBuf[1024];
    getcwd(cwdBuf,1024);
    std::string cwd = cwdBuf;

    /* home_dir */
    std::string home_dir;
    if(userName == "root"){
        return std::string("/root");
    } else {
        return std::string("/home/") + userName;
    }
}

/* Show prompt */
bool show_prompt(){
    /* get user information */
    passwd *pwd = getpwuid(getuid());
    std::string userName = pwd->pw_name;

    /* get cwd */
    char cwdBuf[1024];
    getcwd(cwdBuf,1024);
    std::string cwd = cwdBuf;

    /* home dir */
    std::string homeDir = get_home_dir();
    if(cwd.substr(0, homeDir.size()) == homeDir) {
        cwd = "~" + cwd.substr(homeDir.size(), cwd.size()-homeDir.size());
    }
    
    /* hostname */
    gethostname(cwdBuf,1024);
    std::string host_name = cwdBuf;

    /* show */
    std::cout<<
    "\033[1;35m" <<
    userName << '@' << host_name <<
    "\033[0m" << 
    ":" <<
    "\033[1;36m" <<
    cwd <<  
    "\033[0m" <<
    std::endl;
    
    return true;
}