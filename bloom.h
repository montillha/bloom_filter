#ifndef BLOOM_H
#define BLOOM_H

#include <stdint.h>

typedef struct {
    uint8_t  *bits;         
    uint32_t  m;           
    uint32_t  k;          
    uint64_t  inseridos;   
    double    falso_pos;   
} BloomFilter;


BloomFilter* bf_criar(uint64_t n_esperado, double falso_positivo);

void bf_inserir(BloomFilter *bf, const char *elemento);

int bf_consultar(BloomFilter *bf, const char *elemento);

double bf_taxa_real(BloomFilter *bf);

void bf_destruir(BloomFilter *bf);

void bf_info(BloomFilter *bf);

#endif
