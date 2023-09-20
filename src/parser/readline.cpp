#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <config.h>
#include <memory.h>
#include <minsh/minsh.h>

#include <termios.h>


#define CUR_MOVE_LEFT(y)    printf("\033[%dD", (y))
#define CUR_MOVE_RIGHT(y)   printf("\033[%dC",(y))


enum EscKey {
    ESCKEY_INVALID,

    ESCKEY_ARROW_UP,
    ESCKEY_ARROW_DOWN,
    ESCKEY_ARROW_RIGHT,
    ESCKEY_ARROW_LEFT,
};

int get_ch() {
    int ch;
    struct termios oldt, newt;

    /* back up settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* new setting */
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);       // no ECHO & LINEBUFFER

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* get a char */
    ch = getchar();

    /* recover */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

// a really straight-forword implementation
int esc_match() {
    // layer 1
    int ch = get_ch();
    if(ch != '[') {
        return ESCKEY_INVALID;
    }
    // layer 2
    ch = get_ch();
    switch(ch) {
    case 'A':
        return ESCKEY_ARROW_UP; break;
    case 'B':
        return ESCKEY_ARROW_DOWN; break;
    case 'C':
        return ESCKEY_ARROW_RIGHT; break;
    case 'D':
        return ESCKEY_ARROW_LEFT; break; 
    default:
        break;
    }
    return ESCKEY_INVALID;
}

size_t read_line(char *_buf, uint32_t _maxSize) {
    char *backup;
    uint32_t bTop;

    int newChar;
    uint32_t cur = 0, top = 0;
    uint32_t hisPtr = 0;

    backup = new char[_maxSize+1];

    while(1) {
        uint32_t offset = cur;
        uint32_t length = top;

        /* step 1: read a new char */
        newChar = get_ch();

        /* step 2: judge the char */
        if(newChar >= 32 && newChar < 127) {    // printable && no-backspace
            if(top + 2 < _maxSize) {            // save for <ENTER>
                for(uint32_t i = top; i > cur; i--) {
                    _buf[i] = _buf[i-1];
                }
                _buf[cur] = newChar;
                top++;
                cur++;
            }
        } else
        if(newChar == 127) {                    // backspace
            if(cur > 0) {
                for(uint32_t i = cur; i < top; i++) {
                    _buf[i-1] = _buf[i];
                }
                top--;
                cur--;
            }
        } else {                                // special
            if(newChar == 27) {                 // <Esc>
                // try to match something
                int esckey = esc_match();

                switch(esckey) {
                case ESCKEY_ARROW_UP:
                    // get prev history
                    if(hisPtr < MinSH::get_his_count()) {
                        // backup
                        if(hisPtr == 0) {
                            memcpy(backup, _buf, top);
                            bTop = top;
                        }
                        // get & replace
                        std::string tmpHis = MinSH::get_history(hisPtr+1);
                        memcpy(_buf, tmpHis.c_str(), tmpHis.size());            /** WARNING: not that safe to copy **/
                        top = tmpHis.size();

                        if(cur > top) {
                            cur = top;
                        }
                        hisPtr++;
                    }
                    break;
                case ESCKEY_ARROW_DOWN:
                    // get next history
                    if(hisPtr > 0) {
                        // restore backup
                        if(hisPtr == 1) {
                            memcpy(_buf, backup, bTop);
                            top = bTop;
                            if(cur > top) { 
                                cur = top;
                            }
                        } else {    // get & replace
                            std::string tmpHis = MinSH::get_history(hisPtr-1);
                            memcpy(_buf, tmpHis.c_str(), tmpHis.size());        /** WARNING: not that safe to copy **/
                            top = tmpHis.size();
                            if(cur > top) {
                                cur = top;
                            }
                        }
                        hisPtr--;
                    }
                    break;
                case ESCKEY_ARROW_RIGHT:
                    if(cur < top) {
                        cur++;
                    }
                    break;
                case ESCKEY_ARROW_LEFT:
                    if(cur > 0) {
                        cur--;
                    }
                    break;
                default:
                    break;
                }
            } else
            if(newChar == '\n') {               // return
                printf("\n");
                break;
            }
        }

        /* step 3: refresh terminal */
        if(offset != 0)
            CUR_MOVE_LEFT(offset);                  // move to beginning
        _buf[top] = '\0';                       // write a lot
        printf("%s", _buf);
        if(length > top) {
            for(uint32_t i = 0; i < length - top; i++) {
                printf(" ");
            }
            if(length-cur != 0)
                CUR_MOVE_LEFT(length-cur);              // move to real position
        }
        else {
            if(top-cur != 0)
                CUR_MOVE_LEFT(top-cur);              // move to real position
        }

    }

    // NULL terminate
    _buf[top] = '\0';

    delete [] backup;

    return top;
}

/* Try to read a new line */
bool read_newline() {
    if(!initFlag) {
        printf("> ");
    } else {
        initFlag = false;
    }
    bool readFlag = true;

    char inputBuf[CONFIG_BUF_MEM_SIZE];

    while(readFlag){
        readFlag = false;
        // read a line

        read_line(inputBuf, CONFIG_BUF_SIZE);
        //std::cin.getline(inputBuf, CONFIG_BUF_SIZE);

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