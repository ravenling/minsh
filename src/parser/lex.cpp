#include "parser.h"

#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <map>

// TODO: change this include
#include "../config.h"
#include "../minsh/minsh.h"

std::map<std::string, uint8_t> op_tab = {

    {"<",   RD_LESS},
    {">",   RD_GREAT},
    {"<<",  RD_DLESS},
    {">>",  RD_DGREAT},
    {"<&",  RD_GREATAND},
    {">&",  RD_LESSAND},
    {"<>",  RD_LESSGREAT},
    {"<<-", RD_DLESSDASH},
    {">|",  RD_CLOBBER},

    {"||",  OP_DPIPE},
    {"&&",  OP_DAND},
    {"|",   OP_PIPE},
    {"!",   OP_BANG},
    {"&",   OP_AND},
    {";",   OP_SEMICOLON},
    {"{",   OP_LBRACE},
    {"}",   OP_RBRACE},

};

std::map<std::string, uint8_t> rs_tab= {

    {"case",    TK_CASE},
    {"do",      TK_DO},
    {"done",    TK_DONE},
    {"elif",    TK_ELIF},
    {"else",    TK_ELSE},
    {"esac",    TK_ESAC},
    {"fi",      TK_FI},
    {"for",     TK_FOR},
    {"if",      TK_IF},
    {"in",      TK_IN},
    {"then",    TK_THEN},
    {"until",   TK_UNTIL},
    {"while",   TK_WHILE},

};

bool is_operator(std::string &_str);
bool is_op_begin(char _c);
bool is_assign(std::string &_str);
bool is_reserve(std::string &_str);

bool cmdFlag = false;

yy::parser::location_type loc;

namespace yy {

/* TODO: implement yylex */
parser::symbol_type yylex(){
    
    // Step 1: tokenize
    Token newToken = get_token();
    Assert(newToken._type == TK_OPERATOR || newToken._type == TK_NEWLINE || newToken._type == TK_WORD, "tokenizer error", 301);

    // Step 2: grammer
    Assert(grammer_conv(newToken), "grammer conversion failed", 302);

    // Step 3: to symbol_type
    parser::symbol_type newSymbol = get_symbol(newToken);

    return newSymbol;
}

parser::symbol_type get_symbol(Token &_token) {
    switch (_token._type) {
    case TK_NEW:
        return parser::make_YYerror(loc); break;
    case TK_WORD:
        return parser::make_WORD(_token._val, loc); break;
    case TK_ASSIGNMENT_WORD:
        return parser::make_ASSIGNMENT_WORD(_token._val, loc); break;
    case TK_NAME:
        return parser::make_NAME(_token._val, loc); break;
    case TK_NEWLINE:
        return parser::make_NEWLINE(_token._val, loc); break;
    case TK_IO_NUMBER:
        return parser::make_IO_NUMBER((int8_t)(std::stoi(_token._val)), loc); break;
    case TK_OPERATOR:
        switch(op_tab[_token._val]) {
            case RD_LESS:
                return parser::symbol_type('<', loc); break;
            case RD_GREAT:
                return parser::symbol_type('>', loc); break;
            case RD_DLESS:
                return parser::make_DLESS(RD_DLESS, loc); break;
            case RD_DGREAT:
                return parser::make_DGREAT(RD_DGREAT, loc); break;
            case RD_LESSAND:
                return parser::make_LESSAND(RD_LESSAND, loc); break;
            case RD_GREATAND:
                return parser::make_GREATAND(RD_GREATAND, loc); break;
            case RD_LESSGREAT:
                return parser::make_LESSGREAT(RD_LESSGREAT, loc); break;
            case RD_DLESSDASH:
                return parser::make_DLESSDASH(RD_DLESSDASH, loc); break;
            case RD_CLOBBER:
                return parser::make_CLOBBER(RD_CLOBBER, loc); break;
            case OP_DAND:
                return parser::make_AND_IF(OP_DAND, loc); break;
            case OP_DPIPE:
                return parser::make_OR_IF(OP_DPIPE, loc); break;
            case OP_PIPE:
                return parser::symbol_type('|', loc); break;
            case OP_BANG:
                return parser::symbol_type('!', loc); break;
            case OP_AND:
                return parser::symbol_type('&', loc); break;
            case OP_SEMICOLON:
                return parser::symbol_type(';', loc); break;
            case OP_LBRACE:
                return parser::make_Lbrace(loc); break;
            case OP_RBRACE:
                return parser::make_Rbrace(loc); break;
        default:
            return parser::make_YYerror(loc);
        }
        break;
    case TK_CASE:
        return parser::make_Case(loc); break;
    case TK_DO:
        return parser::make_Do(loc); break;
    case TK_DONE:
        return parser::make_Done(loc); break;
    case TK_ELIF:
        return parser::make_Elif(loc); break;
    case TK_ELSE:
        return parser::make_Else(loc); break;
    case TK_ESAC:
        return parser::make_Esac(loc); break;
    case TK_FI:
        return parser::make_Fi(loc); break;
    case TK_FOR:
        return parser::make_For(loc); break;
    case TK_IF:
        return parser::make_If(loc); break;
    case TK_IN:
        return parser::make_In(loc); break;
    case TK_THEN:
        return parser::make_Then(loc); break;
    case TK_UNTIL:
        return parser::make_Until(loc); break;
    case TK_WHILE:
        return parser::make_While(loc); break;
    default:
        return parser::make_YYerror(loc);
    }
    return parser::make_YYerror(loc);
}

};

