/*
Napisati konkurentni program koji simulira prioritetni rasporedjivac
u jednoprocesorskom sistemu. Procesi su predstavljeni nitima. Da bi se izvrsili pozivaju
metodu izvrsi deljene promenljive tipa Rasporedjivac.
Svaki proces sadrzi odredjeni broj naredbi.
Naredbe se izvrsavaju jedna po jedna. Za svaku naredbu, proces mora da saceka da procesor bude slobodan.
Izvrsavanje jedne naredbe traje 300 ms. Nakon izvrsavanja naredbe, proces odlazi u stanje spreman,
a drugi spreman proces dobija procesor.

Procesi su klasifikovani prema prioritetu. 0 je najvisi prioritet dok je N najmanji!!

Za svaki prioritet, postoji odvojen red cekanja na procesor.
Prednost u koriscenju procesora imaju procesi sa visim prioritetom.
Medju procesima istog prioriteta, rasporedjivanje se obavlja po FIFO principu
(tj. ciklicno ce se smenjivati razlicite naredbe procesa istih prioriteta na procesoru).

Prioritet procesa je odredjen staticki u trenutku kreiranja procesa.

Na kraju izvrsavanja svake naredbe proces treba da ispise:
Proces X izvrsio naredbu broj Y.

Za uspostavljanje reda cekanja po FIFO principu koristiti datu klasu cv_hrono
 (moraju se ubaciti fajlov cv_hrono.cpp i cv_hrono.h u projekat)

Napomena: Komentari su obavezni
*/

#include <iostream>
#include "cv_hrono.h"  //sadrzi u sebi mutex, thread, cv...
#include <queue>
#include <vector>


class Red{
    private:
        int prioritet;
        cv_hrono* cv;
        queue<int> spremni;
    public:
        Red(int p) : prioritet(p) {
            cv = new cv_hrono;
        }
        void dodaj_u_red(int id_procesa,unique_lock<mutex>& l) {
            spremni.push(id_procesa);
            cv->wait(l);
        }
        int izbaciIzReda() {
            int izbacen = spremni.front();
            spremni.pop();
            cv->notify_one();
            return izbacen;
        }
        int preuzmiPrioritet() {
            return prioritet;
        }
        bool prazan() {
            return spremni.empty();
        }

};

class Rasporedjivac {
    private:
        int aktivniProces;
        mutex m;
        vector<Red> redovi;
	public:
		Rasporedjivac(int broj_nivoa_prioriteta);
		void izvrsi(int id_procesa, int broj_naredbi, int prioritet);
};

Rasporedjivac::Rasporedjivac(int br_nivoa_prioriteta) {
    aktivniProces = -1;
    for(int i = 0; i < br_nivoa_prioriteta; i++)
        redovi.push_back(Red(i));
}

void Rasporedjivac::izvrsi(int id_procesa, int broj_naredbi, int prioritet) {
    for(int i = 0; i < broj_naredbi; i++) {
        unique_lock<mutex> l(m);

        if(aktivniProces == -1)
            aktivniProces = id_procesa;

        while(aktivniProces != id_procesa)
            redovi[prioritet].dodaj_u_red(id_procesa,l);

        l.unlock();
        this_thread::sleep_for(milliseconds(300));
        l.lock();

        int obavesti = -1;

        for(auto it = redovi.begin(); it != redovi.end(); it++)
            if(!it->prazan()) {
                obavesti = it->preuzmiPrioritet();
                break;
            }

        if(obavesti != -1) {
            int sledeci;
            if(obavesti > prioritet && i < broj_naredbi - 1 )
                sledeci = id_procesa;
            else
                sledeci = redovi[obavesti].izbaciIzReda();
            aktivniProces = sledeci;
        }
            else
                aktivniProces = -1;

        cout << "Proces " << id_procesa << " zavrsio sa izvrsavanjem naredbe " << i << endl;
    }
}

mutex term_m;

const int BROJ_PROCESA = 5;
const int BROJ_PRIORITETA = 3;

void proces(Rasporedjivac& r, int id_procesa, int broj_naredbi) {
    int prioritet = rand() % BROJ_PRIORITETA;
    {
        unique_lock<mutex> l(term_m);
        cout << "Kreira se proces " << id_procesa << " prioriteta " <<
            prioritet << " sa brojem naredbi " << broj_naredbi << endl;
    }
	r.izvrsi(id_procesa, broj_naredbi, prioritet);
	unique_lock<mutex> l(term_m);
	cout << "Proces " << id_procesa << " sa prioritetom " <<
        prioritet << " se zavrsio." << endl;
}

int main() {
    srand(time(NULL));

    Rasporedjivac r(BROJ_PRIORITETA);
    thread procesi[BROJ_PROCESA];

    for (int i = 0; i < BROJ_PROCESA; i++)
	   procesi[i] = thread(proces, ref(r), i+1, rand()%10 + 1);

    for (int i = 0; i < BROJ_PROCESA; i++)
	   procesi[i].join();
}

