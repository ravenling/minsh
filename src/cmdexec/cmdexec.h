#ifndef CMDEXEC_H
#define CMDEXEC_H

#include <memory>
#include <parser/ast.h>

/**
 * @param
 * @return  0: success ; other: error
 * @brief   execute complete command
 **/
int exec_completecommand(std::shared_ptr<CompleteCommand> _cmd);

#endif