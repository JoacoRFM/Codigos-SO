#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);   // fd[0] = extremo de LECTURA, fd[1] = extremo de ESCRITURA

    pid_t pid = fork();   // Duplica el proceso; ambos (padre e hijo)
                          // heredan fd[0] y fd[1] abiertos

    if (pid == 0) {
        // ---------- CÓDIGO DEL HIJO ----------
        // El hijo va a LEER del pipe, así que no necesita el
        // extremo de escritura: lo cierra.
        close(fd[1]);

        // dup2(origen, destino): hace que "destino" apunte a lo
        // mismo que "origen". Aquí: STDIN_FILENO (el 0, entrada
        // estándar) ahora ES el extremo de lectura del pipe.
        // A partir de esta línea, leer de stdin = leer del pipe.
        dup2(fd[0], STDIN_FILENO);

        // Ya no necesitamos el descriptor original fd[0] porque
        // STDIN_FILENO ya apunta a lo mismo.
        close(fd[0]);

        // Preparamos los argumentos para execv (argv[] del nuevo programa)
        char *args[] = {"./programa2", NULL};

        // execv REEMPLAZA la imagen del proceso actual (el hijo)
        // por el programa "./programa2". Si tiene éxito, el código
        // que sigue (perror, exit) NUNCA se ejecuta, porque el
        // proceso ya "es" programa2 ahora.
        // OJO: los descriptores de archivo (como el stdin ya
        // redirigido) SE MANTIENEN a través de execv — por eso
        // esta técnica funciona.
        execv("./programa2", args);

        // Solo se llega aquí si execv FALLÓ
        perror("execv");
        exit(1);
    }

    // ---------- CÓDIGO DEL PADRE ----------
    // El padre va a ESCRIBIR al pipe, no necesita leer.
    close(fd[0]);

    char mensaje[] = "Hola desde el proceso padre\n";

    // Escribe el mensaje al extremo de escritura del pipe
    write(fd[1], mensaje, sizeof(mensaje) - 1);

    close(fd[1]);   // Importante: cerrar esto para que el hijo
                    // reciba EOF cuando termine de leer

    wait(NULL);   // Espera a que el proceso hijo termine (evita zombie)

    return 0;
}