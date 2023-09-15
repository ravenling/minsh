#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <memory>
#include "parser.h"

typedef enum ASTType{
    AST_PROGRAM,
    AST_CMP_CMD,
    AST_AND_OR_CMD,
    AST_PIPELINE,
    AST_SIMPLE_CMD,
    AST_IOREDIRECT,
    AST_IOFILE,
    AST_PRESUF_WORD,
} node_type;

struct ASTNode {
    uint8_t _type;

    ~ASTNode() = default;

    virtual void debug_print() {
        std::cout << "(AST node)" << std::endl;
    }

};

/* base class PrefixSuffix */
struct PrefixSuffix : public ASTNode {};

// WORD & ASSIGNMENT WORD
struct PrefixSuffixWord : public PrefixSuffix{
    std::string _word;

    PrefixSuffixWord(std::string _newword)
    :_word(_newword){_type = AST_PRESUF_WORD;}

    void debug_print() {
        std::cout << "[PSWord] " << _word << std::endl;
    }
    
};

// io_file & io_here
struct IOFile final : public ASTNode {
    uint8_t _redirtype;
    std::string _filename;

    IOFile(uint8_t _type, std::string _name)
    :_redirtype(_type), _filename(_name){_type = AST_IOFILE;}

    void debug_print() {
        std::vector<std::string> redtab
        {"<",">","<<",">>","<&",">&","<>","<<-",">|"};
        std::cout << redtab[_redirtype] << "  ";
        std::cout << _filename;
    }
};

struct IORedirect final : public PrefixSuffix {
    // _ionumber should be ignored in some situation
    int8_t _ionumber;
    std::shared_ptr <IOFile> _iofile;

    IORedirect(int8_t _ionum, std::shared_ptr <IOFile> _file)
    :_ionumber(_ionum), _iofile(std::move(_file)){_type = AST_IOREDIRECT;}

    void debug_print() {
        std::cout << "[IORedirect] " << (int)(_ionumber) << "  ";
        _iofile->debug_print();
        std::cout << std::endl;
    }

};

/* base class Command */
struct Command : public ASTNode {};

struct SimpleCommand final : public Command {
    std::vector <std::shared_ptr<PrefixSuffix>> _prefix;
    std::string _cmdword;
    std::vector <std::shared_ptr<PrefixSuffix>> _suffix;

    SimpleCommand(  std::vector <std::shared_ptr<PrefixSuffix>> _pre, 
                    std::string _cmd,
                    std::vector <std::shared_ptr<PrefixSuffix>> _suf)
    :_prefix(std::move(_pre)), _cmdword(_cmd), _suffix(std::move(_suf)){_type = AST_SIMPLE_CMD;}

    void debug_print() {
        std::cout << "<SimpleCommand>" << std::endl;

        std::cout << "#### PREFIX" << std::endl;
        std::cout << "PREFIX count: " << _prefix.size() << std::endl;
        for(auto pre : _prefix) {
            switch(pre->_type) {
            case AST_PRESUF_WORD:
                std::dynamic_pointer_cast<PrefixSuffixWord>(pre)->debug_print(); 
                break;
            case AST_IOREDIRECT: 
                std::dynamic_pointer_cast<IORedirect>(pre)->debug_print(); 
                break;
            default:
                std::cout << "{Unexpected Prefix Type}" << std::endl;
                break;
            }
        }

        std::cout << "#### CMDWORD" << std::endl;
        std::cout << _cmdword << std::endl;

        std::cout << "#### SUFFIX" << std::endl;
        std::cout << "SUFFIX count: " << _suffix.size() << std::endl;
        for(auto suf : _suffix) {
            switch(suf->_type) {
            case AST_PRESUF_WORD:
                std::dynamic_pointer_cast<PrefixSuffixWord>(suf)->debug_print(); 
                break;
            case AST_IOREDIRECT: 
                std::dynamic_pointer_cast<IORedirect>(suf)->debug_print(); 
                break;
            default:
                std::cout << "{Unexpected Prefix Type}" << std::endl;
                break;
            }
        }

        std::cout << "<\\SimpleCommand>" << std::endl;
    }

};

struct Pipeline final : public ASTNode {
    bool _bangpref;
    // -1 -> and ; 0 -> nosuf ; 1 -> or
    int _andorsuf;

    std::vector <std::shared_ptr<Command>> _cmdlist;

    Pipeline(   std::vector <std::shared_ptr<Command>> _newlist,
                bool _pre, int _suf)
    :_bangpref(_pre), _andorsuf(_suf), _cmdlist(std::move(_newlist)){_type = AST_PIPELINE;}

    void debug_print() {
        std::cout << "<Pipeline>";
        std::cout << (_bangpref ? " ! (Bang)" : "") << std::endl;
        std::cout << "CMD count: " << _cmdlist.size() << std::endl;

        for(auto cmd : _cmdlist) {
            switch(cmd->_type) {
            case AST_SIMPLE_CMD:
                std::dynamic_pointer_cast<SimpleCommand>(cmd)->debug_print();
                break; 
            default:
                std::cout << "{Unexpected Command Type}" << std::endl;
                break;
            }
        }

        std::cout << "<\\Pipeline>" << std::endl;
        switch (_andorsuf) {
        case -1:
            std::cout << "&& (And)"; break;
        case 0:
            break;
        case 1:
            std::cout << "|| (Or)"; break; 
        default:
            std::cout << "(Unexpected Suffix)"; break; 
            break;
        }
        std::cout << std::endl;
    }

};

struct AndOrCommand final : public ASTNode {
    bool _isasync;
    std::vector <std::shared_ptr<Pipeline>> _pipelinelist;

    AndOrCommand(   std::vector <std::shared_ptr<Pipeline>> _newlist,
                    bool _flag)
    :_isasync(_flag), _pipelinelist(std::move(_newlist)){_type = AST_AND_OR_CMD;}

    void debug_print() {
        std::cout << "<AndOrCommand>" << std::endl;
        for(auto pip : _pipelinelist) {
            pip->debug_print();
        }
        std::cout << "<\\AndOrCommand>";
        std::cout << (_isasync ? "& (Async)" : "; (Sync)") << std::endl;
    }

};
struct CompleteCommand final : public ASTNode {
    std::vector <std::shared_ptr<AndOrCommand>> _andorlist;

    CompleteCommand(std::vector <std::shared_ptr<AndOrCommand>> _newlist)
    :_andorlist(std::move(_newlist)){_type = AST_CMP_CMD;}

    void debug_print() {
        std::cout << "<CompleteCommand>" << std::endl;
        for(auto andor : _andorlist) {
            andor->debug_print();
        }
        std::cout << "<\\CompleteCommand>" << std::endl;
    }

};

struct MinSHProgram final: public ASTNode {
    std::vector <std::shared_ptr<CompleteCommand>> _cmplist;

    MinSHProgram(std::vector <std::shared_ptr<CompleteCommand>> _newlist)
    :_cmplist(std::move(_newlist)){_type = AST_PROGRAM;}

    void debug_print() {
        std::cout << "<MinSHProgram>" << std::endl;
        std::cout << "CMD count: " << _cmplist.size() << std::endl;
        for(auto cmp : _cmplist) {
            cmp->debug_print();
        }
        std::cout << "<\\MinSHProgram>" << std::endl;
    }
};


#endif