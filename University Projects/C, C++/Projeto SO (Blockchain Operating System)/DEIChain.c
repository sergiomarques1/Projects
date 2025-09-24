/*	Hugo Miguel Borges Neves - 2022214562
	Sérgio Lopes Marques - 2022222096
*/

#include "DEIChain.h"
#include "pow.h"

FILE *ficheiro_log;
sem_t *sem_log;

FILE *ficheiro_config;

Config config; 

pid_t id_controller;
pid_t id_miner;

pid_t id_validators[MAX_VALIDATORS];
int num_validators = 0;

pid_t id_statistics;

pthread_t *threads_miner;
int *id_threads;

pthread_t thread_controlo;
int id_controlo;

TransactionsPool *pool;
BlockchainLedger *ledger;

int id_pool;
int id_ledger;

sem_t *sem_pool;
sem_t *sem_ledger;

pthread_mutex_t mutex_ledger;
pthread_cond_t cond_ledger;
volatile sig_atomic_t ledger_cheio = 0;

pthread_mutexattr_t attrmutex;
pthread_condattr_t attrcondv;

int id_mq;

char hash_buffer[HASH_SIZE] = INITIAL_HASH;

int *validos_thread;
int *invalidos_thread;
time_t tempo_medio = 0;
int *creditos_thread;
int blocos_testados = 0;
int total_blocos = 0;

volatile sig_atomic_t terminar = 0;
volatile sig_atomic_t ctrl_c = 0;
volatile sig_atomic_t sair_miner = 0;
volatile sig_atomic_t sair_validator = 0;
volatile sig_atomic_t sair_statistics = 0;

pthread_mutex_t mutex_miner;
pthread_cond_t cond_miner;

void escrever_msg_log(char *msg);
void ler_config(char *nome_ficheiro);
void *controlar_validators();
void criar_shm();
void miner();
void *miner_thread();
void validator();
void aging();
void statistics();
void mostrar_statistics();
void mostrar_dump();
void terminar_miner();
void terminar_validator();
void terminar_statistics();
void ctrl_c_handler();
void clean();
void erro(char *msg);
void sigusr1_handler();


