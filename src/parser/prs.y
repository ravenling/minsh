%{
// TODO: coding this part 
#include <iostream>
#include <memory>
#include "ast.h"
#include "parsor_bison.tab.h"

void yyerror(YYLTYPE *locp, const char* s){
    std::cerr << "Parser Error at LINE " << locp->first_line << " COLUMN " << locp->first_column << ":" << s << std::endl;
}

extern int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

using namespace ast;

%}

// 开启yyerror中的位置参数
%locations
// 无全局/静态变量，可重用parser
%define api.pure full
// enable verbose syntax error message
//%define parse.error verbose

/* -----------------------
   TOKENS
   ----------------------- */

// TOKEN
%token WORD
%token ASSIGNMENT_WORD
%token NAME
%token NEWLINE
%token IO_NUMBER

// OPERATOR
%token AND_IF OR_IF DSEMI
%token DLESS DGREAT LESSAND GREATAND LESSGREAT DLESSDASH
%token CLOBBER

// RESERVE WORDS
%token If Then Else Elif Fi Do Done
%token Case Esac While Until For
%token Lbrace Rbrace Bang
%token In


// TODO: Node class type
%type <sv_node> stmt dml ddl sys
%type <sv_updexpr> upd
%type <sv_updexprs> updList


// TODO: 优先级 maybe we won't use...
/*
%left OR
%left AND
%left NOT
%left '<' '>' '=' NEQ LE GE
%left '+' '-'
%left '*' '/'
%left UMINUS
*/

%%

/* -----------------------
   GRAMMER
   reference: https://pubs.opengroup.org/onlinepubs/9699919799.2016edition/utilities/V3_chap02.html#tag_18_09_02
   ----------------------- */

program             :   linebreak   complete_commands   linebreak
                        {}
                    |   linebreak
                        {}
                    ;

complete_commands   :   complete_command    newline_list    complete_command
                        {}
                    |                                       complete_command                              
                        {}
                    ;

complete_command    :   list    separator_op
                        {}
                    |   list
                        {}
                    ;

list                :   list    separator_op    and_or
                        {}
                    |                           and_or
                        {}
                    ;

and_or              :                               pipeline
                        {}
                    |   and_or  AND_IF  linebreak   pipeline
                        {}
                    |   and_or  OR_IF   linebreak   pipeline
                        {}
                    ;

pipeline            :           pipe_sequence
                        {}
                    |   Bang    pipe_sequence
                        {}
                    ;

pipe_sequence       :                                       command
                        {}
                    |   pipe_sequence   '|'     linebreak   command
                        {}
                    ;

/* TODO: maybe add other type of command */
command             :   simple_command
                        {}
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
                        {}
                    ;

cmd_word            :   WORD            /* rule 7b */
                        {}
                    ;

// change io_redirect to redirect_list
cmd_prefix          :                   redirect_list
                        {}
                    |   cmd_prefix      redirect_list
                        {}
                    |                   ASSIGNMENT_WORD
                        {}
                    |   cmd_prefix      ASSIGNMENT_WORD
                        {}
                    ;

// change io_redirect to redirect_list
cmd_suffix          :                   redirect_list
                        {}
                    |   cmd_suffix      redirect_list
                        {}
                    |                   WORD
                        {}
                    |   cmd_suffix      WORD
                        {}
                    ;

redirect_list       :                   io_redirect
                        {}
                    |   redirect_list   io_redirect
                        {}
                    ;

io_redirect         :                   io_file
                        {}
                    |   IO_NUMBER       io_file
                        {}
                    |                   io_here
                        {}
                    |   IO_NUMBER       io_here
                        {}
                    ;

io_file             :   '<'         filename
                        {}
                    |   LESSAND     filename
                        {}
                    |   '>'         filename
                        {}
                    |   GREATAND    filename
                        {}
                    |   DGREAT      filename
                        {}
                    |   LESSGREAT   filename
                        {}
                    |   CLOBBER     filename
                        {}
                    ;

filename            :   WORD            /* rule 2 */
                        {}
                    ;

io_here             :   DLESS       here_end
                        {}
                    |   DLESSDASH   here_end
                        {}
                    ;

here_end            :   WORD            /* rule 3 */

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
                        {}
                    |   ';'             /* sync */
                        {}
                    ;


// TODO: delete this part
/*
value:          CONST_INT
        {
            $$ = std::make_shared<ConstInt>($1);
        }
        |       CONST_FLOAT
        {
            $$ = std::make_shared<ConstFloat>($1);
        }
        |       CONST_STRING
        {
            $$ = std::make_shared<ConstString>($1);
        }
        ;
*/

%%