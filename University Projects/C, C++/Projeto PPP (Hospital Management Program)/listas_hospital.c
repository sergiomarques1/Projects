#include "hospital.h"

// Função que inicializa uma lista de doentes
void cria_doentes(ListaDoentes *doentes){
    doentes->inicio = NULL;
}

// Função que verifica se uma lista de doentes está vazia
int vazia_doentes(ListaDoentes *doentes){ 
    if(doentes->inicio == NULL){
        return 1;
    }
    return 0;
}

// Função que encontra a posição correta para inserir um novo no através do nome do doente com o objetivo de os ordenar por ordem alfabética
void pesquisa_doentes_nome(ListaDoentes *doentes, noDoente **ant, noDoente **atual, char *nome){ 
    *ant = NULL;
    *atual = doentes->inicio;
    while((*atual) != NULL && strcmp((*atual)->doente.nome, nome) <= 0){
        *ant = *atual;
        *atual = (*atual)->prox;
    }
}

// Função que encontra a posição correta para eliminar um no através do id do doente
void pesquisa_doentes_id(ListaDoentes *doentes, noDoente **ant, noDoente **atual, int id){ 
    *ant = NULL;
    *atual = doentes->inicio;
    while((*atual) != NULL && (*atual)->doente.id != id){
        *ant = *atual;
        *atual = (*atual)->prox;
    }
}

// Função que encontra a posição correta para inserir um novo no através da maior tensão máxima dos registos do doente com o objetivo de os ordenar por ordem decrescente
void pesquisa_doentes_max(ListaDoentes *doentes, noDoente **ant, noDoente **atual, int tensao_max){ 
    *ant = NULL;
    *atual = doentes->inicio;
    while((*atual) != NULL && (*atual)->doente.registos->inicio->registo.tensao_max > tensao_max){
        *ant = *atual;
        *atual = (*atual)->prox;
    }
}

// Função que insere um doente na lista através da maior tensão máxima dos seus registos
void insere_doentes_max(ListaDoentes *doentes, Doente doente){ 
    noDoente *ant;
    noDoente *atual;
    noDoente *no = (noDoente*)malloc(sizeof(noDoente));
    if(no != NULL){
        no->doente = doente;
        pesquisa_doentes_max(doentes, &ant, &atual, no->doente.registos->inicio->registo.tensao_max);
        if(ant != NULL){
            no->prox = atual;
            ant->prox = no;
        }
        else{
            doentes->inicio = no;
            no->prox = atual;
        }
    }
}

// Função que insere um doente na lista através do seu nome
void insere_doentes(ListaDoentes *doentes, Doente doente){ 
    noDoente *ant;
    noDoente *atual;
    noDoente *no = (noDoente*)malloc(sizeof(noDoente));
    if(no != NULL){
        no->doente = doente;
        pesquisa_doentes_nome(doentes, &ant, &atual, no->doente.nome);
        if(ant != NULL){
            no->prox = atual;
            ant->prox = no;
        }
        else{
            doentes->inicio = no;
            no->prox = atual;
        }
    } 
}

// Função que elimina um doente da lista através do seu id
void retira_doentes(ListaDoentes *doentes, int id){ 
    noDoente *ant;
    noDoente *atual;
    pesquisa_doentes_id(doentes, &ant, &atual, id);
    if(atual != NULL && atual->doente.id == id){
        if(ant != NULL){
            ant->prox = atual->prox;
        }
        else{
            doentes->inicio = atual->prox;
        }
        destroi_registos(atual->doente.registos);
        free(atual);
        printf("\nDoente eliminado com sucesso!\n");
    }
    else{
        printf("\nNão existe um doente com esse ID!\n");
    }
}

// Função que esvazia uma lista de doentes
void destroi_doentes(ListaDoentes *doentes){ 
    noDoente *no;
    while(doentes->inicio != NULL){
        no = doentes->inicio;
        doentes->inicio = doentes->inicio->prox;
        destroi_registos(no->doente.registos);
        free(no);
    }
}

// Função que inicializa uma lista de registos
void cria_registos(ListaRegistos *registos){ 
    registos->inicio = NULL;
}

// Função que verifica se uma lista de registos está vazia
int vazia_registos(ListaRegistos *registos){ 
    if(registos->inicio == NULL){
        return 1;
    }
    return 0;
}

