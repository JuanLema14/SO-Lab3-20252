#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

double CalcPi(int n, int num_threads);

double GetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

double f(double a)
{
    return (4.0 / (1.0 + a * a));
}

typedef struct
{
    int thread_id;
    int n;
    int num_threads;
    double fH;
} thread_args_t;

void *CalcPiThread(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;

    int thread_id = args->thread_id;
    int n = args->n;
    int num_threads = args->num_threads;
    double fH = args->fH;

    int start = thread_id;
    int step = num_threads;

    double local_sum = 0.0;
    double fX;

    for (int i = start; i < n; i += step)
    {
        fX = fH * ((double)i + 0.5);
        local_sum += f(fX);
    }

    double *result = (double *)malloc(sizeof(double));
    *result = local_sum;

    pthread_exit((void *)result);
}

double CalcPi(int n, int num_threads)
{
    pthread_t *threads;
    thread_args_t *args;
    double fH = 1.0 / (double)n;
    double total_sum = 0.0;

    threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    args = (thread_args_t *)malloc(num_threads * sizeof(thread_args_t));

    for (int i = 0; i < num_threads; i++)
    {
        args[i].thread_id = i;
        args[i].n = n;
        args[i].num_threads = num_threads;
        args[i].fH = fH;

        if (pthread_create(&threads[i], NULL, CalcPiThread, &args[i]) != 0)
        {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++)
    {
        void *ret_val;

        if (pthread_join(threads[i], &ret_val) != 0)
        {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(1);
        }

        double *partial_sum = (double *)ret_val;
        total_sum += *partial_sum;

        free(partial_sum);
    }

    free(threads);
    free(args);

    return fH * total_sum;
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    int num_threads = 1;
    const double fPi25DT = 3.141592653589793238462643;
    double fPi;
    double fTimeStart, fTimeEnd;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <number_of_intervals> <number_of_threads>\n", argv[0]);
        fprintf(stderr, "Example: %s 2000000000 4\n", argv[0]);
        return 1;
    }

    if (argc >= 2)
    {
        n = atoi(argv[1]);
    }

    if (argc >= 3)
    {
        num_threads = atoi(argv[2]);
    }

    if (n <= 0 || n > 2147483647)
    {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }

    if (num_threads <= 0)
    {
        printf("\nnumber of threads must be positive\n");
        return 1;
    }

    printf("Calculating PI with %d intervals using %d threads...\n", n, num_threads);

    fTimeStart = GetTime();

    /* the calculation is done here*/
    fPi = CalcPi(n, num_threads);

    fTimeEnd = GetTime();

    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));

    printf("Execution time      = %.6f seconds\n", fTimeEnd - fTimeStart);

    return 0;
}