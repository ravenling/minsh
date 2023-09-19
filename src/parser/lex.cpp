#include "parser.h"

#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <map>

#include <minsh/minsh.h>
#include <config.h>

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

void next_ploc(char _c) {
    if(_c == '\n') {
        ploc.lines(1);
    }
    else{
        ploc.columns(1);
    }
}

bool is_operator(std::string &_str);
bool is_op_begin(char _c);
bool is_assign(std::string &_str);
bool is_reserve(std::string &_str);
bool is_blank(std::string &_str);

bool is_c_blank(char _c);

bool cmdFlag = false;

yy::parser::location_type ploc;

inline void accept_char(char _c, Token &_token) {
    _token._val.push_back(_c);
    next_ploc(_c);
}

// WORD buffer for substitution
std::queue<Token> wordBuf;

bool isSimple = true;       // constant for now
bool isCmdName = true;


namespace yy {

parser::symbol_type yylex(){
    
    parser::symbol_type newSymbol;

    if(wordBuf.size() == 0) {
        int retVal = 1;
        Token newToken;

        while(retVal != 0) {

            // Step 1: tokenize
            newToken = get_token();
            Assert(newToken._type == TK_OPERATOR || newToken._type == TK_NEWLINE || newToken._type == TK_WORD, "tokenizer error", 301);

            // Step 2: substitution
            if(newToken._type == TK_WORD) {
                retVal = token_substitution(newToken);
                Assert(retVal == 0 || retVal == 1, "substitution failed", 303);
            } else {
                retVal = 0;
            }

        }

        // Step 3: grammer
        Assert(grammer_conv(newToken), "grammer conversion failed", 302);

        // Step 4: to symbol_type
        parser::symbol_type tmpSymbol = get_symbol(newToken);
        newSymbol.move(tmpSymbol);
    
    } else {

        parser::symbol_type tmpSymbol = get_symbol(wordBuf.front());
        newSymbol.move(tmpSymbol);
        wordBuf.pop();
    }

    if(wordBuf.size() == 0) {
        // Step 5: update location
        ploc.step();
    }

    return newSymbol;
}

parser::symbol_type get_symbol(Token &_token) {
    switch (_token._type) {
    case TK_NEW:
        return parser::make_YYerror(ploc); break;
    case TK_WORD:
        return parser::make_WORD(_token._val, ploc); break;
    case TK_ASSIGNMENT_WORD:
        return parser::make_ASSIGNMENT_WORD(_token._val, ploc); break;
    case TK_NAME:
        return parser::make_NAME(_token._val, ploc); break;
    case TK_NEWLINE:
        isCmdName = true;
        return parser::make_NEWLINE(_token._val, ploc); break;
    case TK_IO_NUMBER:
        return parser::make_IO_NUMBER((int8_t)(std::stoi(_token._val)), ploc); break;
    case TK_OPERATOR:
        switch(op_tab[_token._val]) {
            case RD_LESS:
                return parser::symbol_type('<', ploc); break;
            case RD_GREAT:
                return parser::symbol_type('>', ploc); break;
            case RD_DLESS:
                return parser::make_DLESS(RD_DLESS, ploc); break;
            case RD_DGREAT:
                return parser::make_DGREAT(RD_DGREAT, ploc); break;
            case RD_LESSAND:
                return parser::make_LESSAND(RD_LESSAND, ploc); break;
            case RD_GREATAND:
                return parser::make_GREATAND(RD_GREATAND, ploc); break;
            case RD_LESSGREAT:
                return parser::make_LESSGREAT(RD_LESSGREAT, ploc); break;
            case RD_DLESSDASH:
                return parser::make_DLESSDASH(RD_DLESSDASH, ploc); break;
            case RD_CLOBBER:
                return parser::make_CLOBBER(RD_CLOBBER, ploc); break;
            case OP_DAND:
                isCmdName = true;
                return parser::make_AND_IF(OP_DAND, ploc); break;
            case OP_DPIPE:
                isCmdName = true;
                return parser::make_OR_IF(OP_DPIPE, ploc); break;
            case OP_PIPE:
                isCmdName = true;
                return parser::symbol_type('|', ploc); break;
            case OP_BANG:
                return parser::symbol_type('!', ploc); break;
            case OP_AND:
                isCmdName = true;
                return parser::symbol_type('&', ploc); break;
            case OP_SEMICOLON:
                isCmdName = true;
                return parser::symbol_type(';', ploc); break;
            case OP_LBRACE:
                return parser::make_Lbrace(ploc); break;
            case OP_RBRACE:
                return parser::make_Rbrace(ploc); break;
        default:
            return parser::make_YYerror(ploc);
        }
        break;
    case TK_CASE:
        return parser::make_Case(ploc); break;
    case TK_DO:
        return parser::make_Do(ploc); break;
    case TK_DONE:
        return parser::make_Done(ploc); break;
    case TK_ELIF:
        return parser::make_Elif(ploc); break;
    case TK_ELSE:
        return parser::make_Else(ploc); break;
    case TK_ESAC:
        return parser::make_Esac(ploc); break;
    case TK_FI:
        return parser::make_Fi(ploc); break;
    case TK_FOR:
        return parser::make_For(ploc); break;
    case TK_IF:
        return parser::make_If(ploc); break;
    case TK_IN:
        return parser::make_In(ploc); break;
    case TK_THEN:
        return parser::make_Then(ploc); break;
    case TK_UNTIL:
        return parser::make_Until(ploc); break;
    case TK_WHILE:
        return parser::make_While(ploc); break;
    default:
        return parser::make_YYerror(ploc);
    }
    return parser::make_YYerror(ploc);
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
     * Won't appear: 3,4,5,6,8,9
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

bool is_blank(std::string &_str) {
    for(auto c : _str) {
        if(!(isblank(c) || c == '\n')) {
            return false;
        }
    }
    return true;
}

bool is_c_blank(char _c) {
    return isblank(_c) || _c == '\n';
}

/* Substitution */
int token_substitution(Token &_token) {
    std::string expRes;

    /* step 0: alias */
    if(isCmdName && MinSH::is_alias(_token._val)) {
        std::string tmpali = MinSH::get_alias(_token._val);
        if(!is_blank(tmpali)) {
            isCmdName = false;
            // parse by blank and return & send to wordBuf
            size_t index = 0;
            for(; index < tmpali.size(); index++) {
                if(is_c_blank(tmpali[index])) continue;
                expRes.push_back(tmpali[index]);
                if(index >= tmpali.size() || is_c_blank(tmpali[index+1])) {
                    wordBuf.push(Token{TK_WORD, expRes, ' '});
                    expRes.clear();
                } 
            }
        } else {
            // get new token
            isCmdName = true;
            return 1;
        }
    }

    /* step 1: quote & substitution */
    int quoteFlag = 0;       // 0 - non-quote ; 1 - "..." ; 2 - '...' 
    std::string &val = _token._val;

    for(size_t i = 0; i < _token._val.size(); i++) {
        char c = val[i];

        /* in quote */
        // <single quote>
        if(quoteFlag == 2) {
            if(c == '\'') {
                quoteFlag = 0;
            }
            else {
                expRes.push_back(c); 
            }
            continue;
        }

        // <backslash>
        if(c == '\\') {
            i++; expRes.push_back(val[i]); continue;
        }

        // <double quote>
        if(quoteFlag == 1) {
            if(c == '"') {
                quoteFlag = 0;
                continue;
            } else
            if(c != '$'){
                expRes.push_back(c); 
                continue;
            }
        }

        /* begin quote */
        if(c == '"' && quoteFlag == 0) {
            quoteFlag = 1;
            continue;
        }

        if(c == '\'' && quoteFlag == 0) {
            quoteFlag = 2;
            continue;
        }

        /* parameter expansion (only $NAME form) */
        if(c == '$') {
            // $NAME
            if(isalpha(val[i+1]) || val[i+1] == '_') {
                int n = 1;
                std::string name;
                name.push_back(val[i+1]);
                for(n = 1; n+i+1 < val.size(); n++) {
                    if(isalnum(val[i+n+1]) || val[i+n+1] == '_') {
                        name.push_back(val[i+n+1]);
                        continue;
                    }
                    break;
                }
                // NAME length > 0
                if(n > 0) {
                    std::string apdstr;
                    if(MinSH::_envVar.count(name) > 0) {
                        apdstr = MinSH::_envVar[name];
                    } else {
                        apdstr = MinSH::get_var(name);
                    }

                    if(quoteFlag > 0) {             // quoted
                        expRes.append(apdstr);
                    } else {                        // unquoted
                        if(is_c_blank(apdstr.front()) && expRes.size() > 0) {
                            wordBuf.push(Token{TK_WORD, expRes, ' '});
                            expRes.clear();
                        }
                        size_t index = 0;
                        for(; index < apdstr.size(); index++) {
                            while(index < apdstr.size() && is_c_blank(apdstr[index])) { index++;}
                            if(index == apdstr.size()) break;
                            expRes.push_back(apdstr[index]);
                            if(index+1 < apdstr.size() && is_c_blank(apdstr[index+1])) {
                                wordBuf.push(Token{TK_WORD, expRes, ' '});
                                expRes.clear();
                            }
                        }
                    }
                    i += n;
                    continue;
                }
            }

        }

        // Simple char
        expRes.push_back(c);
        
    }


    wordBuf.push(Token{TK_WORD, expRes, ' '});

    _token = wordBuf.front();
    wordBuf.pop();

    if(!is_assign(_token._val)) {
        isCmdName = false;
    }

    return 0;
}

/* Token recognition */
Token get_token() {
    Token newToken{TK_NEW,""};

    int quoteFlag = 0;       // 0 - non-quote ; 1 - "..." ; 2 - '...' 

    while(1) {
        if(MinSH::get_buf_count() <= 0) {
            read_newline();
        }
        char c = MinSH::read_buf();

        // rule 4.1 -- <single quote>
        if(quoteFlag == 2 && c != '\'') {
            accept_char(c, newToken);
            MinSH::pop_buf();
            continue;
        }

        // rule 4.2 -- <backslash>
        if(quoteFlag < 2 && c == '\\') {
            if(newToken._type == TK_NEW) {
                newToken._type = TK_WORD;
            }
            if(newToken._type == TK_WORD) {
                accept_char(c, newToken);
                MinSH::pop_buf();
                c = MinSH::read_buf();
                accept_char(c, newToken);
                MinSH::pop_buf();
                continue;
            } else {
                newToken._delim = c;
                return newToken;
            }
        }

        // rule 4.3 -- <double quote>
        if(quoteFlag == 1 && c != '"' && c!='$') {
            accept_char(c, newToken);
            MinSH::pop_buf();
            continue;
        }

        // rule 1 -- EOF then delimit
        // actually, it is LF for now
        if(c == '\n') {
            if(newToken._type == TK_NEW) {
                newToken._type = TK_NEWLINE;
                newToken._delim = -1;
                accept_char(c, newToken);
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
                accept_char(c, newToken);
                MinSH::pop_buf();
                continue;
            }
            else {
                newToken._delim = c;
                return newToken;
            }
        }

        // rule 4.4 -- quote begin
        if(c == '\'') {   // single
            if(quoteFlag == 2) {
                quoteFlag = 0;
                accept_char(c, newToken);
                MinSH::pop_buf();
                continue;
            } else 
            if(quoteFlag == 0){
                // New Token or WORD
                if(newToken._type == TK_NEW || newToken._type == TK_WORD) {
                    newToken._type = TK_WORD;
                    quoteFlag = 2;
                    accept_char(c, newToken);
                    MinSH::pop_buf();
                    continue;
                } else {
                    newToken._delim = c;
                    return newToken;
                }
            }
        }
        if(c == '\"') {
            if(quoteFlag == 1) {
                quoteFlag = 0;
                accept_char(c, newToken);
                MinSH::pop_buf();
                continue;
            } else 
            if(quoteFlag == 0){
                // New Token or WORD
                if(newToken._type == TK_NEW || newToken._type == TK_WORD) {
                    newToken._type = TK_WORD;
                    quoteFlag = 1;
                    accept_char(c, newToken);
                    MinSH::pop_buf();
                    continue;
                } else {
                    newToken._delim = c;
                    return newToken;
                }
            }
        }

        // TODO: rule 5 -- substitution (for now, don't need this part)

        // rule 6 -- new operator
        if(is_op_begin(c)) {
            if(newToken._type == TK_NEW) {
                newToken._type = TK_OPERATOR;
                accept_char(c, newToken);
                ploc.step();
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
                next_ploc(c);
                MinSH::pop_buf();
                continue;
            }
            else {
                newToken._delim = c;
                next_ploc(c);
                MinSH::pop_buf();
                return newToken;
            }
        }

        // rule 8 -- word append
        if(newToken._type == TK_WORD) {
            accept_char(c, newToken);
            MinSH::pop_buf();
            continue;
        }

        // TODO: rule 9 -- comment (maybe)

        // rule 10 -- new word
        if(1) {
            newToken._type = TK_WORD;
            accept_char(c, newToken);
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