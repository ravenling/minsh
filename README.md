# MinSH
Minimal Interactive Shell project for ECUST's CS course: ***The OS Course Design***
# source dir structure
```
├── doc
│   ├── errorcode_CH.txt                -- error codes
│   └── standard(recommand)_CH.txt      -- code standard
├── Makefile
├── README.md
└── src
    ├── builtin                         -- builtin functions
    │   ├── builtin.cpp
    │   └── builtin.h
    ├── cmdexec                         -- command execution functions
    │   ├── cmdexec.cpp
    │   ├── cmdexec.h
    │   └── findcmd.cpp
    ├── common.h
    ├── config.h                        -- config macros
    ├── debug                           -- debug functions
    │   ├── dbg.cpp
    │   └── dbg.h
    ├── init                            -- init functions
    │   ├── init.cpp
    │   └── init.h
    ├── main.cpp
    ├── minsh                           -- MinSH class
    │   ├── minsh.cpp
    │   └── minsh.h
    └── parser                          -- MinSH parser
        ├── ast.h
        ├── lex.cpp
        ├── parser.cpp
        ├── parser.h
        ├── prs.y
        └── readline.cpp
```
# how to build
1. cd to MinSH directory (you are already here)    
2. enter ***make*** to build
>```sh
>make
>```
3. enter ***make run*** for build & run
>```sh
>make run
>```
4. enter ***make clean** to remove build directory
>```sh
>make clean
>```
# project references
[1]shell language reference: https://pubs.opengroup.org/onlinepubs/9699919799.2016edition/utilities/V3_chap02.html.