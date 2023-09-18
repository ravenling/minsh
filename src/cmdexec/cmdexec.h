#ifndef CMDEXEC_H
#define CMDEXEC_H

#include <memory>
#include <parser/ast.h>
#include <builtin/builtin.h>

/**
 * @param   command name/path
 * @return  absolute path of cmd, return "" when fail
 * @brief   find where command is
 **/
std::string find_external_command(std::string _cmdname);

/**
 * @param   command
 * @return  0: success ; other: error
 * @brief   execute complete command
 **/
int exec_completecommand(std::shared_ptr<CompleteCommand> _cmd);

#endif