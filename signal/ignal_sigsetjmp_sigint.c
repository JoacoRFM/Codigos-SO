#include <stdio.h>
#include <signal.h>   // Para signal(), SIGINT, etc.
#include <setjmp.h>   // Para sigsetjmp() y siglongjmp()
#include <unistd.h>   // Para sleep()

// Variable global (static) que guarda el "estado" del programa
// en el punto donde se llama sigsetjmp(). Debe ser global/static
// porque el manejador de señal (otra función) necesita accederla.
static sigjmp_buf punto_salto;

// Función manejadora: se ejecuta automáticamente cuando llega la señal
// que registremos con signal(). El parámetro "signo" es el número
// de la señal que disparó esta función (ej: SIGINT = 2).
void manejador(int signo)
{
    printf("\nSe recibió la señal %d\n", signo);

    // siglongjmp "teletransporta" la ejecución de vuelta al lugar
    // donde se guardó punto_salto con sigsetjmp().
    // El segundo argumento (1) es el valor que sigsetjmp() va a
    // devolver esta vez (en vez de 0).
    siglongjmp(punto_salto, 1);
}

int main(void)
{
    // Le decimos al SO: "cuando llegue SIGINT (Ctrl+C), no mates
    // el programa como haces normalmente, mejor ejecuta manejador()".
    signal(SIGINT, manejador);

    // sigsetjmp GUARDA el punto de retorno la primera vez que se
    // ejecuta normalmente, y devuelve 0.
    // Si se llega aquí por un siglongjmp() (desde el manejador),
    // en vez de guardar nada, "regresa" a este punto pero devolviendo
    // el valor que le pasamos a siglongjmp (en este caso, 1).
    // El segundo argumento (1) indica que también restaure la máscara
    // de señales bloqueadas al momento del salto.
    if (sigsetjmp(punto_salto, 1) == 0) {
        // ---- Esta rama se ejecuta SOLO la primera vez (flujo normal) ----
        printf("Programa iniciado.\n");
        printf("Pulsa Ctrl+C para enviar SIGINT.\n");

        while (1) {
            printf("Ejecutando código normal...\n");
            sleep(2);  // Aquí el programa "espera"; Ctrl+C puede
                       // interrumpir esto en cualquier momento y
                       // disparar el manejador.
        }
    } else {
        // ---- Esta rama se ejecuta cuando volvemos por siglongjmp ----
        // O sea, después de que el usuario presionó Ctrl+C y el
        // manejador hizo el salto.
        printf("¡Se realizó el salto de código!\n");
        printf("Continuando desde el punto de recuperación...\n");
    }

    // Este printf se ejecuta siempre al final, tanto si el programa
    // corrió normal sin señal (nunca pasa aquí porque el while(1)
    // es infinito) como si vino del salto (else).
    printf("Fin del programa.\n");

    return 0;
}