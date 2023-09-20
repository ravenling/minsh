#include "parser.h"

#include <string.h>
#include <config.h>
#include <minsh/minsh.h>

bool read_line(char *_buf, uint32_t _maxSize) {



    return false;
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

        std::cin.getline(inputBuf, CONFIG_BUF_SIZE);
        //read_line(inputBuf, CONFIG_BUF_SIZE);

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