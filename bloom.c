#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bloom.h"

// ============================================================
//  CALCULO DOS PARAMETROS IDEAIS
//
//  m = tamanho em bits
//  Formula: m = -(n * ln(p)) / (ln(2)^2)
//
//  k = numero de funcoes hash
//  Formula: k = (m / n) * ln(2)
// ============================================================

static uint32_t calcular_m(uint64_t n, double p) {
    return (uint32_t)(-(double)n * log(p) / (log(2.0) * log(2.0)));
}

static uint32_t calcular_k(uint32_t m, uint64_t n) {
    return (uint32_t)((double)m / (double)n * log(2.0));
}

// ============================================================
//  FUNCOES HASH
//
//  Tecnica "double hashing":
//  h_i(x) = h1(x) + i * h2(x)
//
//  Gera k hashes independentes usando apenas 2 funcoes base,
//  evitando implementar k funcoes diferentes.
// ============================================================

// Hash base 1: djb2
static uint64_t hash_djb2(const char *s) {
    uint64_t h = 5381;
    while (*s)
        h = ((h << 5) + h) ^ (uint8_t)(*s++);
    return h;
}

// Hash base 2: FNV-1a
static uint64_t hash_fnv1a(const char *s) {
    uint64_t h = 14695981039346656037ULL;
    while (*s) {
        h ^= (uint8_t)(*s++);
        h *= 1099511628211ULL;
    }
    return h;
}

// Gera o i-esimo hash usando double hashing
static uint32_t hash_i(BloomFilter *bf, const char *s, uint32_t i) {
    uint64_t h1 = hash_djb2(s);
    uint64_t h2 = hash_fnv1a(s);
    return (uint32_t)((h1 + (uint64_t)i * h2) % bf->m);
}

// ============================================================
//  OPERACOES DE BIT
// ============================================================

static void bit_set(BloomFilter *bf, uint32_t pos) {
    bf->bits[pos / 8] |= (1u << (pos % 8));
}

static int bit_get(BloomFilter *bf, uint32_t pos) {
    return (bf->bits[pos / 8] >> (pos % 8)) & 1;
}



BloomFilter* bf_criar(uint64_t n_esperado, double falso_positivo) {
    BloomFilter *bf = malloc(sizeof(BloomFilter));
    if (!bf) return NULL;

    bf->m         = calcular_m(n_esperado, falso_positivo);
    bf->k         = calcular_k(bf->m, n_esperado);
    bf->inseridos = 0;
    bf->falso_pos = falso_positivo;

    // Aloca e zera o array de bits
    uint32_t bytes = (bf->m + 7) / 8;
    bf->bits = calloc(bytes, 1);
    if (!bf->bits) {
        free(bf);
        return NULL;
    }

    return bf;
}

void bf_inserir(BloomFilter *bf, const char *elemento) {
    for (uint32_t i = 0; i < bf->k; i++)
        bit_set(bf, hash_i(bf, elemento, i));
    bf->inseridos++;
}

int bf_consultar(BloomFilter *bf, const char *elemento) {
    for (uint32_t i = 0; i < bf->k; i++)
        if (!bit_get(bf, hash_i(bf, elemento, i)))
            return 0; 
    return 1;         
}

double bf_taxa_real(BloomFilter *bf) {
    // Formula da taxa de falso positivo real apos n insercoes
    return pow(1.0 - exp(-(double)bf->k * (double)bf->inseridos / (double)bf->m), (double)bf->k);
}

void bf_destruir(BloomFilter *bf) {
    if (bf) {
        free(bf->bits);
        free(bf);
    }
}

void bf_info(BloomFilter *bf) {
    uint32_t bytes = (bf->m + 7) / 8;
    printf("  Tamanho em bits      : %u\n", bf->m);
    printf("  Tamanho em bytes     : %u (%.4f MB)\n", bytes, (double)bytes / 1024.0 / 1024.0);
    printf("  Funcoes hash (k)     : %u\n", bf->k);
    printf("  Falso positivo alvo  : %.2f%%\n", bf->falso_pos * 100.0);
    printf("  Elementos inseridos  : %lu\n", bf->inseridos);
    printf("  Falso positivo real  : %.6f%%\n", bf_taxa_real(bf) * 100.0);
}