int main(int argc, char *argv[]){

	if(argc != 2){
		printf("Erro: Argumentos em falta ou em excesso!\n");
		printf("--> ./DEIChain <nome do ficheiro de configurações>\n");
		exit(1);
	}
	
	sem_unlink("SEM_LOG");
	sem_log = sem_open("SEM_LOG", O_CREAT | O_EXCL, 0777, 1);
	
	ficheiro_log = fopen("DEIChain_log.txt", "w");
    if(ficheiro_log == NULL){
        printf("Erro na abertura do ficheiro de log!\n");
        exit(1);
    }
    
    escrever_msg_log("A INICIAR O PROGRAMA DEIChain");
    
    signal(SIGINT, ctrl_c_handler);
    escrever_msg_log("HANDLER DO SINAL SIGINT CONFIGURADO COM SUCESSO");
    
    signal(SIGUSR1, mostrar_dump);
    escrever_msg_log("HANDLER DO SINAL SIGUSR1 CONFIGURADO COM SUCESSO");
    
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR){
        erro("ERRO AO TENTAR IGNORAR O SINAL SIGTSTP");
    }
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR){
        erro("ERRO AO TENTAR IGNORAR O SINAL SIGQUIT");
    }
    escrever_msg_log("SINAIS SIGTSTP E SIGQUIT IGNORADOS COM SUCESSO");

    id_controller = getpid();
    
    ler_config(argv[1]);
    
    threads_miner = (pthread_t*)malloc(config.NUM_MINERS * sizeof(pthread_t));
    id_threads = (int*)malloc(config.NUM_MINERS * sizeof(int));
    
    validos_thread = (int*)malloc(config.NUM_MINERS * sizeof(int));
    invalidos_thread = (int*)malloc(config.NUM_MINERS * sizeof(int));
    creditos_thread = (int*)malloc(config.NUM_MINERS * sizeof(int));
    
	for(int i = 0; i < config.NUM_MINERS; i++){
		id_threads[i] = i + 1;
	}
    
    sem_unlink("SEM_POOL");
	sem_pool = sem_open("SEM_POOL", O_CREAT | O_EXCL, 0777, 1);
	
	sem_unlink("SEM_LEDGER");
	sem_ledger = sem_open("SEM_LEDGER", O_CREAT | O_EXCL, 0777, 1);
	
	pthread_mutex_init(&mutex_ledger, NULL);
    pthread_cond_init(&cond_ledger, NULL);
    
    pthread_mutex_init(&mutex_miner, NULL);
    pthread_cond_init(&cond_miner, NULL);
  	
  	pthread_mutexattr_init(&attrmutex);
  	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

  	pthread_condattr_init(&attrcondv);
  	pthread_condattr_setpshared(&attrcondv, PTHREAD_PROCESS_SHARED);

	
    criar_shm();
    
    if((id_mq = msgget(IPC_PRIVATE, IPC_CREAT | O_EXCL | 0777)) < 0){
        erro("ERRO AO CRIAR A MESSAGE QUEUE");
    }
    escrever_msg_log("MESSAGE QUEUE CRIADA COM SUCESSO");
    
    if(mkfifo(PIPE, O_CREAT| O_EXCL| 0777) < 0 && errno != EEXIST){
      	erro("ERRO AO CRIAR O BACK PIPE");
   	}
   	escrever_msg_log("PIPE CRIADO COM SUCESSO");
    
	id_miner = fork();
	
	if(id_miner < 0){
		erro("ERRO AO CRIAR O PROCESSO MINER");
	}
	else if(id_miner == 0){
		miner();
    }
    
    id_validators[num_validators] = fork();
	
	if(id_validators[num_validators] < 0){
		erro("ERRO AO CRIAR O PROCESSO VALIDATOR");
	}
	else if(id_validators[num_validators] == 0){
		validator();
    }
    
    num_validators++;
    
    id_statistics = fork();
	
	if(id_statistics < 0){
		erro("ERRO AO CRIAR O PROCESSO STATISTICS");
	}
	else if(id_statistics == 0){
		statistics();
    }
    
    pthread_create(&thread_controlo, NULL, controlar_validators, &id_controlo);
    
    srand(time(NULL));  // Seed RNG
  	OpenSSL_add_all_algorithms();
   
    pthread_mutex_lock(&mutex_ledger);
    while(!terminar && !ledger_cheio) {
        pthread_cond_wait(&cond_ledger, &mutex_ledger);
    }
    pthread_mutex_unlock(&mutex_ledger);
    
    clean();
}
    
    
void escrever_msg_log(char *msg){

    sem_wait(sem_log);

    time_t tempo;
    struct tm* tempo_atual;
    tempo = time(NULL);
    tempo_atual = localtime(&tempo);

    fprintf(ficheiro_log, "%02d:%02d:%02d %s\n", tempo_atual->tm_hour, tempo_atual->tm_min, tempo_atual->tm_sec, msg);
    fflush(ficheiro_log);
    printf("%02d:%02d:%02d %s\n", tempo_atual->tm_hour, tempo_atual->tm_min, tempo_atual->tm_sec, msg);

    sem_post(sem_log);
}
	

