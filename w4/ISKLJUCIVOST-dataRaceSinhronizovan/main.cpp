/*
Izmeniti program data_race.cpp i realizovati zaÅ¡titu pristupa brojaÄu uz pomoÄ‡
klase mutex i njenih operacija lock() i unlock(). Primetiti koliko sada traje izvrÅ¡avanje programa.
*/


#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int ITERATIONS = 100000;

mutex m;

void inkrement(int &a) {
    for(int i = 0; i < ITERATIONS; i++) {
        m.lock();
        a++;
        m.unlock();
        }
}

void dekrement(int &a) {
    for(int i = 0; i < ITERATIONS; i++) {
        m.lock();
        a--;
        m.unlock();
    }
}



int main()
{
    int a = 0;
    thread nit1(inkrement, ref(a));
    thread nit2(dekrement, ref(a));
    nit1.join();
    nit2.join();

    cout << a << endl;
    return 0;
}
