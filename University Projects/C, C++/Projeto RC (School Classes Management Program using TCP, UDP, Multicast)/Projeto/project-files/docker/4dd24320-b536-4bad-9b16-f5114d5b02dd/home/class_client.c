#include "class_server.h"

void lerMsg(int fd);      // função utilizada para ler as mensagens que são enviadas pelo servidor
void erro(char *msg);

int main(int argc, char *argv[]) {
  char endServer[100];
  int fd;
  struct sockaddr_in addr;
  struct hostent *hostPtr;

  if (argc != 3) {
    printf("cliente <host> <port>\n");
    exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0)
    erro("Não consegui obter endereço");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));

  if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	  erro("socket");
  if (connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	  erro("Connect");

  lerMsg(fd);     // leitura da mensagem incial de boas-vindas

  char buffer[BUF_SIZE];     // string utilizada para ler o que é escrito na consola do cliente
  
  do{
    do{
      fgets(buffer, BUF_SIZE, stdin);
    }while(strcmp(buffer, "\n") == 0);
    buffer[strlen(buffer) - 1] = '\0';      // colocar um '\0' no último caracter da string (que deverá ser um '\n'), para que não seja lido o '\n' e indicar que a string acaba ali  
    write(fd, buffer, strlen(buffer));
    lerMsg(fd);
  } while(strcmp(buffer, "SAIR") != 0);     // possibilidade de ler e escrever mensagens de e para o servidor, até que seja escrito "SAIR" na consola do cliente

  lerMsg(fd);     // leitura da mensagem final de despedida
  close(fd);      // fecho do socket
  exit(0);
  
  return 0;
}

void lerMsg(int fd){      

  int nread = 0;
	char buffer[BUF_SIZE];

  nread = read(fd, buffer, BUF_SIZE-1);
  if(nread > 0){
    buffer[nread] = '\0';
	  printf("%s\n\n", buffer);
	  fflush(stdout);
  }
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}