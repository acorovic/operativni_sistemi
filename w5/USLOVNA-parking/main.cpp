/*
    Definisati klasu Parking koja modeluje Parking prostor kapaciteta jednog mesta.
    Ova klasa ima operacije:
        void Parking::udji();
        void Parking::izadji();

    Automobili koji dolaze na Parking su predstavljeni nitima.
				Za ulazak na Parking, automobil poziva metodu udji().
				Za izlazak sa Parkinga, automobil poziva metodu izadji().
				Automobil se na Parkingu zadrzava 3 sekunde.
    Pri ulasku, ukoliko je Parking zauzet, automobil mora da saceka da se Parking oslobodi.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

mutex term_m;

class Parking{
    private:
        mutex m;
        condition_variable slobodno;
        bool pSlobodan;
    public:
        Parking() : pSlobodan(true) {}
        void udji();
        void izadji();
};

void Parking::udji() {
    unique_lock<mutex> lock(m);
    while(!pSlobodan)
        slobodno.wait(lock);
    pSlobodan = false;
}

void Parking::izadji() {
    unique_lock<mutex> lock(m);
    pSlobodan = true;
    slobodno.notify_one();
}

void automobil(Parking &p, int rbr) {
    unique_lock<mutex> term(term_m);
    cout << "automobil " << rbr << " zeli da udje na parking" << endl;
    term.unlock();
    p.udji();
    term.lock();
    cout << "automobil " << rbr << " usao na parking" << endl;
    term.unlock();
    this_thread::sleep_for(seconds(1));
    term.lock();
    p.izadji();
    cout << "automobil " << rbr << " izasao sa parkinga" << endl;

}

const int brAutomobila = 5;

int main()
{
    Parking p;
    thread niti[brAutomobila];
    for(int i = 0; i < brAutomobila; i++)
        niti[i] = thread(automobil,ref(p), i+1);
    for(int i = 0; i < brAutomobila; i++)
        niti[i].join();

    return 0;
}
