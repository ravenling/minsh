%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "parser.h"
#include "prs.tab.hh"
#include "location.hh"

namespace yy {

void error(const parser::location_type &_locp, const std::string &_msg){
    std::cerr << "Parser Error at LINE " << _locp.begin.line << " COLUMN " << _locp.begin.column << ":" << _msg << std::endl;
}

extern parser::symbol_type yylex();

}

%}

%require "3.2"
// language
%language "c++"

// error location variable
%locations

// avoid union limitations
%define api.value.type variant
%define api.token.constructor
// enable verbose syntax error message
//%define parse.error verbose

/* -----------------------
   TOKENS
   ----------------------- */

// TOKEN
%token <std::string> WORD
%token <std::string> ASSIGNMENT_WORD
%token <std::string> NAME
%token <std::string> NEWLINE
%token <int8_t> IO_NUMBER

// OPERATOR
%token <uint8_t> AND_IF OR_IF DSEMI
%token <uint8_t> DLESS DGREAT LESSAND GREATAND LESSGREAT DLESSDASH
%token <uint8_t> CLOBBER

// RESERVE WORDS
%token If Then Else Elif Fi Do Done
%token Case Esac While Until For
%token Lbrace Rbrace Bang
%token In

// TODO: Node class type
%type <std::shared_ptr<MinSHProgram>> program
%type <std::vector<std::shared_ptr<CompleteCommand>>> complete_commands
%type <std::shared_ptr<CompleteCommand>> complete_command
%type <std::vector<std::shared_ptr<AndOrCommand>>> list
%type <std::shared_ptr<AndOrCommand>> and_or
%type <std::shared_ptr<Pipeline>> pipeline
%type <std::vector<std::shared_ptr<Command>>> pipe_sequence
%type <std::shared_ptr<Command>> command
%type <std::shared_ptr<SimpleCommand>> simple_command
%type <std::vector<std::shared_ptr<PrefixSuffix>>> cmd_prefix cmd_suffix
%type <std::string> cmd_name cmd_word
//%type <std::vector<std::shared_ptr<IORedirect>>> redirect_list
%type <std::shared_ptr<IORedirect>> io_redirect
%type <std::shared_ptr<IOFile>> io_file io_here
%type <std::string> filename here_end
%type <uint8_t> separator_op

// Precedence
%precedence "cmd_suffix"
%precedence IO_NUMBER

%%

/* -----------------------
   GRAMMER
   reference: https://pubs.opengroup.org/onlinepubs/9699919799.2016edition/utilities/V3_chap02.html#tag_18_09_02
   ----------------------- */

program             :   linebreak   complete_commands   linebreak
                        {
                            g_progAST = std::make_shared<MinSHProgram>($2);
                        }
                    |   linebreak
                        {
                            g_progAST = std::make_shared<MinSHProgram>(std::vector<std::shared_ptr<CompleteCommand>>{});
                        }
                    ;

complete_commands   :   complete_commands   newline_list    complete_command
                        {   
                            $$.push_back($3);
                        }
                    |                                       complete_command                              
                        {
                            $$ = std::vector<std::shared_ptr<CompleteCommand>>{$1};
                        }
                    ;

complete_command    :   list    separator_op
                        {
                            (*$$->_andorlist.rbegin())->_isasync = ($2 == '&');
                            $$ = std::make_shared<CompleteCommand>($1);
                        }
                    |   list
                        {
                            $$ = std::make_shared<CompleteCommand>($1);
                        }
                    ;

list                :   list    separator_op    and_or
                        {
                            (*$$.rbegin())->_isasync = ($2 == '&');
                            $$.push_back($3);
                        }
                    |                           and_or
                        {
                            $$ = std::vector<std::shared_ptr<AndOrCommand>>{$1};
                        }
                    ;

and_or              :                               pipeline
                        {
                            $$ = std::make_shared<AndOrCommand>(std::vector<std::shared_ptr<Pipeline>>{$1}, false);
                        }
                    |   and_or  AND_IF  linebreak   pipeline
                        {
                            (*$$->_pipelinelist.rbegin())->_andorsuf = -1;
                            $$->_pipelinelist.push_back($4);
                        }
                    |   and_or  OR_IF   linebreak   pipeline
                        {
                            (*$$->_pipelinelist.rbegin())->_andorsuf = 1;
                            $$->_pipelinelist.push_back($4);
                        }
                    ;

pipeline            :           pipe_sequence
                        {
                            $$ = std::make_shared<Pipeline>($1, false, 0);
                        }
                    |   Bang    pipe_sequence
                        {
                            $$ = std::make_shared<Pipeline>($2, true, 0);
                        }
                    ;

