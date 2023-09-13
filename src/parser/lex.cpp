#include "prs.tab.hh"
#include "location.hh"

using namespace yy;

/* TODO: implement yylex */
parser::symbol_type yylex(){
    static int tmpcount = 0;
    parser::location_type tmploc;

    int tmp = tmpcount;
    tmpcount = (tmpcount + 1) % 5;
    tmploc.begin.line = tmp;
    tmploc.begin.column = tmp;

    switch (tmp) {
    case 0:
        return parser::make_WORD(std::string("echo"), tmploc);
        break;
    case 1:
        return parser::make_WORD(std::string("hello"), tmploc);
        break;
    case 2:
        return parser::symbol_type('>', tmploc);
        break;
    case 3:
        return parser::make_WORD(std::string("./a.out"), tmploc);
        break; 
    case 4:
    default:
        break;
    }

    return parser::make_YYerror(tmploc);
}