#include "dbg.h"

#include <iostream>
#include <fstream>
#include <assert.h>

std::string get_prompt_str(err_type _type, std::string _msg) {
    std::string res;
    switch (_type) {
    case DEBUG_INFO:
        res = "[INFO] "; break;
    case DEBUG_TODO:
        res = "[TODO] "; break;
    case DEBUG_WARN:
        res = "[WARN] "; break;
    case DEBUG_ERR:
        res = "[ERROR] "; break;
    case DEBUG_CRIT:
        res = "[CRITICAL] "; break; 
    default:
        assert(0);
    }
    res += _msg + "\n";
    return res;
}

void Panic(std::string _msg, int _exit, int _code) {
    if(_exit) {
        Log(_msg, DEBUG_CRIT);
        std::cerr << get_prompt_str(DEBUG_CRIT, _msg);
        exit(_code);
    }
    else {
        Log(_msg, DEBUG_ERR);
        std::cerr << get_prompt_str(DEBUG_ERR, _msg);
    }
    return;
}

void Assert(bool _cond, std::string _msg, int _code) {
    if(!_cond) {
        Log(_msg, DEBUG_CRIT);
        std::cerr << get_prompt_str(DEBUG_CRIT, _msg);
        exit(_code);
    }
    return;
}

void Log(std::string _msg, err_type _type) {
    #ifdef CONFIG_LOG
    std::string logMsg = 
    g_log_file << get_prompt_str(_type, _msg) << std::endl;
    #endif
    return;
}

void Todo(std::string _msg) {
    Log(_msg, DEBUG_TODO);
    std::cout << get_prompt_str(DEBUG_TODO, _msg);
}
