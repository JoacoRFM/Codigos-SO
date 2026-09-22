#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TAM 100

int main(int argc, char *argv[])
{
    int fd_pipe;
    char mensaje[TAM];

    // =====================================
    // Verificar argumento recibido por execv
    // =====================================
    if (argc != 2) {
        printf("[C] Error: falta el descriptor del Pipe\n");
        return 1;
    }

    // Convertir argv[1] de texto a entero
    fd_pipe = atoi(argv[1]);

    printf("\n[C] ================================\n");
    printf("[C] Soy el Proceso C\n");
    printf("[C] PID = %d\n", getpid());
    printf("[C] Descriptor del Pipe = %d\n", fd_pipe);
    printf("[C] Esperando mensajes de A...\n");
    printf("[C] ================================\n");

    // =====================================
    // Escuchar el Pipe
    // =====================================
    while (1) {

        int n = read(fd_pipe, mensaje, TAM);

        if (n > 0) {

            mensaje[n] = '\0';

            printf("\n[C] PID = %d\n", getpid());
            printf("[C] Mensaje recibido por PIPE:\n");
            printf("[C] %s\n", mensaje);

        } else if (n == 0) {

            printf("[C] Pipe cerrado.\n");
            break;

        } else {

            perror("[C] Error leyendo Pipe");
            break;
        }
    }

    close(fd_pipe);

    return 0;
}