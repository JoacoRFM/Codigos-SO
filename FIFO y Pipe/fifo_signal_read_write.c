#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int fd;
char * myfifo = "/tmp/myfifo";
char buf[1000];

// Dos puntos de salto DISTINTOS, uno por cada señal que se maneja
static sigjmp_buf punto_salto2;    // para SIGINT
static sigjmp_buf punto_salto10;   // para SIGUSR1

// Manejador de SIGINT: salta al punto guardado en punto_salto2
void manejador2(int signo)
{
    printf("\nSe recibió la señal %d\n", signo);
    siglongjmp(punto_salto2, 1);
}

// Manejador de SIGUSR1: salta al punto guardado en punto_salto10
void manejador10(int signo)
{
    printf("\nSe recibió la señal %d\n", signo);
    siglongjmp(punto_salto10, 1);
}

int main(void)
{
    // Registra CADA señal con SU PROPIO manejador
    signal(SIGINT, manejador2);    // Ctrl+C -> manejador2
    signal(SIGUSR1, manejador10);  // kill -SIGUSR1 <pid> -> manejador10

    while (1) {
        // ---- Bloque de LECTURA ----
        // La primera vez (ejecución normal) sigsetjmp devuelve 0,
        // así que solo imprime mensajes y NO lee nada todavía.
        if (sigsetjmp(punto_salto2, 1) == 0) {
            printf("Programa iniciado.\n");
            printf("Pulsa Ctrl+C para enviar SIGINT.\n");
        } else {
            // Solo se llega aquí si YA llegó un SIGINT en algún
            // momento (el manejador2 hizo el siglongjmp hasta acá)
            fd = open(myfifo, O_RDONLY);   // Se bloquea hasta que alguien escriba
            read(fd, buf, 1000);
            printf("Received: %s\n", buf);
            close(fd);
        }

        // ---- Bloque de ESCRITURA ----
        if (sigsetjmp(punto_salto10, 1) == 0) {
            printf("Programa iniciado.\n");
            printf("Pulsa Ctrl+C para enviar SIGINT.\n");
        } else {
            // Solo se llega aquí si YA llegó un SIGUSR1
            fd = open(myfifo, O_WRONLY);   // Se bloquea hasta que alguien lea
            write(fd, "jajajaja", sizeof("jajajaja"));
            close(fd);
        }
    }
    // Esta línea NUNCA se ejecuta: el while(1) no tiene break ni return
    printf("Fin del programa.\n");

    return 0;
}