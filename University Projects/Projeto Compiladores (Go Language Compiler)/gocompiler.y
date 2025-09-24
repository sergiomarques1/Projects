/* 
	André Cardoso - 2022222265
	Sérgio Marques - 2022222096
*/

%{
#include <stdio.h>
#include "ast.h"

extern int yylex(void);
extern void yyerror(char *);
extern char *yytext;
extern int yylineno;
extern int coluna;

struct node *program;

%}

// tokens
%token <lexeme> NATURAL DECIMAL STRLIT PRINT CMDARGS BLANKID SEMICOLON
%token <lexeme> ASSIGN LPAR RPAR LBRACE RBRACE LSQ RSQ PLUS MINUS STAR DIV MOD OR AND NOT EQ NE LT LE GT GE
%token <lexeme> PACKAGE VAR FUNC INT FLOAT32 BOOL STRING RETURN IF ELSE FOR PARSEINT COMMA IDENTIFIER RESERVED

// operadores
%left COMMA
%right ASSIGN
%left OR
%left AND
%left EQ NE LT LE GT GE
%left PLUS MINUS
%left STAR DIV MOD
%right NOT

%nonassoc IF_SEM_ELSE
%nonassoc ELSE
%nonassoc RPAR LPAR RSQ LSQ

%type <node> Program Declarations VarDeclaration VarSpec Type
%type <node> FuncDeclaration Parameters ParamDecl ParamRep
%type <node> FuncBody VarsAndStatements Statement StateRep
%type <node> ParseArgs FuncInvocation FuncInvRep Expr

%union{
    char *lexeme;
    struct node *node;
}

%%

Program:
            PACKAGE IDENTIFIER SEMICOLON Declarations                       { $$ = program = newnode(Program, NULL); 
                                                                              add_filho(program, $4); }
            ;

Declarations:                                                   
              /* nada */                                                    { $$ = NULL; }
            | VarDeclaration SEMICOLON Declarations                         { $$ = $1; add_irmao($1, $3); }
            | FuncDeclaration SEMICOLON Declarations                        { $$ = $1; add_irmao($1, $3); }
            ;

VarDeclaration: 
              VAR IDENTIFIER VarSpec Type                                   { $$ = newnode(VarDecl, NULL); 
                                                                              add_filho($$, $4); 
                                                                              add_filho($$, newnode(Identifier, $2)); 
                                                                              tipo_var($3, $4);
                                                                              add_irmao($$, $3); }

            | VAR LPAR IDENTIFIER VarSpec Type SEMICOLON RPAR               { $$ = newnode(VarDecl, NULL);
                                                                              add_filho($$, $5); 
                                                                              add_filho($$, newnode(Identifier, $3));
                                                                              tipo_var($4, $5);
                                                                              add_irmao($$, $4); }
            ;

VarSpec: 
              /* nada */                                                    { $$ = NULL; }
            | COMMA IDENTIFIER VarSpec                                      { $$ = newnode(VarDecl, NULL); 
                                                                              add_filho($$, newnode(Identifier, $2));
                                                                              add_irmao($$, $3); }

Type: 
              INT                                                           { $$ = newnode(Int, NULL); }
            | FLOAT32                                                       { $$ = newnode(Float32, NULL); }
            | BOOL                                                          { $$ = newnode(Bool, NULL); }
            | STRING                                                        { $$ = newnode(String, NULL); }

