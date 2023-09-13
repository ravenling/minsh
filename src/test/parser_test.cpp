#include "../parser/parser.h"

int main() {
    yy::parser test_parser;

    test_parser();
    
    g_progAST->debug_print();

    return 0;
}