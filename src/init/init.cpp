#include "init.h"
#include <debug/dbg.h>
#include <common.h>

/* TODO: implement minsh_init */

bool minsh_init() {
    Todo("Implement minsh_init !!!");
    return true;
}

/*init alias*/
bool init_alias(){
    extern std::map <std::string, std::string> g_alias;

    /*add more*/
    g_alias.insert(std::pair<std::string, std::string>("ll", "ls -l"));
    g_alias.insert(std::pair<std::string, std::string>("vi", "vim"));
    g_alias.insert(std::pair<std::string, std::string>("mv", "mv -i"));

    return true;
}

/*init cmd prompt*/
bool show_prompt(){
    /*get user information*/
    passwd *pwd = getpwuid(getuid());
    std::string user_name = pwd->pw_name;
    /*get cwd*/
    char cwd_buf[1024];
    getcwd(cwd_buf,1024);
    std::string cwd = cwd_buf;
    /*home_dir*/
    std::string home_dir;
    if(user_name == "root"){
        home_dir = "/root";
    }else{
        home_dir = "/home/" + user_name;
    }
    if (cwd == home_dir)
        cwd = "~";
    
    /*hostname*/
    gethostname(cwd_buf,1024);
    std::string host_name = cwd_buf;

    /*show*/
    std::cout<<user_name<<'@'<<host_name<<":"<<cwd<<std::endl;
    
    return true;
}
