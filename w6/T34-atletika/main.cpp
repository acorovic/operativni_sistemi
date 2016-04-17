/*
	Modelovati koriscenje zaletista na atletskom mitingu.
	Isto zaletiste koriste dve discipline: skok u dalj i bacanje koplja.
	Zaletiste ne mogu istovremeno da koriste dva takmicara.
	Discipline se naizmenicno smenjuju na zaletistu (jedan skakac u dalj, pa jedan bacac koplja i tako redom).

	Skok u dalj za jednog takmicara traje 1 sekundu. Bacanje koplja 2 sekunde.
	Metodu skaci poziva skakac u dalj. Metoda vraca duzinu u metrima koliko je takmicar skocio
	(izmedju 0 i 9 metara moze skociti) i koliko je ukupno trebalo vremena da zavrsi skok
	(koliko je zajedno trajalo cekanje i skakanje).
	Metodu baciKoplje poziva bacac koplja. Metoda vraca duzinu u metrima koliko je takmicar bacio koplje
	(izmedju 0 i 100 metara moze baciti) i koliko je ukupno trebalo vremena da zavrsi bacanje koplja
	(koliko je zajedno trajalo cekanje i bacanje koplja).
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <cmath>

using namespace std;
using namespace chrono;

const int trajanjeSkok = 1;
const int trajanjeKoplje = 2;

struct retVal{
    duration<double,milli> trajanje;
    double rez;
};

class Zaletiste{
    private:
        condition_variable skakaci, bacaci;
        mutex m;
        bool slobodno;
        int cekaBacanje, cekaSkakanje;
    public:
        Zaletiste() {
            slobodno = true;
            cekaBacanje = 0;
            cekaSkakanje = 0;
        }
        retVal skoci();
        retVal baci();
};

retVal Zaletiste::skoci() {
    system_clock::time_point pocetak = system_clock::now();
    unique_lock<mutex> lock(m);
    ++cekaSkakanje;
    while(!slobodno)
        skakaci.wait(lock);
    --cekaSkakanje;
    slobodno = false;
    lock.unlock();
    this_thread::sleep_for(seconds(trajanjeSkok));
    system_clock::time_point kraj = system_clock::now();
    lock.lock();
    slobodno = true;
    if(cekaBacanje)
        bacaci.notify_one();
    else
        skakaci.notify_one();

    retVal povratna;
    povratna.trajanje = kraj - pocetak;
    povratna.rez = rand()%10 + 1;
    return povratna;
}

retVal Zaletiste::baci() {
    system_clock::time_point pocetak = system_clock::now();
    unique_lock<mutex> lock(m);
    ++cekaBacanje;
    while(!slobodno)
        bacaci.wait(lock);
    --cekaBacanje;
    slobodno = false;
    lock.unlock();
    this_thread::sleep_for(seconds(trajanjeKoplje));
    system_clock::time_point kraj = system_clock::now();
    lock.lock();
    slobodno = true;
    if(cekaSkakanje)
        skakaci.notify_one();
    else
        bacaci.notify_one();
    retVal povratna;
    povratna.trajanje = kraj - pocetak;
    povratna.rez = rand()%100 + 1;
    return povratna;
}

void skakac(Zaletiste& staza, int rbr) {
	retVal rez = staza.skoci();
	cout << "Takmicar sa brojem " << rbr << " skocio " << rez.rez << " metara."
         << ", a cekao " << rez.trajanje.count() << " milisekundi. " << endl;
}

void bacac(Zaletiste& staza, int rbr) {
	retVal rez = staza.baci();
	cout << "Takmicar sa brojem " << rbr << " bacio koplje " << rez.rez << " metara."
         << ", a cekao " << rez.trajanje.count() << " milisekundi. " << endl;
}

const int SKAKACA = 10;
const int BACACA = 10;

int main() {
    Zaletiste staza;
    thread skakaci[SKAKACA];                            //Jednak broj skakaca i bacaca, radi lakseg pracenja programa.
    thread bacaci[BACACA];

    for (int i = 0; i < 10; ++i) {
        skakaci[i] = thread(skakac, ref(staza), i+1);
        bacaci[i] = thread(bacac, ref(staza), i+1);
    }

    for (int i = 0; i < BACACA; ++i)
        bacaci[i].join();

    for (int i = 0; i < SKAKACA; ++i)
        skakaci[i].join();

}
