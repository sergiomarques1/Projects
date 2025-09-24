/*	Hugo Miguel Borges Neves - 2022214562
	Sérgio Lopes Marques - 2022222096
*/

#ifndef POW_H
#define POW_H

#include "DEIChain.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POW_MAX_OPS 10000000
#define INITIAL_HASH "00006a8e76f31ba74e21a092cca1015a418c9d5f4375e7a4fec676e1d2ec1436"

#ifndef DEBUG
#define DEBUG 0
#endif


typedef enum {
    EASY = 1,
    NORMAL = 2,
    HARD = 3
} DifficultyLevel;

typedef struct {
    char hash[HASH_SIZE];
    double elapsed_time;
    int operations;
    int error;
} PoWResult;

void compute_sha256(const Bloco *input, char *output, int txs_per_block);
PoWResult proof_of_work(Bloco *block, int txs_per_block);
int verify_nonce(const Bloco *block, int txs_per_block);
int check_difficulty(const char *hash, const int reward);
DifficultyLevel getDifficultFromReward(const int reward);

#endif 
