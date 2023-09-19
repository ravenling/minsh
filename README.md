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
builtin     - builtin functions    
minsh       - main loop    
# current goal
1.  variable substitution
2.  quote
# how to build
1.  cd to /minsh
2.  enter make to build
3.  enter make run to build&run
# reference
https://pubs.opengroup.org/onlinepubs/9699919799.2016edition/utilities/V3_chap02.html