#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <arpa/inet.h>

#define BUF_SIZE    1024
#define MAX_CLIENTES 200
#define MAX_TURMAS 100
#define MAX_STRING  100

typedef struct{
    char nome[MAX_STRING];
    char ip[MAX_STRING];
    int socket;
    int tamanho_max;
    int tamanho_atual;
} Turma;

typedef struct{
    char nome[MAX_STRING];
    char passe[MAX_STRING];
    char funcao[MAX_STRING];
    Turma turmas[MAX_TURMAS];
    int num_turmas;
} Cliente;

void insere_clientes(Cliente *clientes, int *num_clientes, char *nome, char *passe, char* funcao);
char *elimina_clientes(Cliente *clientes, int *num_clientes, char *nome);
char *imprime_clientes(Cliente *clientes, int *num_clientes);
Cliente *verifica_cliente(Cliente *clientes, int *num_clientes, char *nome, char *passe);
int verifica_nome(Cliente *clientes, int *num_clientes, char *nome);
int verifica_ip(Turma *turmas, int *num_turmas, char *ip);
char *gere_ip(Turma *turmas, int *num_turmas);
int criar_multicast(char *ip);
char *imprime_turmas(Turma *turmas, int num_turmas);
char *imprime_turmas_inscritas(Turma *turmas, int num_turmas);
int verifica_turma(Turma *turmas, int *num_turmas, char* nome);

#endif 