void ler_config(char *nome_ficheiro){

	ficheiro_config = fopen(nome_ficheiro, "r");
	if(ficheiro_config == NULL){
		erro("ERRO NA ABERTURA DO FICHEIRO DE CONFIGURAÇÕES");
  	}
  
  	if(fscanf(ficheiro_config, "%d %d %d %d", &config.NUM_MINERS, &config.TX_POOL_SIZE, &config.TRANSACTIONS_PER_BLOCK, &config.BLOCKCHAIN_BLOCKS) != 4){
        erro("ERRO AO LER OS VALORES DO FICHEIRO DE CONFIGURAÇÕES");
    }
    
    if((config.NUM_MINERS > 0) && (config.TX_POOL_SIZE > 0) && (config.TRANSACTIONS_PER_BLOCK > 0) && (config.BLOCKCHAIN_BLOCKS > 0)){
        escrever_msg_log("DADOS DO FICHEIRO DE CONFIGURAÇÕES LIDOS COM SUCESSO");
    }
    else{
        erro("DADOS DO FICHEIRO DE CONFIGURAÇÕES INVÁLIDOS");
    }
    
    fclose(ficheiro_config);
}

void *controlar_validators(){
	
	 while(terminar == 0){
    
    	pthread_mutex_lock(&pool->mutex_ctrl);
    	
    	while(pool->alterada == 0 && !terminar){
    		pthread_cond_wait(&pool->cond_ctrl, &pool->mutex_ctrl);
    	}
    	pool->alterada = 0;
    	
    	pthread_mutex_unlock(&pool->mutex_ctrl);
    	
    	
	 	if(terminar == 1){
	 		pthread_exit(NULL);
	 	}
    	
    	sem_wait(sem_pool);
    	
    	if((pool->tam_pool >= config.TX_POOL_SIZE * 0.6 && num_validators == 1) || (pool->tam_pool >= config.TX_POOL_SIZE * 0.8 && num_validators == 2)){
    		
    		sem_post(sem_pool);
    		
    		id_validators[num_validators] = fork();	
    		
    		if(id_validators[num_validators] < 0){
				erro("ERRO AO CRIAR O PROCESSO VALIDATOR");
			}
			else if(id_validators[num_validators] == 0){
				validator();
    		}
    		
    		num_validators++;
    	}	
    	else if((pool->tam_pool <= config.TX_POOL_SIZE * 0.4) && num_validators >= 2){
    		
    		sem_post(sem_pool);
    		
    		for(int i = 1; i < num_validators; i++){
    			kill(id_validators[i], SIGUSR2);
    		}
    	}
    	else{
    		sem_post(sem_pool);
    	}
    	
    }

	return NULL;

}


void criar_shm(){
	
    id_pool = shmget(KEY_POOL, sizeof(TransactionsPool) + config.TX_POOL_SIZE * sizeof(Entrada), IPC_CREAT | 0777);
    if (id_pool == -1) {
        erro("ERRO AO CRIAR SHARED MEMORY PARA A POOL");
    }

    pool = (TransactionsPool*) shmat(id_pool, NULL, 0);
    if (pool == (void*)-1) {
        erro("ERRO AO ANEXAR SHARED MEMORY PARA A POOL");
    }
    
    pool->transacoes_pendentes = (Entrada*)(pool + 1);
    pool->tam_pool = 0;
    
    for (int i = 0; i < config.TX_POOL_SIZE; i++) {
    	pool->transacoes_pendentes[i].empty = 1;
    	pool->transacoes_pendentes[i].age = 0;
	}
    escrever_msg_log("SHARED MEMORY PARA A POOL CRIADA COM SUCESSO");
    
    pthread_mutex_init(&pool->mutex_tx, &attrmutex);
    pthread_mutex_init(&pool->mutex_ctrl, &attrmutex);
    pthread_mutex_init(&pool->mutex_gen, &attrmutex);
    
    pthread_cond_init(&pool->cond_tx, &attrcondv);
    pthread_cond_init(&pool->cond_ctrl, &attrcondv);
    pthread_cond_init(&pool->cond_gen, &attrcondv);
    
    pool->alterada = 0;
  
    
    id_ledger = shmget(IPC_PRIVATE, sizeof(BlockchainLedger) + config.BLOCKCHAIN_BLOCKS *(sizeof(Bloco) + config.TRANSACTIONS_PER_BLOCK * sizeof(Transaction)), IPC_CREAT | 0777);
    if (id_ledger == -1) {
        erro("ERRO AO CRIAR SHARED MEMORY PARA O LEDGER");
    }

    ledger = (BlockchainLedger*) shmat(id_ledger, NULL, 0);
    if (ledger == (void*)-1) {
        erro("ERRO AO ANEXAR SHARED MEMORY PARA O LEDGER");
    }
    
    ledger->blocos = (Bloco*)(ledger + 1);
    ledger->tam_ledger = 0;
    
    for (int i = 0; i < config.BLOCKCHAIN_BLOCKS; i++) {
        ledger->blocos[i].transactions = (Transaction*)((char*)ledger->blocos + 
                                        config.BLOCKCHAIN_BLOCKS * sizeof(Bloco) + 
                                        i * config.TRANSACTIONS_PER_BLOCK * sizeof(Transaction));
    }
    
    escrever_msg_log("SHARED MEMORY PARA O LEDGER CRIADA COM SUCESSO");

}

