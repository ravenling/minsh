#include <iostream>
#include "../parser/parser.h"

int main() {
    yy::parser test_parser;

    std::cout << "parser created" << std::endl;
    test_parser();
    
    std::cout << "token parsed" << std::endl;

    g_progAST->debug_print();
    std::cout << "ast printed" << std::endl;

    return 0;
}