bool grammer_conv(Token &_token) {
    // Sep_op
    if(_token._type == TK_OPERATOR && (_token._val == ";" || _token._val == "&")) {
        cmdFlag = false;
    }

    /* Part 1 */
    // rule 1 -- operator (and newline actually)
    if(_token._type != TK_WORD) {
        return true;
    }

    // rule 2 -- number with '<' or '>'
    bool numFlag = true;
    for(auto c : _token._val) {
        if(!isdigit(c)) numFlag = false;
    }
    if(numFlag && (_token._delim == '<' || _token._delim == '>')) {
        _token._type = TK_IO_NUMBER;
        return true;
    }

    /* Part 2 */
    /**
     * No Check: rule 2 
     * TODO: Won't appear 3,4,5,6,8,9
     * Check seq: sep_op -> 7 -> 1
     **/

    // rule 7 -- Assignment
    if(!cmdFlag) {
        if(is_assign(_token._val)) {
            _token._type = TK_ASSIGNMENT_WORD;
            return true;
        }
    }

    // rule 1 -- Reserve / CMD
    if(!cmdFlag & is_reserve(_token._val)) {
        _token._type = rs_tab[_token._val];
        return true;
    }
    else {
        cmdFlag = true;
        return true;
    }
    
    return true;
}

bool is_reserve(std::string &_str) {
    return rs_tab.count(_str) > 0;
}

bool is_assign(std::string &_str) {
    // Check NAME
    if(!isalpha(_str[0]) && _str[0] != '_') {
        return false;
    }
    std::size_t i = 1;
    for(; i < _str.size(); i++) {
        if(_str[i] == '=') break;
        if(!isalnum(_str[i]) && _str[i] != '_') {
            return false;
        }
    }
    if(i == _str.size()) {
        return false;
    }

    // Check '=' (we only consider this <assignment>)
    if(_str[i] != '=') {
        return false;
    }

    return true;
}

/* Token recognition */
Token get_token() {
    Token newToken{TK_NEW,""};

    while(1) {
        if(MinSH::get_buf_count() <= 0) {
            read_newline();
        }
        char c = MinSH::read_buf();

        // rule 1 -- EOF then delimit
        // TODO: actually, it is LF for now
        if(c == '\n') {
            if(newToken._type == TK_NEW) {
                newToken._type = TK_NEWLINE;
                newToken._delim = -1;
                MinSH::pop_buf();
            }
            else {
                newToken._delim = -1;
            }
            return newToken;
        }

        // rule 2 -- Greedy operator
        // &
        // rule 3 -- End of operator
        if(newToken._type == TK_OPERATOR) {
            std::string tmpStr(newToken._val);
            tmpStr.push_back(c);
            if(is_operator(tmpStr)) {
                newToken._val.push_back(c);
                MinSH::pop_buf();
                continue;
            }
            else {
                newToken._delim = c;
                return newToken;
            }
        }

        // TODO: rule 4 -- quote

        // TODO: rule 5 -- substitution (maybe)

        // rule 6 -- new operator
        if(is_op_begin(c)) {
            if(newToken._type == TK_NEW) {
                newToken._type = TK_OPERATOR;
                newToken._val.push_back(c);
                MinSH::pop_buf();
                continue;
            }
            else {
                newToken._delim = c; 
                return newToken;
            }
        }

        // rule 7 -- blank delim
        if(isblank(c)) {
            if(newToken._type == TK_NEW) {
                MinSH::pop_buf();
                continue;
            }
            else {
                newToken._delim = c;
                MinSH::pop_buf();
                return newToken;
            }
        }

        // rule 8 -- word append
        if(newToken._type == TK_WORD) {
            newToken._val.push_back(c);
            MinSH::pop_buf();
            continue;
        }

        // TODO: rule 9 -- comment (maybe)

        // rule 10 -- new word
        if(1) {
            newToken._type = TK_WORD;
            newToken._val.push_back(c);
            MinSH::pop_buf();
            continue;
        }
        
    }
    
    assert(0);
    return newToken;
}

bool is_op_begin(char _c) {
    for(auto &s : op_tab) {
        if(_c == s.first[0]) return true;
    }
    return false;
}

bool is_operator(std::string &_str) {
    return op_tab.count(_str) > 0;
}

/* Try to read a new line */
bool read_newline() {
    printf("> ");
    bool readFlag = true;
    char inputBuf[CONFIG_BUF_MEM_SIZE];
    while(readFlag){
        readFlag = false;
        // read a line
        std::cin.getline(inputBuf, CONFIG_BUF_SIZE-1);
        int len = strlen(inputBuf);
        // \ + \n should be removed and read again
        bool rmFlag = false;
        for(int i = len - 1; i >= 0; i--) {
            if(inputBuf[i] != '\\') break;
            rmFlag = !rmFlag;
        }
        if(rmFlag) {  // remove
            readFlag = true;
            printf("> ");
            len--;
        } else {
            inputBuf[len] = '\n';
            len++;
        }
        // Send to buffer
        if(!MinSH::write_buf(inputBuf, len)){
            return false;
        }
    }
    return true;
}