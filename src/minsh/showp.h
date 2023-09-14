#ifndef SHOWP_H
#define SHOWP_H

#include <pwd.h>
#include <unistd.h>
#include <string>
#include <iostream>
std::string home_dir;
/* init cmd prompt */
bool show_prompt(){
    /* get user information */
    passwd *pwd = getpwuid(getuid());
    std::string user_name = pwd->pw_name;
    /* get cwd */
    char cwd_buf[1024];
    getcwd(cwd_buf,1024);
    std::string cwd = cwd_buf;
    /* home_dir */
    if(user_name == "root"){
        home_dir = "/root";
    }else{
        home_dir = "/home/" + user_name;
    }
    if (cwd == home_dir)
        cwd = "~";
    
    /* hostname */
    gethostname(cwd_buf,1024);
    std::string host_name = cwd_buf;

    /* show */
    std::cout<<user_name<<'@'<<host_name<<":"<<cwd<<std::endl;
    
    return true;
}

#endif