void miner(){

		
	escrever_msg_log("PROCESSO MINER CRIADO COM SUCESSO");

	signal(SIGINT, SIG_IGN);
	signal(SIGUSR2, terminar_miner);


	char msg[100];

	for(int i = 0; i < config.NUM_MINERS; i++){
		sprintf(msg, "THREAD MINER %d LANCADA COM SUCESSO", (i + 1)); 
		escrever_msg_log(msg);
		pthread_create(&threads_miner[i], NULL, miner_thread, &id_threads[i]);
	}

	pthread_mutex_lock(&mutex_miner);
	while(!sair_miner){
		pthread_cond_wait(&cond_miner, &mutex_miner);
	}
	pthread_mutex_unlock(&mutex_miner);
	
	char msg_saida[100];
	
	for(int i = 0; i < config.NUM_MINERS; i++){
		sprintf(msg_saida, "THREAD MINER %d TERMINADA", (i + 1)); 
		escrever_msg_log(msg_saida);
		pthread_join(threads_miner[i], NULL);
	}
	
	free(threads_miner);
	free(id_threads);
	
	escrever_msg_log("PROCESSO MINER TERMINADO");

	exit(0);

}

void *miner_thread(void* id_ptr){

	int id = *((int *)id_ptr);
	
	int fd;
	
	if((fd = open(PIPE, O_WRONLY)) < 0) {
        erro("ERRO AO ABRIR O PIPE PARA LEITURA");
    }
	
	while(!sair_miner){
		
		pthread_mutex_lock(&pool->mutex_tx);
		
		while(pool->tam_pool < config.TRANSACTIONS_PER_BLOCK && !sair_miner){
			pthread_cond_wait(&pool->cond_tx, &pool->mutex_tx);
		}
		
		pthread_mutex_unlock(&pool->mutex_tx);
		
		if(sair_miner == 1){
			pthread_exit(NULL);
		}		
				
		Transaction *transacoes = (Transaction*) malloc(sizeof(Transaction) * config.TRANSACTIONS_PER_BLOCK);
        int num_transacoes = 0;
		
		sem_wait(sem_pool);
		
		pool->transacoes_pendentes = (Entrada*)(pool + 1);
		
		for(int i = 0; i < config.TX_POOL_SIZE && num_transacoes < config.TRANSACTIONS_PER_BLOCK; i++){
			if(pool->transacoes_pendentes[i].empty == 0){
				transacoes[num_transacoes++] = pool->transacoes_pendentes[i].transaction;
			}
		}
		
		sem_post(sem_pool);
        
        if(num_transacoes == config.TRANSACTIONS_PER_BLOCK) {
            
            Bloco *novo_bloco = malloc(sizeof(Bloco) + config.TRANSACTIONS_PER_BLOCK * sizeof(Transaction));
            
            novo_bloco->id = id;
            novo_bloco->transactions = (Transaction*)((char*)novo_bloco + sizeof(Bloco));
            
            for(int i = 0; i < config.TRANSACTIONS_PER_BLOCK; i++) {
                novo_bloco->transactions[i] = transacoes[i];
            }
            
            char msg[100];
            
            sprintf(msg, "THREAD MINER %d INICIOU PROOF OF WORK", id);
            escrever_msg_log(msg);
            
  			PoWResult r;
  			do {
    			novo_bloco->tempo = time(NULL);
    			r = proof_of_work(novo_bloco, config.TRANSACTIONS_PER_BLOCK);
  			} while (r.error == 1);

           	sem_wait(sem_ledger);
            
            ledger->blocos = (Bloco*)(ledger + 1);
            
            if (ledger->tam_ledger == 0){
            	strncpy(novo_bloco->hash_anterior, INITIAL_HASH, HASH_SIZE);
        	}
        	else{
            	strncpy(novo_bloco->hash_anterior, r.hash, HASH_SIZE);
        	}
        	
        	r = proof_of_work(novo_bloco, config.TRANSACTIONS_PER_BLOCK);
            
            sem_post(sem_ledger);
            
            if(write(fd, novo_bloco, sizeof(Bloco) + config.TRANSACTIONS_PER_BLOCK * sizeof(Transaction)) < 0) {
                erro("ERRO AO ESCREVER NO PIPE");
            }
            
            sprintf(msg, "THREAD MINER %d ENVIOU BLOCO PARA VALIDAÇÃO", id);
            escrever_msg_log(msg);
            
            free(novo_bloco);
        }
        free(transacoes);

	}
	
	close(fd);
	
	return NULL;

}		

