/*
Modelovati soping na rasprodaji.
Kupci dolaze u prodavnicu da kupe odecu.

Kupac najpre probava odecu u jednoj od kabina za probavanje.
Broj kabina za probavanje prosledjuje se pri instanciranju klase Prodavnica.
Ako su sve kabine zauzete, kupac mora da saceka sa probavanjem dok se neka kabina ne oslobodi.

Nakon probavanja odece, kupac vrsi kupovinu, ako mu odeca odgovara.
Sansa da mu odeca odgovara je 50%.
Probavanje odece traje 1 sekundu.

Metoda "kupi" vraca informaciju da li je kupac kupio odecu i koliko dugo je cekao da
udje u kabinu.

Data funkcija "kupac" modeluje ponasanje kupca.
Ako mu isprobana odeca ne odgovara, kupac odlazi da pronadje
drugi komad odece i onda ponovo odlazi da proba odecu u kabini.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <cmath>

using namespace std;
using namespace chrono;

const int vremeIsprobavanja = 1;

struct povratna_vrednost {
	bool kupio;
	duration<double, milli> cekao_na_kabinu;
};

class Prodavnica {
        int slobodnih_kabina;
        mutex m;
        condition_variable slobodno;
    public:
        Prodavnica(int n): slobodnih_kabina(n) {}
        povratna_vrednost kupi();
};

povratna_vrednost Prodavnica::kupi() {
    unique_lock<mutex> lock(m);
    system_clock::time_point pocetak = system_clock::now();
    while(slobodnih_kabina == 0)
        slobodno.wait(lock);
    system_clock::time_point kraj = system_clock::now();
    slobodnih_kabina--;
    povratna_vrednost pov;
    pov.cekao_na_kabinu = kraj - pocetak;
    lock.unlock();
    this_thread::sleep_for(seconds(vremeIsprobavanja));
    lock.lock();
    slobodnih_kabina++;
    pov.kupio = rand()%2;
    slobodno.notify_one();
    return pov;
}

mutex term_m;

void kupac(Prodavnica &p, int id_kupca) {
    {
        unique_lock<mutex> l(term_m);
        cout << "Kupac " << id_kupca << " usao u prodavnicu." << endl;
    }
    povratna_vrednost pv;
    do {
        pv = p.kupi();
        unique_lock<mutex> l(term_m);
        if (pv.kupio)
            cout << "Kupac " << id_kupca << " kupio odecu. Cekao na kabinu: "
                << pv.cekao_na_kabinu.count() << " milisekundi." << endl;
        else {
            cout << "Kupac " << id_kupca << " nije kupio odecu. Cekao na kabinu: "
                << pv.cekao_na_kabinu.count() << " milisekundi." << endl;
            this_thread::sleep_for(seconds(1)); //kupac trazi novi komad odece
        }
    } while (!pv.kupio);
}

const int brKupaca = 10;
const int brKabina = 3;

int main()
{
    Prodavnica p(brKabina);
    thread niti[brKupaca];
    for(int i = 0; i < brKupaca; i++) {
        niti[i] = thread(kupac, ref(p), i+1);
    }
    for(int i = 0; i < brKupaca; i++) {
        niti[i].join();
    }
    return 0;
}
