#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

double CalcPi(int n, int num_threads);

// Función para obtener tiempo en segundos
double GetTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Función f(x) = 4/(1+x^2)
double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

// Estructura para pasar argumentos a cada hilo
typedef struct {
    int thread_id;      // ID del hilo
    int n;              // Número total de intervalos
    int num_threads;    // Número total de hilos
    double fH;          // Ancho de cada intervalo
} thread_args_t;

// Función que ejecutará cada hilo
void* CalcPiThread(void* arg)
{
    thread_args_t* args = (thread_args_t*)arg;
    
    int thread_id = args->thread_id;
    int n = args->n;
    int num_threads = args->num_threads;
    double fH = args->fH;
    
    // Calcular rango de iteraciones para este hilo
    int start = thread_id;
    int step = num_threads;
    
    // Suma local para este hilo
    double local_sum = 0.0;
    double fX;
    
    // Cada hilo procesa sus iteraciones con salto = num_threads
    // Esto evita condiciones de carrera sin necesidad de mutex
    for (int i = start; i < n; i += step)
    {
        fX = fH * ((double)i + 0.5);
        local_sum += f(fX);
    }
    
    // Alocar memoria para retornar el resultado
    double* result = (double*)malloc(sizeof(double));
    *result = local_sum;
    
    pthread_exit((void*)result);
}

// Cálculo paralelo de PI
double CalcPi(int n, int num_threads)
{
    pthread_t* threads;
    thread_args_t* args;
    double fH = 1.0 / (double)n;
    double total_sum = 0.0;
    
    // Alocar arrays para hilos y argumentos
    threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    args = (thread_args_t*)malloc(num_threads * sizeof(thread_args_t));
    
    // Crear hilos
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
    
    // Esperar a que todos los hilos terminen y recolectar resultados
    for (int i = 0; i < num_threads; i++)
    {
        void* ret_val;
        
        if (pthread_join(threads[i], &ret_val) != 0)
        {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(1);
        }
        
        // Obtener suma parcial del hilo
        double* partial_sum = (double*)ret_val;
        total_sum += *partial_sum;
        
        // Liberar memoria del resultado
        free(partial_sum);
    }
    
    // Liberar memoria de threads y args
    free(threads);
    free(args);
    
    // Retornar resultado final
    return fH * total_sum;
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    int num_threads = 1;
    const double fPi25DT = 3.141592653589793238462643;
    double fPi;
    double fTimeStart, fTimeEnd;
    
    // Leer argumentos: ./pi_p <n> <num_threads>
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
    
    // Get initial time 
    fTimeStart = GetTime();

    /* the calculation is done here*/
    fPi = CalcPi(n, num_threads);

    // Get final time
    fTimeEnd = GetTime();
    
    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));
    
    // Report time
    printf("Execution time      = %.6f seconds\n", fTimeEnd - fTimeStart);

    return 0;
}