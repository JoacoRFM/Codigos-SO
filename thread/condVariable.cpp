#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>   // std::condition_variable

std::mutex mtx;
std::condition_variable cv;   // Variable de condición: permite que un hilo
                               // "duerma" hasta que otro lo despierte
bool ready = false;   // Bandera compartida: "¿ya podemos continuar?"

void print_id (int id) {
  // unique_lock es como lock_guard pero más flexible: permite
  // liberar y re-adquirir el lock manualmente, lo cual es NECESARIO
  // para usar cv.wait().
  std::unique_lock<std::mutex> lck(mtx);

  // BUG: "id == id - 1" es SIEMPRE falso (un número nunca es igual
  // a sí mismo menos 1). Entonces la condición del while nunca
  // depende realmente de "ready", y básicamente el while actúa
  // como si dijera "while (false)" -- nunca entra al cv.wait().
  // Lo que probablemente se quería era otra condición (o simplemente
  // "while (!ready)"), para que el hilo espere hasta que go() avise.
  while (!ready && id == id -1) cv.wait(lck);
  // cv.wait(lck): libera el mutex y DUERME el hilo hasta que alguien
  // llame a cv.notify_*() -- al despertar, vuelve a tomar el mutex
  // automáticamente antes de continuar.

  std::cout << "thread " << id << '\n';
}

void go() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;             // Marca que ya se puede continuar
  cv.notify_all();          // Despierta a TODOS los hilos que estén
                             // esperando en cv.wait()
}

int main ()
{
  std::thread threads[10];
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_id,i);

  std::cout << "10 threads ready to race...\n";
  go();   // Dispara el "arranque" -- en teoría los 10 hilos estaban
          // esperando este aviso para imprimir casi simultáneamente

  for (auto& th : threads) th.join();

  return 0;
}