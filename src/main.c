#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

typedef struct {
    const double *A;
    const double *B;
    double *C;
    int n;
    int inicio;
    int fim;
} DadosThread;

/* Multiplicação sequencial */
void multiplicar(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            double soma = 0.0;

            for (int k = 0; k < n; k++) {
                soma += A[i * n + k] * B[k * n + j];
            }

            C[i * n + j] = soma;
        }
    }
}

/* Função executada por cada thread */
void *worker(void *arg) {
    DadosThread *d = (DadosThread *)arg;

    for (int i = d->inicio; i < d->fim; i++) {
        for (int j = 0; j < d->n; j++) {

            double soma = 0.0;

            for (int k = 0; k < d->n; k++) {
                soma += d->A[i * d->n + k] *
                        d->B[k * d->n + j];
            }

            d->C[i * d->n + j] = soma;
        }
    }

    return NULL;
}

/* Multiplicação paralela */
void multiplicar_paralela(
    const double *A,
    const double *B,
    double *C,
    int n,
    int num_threads)
{
    pthread_t threads[num_threads];
    DadosThread dados[num_threads];

    int linhas_por_thread = n / num_threads;
    int resto = n % num_threads;

    int inicio = 0;

    for (int t = 0; t < num_threads; t++) {

        int qtd = linhas_por_thread + (t < resto ? 1 : 0);

        dados[t].A = A;
        dados[t].B = B;
        dados[t].C = C;
        dados[t].n = n;
        dados[t].inicio = inicio;
        dados[t].fim = inicio + qtd;

        pthread_create(&threads[t], NULL, worker, &dados[t]);

        inicio += qtd;
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
}

/* Checksum para validação */
double checksum(const double *C, int n) {
    double soma = 0.0;

    for (long long i = 0; i < (long long)n * n; i++) {
        soma += C[i];
    }

    return soma;
}

/* Medição de tempo */
double tempo_atual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {

    int n = 1000;          // tamanho da matriz
    int num_threads = 8;   // número de threads

    printf("Tamanho da matriz: %d x %d\n", n, n);
    printf("Threads: %d\n\n", num_threads);

    double *A = malloc(sizeof(double) * n * n);
    double *B = malloc(sizeof(double) * n * n);
    double *C_seq = malloc(sizeof(double) * n * n);
    double *C_par = malloc(sizeof(double) * n * n);

    if (!A || !B || !C_seq || !C_par) {
        printf("Erro de alocacao.\n");
        return 1;
    }

    /* Inicialização */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            A[i * n + j] = (i + j) % 100;
            B[i * n + j] = (i - j) % 100;
        }
    }

    /* Versão sequencial */
    double inicio = tempo_atual();

    multiplicar(A, B, C_seq, n);

    double tempo_seq = tempo_atual() - inicio;

    /* Versão paralela */
    inicio = tempo_atual();

    multiplicar_paralela(A, B, C_par, n, num_threads);

    double tempo_par = tempo_atual() - inicio;

    /* Validação */
    double chk_seq = checksum(C_seq, n);
    double chk_par = checksum(C_par, n);

    printf("Checksum sequencial: %.12f\n", chk_seq);
    printf("Checksum paralelo  : %.12f\n", chk_par);
    printf("Diferenca checksum : %.12e\n",
        fabs(chk_seq - chk_par));

    const double TOL = 1e-6;
    long long erros = 0;

    for (long long i = 0; i < (long long)n * n; i++) {
        if (fabs(C_seq[i] - C_par[i]) > TOL)
            erros++;
    }

    printf("Elementos divergentes: %lld\n", erros);

    if (erros == 0)
        printf("Validacao: OK\n\n");
    else
        printf("Validacao: FALHOU\n\n");

    printf("Tempo sequencial : %.3f s\n", tempo_seq);
    printf("Tempo paralelo   : %.3f s\n", tempo_par);
    printf("Speedup          : %.2f x\n",
        tempo_seq / tempo_par);

    /* Comparação elemento a elemento */
    int correto = 1;

    for (long long i = 0; i < (long long)n * n; i++) {
        if (fabs(C_seq[i] - C_par[i]) > 1e-9) {
            correto = 0;
            printf("Erro encontrado na posicao %lld\n", i);
            break;
        }
    }

    printf("Validacao: %s\n\n",
           correto ? "OK" : "FALHOU");

    printf("Tempo sequencial : %.3f s\n", tempo_seq);
    printf("Tempo paralelo   : %.3f s\n", tempo_par);
    printf("Speedup          : %.2f x\n",
           tempo_seq / tempo_par);

    free(A);
    free(B);
    free(C_seq);
    free(C_par);

    return 0;
}