pipe_sequence       :                                       command
                        {
                            $$ = std::vector<std::shared_ptr<Command>>{$1};
                        }
                    |   pipe_sequence   '|'     linebreak   command
                        {
                            $$.push_back($4);
                        }
                    ;

/* TODO: maybe add other type of command */
command             :   simple_command
                        { $$ = $1;}
                    ;

simple_command      :   cmd_prefix      cmd_word    cmd_suffix
                        {}
                    |   cmd_prefix      cmd_word
                        {}
                    |   cmd_prefix
                        {}
                    |   cmd_name        cmd_suffix
                        {}
                    |   cmd_name
                        {}
                    ;

cmd_name            :   WORD            /* rule 7a */
                        { $$ = $1; }
                    ;

cmd_word            :   WORD            /* rule 7b */
                        { $$ = $1; }
                    ;

// change io_redirect to redirect_list
cmd_prefix          :                   io_redirect
                        {
                            $$ = std::vector<std::shared_ptr<PrefixSuffix>>{$1};
                        }
                    |   cmd_prefix      io_redirect
                        {
                            $$.push_back($2);
                        }
                    |                   ASSIGNMENT_WORD
                        {
                            $$ = std::vector<std::shared_ptr<PrefixSuffix>>{
                                std::make_shared<PrefixSuffixWord>($1)
                            };
                        }
                    |   cmd_prefix      ASSIGNMENT_WORD
                        {
                            $$.push_back(
                                std::make_shared<PrefixSuffixWord>($2)
                            );
                        }
                    ;

// change io_redirect to redirect_list
cmd_suffix          :                   io_redirect
                        {
                            $$ = std::vector<std::shared_ptr<PrefixSuffix>>{$1};
                        }
                    |   cmd_suffix      io_redirect
                        {
                            $$.push_back($2);
                        }
                    |                   WORD
                        {
                            $$ = std::vector<std::shared_ptr<PrefixSuffix>>{
                                std::make_shared<PrefixSuffixWord>($1)
                            };
                        }
                    |   cmd_suffix      WORD
                        {
                            $$.push_back(
                                std::make_shared<PrefixSuffixWord>($2)
                            );
                        }
                    ;

/*
redirect_list       :                   io_redirect
                        {
                            $$ = std::vector<std::shared_ptr<IORedirect>>{$1};
                        }
                    |   redirect_list   io_redirect
                        {
                            $$.push_back($2);
                        }
                    ;
*/

io_redirect         :                   io_file
                        {$$ = std::make_shared<IORedirect>(-1, $1);}
                    |   IO_NUMBER       io_file
                        {$$ = std::make_shared<IORedirect>($1, $2);}
                    |                   io_here
                        {$$ = std::make_shared<IORedirect>(-1, $1);}
                    |   IO_NUMBER       io_here
                        {$$ = std::make_shared<IORedirect>($1, $2);}
                    ;

io_file             :   '<'         filename
                        {$$ = std::make_shared<IOFile>(RD_LESS, $2);}
                    |   LESSAND     filename
                        {$$ = std::make_shared<IOFile>(RD_LESSAND, $2);}
                    |   '>'         filename
                        {$$ = std::make_shared<IOFile>(RD_GREAT, $2);}
                    |   GREATAND    filename
                        {$$ = std::make_shared<IOFile>(RD_GREATAND, $2);}
                    |   DGREAT      filename
                        {$$ = std::make_shared<IOFile>(RD_DGREAT, $2);}
                    |   LESSGREAT   filename
                        {$$ = std::make_shared<IOFile>(RD_LESSGREAT, $2);}
                    |   CLOBBER     filename
                        {$$ = std::make_shared<IOFile>(RD_CLOBBER, $2);}
                    ;

filename            :   WORD            /* rule 2 */
                        {$$ = $1;}
                    ;

io_here             :   DLESS       here_end
                        {$$ = std::make_shared<IOFile>(RD_DLESS, $2);}
                    |   DLESSDASH   here_end
                        {$$ = std::make_shared<IOFile>(RD_DLESSDASH, $2);}
                    ;

here_end            :   WORD            /* rule 3 */
                        {$$ = $1;}
                    ;

newline_list        :                   NEWLINE
                        {}
                    |   newline_list    NEWLINE
                        {}
                    ;

linebreak           :   newline_list
                        {}
                    |   /* empty */
                        {}
                    ;

separator_op        :   '&'             /* async */
                        {$$ = '&';}
                    |   ';'             /* sync */
                        {$$ = ';';}
                    ;

%%