void terminar_miner(){
	
	sair_miner = 1;
	pthread_cond_broadcast(&cond_miner);
	pthread_cond_broadcast(&pool->cond_tx);
}

void validator(){

	char msg[100];
	
	sprintf(msg, "PROCESSO VALIDATOR %d CRIADO COM SUCESSO", (num_validators + 1)); 
	escrever_msg_log(msg);
	
	signal(SIGINT, SIG_IGN);
	signal(SIGUSR2, terminar_validator);

	int fd;
	
	if((fd = open(PIPE, O_RDONLY | O_NONBLOCK)) < 0) {
        erro("ERRO AO ABRIR O PIPE PARA LEITURA");
    }
    
	size_t tam_buffer = sizeof(Bloco) + config.TRANSACTIONS_PER_BLOCK * sizeof(Transaction);
	void *buffer_bloco = malloc(tam_buffer);
	
	while(!sair_validator){
		
        if(read(fd, buffer_bloco, tam_buffer) > 0) {	
        	
        	if(sair_validator){
        		break;
        	}
        
        	Bloco *novo_bloco = (Bloco*) buffer_bloco;
        	novo_bloco->transactions = (Transaction *)((char *)novo_bloco + sizeof(Bloco));
        	
        	char msg[200];
        	
        	sprintf(msg, "BLOCO COM A HASH %s RECEBIDO NO VALIDATOR %d", novo_bloco->hash_anterior, (num_validators + 1)); 
			escrever_msg_log(msg);
        	
            int valido = 1;
            
            sem_wait(sem_ledger);
            
            sem_wait(sem_pool);
            
            pool->transacoes_pendentes = (Entrada*)(pool + 1);
            
            
            for(int i = 0; i < config.TRANSACTIONS_PER_BLOCK; i++){
                int encontrada = 0;
                for(int j = 0; j < config.TX_POOL_SIZE; j++){
                    if(pool->transacoes_pendentes[j].empty == 0 && 
                       novo_bloco->transactions[i].id == pool->transacoes_pendentes[j].transaction.id){
                        encontrada = 1;
                        break;
                    }
                }
                if(!encontrada){
                    valido = 0;
                    break;
                }
            }
            
     		aging();
            		
            sem_post(sem_pool);
           
            
            if (!verify_nonce(novo_bloco, config.TRANSACTIONS_PER_BLOCK)) {
            	valido = 0;
    		}
    		
    		Mensagem mensagem;
            		
            if(valido) {
            	
            	sem_wait(sem_pool);
            	
            	pool->transacoes_pendentes = (Entrada*)(pool + 1);
            	
          		for(int i = 0; i < config.TRANSACTIONS_PER_BLOCK; i++){
          			for(int j = 0; j < config.TX_POOL_SIZE; j++){
          				if(novo_bloco->transactions[i].id == pool->transacoes_pendentes[j].transaction.id && pool->transacoes_pendentes[j].empty == 0){
          					pool->transacoes_pendentes[j].empty = 1;
          					pool->tam_pool--;
          					pthread_cond_broadcast(&pool->cond_gen);
          					pool->alterada = 1;
                			pthread_cond_broadcast(&pool->cond_ctrl);
                			break;
          				}
          			}
          		}
            	
            	aging();
            	
      			sem_post(sem_pool);
            	
                if(ledger->tam_ledger < config.BLOCKCHAIN_BLOCKS) {
                
                    Bloco *bloco_ledger = &ledger->blocos[ledger->tam_ledger];
                    
                    bloco_ledger->id = novo_bloco->id;
                    bloco_ledger->tempo = novo_bloco->tempo;
                    bloco_ledger->nonce = novo_bloco->nonce;
                    strncpy(bloco_ledger->hash_anterior, novo_bloco->hash_anterior, HASH_SIZE);
                    
                    for(int i = 0; i < config.TRANSACTIONS_PER_BLOCK; i++) {
                        bloco_ledger->transactions[i] = novo_bloco->transactions[i];
                    }
                    
                    mensagem.creditos = 0;
                    for(int i = 0; i < config.TRANSACTIONS_PER_BLOCK; i++) {
                        mensagem.creditos += novo_bloco->transactions[i].recompensa;
                    }
                    mensagem.tempo = time(NULL) - novo_bloco->tempo;
                    
                    ledger->tam_ledger++;
                    
                    if(ledger->tam_ledger >= config.BLOCKCHAIN_BLOCKS){
                    	pthread_mutex_lock(&mutex_ledger);
            			ledger_cheio = 1;
            			escrever_msg_log("TAMANHO MAXIMO BLOCKCHAIN ATINGIDO: A PREPARAR O ENCERRAMENTO DO PROGRAMA DEIChain");
            			pthread_cond_signal(&cond_ledger);
            			pthread_mutex_unlock(&mutex_ledger);
            		}
                     
                    sprintf(msg, "BLOCO #%d COM O ID %d ADICIONADO AO LEDGER POR VALIDATOR %d", ledger->tam_ledger, novo_bloco->id, num_validators + 1);
                    escrever_msg_log(msg);
                }
            } 
            else {
                mensagem.creditos = 0;
                mensagem.tempo = 0;
            }
            
            mensagem.msg_type = 1;
            mensagem.id_thread = novo_bloco->id;
            mensagem.valido = valido;
            
            if(msgsnd(id_mq, &mensagem, sizeof(Mensagem) - sizeof(long), 0) < 0) {
                    erro("ERRO AO ENVIAR MENSAGEM PARA STATISTICS");
           	}
         
            
            sem_post(sem_ledger);
         
      	}
		
	}
	
	close(fd);
	free(buffer_bloco);
	
	char msg_saida[100];
	
	sprintf(msg_saida, "PROCESSO VALIDATOR %d TERMINADO", (num_validators + 1)); 
	escrever_msg_log(msg_saida);

	exit(0);
	
}


