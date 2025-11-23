#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estructura para pasar datos al hilo trabajador
typedef struct {
    long long* sequence;  // Puntero al arreglo compartido
    int n;                // Número de elementos a generar
} fib_args_t;

// Función que ejecuta el hilo trabajador
// Genera la secuencia de Fibonacci y la almacena en el arreglo compartido
void* GenerateFibonacci(void* arg) {
    fib_args_t* args = (fib_args_t*)arg;
    long long* seq = args->sequence;
    int n = args->n;
    
    // Casos base
    if (n >= 1) {
        seq[0] = 0;  // f(0) = 0
    }
    
    if (n >= 2) {
        seq[1] = 1;  // f(1) = 1
    }
    
    // Generar resto de la secuencia
    // f(n) = f(n-1) + f(n-2)
    for (int i = 2; i < n; i++) {
        seq[i] = seq[i-1] + seq[i-2];
    }
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int n;
    long long* fibonacci_sequence;
    pthread_t worker_thread;
    fib_args_t args;
    
    // Verificar argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_de_elementos>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 10\n", argv[0]);
        exit(1);
    }
    
    n = atoi(argv[1]);
    
    if (n <= 0) {
        fprintf(stderr, "Error: El número de elementos debe ser positivo\n");
        exit(1);
    }
    
    // Alocar memoria para el arreglo compartido
    fibonacci_sequence = (long long*)malloc(n * sizeof(long long));
    
    if (fibonacci_sequence == NULL) {
        fprintf(stderr, "Error: No se pudo alocar memoria\n");
        exit(1);
    }
    
    // Preparar argumentos para el hilo trabajador
    args.sequence = fibonacci_sequence;
    args.n = n;
    
    printf("Generando los primeros %d números de Fibonacci...\n", n);
    
    // Crear hilo trabajador
    if (pthread_create(&worker_thread, NULL, GenerateFibonacci, &args) != 0) {
        fprintf(stderr, "Error: No se pudo crear el hilo trabajador\n");
        free(fibonacci_sequence);
        exit(1);
    }
    
    // Esperar a que el hilo trabajador termine
    // pthread_join bloquea el hilo main hasta que el trabajador finalice
    // Esto garantiza que el arreglo esté completamente lleno antes de imprimirlo
    if (pthread_join(worker_thread, NULL) != 0) {
        fprintf(stderr, "Error: No se pudo hacer join del hilo trabajador\n");
        free(fibonacci_sequence);
        exit(1);
    }
    
    // Ahora es seguro imprimir la secuencia
    printf("Secuencia de Fibonacci:\n");
    for (int i = 0; i < n; i++) {
        printf("f(%d) = %lld\n", i, fibonacci_sequence[i]);
    }
    
    // Liberar memoria
    free(fibonacci_sequence);
    
    return 0;
}