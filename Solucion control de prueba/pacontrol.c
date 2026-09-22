#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <fcntl.h>

#define TAM 100

// Estructura para la cola de mensajes
struct mensaje {
    long tipo;
    char texto[TAM];
};

// Variables globales para que los manejadores puedan usarlas
int pipefd[2];
int id_cola;

void manejador_sigint(int signo)
{
    struct mensaje msg;

    printf("\n[A] Recibí SIGINT (señal %d)\n", signo);

    msg.tipo = 1;
    strcpy(msg.texto, "Mensaje enviado desde A hacia B mediante Message Passing");

    if (msgsnd(id_cola, &msg, sizeof(msg.texto), 0) == -1) {
        perror("Error en msgsnd");
    } else {
        printf("[A] Mensaje enviado a B por la cola\n");
    }
}

void manejador_sigtrap(int signo)
{
    char mensaje[TAM];

    printf("\n[A] Recibí SIGTRAP (señal %d)\n", signo);

    strcpy(mensaje, "Mensaje enviado desde A hacia C mediante Pipe");

    if (write(pipefd[1], mensaje, strlen(mensaje) + 1) == -1) {
        perror("Error escribiendo en pipe");
    } else {
        printf("[A] Mensaje enviado a C por el Pipe\n");
    }
}

int main()
{
    pid_t pid_b;

    // ==============================
    // 1. Crear PIPE
    // ==============================
    if (pipe(pipefd) == -1) {
        perror("Error creando pipe");
        exit(EXIT_FAILURE);
    }

    // ==============================
    // 2. Crear COLA DE MENSAJES
    // ==============================
    id_cola = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    if (id_cola == -1) {
        perror("Error creando cola");
        exit(EXIT_FAILURE);
    }

    printf("[A] PID = %d\n", getpid());
    printf("[A] Cola creada: %d\n", id_cola);

    // ==============================
    // 3. Registrar señales
    // ==============================
    signal(SIGINT, manejador_sigint);
    signal(SIGTRAP, manejador_sigtrap);

    // ==============================
    // 4. fork() para crear B
    // ==============================
    pid_b = fork();

    if (pid_b == -1) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    // =====================================================
    // PROCESO B
    // =====================================================
    if (pid_b == 0) {

        pid_t pid_c;

        printf("[B] Soy B. PID = %d, PPID = %d\n",
               getpid(), getppid());

        // B no escribe en el pipe
        close(pipefd[1]);

        // B crea su clon
        pid_c = fork();

        if (pid_c == -1) {
            perror("Error en fork de B");
            exit(EXIT_FAILURE);
        }

        // =================================================
        // CLON DE B -> EXECV -> PROCESO C
        // =================================================
        if (pid_c == 0) {

            char fd_pipe[20];

            printf("[Clon B] PID = %d\n", getpid());
            printf("[Clon B] Ejecutando execv()...\n");

            // Convertimos el descriptor a texto
            sprintf(fd_pipe, "%d", pipefd[0]);

            // Argumentos que recibirá proceso_c
            char *args[] = {
                "./proceso_c",
                fd_pipe,
                NULL
            };

            execv("./proceso_c", args);

            // Solo llega aquí si execv falla
            perror("Error en execv");
            exit(EXIT_FAILURE);
        }

        // =================================================
        // B ORIGINAL -> MESSAGE PASSING
        // =================================================

        struct mensaje msg;

        printf("[B] Esperando mensajes de A...\n");

        while (1) {

            if (msgrcv(id_cola,
                       &msg,
                       sizeof(msg.texto),
                       1,
                       0) == -1) {

                perror("Error en msgrcv");
                break;
            }

            printf("\n[B] PID = %d\n", getpid());
            printf("[B] Mensaje recibido por Message Passing:\n");
            printf("[B] %s\n", msg.texto);
        }

        wait(NULL);
        exit(0);
    }

    // =====================================================
    // PROCESO A
    // =====================================================

    // A no lee del pipe
    close(pipefd[0]);

    printf("[A] Proceso B creado. PID de B = %d\n", pid_b);

    printf("\n========================================\n");
    printf("        PROCESO A ESPERANDO\n");
    printf("========================================\n");
    printf("PID de A: %d\n", getpid());
    printf("\nDesde otra terminal ejecuta:\n");
    printf("kill -2 %d   -> envia SIGINT -> B\n", getpid());
    printf("kill -5 %d   -> envia SIGTRAP -> C\n", getpid());
    printf("========================================\n\n");

    // ==============================
    // 5. A espera señales
    // ==============================
    while (1) {
        pause();
    }

    return 0;
}