void aging(){

	for(int i = 0; i < config.TX_POOL_SIZE; i++) {
    	if(pool->transacoes_pendentes[i].empty == 0) {
            pool->transacoes_pendentes[i].age++;
            
            if(pool->transacoes_pendentes[i].age % 50 == 0) {
                pool->transacoes_pendentes[i].transaction.recompensa++;
            }
        }
    }
}


void terminar_validator(){
	sair_validator = 1;
}

void statistics(){
	
	escrever_msg_log("PROCESSO STATISTICS CRIADO COM SUCESSO");
	
	signal(SIGINT, SIG_IGN);
	signal(SIGUSR1, mostrar_statistics);
	signal(SIGUSR2, terminar_statistics);
	
	for(int i = 0; i < config.NUM_MINERS; i++) {
        validos_thread[i] = 0;
        invalidos_thread[i] = 0;
        creditos_thread[i] = 0;
    }
	
	while(!sair_statistics){
		
		Mensagem mensagem;
		if(msgrcv(id_mq, &mensagem, sizeof(Mensagem) - sizeof(long), 1, IPC_NOWAIT) < 0){
			if(errno == ENOMSG) {
                continue;
            } 
            else if(errno == EINTR) {
                continue;
            } 
            else {
				erro("ERRO AO RECEBER MENSAGEM NA STATISTICS");
			}
		}
		else{
			for(int i = 0; i < config.NUM_MINERS; i++){
				if(mensagem.id_thread == id_threads[i]){
					if(mensagem.valido == 1){
						validos_thread[i]++;
						total_blocos++;
						if(total_blocos == 1){
							tempo_medio = mensagem.tempo;
						}
						else{
							tempo_medio = (tempo_medio * (total_blocos - 1) + mensagem.tempo) / total_blocos;
						}
						creditos_thread[i] += mensagem.creditos;
					}
					else{
						invalidos_thread[i]++;
					}
					blocos_testados++;
					break;
				}
			}
		}
	}
	
	
	escrever_msg_log("PROCESSO STATISTICS TERMINADO");
	
    free(validos_thread);
    free(invalidos_thread);
    free(creditos_thread);

	exit(0);
}

