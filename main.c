#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bloom.h"
#include "gerador.h"
#include "medicao.h"

// ============================================================
//  CONFIGURACAO DO EXPERIMENTO
// ============================================================
#define N_SENHAS_VAZADAS   1000000   // 1 milhao de senhas na base
#define N_TESTES           100000    // 100 mil consultas
#define TAXA_FALSO_POS     0.01      // 1% de falso positivo alvo
#define TAM_SENHA          64        // tamanho maximo de uma senha

// ============================================================
//  FASE 1: INSERCAO
//  Insere 1 milhao de senhas "vazadas" no Bloom Filter
//  e mede o tempo total e por operacao.
// ============================================================
static void fase_insercao(BloomFilter *bf, Resultado *res) {
    char senha[TAM_SENHA];
    printf("[1/3] Inserindo %d senhas vazadas...\n", N_SENHAS_VAZADAS);

    struct timespec inicio = medicao_iniciar();

    for (uint64_t i = 0; i < N_SENHAS_VAZADAS; i++) {
        gerar_senha(i, senha, TAM_SENHA);
        bf_inserir(bf, senha);

        // Progresso a cada 250 mil insercoes
        if ((i + 1) % 250000 == 0)
            printf("    %lu/%d inseridas...\n", i + 1, N_SENHAS_VAZADAS);
    }

    res->tempo_insercao = medicao_finalizar(inicio, N_SENHAS_VAZADAS);
    res->bytes_usados   = (bf->m + 7) / 8;
    printf("    Concluido.\n\n");
}

// ============================================================
//  FASE 2: CONSULTAS
//  Testa 100 mil senhas: metade vazadas, metade seguras.
//  Contabiliza VP, VN, FP e FN.
// ============================================================
static void fase_consultas(BloomFilter *bf, Resultado *res) {
    char senha[TAM_SENHA];
    uint64_t metade = N_TESTES / 2;

    printf("[2/3] Realizando %d consultas (%lu vazadas + %lu seguras)...\n",
           N_TESTES, metade, metade);

    res->total_testados      = N_TESTES;
    res->verdadeiro_positivo = 0;
    res->verdadeiro_negativo = 0;
    res->falso_positivo      = 0;
    res->falso_negativo      = 0;

    struct timespec inicio = medicao_iniciar();

    // --- Testa senhas VAZADAS (devem ser detectadas) ---
    for (uint64_t i = 0; i < metade; i++) {
        gerar_senha(i, senha, TAM_SENHA);
        if (bf_consultar(bf, senha))
            res->verdadeiro_positivo++;
        else
            res->falso_negativo++;   // ERRO GRAVE: senha vazada nao detectada
    }

    // --- Testa senhas SEGURAS (nao devem ser detectadas) ---
    for (uint64_t i = 0; i < metade; i++) {
        gerar_senha_segura(i, senha, TAM_SENHA);
        if (!bf_consultar(bf, senha))
            res->verdadeiro_negativo++;
        else
            res->falso_positivo++;   // Erro aceitavel: senha segura bloqueada
    }

    res->tempo_consulta = medicao_finalizar(inicio, N_TESTES);
    printf("    Concluido.\n\n");
}

// ============================================================
//  FASE 3: INFORMACOES DO FILTRO
// ============================================================
static void fase_info(BloomFilter *bf) {
    printf("[3/3] Informacoes do Bloom Filter:\n");
    bf_info(bf);
    printf("\n");
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║   VERIFICADOR DE SENHAS COMPROMETIDAS            ║\n");
    printf("║   Estrutura: Bloom Filter                        ║\n");
    printf("║   Baseado no modelo do HaveIBeenPwned            ║\n");
    printf("╚══════════════════════════════════════════════════╝\n\n");

    printf("Configuracao:\n");
    printf("  Senhas na base   : %d\n", N_SENHAS_VAZADAS);
    printf("  Consultas        : %d\n", N_TESTES);
    printf("  Falso positivo   : %.0f%%\n\n", TAXA_FALSO_POS * 100);

    // Cria o Bloom Filter com os parametros ideais
    BloomFilter *bf = bf_criar(N_SENHAS_VAZADAS, TAXA_FALSO_POS);
    if (!bf) {
        fprintf(stderr, "Erro: nao foi possivel alocar o Bloom Filter.\n");
        return 1;
    }

    Resultado res = {0};

    fase_insercao(bf, &res);
    fase_consultas(bf, &res);
    fase_info(bf);
    resultado_exibir(&res);

    bf_destruir(bf);
    return 0;
}
