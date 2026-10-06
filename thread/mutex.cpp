#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>        // std::mutex, std::lock_guard
#include <stdexcept>    // std::logic_error

std::mutex mtx;   // Mutex GLOBAL compartido por todos los hilos

void pause_thread(int s, int n)
{
    std::this_thread::sleep_for(std::chrono::seconds(n));

    try {
        // lock_guard toma el mutex al construirse y lo libera automáticamente
        // al salir del bloque (ya sea normalmente o por una excepción).
        // Esto evita que dos hilos entren a la vez a la sección protegida.
        std::lock_guard<std::mutex> lck(mtx);

        int ii = 7;   // variable sin uso real (no afecta la lógica)

        throw (std::logic_error("Error"));   // Lanza una excepción SIEMPRE

        // Esta línea NUNCA se ejecuta porque el throw de arriba
        // interrumpe el flujo antes de llegar aquí
        std::cout << "Yo soy: " << s << " seconds ended\n";
    }
    catch (const std::runtime_error&) {
        // BUG: esto captura std::runtime_error, pero lo que se lanza
        // es std::logic_error. Aunque logic_error y runtime_error
        // son ambas hijas de std::exception, NO hay relación de
        // herencia entre ellas -- por lo tanto este catch NO
        // captura la excepción lanzada, y el programa TERMINARÍA
        // abruptamente (std::terminate) en cada hilo.
        std::cout << "[nop]\n";
    }
}

int main()
{
    std::cout << "Spawning and detaching 3 threads...\n";
    for(int i = 0; i < 100; i++) {
        std::thread (pause_thread, i, 2).detach();
    }
    std::cout << "Done spawning threads.\n";
    std::cout << "(the main thread will now pause for 5 seconds)\n";
    pause_thread(666, 1000);
    return 0;
}