void mostrar_statistics(){

    char msg[10000] = "";
    char temp[1000];
    
    sprintf(temp, "=== ESTATÍSTICAS DO DEIChain ===\n");
    strcat(msg, temp);
    
    sem_wait(sem_ledger);
    
        for(int i = 0; i < config.NUM_MINERS; i++) {
            sprintf(temp, "ID Miner: %d | Blocos Válidos: %d | Blocos Inválidos: %d | Créditos: %d\n", 
                   id_threads[i], validos_thread[i], invalidos_thread[i], creditos_thread[i]);
            strcat(msg, temp);
        }
        
        sprintf(temp, "Blocos na Blockchain: %d | Tempo Médio de Processamento: %ld segundos\n", 
               total_blocos, tempo_medio);
        strcat(msg, temp);
        
        sprintf(temp, "Total Blocos Testados: %d\n", blocos_testados);
        strcat(msg, temp);
        
    
    sem_post(sem_ledger);
    
    sprintf(temp, "==========================================");
    strcat(msg, temp);
    
    escrever_msg_log(msg);
}

void terminar_statistics(){

	mostrar_statistics();
	
	sair_statistics = 1;
	
}

void sigusr1_handler(){
	
	 kill(id_statistics, SIGUSR1);
	 mostrar_dump();
}

