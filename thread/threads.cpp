#include <iostream>
#include <thread>   // std::thread para crear hilos
#include <chrono>   // std::chrono::seconds para medir tiempo

// Función que va a correr cada hilo: espera "n" segundos y luego imprime
void pause_thread(int s, int n)
{
    std::this_thread::sleep_for(std::chrono::seconds(n));  // Duerme el hilo actual
    std::cout << "Yo soy: " << s << " seconds ended\n";
}

int main()
{
    std::cout << "Spawning and detaching 3 threads...\n";

    // Crea 100 hilos (el comentario dice "3" pero el código hace 100 -- ojo con esto)
    for(int i = 0; i < 100; i++) {
        // std::thread(func, args...) crea el hilo y lo arranca inmediatamente.
        // .detach() significa: "este hilo corre de forma independiente,
        // el programa principal NO va a esperarlo con join()".
        // Si el hilo detached no termina antes de que main() acabe,
        // simplemente se mata junto con el proceso.
        std::thread (pause_thread, i, 2).detach();
    }

    std::cout << "Done spawning threads.\n";
    std::cout << "(the main thread will now pause for 5 seconds)\n";

    // El hilo PRINCIPAL (main) también llama a pause_thread directamente
    // (no como hilo nuevo, sino en su propio flujo) y se "duerme" 1000 segundos,
    // dándole tiempo a los 100 hilos detached (que duermen solo 2s) para que terminen.
    pause_thread(666, 1000);

    return 0;
}