/*
Napisati konkurentni program koji simulira rasporedjivanje po algoritmu povratne sprege
u jednoprocesorskom sistemu. Procesi su predstavljeni nitima. Da bi se izvrsili pozivaju
metodu izvrsi deljene promenljive tipa Rasporedjivac.
Svaki proces sadrzi odredjeni broj naredbi.
Naredbe se izvrsavaju jedna po jedna. Za svaku naredbu, proces mora da saceka da procesor bude slobodan.
Izvrsavanje jedne naredbe traje 300 ms. Nakon izvrsavanja naredbe,
proces odlazi u stanje spreman, a drugi spreman proces dobija procesor.

Procesi su klasifikovani prema prioritetu. 0 je najvisi prioritet dok je N najmanji!!
Za svaki prioritet, postoji odvojen red cekanja na procesor.
Prednost u koriscenju procesora imaju procesi sa visim prioritetom.
Medju procesima istog prioriteta, sledeci proces se bira po FIFO principu.

Prioritet procesa se odredjuje dinamicki u toku izvrsavanja procesa.
Na pocetku rada, proces ima najvisi prioritet. Kada izvrsi naredbu, prelazi na jedan nivo
prioriteta ispod. Nakon svake naredbe, procesu se smanjuje prioritet za jedan nivo, dok ne
dodje do najnizeg prioriteta. Ako je dosao do najnizeg prioriteta,
na tom prioritetu ostaje do kraja izvrsavanja.

Na kraju izvrsavanja svake naredbe proces treba da ispise:
Proces X izvrsio naredbu broj Y i presao na prioritet Z.

Napomena: Komentari su obavezni
*/


#include <iostream>
#include "cv_hrono.h"            //sadrzi u sebi mutex, thread, cv...
#include <queue>
#include <vector>

class Red {
    private:
        int prioritet;
        cv_hrono* cv;
        queue<int> spremni;
    public:
        Red(int p) : prioritet(p) {
            cv = new cv_hrono;
        }

        void dodaj_u_red(int id, unique_lock<mutex>& l) {
            spremni.push(id);
            cv->wait(l);
        }

        int izbaciIzReda() {
            int izbacen = spremni.front();
            spremni.pop();
            cv->notify_one();
            return izbacen;
        }

        int uzmiPrioritet() {
            return prioritet;
        }

        bool prazan() {
            return spremni.empty();
        }
};

class Rasporedjivac {
    private:
        mutex m;
        int aktivanProces;
        vector<Red> redovi;
	public:
		Rasporedjivac(int broj_nivoa_prioriteta);
		void izvrsi(int id_procesa, int broj_naredbi);
};

Rasporedjivac::Rasporedjivac(int broj_nivoa_prioriteta) {
    aktivanProces = -1;
    for(int i = 0; i < broj_nivoa_prioriteta; i++)
        redovi.push_back(Red(i));
}

void Rasporedjivac::izvrsi(int id_procesa, int broj_naredbi) {
    int prioritetRasporedjivaca = 0;
    for(int i = 0; i < broj_naredbi; i++) {
        unique_lock<mutex> l(m);

        if(aktivanProces == -1)
            aktivanProces = id_procesa;

        while(aktivanProces != id_procesa)
            redovi[prioritetRasporedjivaca].dodaj_u_red(id_procesa,l);

        l.unlock();
        this_thread::sleep_for(milliseconds(300));
        l.lock();

        int obavesti = -1;

        for(auto it = redovi.begin(); it != redovi.end(); it++) {
            if(!it->prazan()) {
                obavesti = it->uzmiPrioritet();
                break;
            }
        }

        if(obavesti != -1) {
            aktivanProces = redovi[obavesti].izbaciIzReda();
        } else
            aktivanProces = -1;

        if(prioritetRasporedjivaca < redovi.size() - 1)
            prioritetRasporedjivaca++;

        cout << "Proces " << id_procesa << " izvrsio naredbu " << i
					<< " i dobio prioritet " << prioritetRasporedjivaca << endl;


    }
}

mutex term_m;

const int BROJ_PROCESA = 5;
const int BROJ_PRIORITETA = 5;

void proces(Rasporedjivac& r, int id_procesa, int broj_naredbi) {
	{
		unique_lock<mutex> l(term_m);
		cout << "Kreira se proces " << id_procesa << " sa brojem naredbi " << broj_naredbi << endl;
	}
	r.izvrsi(id_procesa, broj_naredbi);
	unique_lock<mutex> l(term_m);
	cout << "Proces " << id_procesa << " se zavrsio." << endl;
}

int main() {
	srand(time(NULL));

	Rasporedjivac r(BROJ_PRIORITETA);
	thread procesi[BROJ_PROCESA];

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i] = thread(proces, ref(r), i+1, rand()%5 + 1);

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i].join();
}
