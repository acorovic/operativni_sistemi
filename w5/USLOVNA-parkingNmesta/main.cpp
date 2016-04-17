/*
    Definisati klasu Parking koja modeluje Parking prostor kapaciteta N mesta.
    Kapacitet Parkinga proslediti kao argument konstruktoru, pri instanciranju deljene promenljive.

	Klasa Parking ima operacije:
        void Parking::udji();
        void Parking::izadji();

	Automobili koji dolaze na Parking su predstavljeni nitima.
	Za ulazak na Parking, automobil poziva metodu udji().
	Za izlazak sa Parkinga, automobil poziva metodu izadji().
	Automobil se na Parkingu zadrzava od 1 do 3 sekunde.
    Pri ulasku, ukoliko su sva Parking mesta zauzeta, automobil mora da saceka da se neko Parking
    mesto oslobodi.
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
        int brMesta;
        condition_variable slobodnoMesto;
        mutex m;
    public:
        Parking(int n) : brMesta(n) {}
        int getMesta() {return brMesta;}
        void udji();
        void izadji();
};

void Parking::udji() {
    unique_lock<mutex> lock(m);
    while(brMesta == 0)
        slobodnoMesto.wait(lock);
    brMesta--;
}

void Parking::izadji() {
    unique_lock<mutex> lock(m);
    brMesta++;
    slobodnoMesto.notify_one();
}

void automobil(Parking &p, int rbr) {
    p.udji();
    unique_lock<mutex> terminal(term_m);
    cout << "autmobil usao " << rbr << " br mesta " << p.getMesta() << endl;
    terminal.unlock();
    this_thread::sleep_for(seconds(1));
    terminal.lock();
    p.izadji();
    cout << "automobil izasao " << rbr << " br mesta " << p.getMesta() << endl;
}

const int brAutomobila = 20;
const int brPMesta = 5;

int main()
{
    Parking p(brPMesta);
    thread niti[brAutomobila];
    for(int i = 0; i < brAutomobila; i++)
        niti[i] = thread(automobil, ref(p), i+1);
    for(int i = 0; i < brAutomobila; i++)
        niti[i].join();
    return 0;
}
