#include <assert.h>
#include <iostream>
#include "../parser/parser.h"
#include <minsh/minsh.h>

int main() {

    while(1) {

        yy::parser test_parser;
        std::cout << "parser created" << std::endl;
        test_parser();
        std::cout << "token parsed" << std::endl;

        g_cmdAST->debug_print();
        std::cout << "ast printed" << std::endl;
        
    }

    return 0;
}