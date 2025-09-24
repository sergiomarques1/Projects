/*	Hugo Miguel Borges Neves - 2022214562
	Sérgio Lopes Marques - 2022222096
*/


#include "pow.h"
#include "DEIChain.h"


int get_max_transaction_reward(const Bloco *block, int txs_per_block) {
  if (block == NULL) return 0;

  int max_reward = 0;
  for (int i = 0; i < txs_per_block; ++i) {
    int reward = block->transactions[i].recompensa;
    if (reward > max_reward) {
      max_reward = reward;
    }
  }

  return max_reward;
}

unsigned char *serialize_block(const Bloco *block, size_t *sz_buf, int txs_per_block) {
  
  *sz_buf = sizeof(int) + HASH_SIZE + sizeof(time_t) + 
            (txs_per_block * sizeof(Transaction)) + 
            sizeof(int);

  unsigned char *buffer = malloc(*sz_buf);
  if (!buffer) return NULL;
  
  memset(buffer, 0, *sz_buf);

  unsigned char *p = buffer;

  memcpy(p, &block->id, sizeof(int));
  p += sizeof(int);

  memcpy(p, block->hash_anterior, HASH_SIZE);
  p += HASH_SIZE;

  memcpy(p, &block->tempo, sizeof(time_t));
  p += sizeof(time_t);

  for (int i = 0; i < txs_per_block; ++i) {
    memcpy(p, &block->transactions[i], sizeof(Transaction));
    p += sizeof(Transaction);
  }

  memcpy(p, &block->nonce, sizeof(int));
  p += sizeof(int);

  return buffer;
}


void compute_sha256(const Bloco *block, char *output, int txs_per_block) {
  
  unsigned char hash[SHA256_DIGEST_LENGTH];
  size_t buffer_sz;

  unsigned char *buffer = serialize_block(block, &buffer_sz, txs_per_block);

  SHA256(buffer, buffer_sz, hash);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(output + (i * 2), "%02x", hash[i]);
  }
  output[HASH_SIZE - 1] = '\0';
  free(buffer);
}


int check_difficulty(const char *hash, const int reward) {
  int minimum = 4; 

  int zeros = 0;
  DifficultyLevel difficulty = getDifficultFromReward(reward);

  while (hash[zeros] == '0') {
    zeros++;
  }

  if (zeros < minimum) return 0;

  char next_char = hash[zeros];

  switch (difficulty) {
    case EASY:  
      if ((zeros == 4 && next_char <= 'b') || zeros > 4) return 1;
      break;
    case NORMAL:  
      if (zeros >= 5) return 1;
      break;
    case HARD: 
      if ((zeros == 5 && next_char <= 'b') || zeros > 5) return 1;
      break;
    default:
      fprintf(stderr, "Invalid Difficult\n");
      exit(2);
  }

  return 0;
}


int verify_nonce(const Bloco *block, int txs_per_block) {

  char hash[SHA256_DIGEST_LENGTH * 2 + 1];
  int reward = get_max_transaction_reward(block, txs_per_block);
  compute_sha256(block, hash, txs_per_block);
  
  return check_difficulty(hash, reward);
}


PoWResult proof_of_work(Bloco *block, int txs_per_block) {
  PoWResult result;

  result.elapsed_time = 0.0;
  result.operations = 0;
  result.error = 0;

  block->nonce = 0;

  int reward = get_max_transaction_reward(block, txs_per_block);

  char hash[SHA256_DIGEST_LENGTH * 2 + 1];
  clock_t start = clock();

  while (1) {
    compute_sha256(block, hash, txs_per_block);

    if (check_difficulty(hash, reward)) {
      result.elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;
      strcpy(result.hash, hash);
      return result;
    }
    block->nonce++;
    if (block->nonce > POW_MAX_OPS) {
      fprintf(stderr, "Giving up\n");
      result.elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;
      result.error = 1;
      return result;
    }
    if (DEBUG && block->nonce % 100000 == 0)
      printf("Nounce %d\n", block->nonce);
    result.operations++;
  }
}

DifficultyLevel getDifficultFromReward(const int reward) {
  if (reward <= 1)
    return EASY;

  if (reward == 2)
    return NORMAL;

  return HARD;
}