void mostrar_dump(){
    
    char dump_msg[10000] = "";
    char temp[1000];
    
    sprintf(temp, "=== DUMP DO BLOCKCHAIN LEDGER ===\n");
    strcat(dump_msg, temp);
    
    sem_wait(sem_ledger);
    
    if(ledger->tam_ledger > 0){
        for(int i = 0; i < ledger->tam_ledger; i++) {
        
            sprintf(temp, "||---- Block %03d -- \n", i);
            strcat(dump_msg, temp);
            
            sprintf(temp, "Block ID: BLOCK-%d\n", ledger->blocos[i].id);
            strcat(dump_msg, temp);
            
            sprintf(temp, "Previous Hash: %s\n", ledger->blocos[i].hash_anterior);
            strcat(dump_msg, temp);
            
            sprintf(temp, "Block Timestamp: %ld\n", ledger->blocos[i].tempo);
            strcat(dump_msg, temp);
            
            sprintf(temp, "Nonce: %d\n", ledger->blocos[i].nonce);
            strcat(dump_msg, temp);
            
            sprintf(temp, "Transactions:\n");
            strcat(dump_msg, temp);
            
            for(int j = 0; j < config.TRANSACTIONS_PER_BLOCK; j++) {
                Transaction *tx = &ledger->blocos[i].transactions[j];
                sprintf(temp, "      [%d] ID: TX-%d-%d | Reward: %d | Value: %.2f | Timestamp: %ld\n", 
                       j, (int)tx->id, j, 
                       tx->recompensa, 
                       tx->valor, 
                       tx->tempo);
                strcat(dump_msg, temp);
            }
            
            sprintf(temp, "||------------------------------\n");
            strcat(dump_msg, temp);
        }
    }
    else{
        sprintf(temp, "Ledger Vazio: Não é Possível Apresentar o Dump\n");
        strcat(dump_msg, temp);
    }
    
    sem_post(sem_ledger);
    
    sprintf(temp, "==========================================");
    strcat(dump_msg, temp);
    
    escrever_msg_log(dump_msg);
   
}

void ctrl_c_handler(){
	
	escrever_msg_log("SINAL SIGINT CAPTURADO: A PREPARAR O ENCERRAMENTO DO PROGRAMA DEIChain");
	
	terminar = 1;
	pthread_cond_broadcast(&cond_ledger);
}

void clean(){
	
	pthread_cond_broadcast(&pool->cond_ctrl);
	pthread_cond_broadcast(&pool->cond_tx);
	
	pthread_join(thread_controlo, NULL);
	
	kill(id_miner, SIGUSR2);
	
	for(int i = 0; i < num_validators; i++){
		kill(id_validators[i], SIGUSR2);
	}
	
	kill(id_statistics, SIGUSR2);
	mostrar_dump();
	
	char msg[100];
	sprintf(msg, "NUMERO DE TRANSACOES POR PROCESSAR: %d", pool->tam_pool); 
	escrever_msg_log(msg);
	
	while(wait(NULL) > 0);
	
	pthread_mutex_destroy(&mutex_ledger);
    pthread_cond_destroy(&cond_ledger);
    
    pthread_mutex_destroy(&pool->mutex_tx);
    pthread_mutex_destroy(&pool->mutex_ctrl);
    pthread_mutex_destroy(&pool->mutex_gen);
    
    pthread_cond_destroy(&pool->cond_tx);
    pthread_cond_destroy(&pool->cond_ctrl);
    pthread_cond_destroy(&pool->cond_gen);
	
	shmdt(pool);
	shmdt(ledger);
	
	shmctl(id_pool, IPC_RMID, NULL);
	shmctl(id_ledger, IPC_RMID, NULL);
	
	sem_close(sem_pool);
	sem_unlink("SEM_POOL");
	
	sem_close(sem_ledger);
	sem_unlink("SEM_LEDGER");
	
	msgctl(id_mq, IPC_RMID, NULL);
	
	unlink(PIPE);
	
	escrever_msg_log("PROGRAMA DEIChain TERMINADO");
	
	fclose(ficheiro_log);
	
	sem_close(sem_log);
	sem_unlink("SEM_LOG");

	exit(0);

}

void erro(char *msg){

	escrever_msg_log(msg);
	
	if(ficheiro_log != NULL){
		fclose(ficheiro_log);
	}
	
	if(ficheiro_config != NULL){
		fclose(ficheiro_config);
	}
	
   	exit(1);
}
