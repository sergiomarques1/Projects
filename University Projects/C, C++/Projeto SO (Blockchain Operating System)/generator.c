/*	Hugo Miguel Borges Neves - 2022214562
	Sérgio Lopes Marques - 2022222096
*/


#include "DEIChain.h"

TransactionsPool *pool;
int id_pool;
sem_t *sem_pool;

FILE *ficheiro_config;
Config config;

void transaction_nova(int recompensa, pid_t id_genrator, int num_transacoes);
void ler_config(char *nome_ficheiro);
void clean();

int main(int argc, char *argv[]){
	
	if(argc != 3){
        printf("Erro: Argumentos em falta ou em excesso!\n");
        printf("--> ./TxGen <recompensa> <intervalo (ms)>\n");
        exit(1);
    }

    for(int i = 0; i < 2; i++){
        if(atoi(argv[i + 1]) <= 0){
            printf("Erro: Os argumentos devem ser valores positivos!\n");
            exit(1);
        }
    }
    
    if((atoi(argv[1]) < 1) | (atoi(argv[1]) > 3)){
   		printf("Erro: O valor da recompensa deve estar entre 1 e 3!\n");
   		exit(1);
   	}
   	
   	if((atoi(argv[2]) < 200) | (atoi(argv[2]) > 3000)){
		printf("Erro: O valor do intervalo deve estar entre 200 e 3000!\n");
		exit(1);
	}
	
	signal(SIGINT, clean);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	ler_config("config.cfg");
	
	int recompensa = atoi(argv[1]);
	int intervalo = atoi(argv[2]);
	
	id_pool = shmget(KEY_POOL, sizeof(TransactionsPool), 0777);
	pool = (TransactionsPool*) shmat(id_pool, NULL, 0);
	
	sem_pool = sem_open("SEM_POOL", 0);
	if (sem_pool == SEM_FAILED) {
    	perror("Erro ao abrir semáforo existente");
    	exit(1);
	}
	
	pid_t id_generator = getpid();
	
	int num_transacoes = 0;
	
	while(1){
		pthread_mutex_lock(&pool->mutex_gen);
		
		while(pool->tam_pool >= config.TX_POOL_SIZE){
			printf("Pool Cheia! À espera ...\n");
			pthread_cond_wait(&pool->cond_gen, &pool->mutex_gen);
		}
		
		transaction_nova(recompensa, id_generator, num_transacoes);
		
		pthread_mutex_unlock(&pool->mutex_gen);
		
		num_transacoes++;
		sleep(intervalo / 1000);
	}
	  		
}


void transaction_nova(int recompensa, pid_t id_generator, int num_transacoes){

	Transaction nova_transaction;
	nova_transaction.id = id_generator + num_transacoes;
	nova_transaction.recompensa = recompensa;
	nova_transaction.valor = rand() % 100;
	nova_transaction.tempo = time(NULL);
	
	printf("Nova transação lançada!\n");
		
	sem_wait(sem_pool);
	
	pool->transacoes_pendentes = (Entrada*)(pool + 1);
	
	for(int i = 0; i < config.TX_POOL_SIZE; i++){
		if(pool->transacoes_pendentes[i].empty == 1){
			pool->transacoes_pendentes[i].transaction = nova_transaction;
			pool->transacoes_pendentes[i].empty = 0;
			pool->transacoes_pendentes[i].age = 0;
			pool->tam_pool++;
			printf("Transação enviada com sucesso para a pool!\n");
			
			if(pool->tam_pool >= config.TRANSACTIONS_PER_BLOCK){
				pthread_cond_broadcast(&pool->cond_tx);
			}
			
			pool->alterada = 1;
			pthread_cond_signal(&pool->cond_ctrl);
			break;
		}
	}
	
	sem_post(sem_pool);

}

void ler_config(char *nome_ficheiro){

	ficheiro_config = fopen(nome_ficheiro, "r");
	if(ficheiro_config == NULL){
		printf("Erro na abertura do ficheiro de configurações!\n");
		exit(1);
  	}
  
  	if(fscanf(ficheiro_config, "%d %d %d %d", &config.NUM_MINERS, &config.TX_POOL_SIZE, &config.TRANSACTIONS_PER_BLOCK, &config.BLOCKCHAIN_BLOCKS) != 4){
        printf("Erro ao ler os valores do ficheiro de configurações!\n");
        exit(1);
    }
    
    if(!((config.NUM_MINERS > 0) && (config.TX_POOL_SIZE > 0) && (config.TRANSACTIONS_PER_BLOCK > 0) && (config.BLOCKCHAIN_BLOCKS > 0))){
        printf("Dados do ficheiro de configurações inválidos!\n");
        exit(1);
    }
    
    fclose(ficheiro_config);

}

void clean(){

	shmdt(pool);
    sem_close(sem_pool);
    
    printf("\nA encerrar Transaction Generator!\n");

	exit(0);
}
