#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>   // Para fcntl(), F_GETFD, FD_CLOEXEC

int main(int argc, char *argv[])
{
    // Verifica que se haya pasado exactamente un argumento (el descriptor)
    if (argc != 2) {
        fprintf(stderr, "Uso: %s descriptor\n", argv[0]);
        return 1;
    }

    // Convierte el string del argumento de vuelta a un entero (el fd)
    int fd = atoi(argv[1]);
    printf("descriptor en el hijo (ARG): %d\n", fd);

    //////////////////////
    // Esto es una VERIFICACIÓN, no es necesario para que el pipe
    // funcione — es para comprobar un detalle importante:
    // fcntl(fd, F_GETFD) consulta las "flags" del descriptor.
    int flags = fcntl(fd, F_GETFD);

    if (flags == -1) {
        perror("fcntl");
    }
    else if (flags & FD_CLOEXEC) {
        // FD_CLOEXEC = "Close On EXEC": si estuviera activado,
        // el descriptor se CERRARÍA automáticamente al hacer execv,
        // y este programa NO podría usarlo.
        printf("FD_CLOEXEC está ACTIVADO\n");
    }
    else {
        // Por defecto, los descriptores creados con pipe() NO tienen
        // FD_CLOEXEC activado, así que sobreviven al execv — por
        // eso esta técnica de "pasar el número por argv" funciona.
        printf("FD_CLOEXEC está DESACTIVADO\n");
    }
    //////////////////////////////////

    char buffer[100];
    ssize_t n;

    // Lee directamente del descriptor fd (el que llegó como argumento),
    // NO de stdin como en programa2 — esa es la diferencia clave
    n = read(fd, buffer, sizeof(buffer) - 1);
    printf("n:%d\n", (int)n);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Recibido: %s", buffer);
    }

    close(fd);

    return 0;
}