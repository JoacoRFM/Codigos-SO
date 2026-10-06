#include <iostream>
#include <thread>
#include <mutex>        // std::mutex, std::lock_guard
#include <stdexcept>    // std::logic_error

std::mutex mtx;   // Mutex compartido

// Lanza una excepción si x es impar
void print_even (int x) {
  if (x%2==0) std::cout << x << " is even\n";
  else throw (std::logic_error("not even"));
}

void print_thread_id (int id) {
  try {
    // Mientras este lock_guard "viva" (dentro del try), ningún otro
    // hilo puede entrar a esta sección -- así se evita que dos hilos
    // impriman mezclado al mismo tiempo (sección crítica).
    std::lock_guard<std::mutex> lck (mtx);
    print_even(id);
  }
  catch (std::logic_error&) {
    // AQUÍ SÍ coincide el tipo: print_even lanza logic_error,
    // y este catch captura logic_error -- funciona correctamente.
    // Cuando salta la excepción, lock_guard libera el mutex
    // automáticamente al destruirse (RAII), así que no hay riesgo
    // de dejar el mutex bloqueado para siempre.
    std::cout << "[exception caught]\n";
  }
}

int main ()
{
  std::thread threads[10];   // Arreglo de 10 hilos (NO detached esta vez)

  // Crea 10 hilos, cada uno llamando print_thread_id con id = 1,2,...,10
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_thread_id, i+1);

  // join() SÍ espera a que cada hilo termine antes de continuar.
  // Esto es la forma "segura" y estándar de esperar hilos
  // (a diferencia del truco de dormir que usan threads.cpp/t.cpp)
  for (auto& th : threads) th.join();

  return 0;
}