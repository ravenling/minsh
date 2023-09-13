#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <memory>
#include "parser.h"

struct ASTNode {
    node_type _type;

    ~ASTNode() = default;

    virtual void debug_print() {
        std::cout << "(AST node)" << std::endl;
    }

};

struct ProgramNode final: public ASTNode {
    std::vector <std::shared_ptr<CompleteCommand>> _cmplist;

    ProgramNode(std::vector <std::shared_ptr<CompleteCommand>> _newlist)
    :_cmplist(std::move(_newlist)){}

    void debug_print() {
        
    }
};

struct CompleteCommand final : public ASTNode {
    std::vector <std::shared_ptr<AndOrCommand>> _andorlist;

    CompleteCommand(std::vector <std::shared_ptr<AndOrCommand>> _newlist)
    :_andorlist(std::move(_newlist)){}

    void debug_print() {

    }

};

struct AndOrCommand final : public ASTNode {
    bool _isAsync;
    std::vector <std::shared_ptr<Pipeline>> _pipelinelist;

    AndOrCommand(std::vector <std::shared_ptr<Pipeline>> _newlist)
    :_pipelist(std::move(_newlist)){}

    void debug_print() {

    }

};

struct Pipeline final : public ASTNode {
    bool _bangpref;
    // -1 -> and ; 0 -> nosuf ; 1 -> or
    int _andorsuf;

    std::vector <std::shared_ptr<Pipeline>> _pipeseqlist;

    Pipeline(std::vector <std::shared_ptr<Pipeline>> _newlist)
    :_pipeseqlist(std::move(_newlist)){}

    void debug_print() {

    }

};

struct PipeSequence final : public ASTNode {
    std::vector <std::shared_ptr<Command>> _cmdlist;

    PipeSequence(std::vector <std::shared_ptr<Command>> _newlist)
    :_cmdlist(std::move(_newlist)){}

    void debug_print() {

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
    :_prefix(std::move(_pre)), _suffix(std::move(_suf)), _cmdword(_cmd){}

    void debug_print() {


    }

};

/* base class PrefixSuffix */
struct PrefixSuffix : public ASTNode {};

// WORD & ASSIGNMENT WORD
struct PrefixSuffixWord : public PrefixSuffix{
    std::string _word;

    AssignmentWord(std::string _newword)
    :_word(_newword){}

    void debug_print() {

    }
    
};

struct IORedirect final : public PrefixSuffix {
    // _ionumber should be ignored in some situation
    uint8_t _ionumber;
    std::shared_ptr <IOFile> _iofile;

    IORedirect(uint8_t _ionum, std::shared_ptr <IOFile> _file)
    :_ionumber(_ionum), _iofile(std::move(_file)){}

    void debug_print() {

    }

};

// io_file & io_here
struct IOFile final : public ASTNode {
    uint8_t _redirtype;
    std::string _filename;

    IOFile(uint8_t _type, std::string _name)
    :_redirtype(_type), _filename(_name){}

    void debug_print() {

    }

}









#endif