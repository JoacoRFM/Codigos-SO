#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>   // Funciones y constantes de colas de mensajes: msgget, msgsnd, msgrcv
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE     128   // Tamaño máximo del texto del mensaje

// Función auxiliar: imprime el error del sistema y termina el programa
void die(char *s)
{
  perror(s);   // Imprime el mensaje de error asociado a errno
  exit(1);
}

// Estructura del mensaje. IMPORTANTE: el primer campo SIEMPRE debe
// llamarse mtype y ser de tipo long — es un requisito del sistema
// para que msgsnd/msgrcv funcionen. Esta struct debe coincidir
// exactamente con la del programa que recibe.
struct msgbuf
{
    long    mtype;          // Tipo de mensaje (lo usa msgrcv para filtrar)
    char    mtext[MAXSIZE];  // Contenido del mensaje
};

main()
{
    int msqid;                        // ID de la cola de mensajes (lo que devuelve msgget)
    int msgflg = IPC_CREAT | 0666;    // Flags: crea la cola si no existe, permisos rw para todos
    key_t key;                        // Llave para identificar la cola
    struct msgbuf sbuf;               // El mensaje que vamos a enviar
    size_t buflen;                    // Longitud real del texto a enviar

    key = 1234;   // Llave fija, acordada de antemano con el receptor
                  // (en un examen, esto podría generarse con ftok())

    // Obtiene el ID de la cola asociada a "key". Como msgflg incluye
    // IPC_CREAT, si la cola no existe, la CREA. Si ya existe (porque
    // el receptor ya la creó, por ejemplo), simplemente la reutiliza.
    if ((msqid = msgget(key, msgflg )) < 0)
      die("msgget");

    // Definimos el tipo de este mensaje como 1.
    // Esto es lo que el receptor va a usar para decidir si "le
    // interesa" este mensaje o no.
    sbuf.mtype = 1;

    printf("Enter a message to add to message queue : ");
    scanf("%[^\n]",sbuf.mtext);   // Lee todo hasta el salto de línea
    getchar();                    // Consume el '\n' que queda en el buffer de entrada

    // msgsnd necesita saber cuántos bytes de mtext enviar realmente
    // (no todo el arreglo de MAXSIZE, solo lo que se usó + el '\0')
    buflen = strlen(sbuf.mtext) + 1 ;

    // Envía el mensaje a la cola identificada por msqid.
    // IPC_NOWAIT: si la cola está llena, no se queda esperando,
    // retorna error inmediatamente en vez de bloquear.
    if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
    {
        // Si falla, imprime info de debug antes de morir
        printf ("%d, %ld, %s, %d \n", msqid, sbuf.mtype, sbuf.mtext, (int)buflen);
        die("msgsnd");
    }
    else
        printf("Message Sent\n");

    exit(0);
}