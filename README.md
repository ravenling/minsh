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
1.  Init function
2.  Basic main loop
3.  Builtin "echo" command