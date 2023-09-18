#include <string>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <minsh/minsh.h>

std::string find_external_command(std::string _cmdname) {
    std::string absPath;
    struct stat statBuf;
    // contain <slash>
    if(_cmdname.find_first_of('/') < _cmdname.size()) { 
        char *tmpBuf = realpath(_cmdname.c_str(), NULL);
        if(stat(tmpBuf, &statBuf) != 0) {
            return "";
        }
        absPath.assign(tmpBuf);
        free(tmpBuf);
    }
    // no <slash>, search current & $PATH
    else {
        // $PATH
        std::string pathVar = MinSH::_envVar["PATH"];

        size_t index = 0;
        while(index < pathVar.size()) {
            size_t newIndex = 0;
            newIndex = pathVar.find_first_of(':', index);

            std::string fullpath = pathVar.substr(index, newIndex-index) + "/" + _cmdname;

            if(stat(fullpath.c_str(), &statBuf) == 0) {
                absPath = fullpath;
                break;
            }

            if(newIndex >= pathVar.size()) break;

            index = newIndex + 1;
        }
    }

    return absPath;
}