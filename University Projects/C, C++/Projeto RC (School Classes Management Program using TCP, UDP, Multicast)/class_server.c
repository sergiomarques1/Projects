#include "class_server.h"

void processar_admin(int s, Cliente *clientes, int *num_clientes, struct sockaddr_in si_outra, socklen_t slen, char nome_ficheiro[]);     // função que processa as ligações udp (ligações feitas por administradores)
void processar_cliente(int client_fd, Cliente *clientes, int *num_clientes, Turma *turmas, int *num_turmas);      // função que processa as ligações tcp (ligações feitas por clientes (aluno, professor))
void importar(Cliente *clientes, int *num_clientes, char nome_ficheiro[]);       // função que importa do ficheiro de texto os nomes e os IPs associados aos domínios
void escrever_ficheiro(Cliente *clientes, int *num_clientes, char nome_ficheiro[]);
void ctrl_c_handler();
void erro(char *msg);

int udp_pid, tcp_pid;

int main(int argc, char *argv[]) {

  if (argc != 4) {
    printf("servidor <PORT0_TURMAS> <PORTO_CONFIG> <ficheiro_config>\n");
    exit(-1);
  }

  // criação da shared memory para os clientes
  int shmid_clientes = shmget(IPC_PRIVATE, sizeof(Cliente) * MAX_CLIENTES + sizeof(int), IPC_CREAT | 0666);
  if(shmid_clientes < 0){
    erro("ID memória partilhada clientes");
  }

  void *shm_clientes = shmat(shmid_clientes, NULL, 0);
  if(shm_clientes == (void *) -1){
    erro("Anexação memória partilhada clientes");
  }

  Cliente *clientes = (Cliente*) shm_clientes;
  int *num_clientes = (int *) (shm_clientes + sizeof(Cliente) * MAX_CLIENTES);
  *num_clientes = 0;

  // criação da shared memory para as turmas
  int shmid_turmas = shmget(IPC_PRIVATE, sizeof(Turma) * MAX_TURMAS + sizeof(int), IPC_CREAT | 0666);
  if(shmid_turmas < 0){
    erro("ID memória partilhada turmas");
  }

  void *shm_turmas = shmat(shmid_turmas, NULL, 0);
  if(shm_turmas == (void *) -1){
    erro("Anexação memória partilhada turmas");
  }

  Turma *turmas = (Turma*) shm_turmas;
  int *num_turmas = (int *) (shm_turmas + sizeof(Turma) * MAX_TURMAS);
  *num_turmas = 0;

  importar(clientes, num_clientes, argv[3]);     // chamada da função que importa os dados do ficheiro de texto

  int PORTO_TURMAS = atoi(argv[1]);
  int PORTO_CONFIG = atoi(argv[2]);
  int fd, client, s;  
  struct sockaddr_in addr, client_addr, si_minha, si_outra;
  int client_addr_size;

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORTO_TURMAS);

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);

  socklen_t slen = sizeof(si_outra);

	// Cria um socket para recepção de pacotes UDP
	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == -1) {
		erro("Erro na criação do socket");
	}

  // Preenchimento da socket address structure
	si_minha.sin_family = AF_INET;
	si_minha.sin_port = htons(PORTO_CONFIG);
	si_minha.sin_addr.s_addr = htonl(INADDR_ANY);

  // Associa o socket à informação de endereço
	if(bind(s,(struct sockaddr*)&si_minha, sizeof(si_minha)) == -1) {
		erro("Erro no bind");
	}

  signal(SIGTERM, ctrl_c_handler);

  if((udp_pid = fork()) == 0){
    signal(SIGTERM, ctrl_c_handler);
    processar_admin(s, clientes, num_clientes, si_outra, slen, argv[3]);
  }
  
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    if (client > 0) {
      if ((tcp_pid = fork()) == 0) {
        signal(SIGTERM, ctrl_c_handler);
        close(fd);
        processar_cliente(client, clientes, num_clientes, turmas, num_turmas);
        exit(0);
      }
      close(client);
    }
  }
  close(s);

  wait(NULL);

  shmdt(shm_clientes);
  shmctl(shmid_clientes, IPC_RMID, NULL);
  shmdt(shm_turmas);
  shmctl(shmid_turmas, IPC_RMID, NULL);

  return 0;
}

