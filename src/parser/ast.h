#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <memory>

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
    :_word(_newword){}

    void debug_print() {
        std::cout << "[PSWord] " << _word;
    }
    
};

// io_file & io_here
struct IOFile final : public ASTNode {
    uint8_t _redirtype;
    std::string _filename;

    IOFile(uint8_t _type, std::string _name)
    :_redirtype(_type), _filename(_name){}

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
    :_ionumber(_ionum), _iofile(std::move(_file)){}

    void debug_print() {
        std::cout << "[IORedirect] " << _ionumber << "  ";
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
    :_prefix(std::move(_pre)), _cmdword(_cmd), _suffix(std::move(_suf)){}

    void debug_print() {
        std::cout << "<SimpleCommand>" << std::endl;

        std::cout << "#### PREFIX" << std::endl;
        for(auto pre : _prefix) {
            pre->debug_print();
        }

        std::cout << "#### CMDWORD" << std::endl;
        std::cout << _cmdword << std::endl;

        std::cout << "#### SUFFIX" << std::endl;
        for(auto suf : _suffix) {
            suf->debug_print();
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
    :_bangpref(_pre), _andorsuf(_suf), _cmdlist(std::move(_newlist)){}

    void debug_print() {
        std::cout << "<Pipeline>";
        std::cout << (_bangpref ? " ! (Bang)" : "") << std::endl;

        for(auto cmd : _cmdlist) {
            cmd->debug_print();
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
    :_isasync(_flag), _pipelinelist(std::move(_newlist)){}

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
    :_andorlist(std::move(_newlist)){}

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
    :_cmplist(std::move(_newlist)){}

    void debug_print() {
        std::cout << "<MinSHProgram>" << std::endl;
        for(auto cmp : _cmplist) {
            cmp->debug_print();
        }
        std::cout << "<\\MinSHProgram>" << std::endl;
    }
};











#endif