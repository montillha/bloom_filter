#include <stdio.h>
#include <string.h>
#include "gerador.h"

// ============================================================
//  GERADOR DE SENHAS SIMULADAS

//  Objetivo: gerar senhas deterministicas e unicas por indice,
//  simulando uma base de senhas vazadas real.

//  Senhas vazadas: prefixo "vazada_" + variacoes
//  Senhas seguras: prefixo "segura_" + variacoes

//  Isso garante que os dois conjuntos nunca se sobrepoem,
//  permitindo medir falsos positivos e negativos com precisao.
// ============================================================

static const char *prefixos[] = {
    "pass", "senha", "admin", "login", "user",
    "qwerty", "abc", "test", "demo", "root"
};
static const int n_prefixos = 10;

static const char *sufixos[] = {
    "123", "!", "@2024", "_ok", "#1",
    "000", "xyz", "99", "##", "!!"
};
static const int n_sufixos = 10;

void gerar_senha(uint64_t indice, char *saida, int tamanho) {
    int p = (int)(indice % n_prefixos);
    int s = (int)((indice / n_prefixos) % n_sufixos);
    uint64_t num = indice / (n_prefixos * n_sufixos);
    snprintf(saida, tamanho, "vazada_%s%lu%s", prefixos[p], num, sufixos[s]);
}

void gerar_senha_segura(uint64_t indice, char *saida, int tamanho) {
    int p = (int)(indice % n_prefixos);
    int s = (int)((indice / n_prefixos) % n_sufixos);
    uint64_t num = indice / (n_prefixos * n_sufixos);
    snprintf(saida, tamanho, "segura_%s%lu%s", prefixos[p], num, sufixos[s]);
}
