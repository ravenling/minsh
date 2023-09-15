#ifndef CMDEXEC_H
#define CMDEXEC_H
#include <stdint.h>
#include <vector>
#include <iostream>
#include <memory>
#include "parser/ast.h"

extern std::shared_ptr<CompleteCommand> g_cmdAST;

int exec_completecommand(std::shared_ptr<CompleteCommand> g_cmdAST){
    auto it_cptcom = (g_cmdAST->_andorlist).begin();
    for(; it_cptcom != (g_cmdAST->_andorlist).end(); ++it_cptcom){    
        exec_andorcommand(it_cptcom);
    }
    return 1;
}

int exec_andorcommand(std::vector<std::shared_ptr<AndOrCommand>> :: iterator it_cptcom){
    auto it_andcom = (it_cptcom->_pipelinelist).begin();
    for(; it_andcom != (it_cptcom->_pipelinelist).end(); ++it_andcom){    
        exec_pipeline(it_andcom);
    }
    return 1;
}

int exec_pipeline(std::vector<std::shared_ptr<Pipeline>> :: iterator it_andcom){

}
#endif