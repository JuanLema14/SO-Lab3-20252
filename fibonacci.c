#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    long long *sequence;
    int n;
} fib_args_t;

void *GenerateFibonacci(void *arg)
{
    fib_args_t *args = (fib_args_t *)arg;
    long long *seq = args->sequence;
    int n = args->n;

    if (n >= 1)
    {
        seq[0] = 0;
    }

    if (n >= 2)
    {
        seq[1] = 1;
    }

    for (int i = 2; i < n; i++)
    {
        seq[i] = seq[i - 1] + seq[i - 2];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int n;
    long long *fibonacci_sequence;
    pthread_t worker_thread;
    fib_args_t args;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <numero_de_elementos>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 10\n", argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);

    if (n <= 0)
    {
        fprintf(stderr, "Error: El número de elementos debe ser positivo\n");
        exit(1);
    }

    fibonacci_sequence = (long long *)malloc(n * sizeof(long long));

    if (fibonacci_sequence == NULL)
    {
        fprintf(stderr, "Error: No se pudo alocar memoria\n");
        exit(1);
    }

    args.sequence = fibonacci_sequence;
    args.n = n;

    printf("Generando los primeros %d números de Fibonacci...\n", n);

    if (pthread_create(&worker_thread, NULL, GenerateFibonacci, &args) != 0)
    {
        fprintf(stderr, "Error: No se pudo crear el hilo trabajador\n");
        free(fibonacci_sequence);
        exit(1);
    }

    if (pthread_join(worker_thread, NULL) != 0)
    {
        fprintf(stderr, "Error: No se pudo hacer join del hilo trabajador\n");
        free(fibonacci_sequence);
        exit(1);
    }

    printf("Secuencia de Fibonacci:\n");
    for (int i = 0; i < n; i++)
    {
        printf("f(%d) = %lld\n", i, fibonacci_sequence[i]);
    }

    free(fibonacci_sequence);

    return 0;
}