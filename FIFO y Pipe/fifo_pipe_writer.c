#include <fcntl.h>      // Para open(), O_WRONLY
#include <sys/stat.h>   // Para mkfifo()
#include <sys/types.h>
#include <unistd.h>     // Para write(), close(), unlink()

int main()
{
    int fd;
    char * myfifo = "/tmp/myfifo";   // Ruta del archivo FIFO

    /* create the FIFO (named pipe) */
    // Crea el archivo especial FIFO en el sistema de archivos.
    // 0666 son los permisos (lectura/escritura para todos).
    // Si el FIFO ya existe, mkfifo falla (pero aquí no se revisa el error).
    mkfifo(myfifo, 0666);

    /* write "Hi" to the FIFO */
    // Abre el FIFO en modo solo escritura.
    // OJO: esto se BLOQUEA hasta que algún otro proceso lo abra
    // en modo lectura (los FIFOs sincronizan escritor y lector así).
    fd = open(myfifo, O_WRONLY);

    // Escribe el string "Hi" incluyendo el '\0' final
    // (sizeof("Hi") = 3, cuenta el terminador nulo)
    write(fd, "Hi", sizeof("Hi"));

    close(fd);   // Cierra el descriptor

    /* remove the FIFO */
    unlink(myfifo);   // Borra el archivo FIFO del sistema de archivos

    return 0;
}