#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

char *categorias[] = {"Program", "VarDecl", "Int", "Float32", "Bool", "String", "FuncDecl", "FuncHeader", "Identifier", "FuncParams", "ParamDecl", "FuncBody", "Assign", "Block", 
                    "If", "For", "Return", "Call", "ParseArgs", "Print", "StrLit", "Or", "And", "Lt", "Gt", "Eq", "Ne", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", 
                    "Not", "Minus", "Plus", "Natural", "Decimal"};
                    
// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token) {
    struct node *novo = malloc(sizeof(struct node));
    novo->category = category;
    novo->token = token;
    novo->filhos = NULL;
    novo->irmaos = NULL;
    return novo;
}

// append a node to the list of children of the parent node
void add_filho(struct node *pai, struct node *filho) {

    if (pai->filhos == NULL) {
        pai->filhos = malloc(sizeof(struct node_list));
        pai->filhos->node = filho;
        pai->filhos->next = NULL;
    } else {
        struct node_list *filhos = pai->filhos;
        while (filhos->next != NULL) {
            filhos = filhos->next;
        }
        filhos->next = malloc(sizeof(struct node_list));
        filhos->next->node = filho;
        filhos->next->next = NULL;
    }
}

void add_irmao(struct node *no, struct node *irmao){

    if (no->irmaos == NULL) {
        no->irmaos = malloc(sizeof(struct node_list));
        no->irmaos->node = irmao;
        no->irmaos->next = NULL;
    } else {
        struct node_list *irmaos = no->irmaos;
        while (irmaos->next != NULL) {
            irmaos = irmaos->next;
        }
        irmaos->next = malloc(sizeof(struct node_list));
        irmaos->next->node = irmao;
        irmaos->next->next = NULL;
    }
}

void tipo_var(struct node *no, struct node *tipo){

    if(no == NULL || tipo == NULL){
        return;
    }

    struct node_list *nova_lista = malloc(sizeof(struct node_list));
    nova_lista->node = tipo;
    nova_lista->next = no->filhos;
    no->filhos = nova_lista;

    struct node_list *irmaos = no->irmaos;
    while(irmaos != NULL){
        tipo_var(irmaos->node, tipo);
        irmaos = irmaos->next;
    }
}

int num_filhos(struct node *no){

    if(no == NULL){
        return 0;
    }

    int num = 1;

    struct node_list *irmaos = no->irmaos;
    while(irmaos != NULL && irmaos->node != NULL){
            num++;
            irmaos = irmaos->node->irmaos;
    } 

    return num;
}

void show(struct node *no, int profundidade){

    if(no == NULL){
        return;
    }

    for(int i = 0; i < profundidade; i++){
        printf("..");
    }

    printf("%s", categorias[no->category]);

    if(no->token != NULL){
        printf("(%s)", no->token);
    }
    printf("\n");

    if (no->filhos != NULL) {
        struct node_list *filhos = no->filhos;
        while (filhos != NULL) {
            show(filhos->node, profundidade + 1);
            filhos = filhos->next;
        }
    }

    if (no->irmaos != NULL) {
        struct node_list *irmaos = no->irmaos;
        while (irmaos != NULL) {
            show(irmaos->node, profundidade);
            irmaos = irmaos->next;
        }
    }
}

void free_arvore(struct node *no) {

    if (no == NULL){
        return;
    }

    // Free recursivo aos nos filhos
    struct node_list *filhos = no->filhos;
    while (filhos != NULL) {
        struct node_list *prox = filhos->next;
        free_arvore(filhos->node);
        free(filhos);
        filhos = prox;
    }

    // Free recursivo aos nos irmãos
    struct node_list *irmaos = no->irmaos;
    while (irmaos != NULL) {
        struct node_list *prox = irmaos->next;
        free(irmaos);
        irmaos = prox;
    }

    // Free ao nó
    free(no);
}
