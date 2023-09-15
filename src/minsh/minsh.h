#ifndef MINSH_H
#define MINSH_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <debug/dbg.h>
//#include <parser/parser.h>
#include "../parser/parser.h"
#include <common.h>

/* TODO: complete the MinSH class */

class MinSH{

private:

    // PWD
    static std::string _pwd;
    // Alias
    static std::map<std::string, std::string> _alias;

    // Input Buffer
    static char _buf[CONFIG_BUF_MEM_SIZE];
    static uint32_t _bHead, _bTail;
    // Complete Commands
    static std::vector<std::shared_ptr<CompleteCommand>> _cmds;

public:

    static void set_pwd(std::string & _newpwd) {_pwd = std::string(_newpwd);}
    static std::string get_pwd() {return _pwd;}

    static std::string get_alias(std::string & _cmd) {return _alias[_cmd];}
    static void set_alias(std::string & _ali, std::string & _cmd) {
        Assert(_ali.size() > 0 && _cmd.size() > 0, "Parameter \"_ali\" and \"_cmd\" in \"set_alias\" cannot be empty string", 201);
        _alias[_ali] = std::string(_cmd);
    }
    static void del_alias(std::string & _ali, bool _isall) {
        if(_isall) {
            _alias.clear();
        }
        else {
            if(_alias.count(_ali) <= 0) {
                Log("Trying to delete an unavalible alias", DEBUG_WARN);
            }
            else {
                _alias.erase(_ali);
            }
        }
    }

    static void init_buf() {_bHead = _bTail = 0;}
    static uint32_t get_buf_count() {
        if(_bHead <= _bTail) {
            return _bTail - _bHead;
        }
        else {
            return CONFIG_BUF_MEM_SIZE - (_bHead - _bTail);
        }
    }
    static bool write_buf(const char *_s, uint32_t _count) {
        if(_count > CONFIG_BUF_SIZE - get_buf_count()) {
            Panic("Command buffer memory exhausted", 0, 0);
            return false;
        }
        else {
            for(uint32_t i = 0; i < _count; i++) {
                _buf[_bTail] = _s[i];
                _bTail = (_bTail + 1) % CONFIG_BUF_MEM_SIZE;
            }
        }
        return true;
    }
    static bool pop_buf() {
        if(get_buf_count() <= 0) {
            Panic("Command buffer pop after empty", 0, 0);
            return false;
        }
        _bHead = (_bHead + 1) % CONFIG_BUF_MEM_SIZE;
        return true;
    }
    static char read_buf() {
        if(get_buf_count() <= 0) {
            Panic("Command buffer read after empty", 0, 0);
            return 0;
        }
        else {
            return _buf[_bHead];
        }
    }
    static void debug_print_buf() {
        uint32_t bufLen = get_buf_count();
        printf("MinSH::_buf content:\n");
        for(uint32_t i = 0; i < bufLen; i++) {
            printf("%c", _buf[(i+_bHead)%CONFIG_BUF_MEM_SIZE]);
        }
        printf("\n");
        printf("MinSH::_buf content ends\n");
    }

};

/* TODO: main loop frame work, use functions from parser.h */

/**
 * @return      MinSH exit code
 * @brief       MinSH main loop
 **/
int minsh_main_loop();

#endif