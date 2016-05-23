/*
Napisati konkurentni program koji simulira rasporedjivanje po sistemu SPN (Najkraci proces sledeci).
Ovo je politika rasporedjivanja bez prekidanja procesa koji je zauzeo procesor. Drugim recima proces
koji je zauzeo procesor izvrsava se u potpunosti pre nego sto procesor moze da se prekljuci na neki
od narednih procesa. Kod prekljucivanja uvek treba voditi racuna da procesor zauzme onaj proces koji
ima najkrace ocekivano vreme izvrsavanja (mereno u stotinama milisekundi).

Klasa Racunar predstavlja deljenu promenljivu kojoj pristupaju niti procesi i rasporedjivac.

Procesi su predstavljeni nitima cije je telo data funkcija proces.
Proces se izvrsava pozivom metode izvrsi klase Racunar. Izvrsavanje ce trajati
onoliko stotina milisekundi koliko je vrednost parametra trajanje. Ukoliko je
procesor zauzet proces ulazi u cekanje kako bi ga rasporedjivac naknadno
rasporedio (kada dodje njegov red na izvrsavanje).

Kada se jedan proces zavrsi, potrebno je da se rasporedi naredni proces.
Rasporedjivanje vrsi posebna nit predstavljena funkcijom rasporedjivac.
Ova nit se aktivira kada je potrebno izvrsiti rasporedjivanje narednog procesa
a rasporedjivanje se vrsi prema SPN algoritmu opisanom na pocetku zadatka.

Napomena: Komentari su obavezni.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "cv_tag.h"
#include <list>

using namespace std;
using namespace chrono;



class Racunar {
    private:
        mutex m;
        cv_tag red;
        condition_variable cv_rasporedjivac;
        int aktivanProces;
        bool rasporedjivacAktivan;
        map<int,int> procesi;
    public:
        Racunar() : aktivanProces(-1), rasporedjivacAktivan(false) {}
        void izvrsi(int id_procesa, int trajanje);
        void rasporedjuj();
};


void Racunar::izvrsi(int id_procesa, int trajanje) {
    unique_lock<mutex> l(m);

    if(aktivanProces == -1)
        aktivanProces = id_procesa;

    while(aktivanProces != id_procesa || rasporedjivacAktivan) {

        procesi[id_procesa] = trajanje;
        red.wait(l, id_procesa);
        procesi.erase(id_procesa);

    }

    l.unlock();
    this_thread::sleep_for(milliseconds(100*trajanje));
    l.lock();

    rasporedjivacAktivan = true;
    cv_rasporedjivac.notify_one();
}


void Racunar::rasporedjuj() {
    unique_lock<mutex> l(m);

    while(!rasporedjivacAktivan)
        cv_rasporedjivac.wait(l);

    if(!procesi.empty()) {
        map<int,int>::iterator it = procesi.begin();
        int najkraciID = it->first;
        int najkraciT = it->second;
        for(; it != procesi.end(); it++) {
            if(it->second < najkraciT) {
                najkraciT = it->second;
                najkraciID = it->first;
            }
        }
        aktivanProces = najkraciID;

        rasporedjivacAktivan = false;
        red.notify(najkraciID);
    } else
        {
            aktivanProces = -1;
            rasporedjivacAktivan = false;
        }

}

mutex term_m;

void proces(Racunar& r, int id_procesa, int trajanje) {
    {
        unique_lock<mutex> l(term_m);
        cout << "Kreiran proces " << id_procesa << " trajanja " << trajanje << endl;
    }
    r.izvrsi(id_procesa, trajanje);
    {
        unique_lock<mutex> l(term_m);
        cout << "Proces " << id_procesa << " trajanja " << trajanje << "zavrsen." << endl;
    }
}


void rasporedjivac(Racunar& r) {
    while(true) {
    r.rasporedjuj();
    }
}

int main() {
    Racunar r;
    thread tr(rasporedjivac, ref(r));
    tr.detach();
    int trajanja[] = {3, 2, 5, 7, 4, 9, 6};
    thread procesi[7];
    for (int i = 0; i < 7; i++) {
        procesi[i] = thread(proces, ref(r), i+1, trajanja[i]);
    }
    for (int i = 0; i < 7; i++)
        procesi[i].join();

    exit(0); //exit 0 - predstavlja signal detach-ovanoj niti da prekine rad jer vise nije neophodna
}
