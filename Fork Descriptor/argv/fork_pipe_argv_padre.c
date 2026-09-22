#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        // ---------- HIJO ----------
        close(fd[1]);   // No necesita escribir, solo leer

        // Convierte el número de descriptor fd[0] (un int) a texto,
        // porque execv solo puede pasar argumentos como strings.
        char descriptor[20];
        sprintf(descriptor, "%d", fd[0]);

        char *args[] = {
            "./programa3.exe",
            descriptor,   // Le pasamos el número del fd como argv[1]
            NULL
        };
        printf("Descriptor del papa: %s\n", descriptor);

        // Reemplaza el proceso hijo por programa3.exe, pasándole
        // el descriptor como argumento de texto
        execv("./programa3.exe", args);

        perror("execv");
        exit(1);
    }

    // ---------- PADRE ----------
    close(fd[0]);

    char mensaje[] = "Keiko y Pedrito\n";
    write(fd[1], mensaje, sizeof(mensaje) - 1);

    close(fd[1]);

    wait(NULL);

    return 0;
}