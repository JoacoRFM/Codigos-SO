#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// Debe coincidir con semaphore.c
#define KEY 0x1111

// Cambiar según cantidad de semáforos necesarios.
#define NUM_SEMS 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void){
    int id;
    union semun u;

    // Crear u obtener conjunto de semáforos.
    // KEY      -> clave compartida
    // NUM_SEMS -> cantidad
    // IPC_CREAT -> crear si no existe
    id = semget(KEY, NUM_SEMS, 0666 | IPC_CREAT);

    if (id < 0){
        perror("semget");
        exit(1);
    }

    // 1 -> exclusión mutua
    // 0 -> esperar una señal
    // N -> N recursos disponibles
    /*u.val = 1;

    if (semctl(id, 0, SETVAL, u) < 0){
        perror("semctl");
        exit(1);
    }*/

    // Semáforo 0: exclusión mutua
    u.val = 1;
    semctl(id, 0, SETVAL, u);

    // Semáforo 1: sincronización
    u.val = 0;
    semctl(id, 1, SETVAL, u);

    // Semáforo 2: tres recursos disponibles
    u.val = 3;
    semctl(id, 2, SETVAL, u);

    // ---------------------------------------------
    // SI NECESITAS DOS SEMÁFOROS:
    //
    // 1. Cambiar NUM_SEMS a 2 en ambos programas.
    //
    // 2. Inicializar el segundo:
    //
    // u.val = 0;
    // semctl(id, 1, SETVAL, u);
    //
    // Ejemplo para alternancia:
    // Semáforo 0 = 1
    // Semáforo 1 = 0
    // ---------------------------------------------

    printf("Semaforos inicializados. ID: %d\n", id);

    return 0;
}