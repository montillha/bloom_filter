#ifndef MEDICAO_H
#define MEDICAO_H

#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <time.h>

// Resultado de uma medicao de tempo
typedef struct {
    double segundos;
    double ms;
    double ns_por_op;    // nanosegundos por operacao
} Tempo;

// Inicia medicao de tempo
struct timespec medicao_iniciar();

// Finaliza medicao e retorna resultado
// n_operacoes = quantas operacoes foram feitas (para calcular ns/op)
Tempo medicao_finalizar(struct timespec inicio, uint64_t n_operacoes);

// Exibe resultado formatado
void medicao_exibir(const char *label, Tempo t);

// Resultado completo de um teste
typedef struct {
    uint64_t total_testados;
    uint64_t verdadeiro_positivo;   // vazada, detectou corretamente
    uint64_t verdadeiro_negativo;   // segura, disse segura
    uint64_t falso_positivo;        // segura, disse vazada (erro aceitavel)
    uint64_t falso_negativo;        // vazada, disse segura (ERRO GRAVE)
    Tempo    tempo_insercao;
    Tempo    tempo_consulta;
    uint32_t bytes_usados;
} Resultado;

// Exibe relatorio completo
void resultado_exibir(Resultado *r);

#endif
