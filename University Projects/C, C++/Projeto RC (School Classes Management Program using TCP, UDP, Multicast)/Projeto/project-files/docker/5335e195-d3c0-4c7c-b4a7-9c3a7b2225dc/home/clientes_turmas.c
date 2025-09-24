#include "class_server.h"

void insere_clientes(Cliente *clientes, int* num_clientes, char *nome, char *passe, char *funcao){

    Cliente *cliente = (Cliente*)malloc(sizeof(Cliente));
    strcpy(cliente->nome, nome);
    strcpy(cliente->passe, passe);
    strcpy(cliente->funcao, funcao);
    clientes->num_turmas = 0;
    clientes[*num_clientes] = *cliente;
    (*num_clientes)++;
}

char *elimina_clientes(Cliente *clientes, int *num_clientes, char *nome){

    for(int i = 0; i < *num_clientes; i++){
        if(strcmp(clientes[i].nome, nome) == 0){
            for(int j = i; j < *num_clientes - 1; j++){
                clientes[j] = clientes[j + 1];
            }
            (*num_clientes)--;
            return "Cliente eliminado com sucesso!\n";
        }
    }

    return "Não foi possível eliminar o cliente pretendido. Não existe nenhum cliente com esse nome!\n";
}

char *imprime_clientes(Cliente *clientes, int *num_clientes){

    char *str;
    str = (char*)malloc(sizeof(char) * 10000);

    if (num_clientes == 0){
        return "Não existem clientes!\n";
    }
    for(int i = 0; i < *num_clientes; i++){
        char nome[MAX_STRING] = "";
        char funcao[MAX_STRING] = "";
        strcat(nome, "Nome: ");
        strcat(nome, clientes[i].nome);
        strcat(nome, "\n");
        strcat(str, nome);
        strcat(funcao, "Função: ");
        strcat(funcao, clientes[i].funcao);
        strcat(funcao, "\n");
        strcat(str, funcao);
        if(i < *num_clientes - 1){
            strcat(str, "-----------------------------\n");
        }
    }

    return str;
}

Cliente *verifica_cliente(Cliente *clientes, int *num_clientes, char *nome, char *passe){

    for(int i = 0; i < *num_clientes; i++){
        if(strcmp(clientes[i].nome, nome) == 0){
            if(strcmp(clientes[i].passe, passe) == 0){
                return &(clientes[i]);
            }
        }
    }
    return NULL;
}

int verifica_nome(Cliente *clientes, int *num_clientes, char *nome){

    for(int i = 0; i < *num_clientes; i++){
        if(strcmp(clientes[i].nome, nome) == 0){
            return -1;
        }
    }
    return 1;
}

int verifica_ip(Turma *turmas, int *num_turmas, char *ip){

  for(int i = 0; i < *num_turmas; i++){
    if(strcmp(ip, turmas[i].ip) == 0){
      return -1;
    }
  }
  return 1;

}
char *gere_ip(Turma *turmas, int *num_turmas){

  char *ip = (char*) malloc(sizeof(char) * MAX_STRING);
  do{
    int oct_1 = (rand() % (239 - 224 + 1)) + 224;
    int oct_2 = rand() % 256;
    int oct_3 = rand() % 256;
    int oct_4 = rand() % 256;

    sprintf(ip, "%d.%d.%d.%d", oct_1, oct_2, oct_3, oct_4);    

  } while(verifica_ip(turmas, num_turmas, ip) < 0);

  return ip;
}

int criar_multicast(char *ip){

    struct sockaddr_in addr;
    int socket_multicast;
    if ((socket_multicast = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(5000);
    int enable = 3;
    if (setsockopt(socket_multicast, IPPROTO_IP, IP_MULTICAST_TTL, &enable, sizeof(enable)) < 0){
        perror("setsockopt");
        exit(1);
    }
    return socket_multicast;
}

char *imprime_turmas(Turma *turmas, int num_turmas){

    char *str = (char*)malloc(sizeof(char) * 10000);;
    
    if(num_turmas == 0){
        strcpy(str, "Não há turmas na lista!\n");
        return str;
    }
    strcat(str, "CLASS ");
    for(int i = 0; i < num_turmas; i++){
        char *nome = (char*)malloc(sizeof(char) * 10000);
        strcat(nome, "{");
        strcat(nome, turmas[i].nome);
        strcat(nome, "}");
        if(i != num_turmas - 1){
            strcat(nome, ",");
        }
        strcat(str, nome);

    }
    return str;
}

char *imprime_turmas_inscritas(Turma *turmas, int num_turmas){

    char *str = (char*)malloc(sizeof(char) * 10000);;
    
    if(num_turmas == 0){
        strcpy(str, "Não há turmas na lista!\n");
        return str;
    }
    strcat(str, "CLASS ");
    for(int i = 0; i < num_turmas; i++){
        char *nome = (char*)malloc(sizeof(char) * 10000);
        char *ip = (char*)malloc(sizeof(char) * 10000);
        strcat(nome, "{");
        strcat(nome, turmas[i].nome);
        strcat(nome, " / ");
        strcat(str, nome);
        strcat(ip, turmas[i].ip);
        strcat(ip, "}");
        if(i != num_turmas - 1){
            strcat(ip, ",");
        }
        strcat(str, ip);
    }
    return str;
}

int verifica_turma(Turma *turmas, int *num_turmas, char* nome){

    for(int i = 0; i < *num_turmas; i++){
        if(strcmp(turmas[i].nome, nome) == 0){
            return i;
        }
    }
    return -1;
}