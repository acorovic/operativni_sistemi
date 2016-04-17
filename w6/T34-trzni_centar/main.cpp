/*
Modelovati placanje robe u trznom centru. U trznom centru postoje 2 kase za placanje.
Kupac pri placanju staje u red na onu kasu na kojoj ceka manji broj kupaca.
Kupac vrsi placanje pozivom metode kupi() koja kao parametar dobija broj artikala koje kupac placa.
Placanje robe traje onoliko sekundi koliko ima artikala.
Povratna vrednost metode je identifikator kase na kojoj je placanje izvrseno.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace chrono;

const int brKasa = 2;
const int brKupaca = 12;
const int maxBrArtiklova = 5;

class Trzni_centar {
    private:
        int brLjudiNaKasi[brKasa];
        bool zauzetaKasa[brKasa];
        condition_variable oslobodiKasu[brKasa];
        mutex m;
    public:
        Trzni_centar() {
            zauzetaKasa[0] = false;
            zauzetaKasa[1] = false;
            brLjudiNaKasi[0] = 0;
            brLjudiNaKasi[1] = 0;
        }
        int kupi(int broj_artikala);
};

int Trzni_centar::kupi(int broj_artikala) {
    {
        unique_lock<mutex> lock(m);
        int otisaoNaKasu = (brLjudiNaKasi[0] > brLjudiNaKasi[1]) ? 1 : 0;
        brLjudiNaKasi[otisaoNaKasu]++;
        while(zauzetaKasa[otisaoNaKasu])
            oslobodiKasu[otisaoNaKasu].wait(lock);
        zauzetaKasa[otisaoNaKasu] = true;
        lock.unlock();
        this_thread::sleep_for(seconds(broj_artikala));
        lock.lock();
        brLjudiNaKasi[otisaoNaKasu]--;
        zauzetaKasa[otisaoNaKasu] = false;
        oslobodiKasu[otisaoNaKasu].notify_one();
        return otisaoNaKasu;

    }
}

mutex term_m;

void kupac(Trzni_centar &p, int kolicina, int rbk) {
    {
        unique_lock<mutex> l(term_m);
        cout << "Kupac " << rbk << " zeli da kupi "
             << kolicina << " komada robe." << endl;
    }
    int kasa = p.kupi(kolicina);
    {
        unique_lock<mutex> l(term_m);
        cout << "Kupac " << rbk << " kupio na kasi " << kasa
             << ", " << kolicina << " komada robe." << endl;
    }
}



int main()
{
    Trzni_centar tc;
    thread niti[brKupaca];
    for(int i = 0; i < brKupaca; i++)
        niti[i] = thread(kupac, ref(tc), rand()%maxBrArtiklova + 1, i + 1);
    for(int i = 0; i < brKupaca; i++)
        niti[i].join();
    return 0;
}
