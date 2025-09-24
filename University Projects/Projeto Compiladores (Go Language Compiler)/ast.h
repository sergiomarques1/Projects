#ifndef _AST_H
#define _AST_H

enum category { Program, VarDecl, Int, Float32, Bool, String, FuncDecl, FuncHeader, Identifier, FuncParams, ParamDecl, FuncBody, Assign, Block, If, For, Return, 
                Call, ParseArgs, Print, StrLit, Or, And, Lt, Gt, Eq, Ne, Le, Ge, Add, Sub, Mul, Div, Mod, Not, Minus, Plus, Natural, Decimal };

struct node {
    enum category category;
    char *token;
    struct node_list *filhos;
    struct node_list *irmaos;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void add_filho(struct node *pai, struct node *filho);
void add_irmao(struct node *no, struct node *irmao);
void tipo_var(struct node *no, struct node *tipo);
int num_filhos(struct node *no);
void free_arvore(struct node *program);
void show(struct node *program, int profundidade);

#endif