FuncDeclaration: 
              FUNC IDENTIFIER LPAR Parameters RPAR Type FuncBody            { $$ = newnode(FuncDecl, NULL);
                                                                              struct node *aux = newnode(FuncHeader, NULL); 
                                                                              add_filho(aux, newnode(Identifier, $2));
                                                                              add_filho(aux, $6); 
                                                                              add_filho(aux, $4);
                                                                              add_filho($$, aux); 
                                                                              add_filho($$, $7); }

            | FUNC IDENTIFIER LPAR RPAR Type FuncBody                       {  $$ = newnode(FuncDecl, NULL);
                                                                              struct node *aux = newnode(FuncHeader, NULL); 
                                                                              add_filho(aux, newnode(Identifier, $2));
                                                                              add_filho(aux, $5); 
                                                                              add_filho(aux, newnode(FuncParams, NULL)); 
                                                                              add_filho($$, aux);
                                                                              add_filho($$, $6); }

            | FUNC IDENTIFIER LPAR Parameters RPAR FuncBody                 { $$ = newnode(FuncDecl, NULL);
                                                                              struct node *aux = newnode(FuncHeader, NULL); 
                                                                              add_filho(aux, newnode(Identifier, $2));
                                                                              add_filho(aux, $4); 
                                                                              add_filho($$, aux);
                                                                              add_filho($$, $6); }

            | FUNC IDENTIFIER LPAR RPAR FuncBody                            { $$ = newnode(FuncDecl, NULL);
                                                                              struct node *aux = newnode(FuncHeader, NULL); 
                                                                              add_filho(aux, newnode(Identifier, $2));
                                                                              add_filho(aux, newnode(FuncParams, NULL)); 
                                                                              add_filho($$, aux);
                                                                              add_filho($$, $5); }
            ;

Parameters: 
            ParamDecl ParamRep                                              { $$ = newnode(FuncParams, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $2); }
            ;

ParamDecl:        
            IDENTIFIER Type                                                 { $$ = newnode(ParamDecl, NULL); 
                                                                              add_filho($$, $2); 
                                                                              add_filho($$, newnode(Identifier, $1)); }                               

ParamRep: 
              /* nada */                                                    { $$ = NULL; }
            | COMMA ParamDecl ParamRep                                      { $$ = $2; 
                                                                              add_irmao($$, $3); }
            ;

FuncBody:   
            LBRACE VarsAndStatements RBRACE                                 { $$ = newnode(FuncBody, NULL); 
                                                                              add_filho($$, $2); }
            ;

VarsAndStatements:
              /* nada */                                                    { $$ = NULL; }
            | VarDeclaration SEMICOLON VarsAndStatements                    { if($$ == NULL){ $$ = $3; }
                                                                              else{ $$ = $1; add_irmao($$, $3); } }

            | Statement SEMICOLON VarsAndStatements                         { if($$ == NULL){ $$ = $3; }
                                                                              else{ $$ = $1; add_irmao($$, $3); } }

            | SEMICOLON VarsAndStatements                                   { $$ = $2; }
            ;

Statement:  
              IDENTIFIER ASSIGN Expr                                        { $$ = newnode(Assign, NULL);
                                                                              if($3 != NULL){ add_filho($$, newnode(Identifier, $1)); add_filho($$, $3); }}

            | LBRACE StateRep RBRACE                                        { if($2 != NULL){
                                                                                if(num_filhos($2) < 2){ $$ = $2; }
                                                                                else{ $$ = newnode(Block, NULL);
                                                                                      add_filho($$, $2);}

                                                                              }
                                                                              else{ $$ = NULL; } }

            | IF Expr LBRACE StateRep RBRACE %prec IF_SEM_ELSE                { $$ = newnode(If, NULL); 
                                                                              struct node *bloco = newnode(Block, NULL); 
                                                                              add_filho(bloco, $4); 
                                                                              if($2 != NULL){
                                                                                add_filho($$, $2);
                                                                                add_filho($$, bloco);
                                                                                add_filho($$, newnode(Block, NULL)); } } 

            | IF Expr LBRACE StateRep RBRACE ELSE LBRACE StateRep RBRACE    { $$ = newnode(If, NULL);
                                                                              struct node *bloco_if = newnode(Block, NULL); 
                                                                              add_filho(bloco_if, $4); 
                                                                              struct node *bloco_else = newnode(Block, NULL);
                                                                              add_filho(bloco_else, $8);
                                                                              if($2 != NULL){
                                                                                add_filho($$, $2);
                                                                                add_filho($$, bloco_if);
                                                                                add_filho($$, bloco_else); } }

            | FOR LBRACE StateRep RBRACE                                    { $$ = newnode(For, NULL);
                                                                              struct node *bloco = newnode(Block, NULL);
                                                                              add_filho(bloco, $3);
                                                                              if($2 != NULL){ add_filho($$, bloco); } }

            | FOR Expr LBRACE StateRep RBRACE                               { $$ = newnode(For, NULL); 
                                                                              struct node *bloco = newnode(Block, NULL);
                                                                              add_filho(bloco, $4);
                                                                              if($2 != NULL){
                                                                                add_filho($$, $2);
                                                                                add_filho($$, bloco); } }
                                                                                
            | RETURN                                                        { $$ = newnode(Return, NULL); }
            | RETURN Expr                                                   { $$ = newnode(Return, NULL);
                                                                              add_filho($$, $2); }

            | FuncInvocation                                                { $$ = newnode(Call, NULL); 
                                                                              add_filho($$, $1); }

            | ParseArgs                                                     { $$ = newnode(ParseArgs, NULL); 
                                                                              add_filho($$, $1); }

            | PRINT LPAR Expr RPAR                                          { $$ = newnode(Print, NULL); 
                                                                              add_filho($$, $3); }

            | PRINT LPAR STRLIT RPAR                                        { $$ = newnode(Print, NULL); 
                                                                              add_filho($$, newnode(StrLit, $3)); }
            | error                                                         { $$ = NULL; }
            ;

