# Verificador de Senhas Comprometidas com Bloom Filter

Implementação em C de um **Bloom Filter** aplicado à verificação de senhas vazadas, inspirado no modelo do [Have I Been Pwned](https://haveibeenpwned.com/).

O programa insere 1 milhão de senhas "vazadas" no filtro e depois faz 100 mil consultas (metade vazadas, metade seguras). No fim, mostra o tempo de inserção e de consulta, a memória usada e a precisão do filtro (verdadeiros/falsos positivos e negativos).

## O que é um Bloom Filter

É uma estrutura de dados probabilística que responde à pergunta "este elemento está no conjunto?" usando muito pouca memória:

- **"Não está"**: a resposta é sempre correta (não existe falso negativo).
- **"Talvez esteja"**: pode ser um falso positivo, com uma taxa controlada.

Para senhas vazadas isso funciona bem: uma senha vazada **nunca** passa pelo filtro, e uma senha segura só é bloqueada por engano numa fração pequena dos casos (configurada em 1%).

## Como funciona

- **Tamanho ideal do vetor de bits:** `m = -(n · ln p) / (ln 2)²`
- **Número ideal de funções hash:** `k = (m / n) · ln 2`
- **Hashing:** *double hashing* `h_i(x) = h1(x) + i · h2(x)`, usando **djb2** e **FNV-1a** como funções base. Assim os `k` hashes saem de apenas duas funções.
- **Taxa de falso positivo teórica:** `(1 - e^(-k·n/m))^k`

Com `n = 1.000.000` e `p = 1%`, o filtro ocupa cerca de **1,14 MB** e usa `k = 6` funções hash.

## Estrutura do projeto

| Arquivo | Descrição |
|---|---|
| `bloom.c` / `bloom.h` | Implementação do Bloom Filter (criar, inserir, consultar, destruir) |
| `gerador.c` / `gerador.h` | Gera senhas simuladas determinísticas (vazadas e seguras, sem sobreposição) |
| `medicao.c` / `medicao.h` | Medição de tempo (`clock_gettime`) e relatório final |
| `main.c` | Executa o experimento: inserção, consultas e relatório |
| `Makefile` | Compilação |

## Como compilar e executar

Precisa de `gcc` e `make` (Linux, WSL ou MinGW/MSYS2 no Windows).

```bash
make        # compila
make run    # compila e executa
make clean  # remove os binários
```

## Configuração

Os parâmetros do experimento ficam em `main.c`:

```c
#define N_SENHAS_VAZADAS   1000000   // senhas inseridas na base
#define N_TESTES           100000    // número de consultas
#define TAXA_FALSO_POS     0.01      // taxa de falso positivo alvo (1%)
```

## Resultado esperado

- **0 falsos negativos**: nenhuma senha vazada passa pelo filtro.
- **Taxa de falso positivo perto de 1%** entre as senhas seguras.
- Tempo de consulta na casa de nanossegundos por operação.
