#include <fcntl.h>      // Para open(), O_RDONLY
#include <stdio.h>      // Para printf()
#include <sys/stat.h>
#include <unistd.h>     // Para read(), close()

#define MAX_BUF 1024

int main()
{
    int fd;
    char * myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];   // Buffer donde se guarda lo que se lee

    /* open, read, and display the message from the FIFO */
    // Abre el FIFO en modo solo lectura.
    // Se BLOQUEA hasta que otro proceso lo abra en modo escritura
    // (justo lo que hace Pipe_writer.c). Por eso el ORDEN de
    // ejecución no importa tanto: se "esperan" mutuamente.
    fd = open(myfifo, O_RDONLY);

    read(fd, buf, MAX_BUF);   // Lee hasta MAX_BUF bytes del FIFO

    printf("Received: %s\n", buf);   // Imprime lo recibido

    close(fd);

    return 0;
}