StateRep: 
              /* nada */                                                    { $$ = NULL; }
            | Statement SEMICOLON StateRep                                  { if($$ != NULL) {$$ = $1; add_irmao($1, $3); }
                                                                              else{$$ = $3;} }
            ;
          
ParseArgs: 
              IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR         { $$ = newnode(Identifier, $1); 
                                                                                                add_irmao($$, $9); }

            | IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR                        { $$ = NULL; }
            ;

FuncInvocation: 
              IDENTIFIER LPAR RPAR                                          { $$ = newnode(Identifier, $1); }
            | IDENTIFIER LPAR Expr FuncInvRep RPAR                          { $$ = newnode(Identifier, $1); 
                                                                              add_irmao($$, $3); 
                                                                              add_irmao($$, $4); }

            | IDENTIFIER LPAR error RPAR                                    { $$ = NULL; }
            ;

FuncInvRep:
              /* nada */                                                    { $$ = NULL; }
            | COMMA Expr FuncInvRep                                         { $$ = $2; 
                                                                              add_irmao($$, $3); }
            ;

Expr: 
              Expr OR Expr                                                  { $$ = newnode(Or, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr AND Expr                                                 { $$ = newnode(And, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr LT Expr                                                  { $$ = newnode(Lt, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr GT Expr                                                  { $$ = newnode(Gt, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr EQ Expr                                                  { $$ = newnode(Eq, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr NE Expr                                                  { $$ = newnode(Ne, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr LE Expr                                                  { $$ = newnode(Le, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr GE Expr                                                  { $$ = newnode(Ge, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr PLUS Expr                                                { $$ = newnode(Add, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr MINUS Expr                                               { $$ = newnode(Sub, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr STAR Expr                                                { $$ = newnode(Mul, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr DIV Expr                                                 { $$ = newnode(Div, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | Expr MOD Expr                                                 { $$ = newnode(Mod, NULL); 
                                                                              add_filho($$, $1); 
                                                                              add_filho($$, $3); }

            | NOT Expr %prec NOT                                            { $$ = newnode(Not, NULL); 
                                                                              add_filho($$, $2); }

            | MINUS Expr %prec NOT                                          { $$ = newnode(Minus, NULL); 
                                                                              add_filho($$, $2); }

            | PLUS Expr %prec NOT                                           { $$ = newnode(Plus, NULL); 
                                                                              add_filho($$, $2); }

            | NATURAL                                                       { $$ = newnode(Natural, $1); }
            | DECIMAL                                                       { $$ = newnode(Decimal, $1); }
            | IDENTIFIER                                                    { $$ = newnode(Identifier, $1); }
            | FuncInvocation                                                { $$ = newnode(Call, NULL); 
                                                                              add_filho($$, $1); }
            | LPAR Expr RPAR                                                { $$ = $2; }
            | LPAR error RPAR                                               { $$ = NULL; }
            ;

%%