void processar_admin(int s, Cliente *clientes, int *num_clientes, struct sockaddr_in si_outra, socklen_t slen, char nome_ficheiro[]){

  int admin_encontrado = 0;     // variável que verifica se já foi feito o login por parte de um utilizador (se já foi ou não encontrado um utilizador) 
  int recv_len;
  char buf[BUF_SIZE];

  while(1){

    recv_len = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *) &si_outra, (socklen_t *)&slen);
    if(recv_len == -1) {
      erro("Erro no recvfrom");
    }
      
    // Para ignorar o restante conteúdo (anterior do buffer)
    buf[recv_len - 1]='\0';

    char *arr_str[100];
    char *str;
    str = strtok(buf, " ");
    int num = 0;
    for(int i = 0; str != NULL; i++){
      arr_str[i] = str;
      str = strtok(NULL, " ");
      num++;
    }
    for(int i = num; i < 20; i++){
      arr_str[i] = " ";
    }

    if(admin_encontrado == 0){
      if(strcmp(arr_str[0], "LOGIN") == 0){
        Cliente *cliente = NULL;
        cliente = verifica_cliente(clientes, num_clientes, arr_str[1], arr_str[2]);
        if(cliente != NULL && strcmp(cliente->funcao, "administrador") == 0){
          admin_encontrado = 1;
          char msg_ok[] = "OK\n";
          //Enviar resposta
          int e = sendto(s, msg_ok, strlen(msg_ok), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
          if(e == -1){
            erro("Error while sending message");
          }
        }
      }
      if(admin_encontrado == 0){
        char msg_rejected[] = "REJECTED\n";
        //Enviar resposta
        int e = sendto(s, msg_rejected, strlen(msg_rejected), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
      }
    }
    else{
      if(strcmp(arr_str[0],"ADD_USER") == 0){
        char *msg_adic;
        if(num == 4){
          if (verifica_nome(clientes, num_clientes, arr_str[1]) > 0){
            if((strcmp(arr_str[3], "aluno") == 0 || strcmp(arr_str[3], "professor") == 0) || strcmp(arr_str[3], "administrador") == 0){
              insere_clientes(clientes, num_clientes, arr_str[1],arr_str[2],arr_str[3]);
              msg_adic = "Cliente adicionado com sucesso!\n";
            }
            else{
              msg_adic = "A função do novo utilizador deverá ser: aluno, professor ou administrador\n";
            }
          }
          else{
            msg_adic = "Já existe um utilizador com esse nome\n";
          }
        }
        else{
          msg_adic = "Dica: ADD_USER <nome do utilizador> <palavra passe> <função>\n";
        }
        //Enviar resposta
        int e = sendto(s, msg_adic, strlen(msg_adic), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
      }
      else if(strcmp(arr_str[0], "DEL") == 0){
        char *msg_elim;
        if(num == 2){
          msg_elim = elimina_clientes(clientes, num_clientes, arr_str[1]);
        }
        else{
          msg_elim = "Dica: DEL <nome do utilizador>\n";
        }
        //Enviar resposta
        int e = sendto(s, msg_elim, strlen(msg_elim), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
      }
      else if(strcmp(arr_str[0], "LIST") == 0){
        char *msg_listar = imprime_clientes(clientes, num_clientes);
        //Enviar resposta
        int e = sendto(s, msg_listar, strlen(msg_listar), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
      }
      else if(strcmp(arr_str[0], "QUIT_SERVER") == 0){
        escrever_ficheiro(clientes, num_clientes, nome_ficheiro);
        char msg_encerrar[] = "Servidor Encerrado!\n";
        //Enviar resposta
        int e = sendto(s, msg_encerrar, strlen(msg_encerrar), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
        kill(tcp_pid, SIGTERM);
        kill(udp_pid, SIGTERM);
      }
      else{
        char msg_aviso[] = "Comando Nao Encontrado!\n"; 
        //Enviar resposta
        int e = sendto(s, msg_aviso, strlen(msg_aviso), 0, (struct sockaddr *)&si_outra, (socklen_t)slen);
        if(e == -1){
          erro("Error while sending message");
        }
      }
    }
  }
}

void processar_cliente(int client_fd, Cliente *clientes, int *num_clientes, Turma *turmas, int *num_turmas)
{

  char msgBV[] = "Bem-vindo! \nUtilize os seus dados de acesso para iniciar a sessão!";      // mensagem de boas-vindas
  write(client_fd, msgBV, strlen(msgBV));     // escrita da mensagem na consola do cliente
  int nread = 0;   
  int mread = 0;
	char buffer[BUF_SIZE];  

  do {
      nread = read(client_fd, buffer, BUF_SIZE-1);
      buffer[nread] = '\0';
      fflush(stdout);
      if(strcmp(buffer, "SAIR") == 0){    // fecho da aplicação do cliente, caso a string lida da consola do mesmo seja "SAIR"
      char sair[] = "A Encerrar a Aplicação...";     // mensagem de despedida
      write(client_fd, sair, strlen(sair));     // escrita da mensagem na consola do cliente
      break;
      }  
      char *arr_str[100];
      char *str;
      str = strtok(buffer, " ");
      int num = 0;
      for(int i = 0; str != NULL; i++){
          arr_str[i] = str;
          str = strtok(NULL, " ");
          num++;
      }
      for(int i = num; i < 20; i++){
        arr_str[i] = " ";
      }
      
      if(strcmp(arr_str[0], "LOGIN") == 0){
        int encontrado = 0;
        Cliente *cliente = NULL;
        cliente = verifica_cliente(clientes, num_clientes, arr_str[1], arr_str[2]);
        if(cliente != NULL && (strcmp(cliente->funcao, "aluno") == 0 || strcmp(cliente->funcao, "professor") == 0)){
          encontrado = 1;
          char msg_ok[] = "OK\n";
          write(client_fd, msg_ok, strlen(msg_ok)); 
          do{
            mread = read(client_fd, buffer, BUF_SIZE-1);
            buffer[mread] = '\0';
            fflush(stdout);
            if(strcmp(buffer, "SAIR") == 0){    // fim da sessão do cliente, caso a string lida da consola do mesmo seja "TERMINAR SESSÃO"
              char terminou[] = "Sessão Terminada!";     // mensagem de fim de sessão
              write(client_fd, terminou, strlen(terminou));     // escrita da mensagem na consola do cliente
              close(client_fd);
              break;
            }

            int count = 0;
            char *arr_str[100];
            char *str;
            str = strtok(buffer, " ");
            for(int i = 0; str != NULL; i++){
                arr_str[i] = str;
                str = strtok(NULL, " ");
                count++;
            }
            char *opcao;
            opcao =  arr_str[0];
            if(strcmp(opcao, "LIST_CLASSES") == 0){
              char *listar_turmas = imprime_turmas(turmas, *num_turmas);
              write(client_fd, listar_turmas, strlen(listar_turmas));
            }
            else if(strcmp(opcao, "LIST_SUBSCRIBED") == 0){
              char *listar_inscritas = imprime_turmas_inscritas(cliente->turmas, cliente->num_turmas);
              write(client_fd, listar_inscritas, strlen(listar_inscritas));
            }
            else if(strcmp(opcao, "SUBSCRIBE_CLASS") == 0){
              char *msg_inscricao = (char*)malloc(sizeof(char) * 10000);
              if(count == 2){
                int indice;
                if((indice = verifica_turma(turmas, num_turmas, arr_str[1])) >= 0){
                  if(verifica_turma(cliente->turmas, &(cliente->num_turmas), arr_str[1]) < 0){
                    if(turmas[indice].tamanho_atual < turmas[indice].tamanho_max){
                      struct sockaddr_in addr;
                      int sock;
                      if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                        perror("socket");
                        close(sock);
                        exit(1);
                      }
                      int reuse = 1;
                      if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
                        perror("SO_REUSEADDR");
                        close(sock);
                        exit(1);
                      }
                      memset(&addr, 0, sizeof(addr));
                      addr.sin_family = AF_INET;
                      addr.sin_addr.s_addr = inet_addr(turmas[indice].ip);
                      addr.sin_port = htons(5000);
                      if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
                        perror("bind");
                        close(sock);
                        exit(1);
                      }
                      struct ip_mreq mreq;
                      mreq.imr_multiaddr.s_addr = inet_addr(turmas[indice].ip);
                      mreq.imr_interface.s_addr = INADDR_ANY;
                      if(setsockopt(turmas[indice].socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
                        erro("setsockopt");
                        close(sock);
                        exit(1);
                      }
                      turmas[indice].tamanho_atual++;
                      cliente->turmas[cliente->num_turmas] = turmas[indice];
                      cliente->num_turmas++;
                      sprintf(msg_inscricao,"ACCEPTED %s",turmas[indice].ip);
                    }
                    else{
                      msg_inscricao = "Impossível fazer a inscrição na turma em questão. Capacidade máxima da turma atingida!";
                    }
                  }
                  else{
                    msg_inscricao = "Já se encontra inscrito na turma em questão";
                  }
                }
                else{
                  msg_inscricao = "Não existe nenhuma turma com esse nome";
                }
              }
              else{
                msg_inscricao = "Dica: SUBSCRIBE_CLASS <nome da turma>";
              }
              write(client_fd, msg_inscricao, strlen(msg_inscricao)); 
            }
            else if(strcmp(cliente->funcao, "professor") == 0){
              if(strcmp(opcao,"CREATE_CLASS") == 0){
                char *msg_criar = (char*) malloc(sizeof(char) * 10000);
                if(count == 3){
                  if(verifica_turma(turmas, num_turmas, arr_str[1]) < 0){
                    Turma *nova_turma = (Turma*)malloc(sizeof(Turma));
                    if(nova_turma != NULL){
                      char *ip = gere_ip(turmas, num_turmas);
                      int socket = criar_multicast(ip);
                      strcpy(nova_turma->nome ,arr_str[1]);
                      strcpy(nova_turma->ip, ip);
                      nova_turma->socket = socket;
                      nova_turma->tamanho_max = atoi(arr_str[2]);
                      nova_turma->tamanho_atual = 0;
                      turmas[*num_turmas] = *nova_turma;
                      (*num_turmas)++;
                      sprintf(msg_criar, "OK %s", ip);
                    }
                  }
                  else{
                    msg_criar = "Já existe uma turma criada com o mesmo nome";
                  }
                }
                else{
                  msg_criar = "Dica: CREATE_CLASS <nome da turma> <tamanho da turma>";
                }
                write(client_fd, msg_criar, strlen(msg_criar));
              }
              else if(strcmp(opcao, "SEND") == 0){
                char *msg_envio;
                int indice;
                if(count == 3){
                  if(verifica_turma(turmas, num_turmas, arr_str[1]) >= 0){
                    if((indice = verifica_turma(cliente->turmas, &(cliente->num_turmas), arr_str[1])) >= 0){
                      struct sockaddr_in addr;
                      memset(&addr, 0, sizeof(addr));
                      addr.sin_family = AF_INET;
                      addr.sin_addr.s_addr = inet_addr(turmas[indice].ip);
                      addr.sin_port = htons(5000);
                      if (sendto(turmas[indice].socket, arr_str[2], strlen(arr_str[2]), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0){
                          erro("sendto");
                          exit(1);
                      } 
                      msg_envio = "Mensagem enviada com sucesso!";
                    }
                    else{
                      msg_envio = "O professor não está inscrito nessa turma! ";
                    }
                  }
                  else{
                    msg_envio = "Não existe nenhuma turma com esse nome!";

                  }
                }
                else{
                  msg_envio = "Dica: SEND <nome da turma> <texto>";
                }
                write(client_fd, msg_envio, strlen(msg_envio));
              }
              else{
                char msg_aviso[] = "Dica: LIST_CLASSES | LIST_SUBSCRIBED | SUBSCRIBE_CLASS | CREATE_CLASS | SEND"; 
                write(client_fd, msg_aviso, strlen(msg_aviso));
              }
            }
            else{
              char msg_aviso[] = "Dica: LIST_CLASSES | LIST_SUBSCRIBED | SUBSCRIBE_CLASS"; 
              write(client_fd, msg_aviso, strlen(msg_aviso));
            }
          } while(mread > 0);
        }
        if(encontrado == 0){
          char rejected[] = "REJECTED";
          write(client_fd, rejected, strlen(rejected));
        }
      }
      else{
        char msg_aviso[] = "Dica: LOGIN <nome de utilizador> <palavra-passe>";
        write(client_fd, msg_aviso, strlen(msg_aviso));
      }

  } while(nread > 0); 

	close(client_fd);
}

void importar(Cliente *clientes, int *num_clientes, char nome_ficheiro[]){

  FILE *f = fopen(nome_ficheiro, "r");     // abertura do ficheiro de texto
  if(f == NULL){
    perror("Erro na abertura do ficheiro de configuração!\n");
    exit(1);
  }
  char buffer[BUF_SIZE];      // string que armazena todo o texto do ficheiro de texto
  while(fgets(buffer, BUF_SIZE, f)){    // ler o ficheiro de texto até não haver mais nada para ler
    char nome[MAX_STRING], passe[MAX_STRING], funcao[MAX_STRING];
    char *str;
    str = strtok(buffer, ";");      // divide a string no ";" e pega na sub-string antes do espaço
    strcpy(nome, str);
    str = strtok(NULL, ";");      // pega na primeira sub-string depois de ";"
    strcpy(passe, str);
    str = strtok(NULL, ";");      // pega na sgunda sub-string depois de ";"
    if(str[strlen(str) - 1] == '\n'){
      str[strlen(str) - 1] = '\0';      // colocar um '\0' no último caracter da string, caso este seja um '\n'
    }
    strcpy(funcao, str);
    insere_clientes(clientes, num_clientes, nome, passe, funcao);
  }

  fclose(f);
}

void escrever_ficheiro(Cliente *clientes, int *num_clientes, char nome_ficheiro[]){

  FILE *ficheiro = fopen(nome_ficheiro, "w");
  if(ficheiro == NULL){
    perror("Erro na abertura do ficheiro de configuração!\n");
    exit(1);
  }

  for(int i = 0; i < *num_clientes; i++){
    fprintf(ficheiro, "%s;%s;%s", clientes[i].nome, clientes[i].passe, clientes[i].funcao);
    if(i != *num_clientes - 1){
      fprintf(ficheiro, "\n");
    }
  }
  
  fclose(ficheiro);
}

void ctrl_c_handler(){

  if(tcp_pid > 0){
    kill(tcp_pid, SIGTERM);
  }
  if(udp_pid > 0){
    kill(udp_pid, SIGTERM);
  }
  exit(0);
}

void erro(char *msg){
	printf("Erro: %s\n", msg);
	exit(-1);
}