#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE     128

void die(char *s)
{
  perror(s);
  exit(1);
}

// MISMA estructura que en el programa emisor. Esto NO es opcional:
// si mtext tuviera otro tamaño o el orden de campos cambiara, el
// programa podría leer datos corruptos o fallar.
struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};

void main()
{
    int msqid;
    key_t key;
    struct msgbuf rcvbuffer;   // Aquí se va a guardar el mensaje recibido

    key = 1234;   // MISMA key que el emisor, para apuntar a la misma cola

    // Aquí NO se usa IPC_CREAT (msgflg es solo 0666) — se espera que
    // la cola YA EXISTA (creada por el que envía, o por otro proceso
    // antes). Si no existe, msgget falla.
    if ((msqid = msgget(key, 0666)) < 0)
      die("msgget()");

    // msgrcv recibe un mensaje de la cola msqid, lo guarda en rcvbuffer,
    // como máximo MAXSIZE bytes de texto, y PIDE ESPECÍFICAMENTE
    // un mensaje de tipo 1 (el 4to parámetro).
    // El último parámetro (0) son flags — 0 significa que si no hay
    // ningún mensaje de tipo 1 todavía, el programa se BLOQUEA
    // esperando hasta que llegue uno.
    if (msgrcv(msqid, &rcvbuffer, MAXSIZE, 1, 0) < 0)
      die("msgrcv");

    printf("%s\n", rcvbuffer.mtext);
    exit(0);
}