#ifndef MINSH_H
#define MINSH_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <debug/dbg.h>
#include <parser/parser.h>
#include <common.h>
#include <unistd.h>

/* TODO: complete the MinSH class */

class MinSH{

private:

    // Alias
    static std::map<std::string, std::string> _alias;
    // Variable
    static std::map<std::string, std::string> _variable;

    // Input Buffer
    static char _buf[CONFIG_BUF_MEM_SIZE];
    static uint32_t _bHead, _bTail;
    // Complete Command
    static std::shared_ptr<CompleteCommand> _cmd;

    // File Descripter
    static std::vector<int> _fdPool;

    // History
    static std::string _currentHis;
    static std::string _history[CONFIG_HISTORY_MEM_SIZE];
    static uint32_t _hHead, _hTail;

public:

    /* Alias */
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

    /* Variable */
    static void init_var() {_variable.clear();}
    static void add_var(std::string &_var, std::string &_val) {
        _variable[_var] = std::string(_val);
    }
    static std::string get_var(std::string &_var) {
        return _variable[_var];
    }

    /* Input buffer */

    static void init_buf() {_bHead = _bTail = 0;}
    static void reset_buf() {init_buf();}
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
        push_current_his(read_buf());   // current history
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

    /* CMD */
    static void set_cmd(std::shared_ptr<CompleteCommand> _newcmd) {_cmd = _newcmd;}
    static std::shared_ptr<CompleteCommand> get_cmd() {return _cmd;}

    /* fd pool */
    static void add_fd(int _fd) {_fdPool.push_back(_fd);}           /** WARNING: not safe ! **/
    static void close_all_fd() {                                    /** WARNING: not safe ! **/
        for(auto fd : _fdPool) {
            close(fd);
        }
        _fdPool.clear();
    }

    /* history */
    static void init_current_his() {_currentHis.clear();}
    static void init_history() {_hHead = _hTail = 0;}
    static uint32_t get_his_count() {
        if(_hHead <= _hTail) {
            return _hTail - _hHead;
        }
        else {
            return CONFIG_HISTORY_MEM_SIZE - (_hHead - _hTail);
        }
    }
    static bool push_current_his(char _c) {
        _currentHis.push_back(_c);
        return true;
    }
    static bool add_history() {
        if(get_his_count() >= CONFIG_HISTORY_SIZE) {
            _hHead = (_hHead + 1) % CONFIG_HISTORY_MEM_SIZE;
        }
        while(_currentHis.size() > 0 && _currentHis.back() == '\n') {     // remove \n
            _currentHis.pop_back();
        }
        _history[_hTail] = _currentHis;
        _hTail = (_hTail + 1) % CONFIG_HISTORY_MEM_SIZE;
        return true;
    }
    static std::string get_history(uint32_t _count) {
        if(_count == 0) {
            return _currentHis;
        }
        if(_count > get_his_count()) {
            return "";
        }
        return _history[(_hTail-_count+CONFIG_HISTORY_MEM_SIZE) % CONFIG_HISTORY_MEM_SIZE];
    }

};

/**
 * @return      MinSH exit code
 * @brief       MinSH main loop
 **/
int minsh_main_loop();

/**
 * @return      home dir
 **/
std::string get_home_dir();

/**
 * @return      prompt printed successfully or not
 **/
bool show_prompt();

#endif