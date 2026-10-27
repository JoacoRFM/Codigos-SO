#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1111  // CLAVE: debe ser igual en todos los procesos que

#define NUM_SEMS 3  // Cantidad de semáforos en el conjunto.

union semun {
    int val;                  // Valor para SETVAL
    struct semid_ds *buf;     // Información del semáforo
    unsigned short *array;    // Valores de varios semáforos
};

// semop(id, &op, 1)
// id  -> identificador del conjunto
// &op -> operación que se ejecutará
// 1   -> cantidad de operaciones

// sembuf = {indice, operacion, banderas}
// indice   -> semáforo dentro del conjunto (0, 1, 2...)
// operacion -1 -> P (esperar/decrementar)
// operacion +1 -> V (liberar/incrementar)
// operacion  0 -> esperar hasta que el valor sea 0
// banderas  0 -> espera normal (sin IPC_NOWAIT)

// P: adquirir recurso / esperar
void semP(int id, int num){
    struct sembuf op = {num, -1, 0};
    if (semop(id, &op, 1) < 0){
        perror("semP");
        exit(1);
    }
}

// V: liberar recurso / señalar
void semV(int id, int num){
    struct sembuf op = {num, +1, 0};
    if (semop(id, &op, 1) < 0){
        perror("semV");
        exit(1);
    }
}

// semget(KEY, cantidad, permisos | flags)
// KEY       -> clave compartida entre procesos
// NUM_SEMS  -> cantidad de semáforos
// 0666      -> permisos de lectura y modificación
// IPC_CREAT -> crear si no existe
// Si ya existe un conjunto compatible, lo obtiene.
int getSemCreate(){
    int id = semget(KEY, NUM_SEMS, 0666 | IPC_CREAT);
    if (id < 0){
        perror("semget create");
        exit(1);
    }
    return id;
}

// Obtener conjunto EXISTENTE.
// Sin IPC_CREAT: falla si no existe.
int getSem(){
    int id = semget(KEY, NUM_SEMS, 0);
    if (id < 0){
        perror("semget");
        exit(1);
    }
    return id;
}

// semctl(id, num, SETVAL, u)
// id     -> identificador del conjunto
// num    -> índice del semáforo (0, 1, 2...)
// SETVAL -> establecer valor del semáforo
// u      -> contiene el valor inicial

// Valores típicos:
// 0 -> esperar señal de otro proceso
// 1 -> exclusión mutua (un recurso disponible)
// N -> N recursos disponibles

// IMPORTANTE: inicializar una sola vez, antes de usar.
void controlSem(int id, int num, int valor){
    union semun u;
    u.val = valor;
    if (semctl(id, num, SETVAL, u) < 0){
        perror("semctl SETVAL");
        exit(1);
    }
}

// GETVAL -> devuelve valor actual.
// Útil para depuración; no usar como sustituto de P.
int getValor(int id, int num){
    int valor = semctl(id, num, GETVAL);
    if (valor < 0){
        perror("semctl GETVAL");
        exit(1);
    }
    return valor;
}

// IPC_RMID -> elimina TODO el conjunto.
// Usar cuando ningún proceso lo necesite.
void eliminarSem(int id){
    if (semctl(id, 0, IPC_RMID) < 0){
        perror("semctl IPC_RMID");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    // Uso:
    // ./semaphore c ABC
    // ./semaphore g XYZ
    
    // argv[0] -> nombre del ejecutable
    // argv[1] -> modo: c o g
    // argv[2] -> cadena a imprimir
    
    // c -> crear/obtener e inicializar
    // g -> obtener sin reinicializar

    if (argc < 3 || (argv[1][0] != 'c' && argv[1][0] != 'g')){
        fprintf(stderr, "Uso: %s c|g CADENA\n", argv[0]);
        return 1;
    }

    int id;

    if (argv[1][0] == 'c'){
        id = getSemCreate();

        // Inicializar semáforo 0 en 1.
        // Cambiar a 0 si debe esperar una señal.
        // Cambiar a N si existen N recursos.
        controlSem(id, 0, 1);
    } else {
        id = getSem();
    }

    printf("sem: %d\n", id);

    char *s = argv[2];
    int l = strlen(s);

    for (int i = 0; i < l; i++){
        // INICIO SECCIÓN CRÍTICA
        semP(id, 0);

        putchar(s[i]);
        fflush(stdout);

        sleep(rand() % 2);

        putchar(s[i]);
        fflush(stdout);

        // Caso especial del código original:
        // si la cadena comienza con 'a', espera
        // entrada de teclado sin liberar el semáforo.
        if (s[0] == 'a'){
            char c;
            scanf(" %c", &c);
        }

        // FIN SECCIÓN CRÍTICA
        semV(id, 0);

        // Fuera de la sección crítica.
        // No mantiene bloqueado el recurso.
        sleep(rand() % 2);
    }

    putchar('\n');

    // NO eliminar aquí si otros procesos siguen usando
    // el semáforo. Hacer la limpieza al finalizar todos.

    return 0;
}