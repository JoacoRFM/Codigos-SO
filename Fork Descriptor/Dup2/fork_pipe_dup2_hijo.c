#include <stdio.h>
#include <unistd.h>

int main(void)
{
    char buffer[100];
    ssize_t n;

    // Lee de STDIN_FILENO normalmente... pero como el padre hizo
    // dup2 antes del execv, este "stdin" en realidad es el pipe.
    // programa2 NO SABE ni le importa que viene de un pipe.
    n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';   // Convierte lo leído en un string válido
        printf("Programa 2 recibió: %s", buffer);
    }

    return 0;
}