// Função que encontra a posição correta para inserir um novo no através da data do registo com o objetivo de os ordenar por ordem decrescente
void pesquisa_registos_data(ListaRegistos *registos, noRegisto **ant, noRegisto **atual, Data data){ 
    *ant = NULL;
    *ant = NULL;
    *atual = registos->inicio;

    while((*atual) != NULL && comparar_datas(data, (*atual)->registo.data) < 0){
        *ant = *atual;
        *atual = (*atual)->prox;
    }
}

// Função que encontra a posição correta para inserir um novo no através da tensão máxima do registo com o objetivo de os ordenar por ordem decrescente
void pesquisa_registos_max(ListaRegistos *registos, noRegisto **ant, noRegisto **atual, int tensao_max){ 
    *ant = NULL;
    *atual = registos->inicio;

    while((*atual) != NULL && (*atual)->registo.tensao_max > tensao_max){
        *ant = *atual;
        *atual = (*atual)->prox;
    }
}

// Função que insere um registo na lista através da sua data
void insere_registos_data(ListaRegistos *registos, Registo registo){ 
    noRegisto *ant;
    noRegisto *atual;
    noRegisto *no = (noRegisto*)malloc(sizeof(noRegisto));
    if(no != NULL){
        no->registo = registo;
        pesquisa_registos_data(registos, &ant, &atual, no->registo.data);
        if(ant != NULL){
            no->prox = atual;
            ant->prox = no;
        }
        else{
            registos->inicio = no;
            no->prox = atual;
        }
    } 
}

// Função que insere um registo na lista através do sua tensão máxima
void insere_registos_max(ListaRegistos *registos, Registo registo){ 
    noRegisto *ant;
    noRegisto *atual;
    noRegisto *no = (noRegisto*)malloc(sizeof(noRegisto));
    if(no != NULL){
        no->registo = registo;
        pesquisa_registos_max(registos, &ant, &atual, no->registo.tensao_max);
        if(ant != NULL){
            no->prox = atual;
            ant->prox = no;
        }
        else{
            registos->inicio = no;
            no->prox = atual;
        }
    } 
}

// Função que esvazia uma lista de registos
void destroi_registos(ListaRegistos *registos){ 
    noRegisto *no;
    while(registos->inicio != NULL){
        no = registos->inicio;
        registos->inicio = registos->inicio->prox;
        free(no);
    }
    free(registos);
}

// Função que cria um doente novo e adicina-o à lista através das informações que entrem como parametros
void adicionar_doentes(ListaDoentes *doentes, int id, char *nome, Data data, char *cc, char *telefone, char *email){ 
    ListaRegistos *registos = (ListaRegistos*)malloc(sizeof(ListaRegistos)); // alocação de memoria para uma lista de registos
    Doente *novo; // criação do novo doente
    novo = (Doente *)malloc(sizeof(Doente)); // alocação de memoria para o novo doente
    if(novo != NULL){
        novo->id = id; // atribuição do id ao doente
        strcpy(novo->nome,nome); // atribuição do nome ao doente
        novo->data = data; // atribuição da data ao doente
        strcpy(novo->cc,cc); // atribuição do cc ao doente
        strcpy(novo->telefone,telefone); // atribuição do telefone ao doente
        strcpy(novo->email,email); // atribuição do email ao doente
        cria_registos(registos); // inicialização da lista de registos
        novo->registos = registos; // atribuição da lista de registos ao doente
        insere_doentes(doentes, *novo); // chamada da função que insere o novo doente na lista de doentes
        free(novo); // Libertação de memoria previamente alocada para o novo doente
    }
}

// Função que cria um registo novo e adicina-o à lista de um doente através das informações que entrem como parametros
void efetuar_registos(noDoente *no, Data data, int tensao_min, int tensao_max, int peso, int altura){ 
    Registo *registo; // criação do novo registo
    registo = (Registo *)malloc(sizeof(Registo)); // alocação de memoria para o novo registo
    if (registo != NULL){
        registo->data = data; // atribuição da data ao registo
        registo->tensao_min = tensao_min; // atribuição da tensão min ao registo
        registo->tensao_max = tensao_max; // atribuição da tensão max ao registo
        registo->peso = peso; // atribuição do peso ao registo
        registo->altura = altura; // atribuição da altura ao registo
        insere_registos_data(no->doente.registos, *registo); // chamada da função que insere o novo registo na lista de registos de um determinado doente
        free(registo); // Libertação de memoria previamente alocada para o novo registo
    }
}