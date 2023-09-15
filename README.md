# MinSH
Minimal Shell for OSexp
# source dir structure
## files
main.cpp    - where program start (kind of...)    
config.h    - config macro definitions    
common.h    - main header file, including subdir header files & global parameters    
## directories
debug       - functions for debug    
init        - minsh initiation    
parser      - command line parser    
cmdexec     - command executor    
minsh       - main loop    
# current goal
1.  Basic main loop
2.  Parser (error handling & history)
3.  cmdexec
4.  Builtin "echo" command
# how to build
1.  cd to /minsh
2.  enter make to build
3.  enter make run to build&run