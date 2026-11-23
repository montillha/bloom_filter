#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include "medicao.h"

struct timespec medicao_iniciar() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t;
}

Tempo medicao_finalizar(struct timespec inicio, uint64_t n_operacoes) {
    struct timespec fim;
    clock_gettime(CLOCK_MONOTONIC, &fim);

    double segundos = (fim.tv_sec - inicio.tv_sec)
                    + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    Tempo t;
    t.segundos  = segundos;
    t.ms        = segundos * 1000.0;
    t.ns_por_op = n_operacoes > 0
                  ? (segundos * 1e9) / (double)n_operacoes
                  : 0.0;
    return t;
}

void medicao_exibir(const char *label, Tempo t) {
    printf("  %-30s %.3f ms  (%.2f ns/op)\n", label, t.ms, t.ns_por_op);
}

void resultado_exibir(Resultado *r) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║              RELATORIO FINAL                     ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");

    printf("\n--- Desempenho ---\n");
    medicao_exibir("Tempo de insercao (1M senhas):", r->tempo_insercao);
    medicao_exibir("Tempo de consulta (100K testes):", r->tempo_consulta);

    printf("\n--- Memoria ---\n");
    printf("  Bytes usados pelo Bloom Filter : %u bytes (%.4f MB)\n",
           r->bytes_usados,
           (double)r->bytes_usados / 1024.0 / 1024.0);
    printf("  Equivalente em Hash Table      : %.2f MB\n",
           (double)r->total_testados * 20.0 / 1024.0 / 1024.0);

    printf("\n--- Precisao (%" PRIu64 " consultas) ---\n", r->total_testados);

    uint64_t total_vazadas = r->verdadeiro_positivo + r->falso_negativo;
    uint64_t total_seguras = r->verdadeiro_negativo + r->falso_positivo;

    printf("  Senhas vazadas testadas        : %" PRIu64 "\n", total_vazadas);
    printf("  Senhas seguras testadas        : %" PRIu64 "\n", total_seguras);

    printf("\n  Verdadeiro Positivo  (vazada detectada) : %" PRIu64 "  (%.2f%%)\n",
           r->verdadeiro_positivo,
           total_vazadas > 0
           ? (double)r->verdadeiro_positivo / (double)total_vazadas * 100.0
           : 0.0);

    printf("  Verdadeiro Negativo  (segura aprovada)  : %" PRIu64 "  (%.2f%%)\n",
           r->verdadeiro_negativo,
           total_seguras > 0
           ? (double)r->verdadeiro_negativo / (double)total_seguras * 100.0
           : 0.0);

    printf("  Falso Positivo       (segura bloqueada) : %" PRIu64 "  (%.4f%%)  <- aceitavel\n",
           r->falso_positivo,
           total_seguras > 0
           ? (double)r->falso_positivo / (double)total_seguras * 100.0
           : 0.0);

    printf("  Falso Negativo       (vazada aprovada)  : %" PRIu64 "  (%.4f%%)  <- deve ser 0\n",
           r->falso_negativo,
           total_vazadas > 0
           ? (double)r->falso_negativo / (double)total_vazadas * 100.0
           : 0.0);

    printf("\n--- Conclusao ---\n");
    if (r->falso_negativo == 0)
        printf("  [OK] ZERO falsos negativos: nenhuma senha vazada passou.\n");
    else
        printf("  [!!] ATENCAO: %" PRIu64 " senhas vazadas passaram pelo filtro!\n",
               r->falso_negativo);

    double taxa_fp = total_seguras > 0
                     ? (double)r->falso_positivo / (double)total_seguras * 100.0
                     : 0.0;
    if (taxa_fp <= 2.0)
        printf("  [OK] Taxa de falso positivo dentro do esperado: %.4f%%\n", taxa_fp);
    else
        printf("  [!!] Taxa de falso positivo acima do esperado: %.4f%%\n", taxa_fp);
}
