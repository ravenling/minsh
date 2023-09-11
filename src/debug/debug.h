#ifndef DEBUG_H
#define DEBUG_H

#include <common.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

#define     DEBUG_INFO      0
#define     DEBUG_WARN      1
#define     DEBUG_ERR       2     
#define     DEBUG_CRIT      3     

typedef int err_type;

static std::string get_prompt_str(err_type _type) {
    switch (_type) {
    case DEBUG_INFO:
        return "[INFO] "; break;
    case DEBUG_WARN:
        return "[WARN] "; break;
    case DEBUG_ERR:
        return "[ERROR] "; break;
    case DEBUG_CRIT:
        return "[CRITICAL] "; break; 
    }
    assert(0);
}

/**
 * @param   _msg    Error message
 * @param   _exit   If true then exit after print
 * @param   _code   Exit code
 **/
void Panic(std::string _msg, int _exit, int _code) {
    if(_exit) {
        Log(_msg, DEBUG_CRIT);
        std::cerr << get_prompt_str(DEBUG_CRIT) + _msg;
        exit(_code);
    }
    else {
        Log(_msg, DEBUG_ERR);
        std::cerr << get_prompt_str(DEBUG_ERR) + _msg;
    }
    return;
}

/**
 * @param   _cond   If true then assert failed
 * @param   _msg    Error message
 * @param   _code   Exit code
 **/
void Assert(bool _cond, std::string _msg, int _code) {
    if(_cond) {
        Log(_msg, DEBUG_CRIT);
        std::cerr << get_prompt_str(DEBUG_CRIT) + _msg;
        exit(_code);
    }
    return;
}

/**
 * @param   _msg    Error message
 * @param   _type   Error type
 **/
void Log(std::string _msg, err_type _type) {
    #ifdef CONFIG_LOG
    std::string logMsg = get_prompt_str(_type) + _msg;
    g_log_file << logMsg << std::endl;
    #endif
    return;
}

#endif