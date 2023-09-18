#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <minsh/minsh.h>

std::string find_external_command(std::string _cmdname) {
    std::string absPath;

    // contain <slash>
    if(_cmdname.find_first_of('/') != _cmdname.size()) { 
        char *tmpbuf = realpath(_cmdname.c_str(), NULL);
        if(tmpbuf == NULL) {
            return "";
        }
        absPath.assign(tmpbuf);
        free(tmpbuf);
    }
    // no <slash>, search current & $PATH
    else {
        char *tmpbuf;
        // current dir 
        char cwdBuf[1024];
        getcwd(cwdBuf,1024);
        std::string cwd = cwdBuf;
        cwd.push_back('/');
        
        tmpbuf = realpath((cwd+_cmdname).c_str(), NULL);

        if(tmpbuf != NULL) {
            absPath.assign(tmpbuf);
            std::cout << "GOTCHA: " << absPath << std::endl;
            return absPath;
        }

        // $PATH
        std::string pathVar = MinSH::_envVar["PATH"];
        size_t index = 0;
        while(index < pathVar.size()) {
            size_t newIndex = 0;
            newIndex = pathVar.find_first_of(':', index);

            std::string fullpath = pathVar.substr(index, newIndex-index) + _cmdname;

            tmpbuf = realpath(_cmdname.c_str(), NULL);

            if(tmpbuf != NULL) {
                absPath.assign(tmpbuf);
                std::cout << "GOTCHA: " << absPath << std::endl;
                index = pathVar.size()-1;
            }

            free(tmpbuf);
            index = newIndex + 1;
        }
    }

    return absPath;
}