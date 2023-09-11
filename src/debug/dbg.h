#ifndef DBG_H
#define DBG_H

#include <string>

#define     DEBUG_INFO      0
#define     DEBUG_TODO      1
#define     DEBUG_WARN      2
#define     DEBUG_ERR       3     
#define     DEBUG_CRIT      4     

typedef int err_type;

/**
 * @param   _msg    Error message
 * @param   _exit   If true then exit after print
 * @param   _code   Exit code
 * @brief   Output ERROR/CRITICAL _msg to stderr & log depending on parameter _exit
 **/
void Panic(std::string _msg, int _exit, int _code);

/**
 * @param   _cond   If true then assert failed
 * @param   _msg    Error message
 * @param   _code   Exit code
 * @brief   Output CRITICAL _msg to stderr & log when parameter _cond is true
 **/
void Assert(bool _cond, std::string _msg, int _code);

/**
 * @param   _msg    Error message
 * @param   _type   Error type
 * @brief   Output _msg to log
 **/
void Log(std::string _msg, err_type _type);

/**
 * @param   _msg    message
 * @brief   Just to remind what need TODO next ;)
 **/
void Todo(std::string _msg);

#endif