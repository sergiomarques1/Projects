#ifndef HOSPITAL_H
#define HOSPITAL_H

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Definições
#define MAX 50

// Inicialização da estrutura para uma Data
typedef struct{ 
    int dia;
    int mes;
    int ano;
} Data;

// Inicialização da estrutura para um Registo
typedef struct{ 
    Data data;
    int tensao_min;
    int tensao_max;
    int peso;
    int altura;
} Registo;

// Inicialização da estrutura para um no de um Registo
typedef struct noRegisto{ 
    Registo registo;
    struct noRegisto *prox;
} noRegisto;

// Inicialização da estrutura para uma lista de Registos
typedef struct{ 
    noRegisto *inicio;
} ListaRegistos;

// Inicialização da estrutura para um Doente
typedef struct{ 
    int id;
    char nome[MAX];
    Data data;
    char cc[MAX];
    char telefone[MAX];
    char email[MAX];
    ListaRegistos *registos;
} Doente;

// Inicialização da estrutura para um no de um Doente
typedef struct noDoente{ 
    Doente doente;
    struct noDoente *prox;
} noDoente;

// Inicialização da estrutura para uma lista de Doentes
typedef struct{ 
    noDoente *inicio;
} ListaDoentes;


// Declaração de funções
void carregar_ficheiro_doentes(char *nome_ficheiro, ListaDoentes *doentes, int *id);
void guardar_ficheiro_doentes(char *nome_ficheiro, ListaDoentes *doentes);
void carregar_ficheiro_registos(char *nome_ficheiro, ListaDoentes *doentes);
void guardar_ficheiro_registos(char *nome_ficheiro, ListaDoentes *doentes);
void cria_doentes(ListaDoentes *doentes);
int vazia_doentes(ListaDoentes *doentes);
void pesquisa_doentes_nome(ListaDoentes *doentes, noDoente **ant, noDoente **atual, char *nome);
void pesquisa_doentes_id(ListaDoentes *doentes, noDoente **ant, noDoente **atual, int id);
void pesquisa_doentes_max(ListaDoentes *doentes, noDoente **ant, noDoente **atual, int tensao_max);
void insere_doentes_max(ListaDoentes *doentes, Doente doente);
void insere_doentes(ListaDoentes *doentes, Doente doente);
void retira_doentes(ListaDoentes *doentes, int id);
void destroi_doentes(ListaDoentes *doentes);
void cria_registos(ListaRegistos *registos);
int vazia_registos(ListaRegistos *registos);
void pesquisa_registos_max(ListaRegistos *registos, noRegisto **ant, noRegisto **atual, int tensao_max);
void pesquisa_registos_data(ListaRegistos *registos, noRegisto **ant, noRegisto **atual, Data data);
void insere_registos_max(ListaRegistos *registos, Registo registo);
void insere_registos_data(ListaRegistos *registos, Registo registo);
void destroi_registos(ListaRegistos *registos);
void imprime_doentes(ListaDoentes *doentes);
void adicionar_doentes(ListaDoentes *doentes, int id, char *nome, Data data, char *cc, char *telefone, char *email);
void efetuar_registos(noDoente *no, Data data, int tensao_min, int tensao_max, int peso, int altura);
void tensoes_altas(ListaDoentes *doentes);
void imprime_registos(ListaDoentes *doentes);
int verificar_data(int dia, int mes, int ano);
int verificar_telefone(char *telefone);
int verificar_cc(ListaDoentes *doentes, char *cc);
int verificar_mail(char *email);
int verificar_id(char *id);
int verificar_nome(char *nome);
int verificar_tensao_max(char *id);
int verificar_tensao_min(char *tensao_min);
int verificar_peso(char *peso);
int verificar_altura(char *altura);
void ler_apagar_doentes(ListaDoentes *doentes);
void ler_adicionar_doentes(ListaDoentes *doentes, int id);
void ler_efetuar_registos(ListaDoentes *doentes);
void imprime_doentes(ListaDoentes *doentes);
void ler_imprime_tensoes_altas(ListaDoentes *doentes);
void ler_imprime_registos(ListaDoentes *doentes);
int comparar_datas(Data data1, Data data2);

#endif