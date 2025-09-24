/*	Hugo Miguel Borges Neves - 2022214562
	Sérgio Lopes Marques - 2022222096
*/


#ifndef DEI_CHAIN_H
#define DEI_CHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <openssl/conf.h>  
#include <openssl/evp.h>  
#include <openssl/crypto.h>  

#define HASH_SIZE 65	// SHA256_DIGEST_LENGTH * 2 + 1
#define MAX_VALIDATORS 3
#define KEY_POOL 1234

#define PIPE "pipe"

typedef struct{
	int NUM_MINERS; 
	int TX_POOL_SIZE; 
	int TRANSACTIONS_PER_BLOCK; 
	int BLOCKCHAIN_BLOCKS; 
} Config;

typedef struct{
	int id;
	int recompensa;
	float valor;
	time_t tempo;
} Transaction;

typedef struct{
	Transaction transaction;
	int empty;
	int age;
} Entrada;

typedef struct{
	Entrada *transacoes_pendentes;
	int tam_pool;
	pthread_mutex_t mutex_tx;
	pthread_cond_t cond_tx;
	pthread_mutex_t mutex_ctrl;
	pthread_cond_t cond_ctrl;
	pthread_mutex_t mutex_gen;
	pthread_cond_t cond_gen;
	int alterada;
} TransactionsPool;

typedef struct{
	int id;
	char hash_anterior[HASH_SIZE];
	time_t tempo;
	Transaction *transactions;
	int nonce;
} Bloco;

typedef struct{
	Bloco *blocos;
	int tam_ledger;
} BlockchainLedger;

typedef struct{
	long msg_type;
	int id_thread;
	int valido;
	int creditos;
	time_t tempo;
} Mensagem;


#endif
