#ifndef GERADOR_H
#define GERADOR_H

#include <stdint.h>

void gerar_senha(uint64_t indice, char *saida, int tamanho);

void gerar_senha_segura(uint64_t indice, char *saida, int tamanho);

#endif
