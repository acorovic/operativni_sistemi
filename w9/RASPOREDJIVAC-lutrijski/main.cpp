/*
Napisati konkurentni program koji simulira lutrijski rasporedjivac
u jednoprocesorskom sistemu. Procesi su predstavljeni nitima.
Da bi se izvrsili pozivaju
metodu izvrsi deljene promenljive tipa Rasporedjivac.
Svaki proces sadrzi odredjeni broj naredbi.
Naredbe se izvrsavaju jedna po jedna. Za svaku naredbu, proces mora da saceka da procesor bude slobodan.
Izvrsavanje jedne naredbe traje 300 ms. Nakon izvrsavanja naredbe,
proces odlazi u stanje spreman, a drugi spreman proces dobija procesor.

Lutrijski rasporedjivac se zasniva na tome da je
svakom procesu inicijalno dodeljen odredjeni broj tiketa.
Pri izboru sledeceg procesa za izvrsavanje, na slucajan nacin se bira jedan tiket.
Proces kojem je dodeljen taj tiket postaje aktivan.
Dodelom razlicitog broja tiketa procesima se implicitno uspostavljaju
priroriteti medju njima, obzirom da proces sa vecim brojem tiketa ima vece sanse
da bude izabran.

Na kraju izvrsavanja svake naredbe proces treba da ispise:
Proces X izvrsio naredbu broj Y.

Napomena: Komentari su obavezni
*/

#include <iostream>
#include <vector>
#include "cv_tag.h"				//sadrzi u sebi mutex, thread, cv...

struct Proces {
	int id; //id je pozitivan ceo broj
	int broj_naredbi;
	vector<int>* tiketi;
	Proces(int i, int b, vector<int>* t):
		id(i), broj_naredbi(b), tiketi(t) {}
};

class Rasporedjivac {
    private:
        mutex m;
        int aktivniProces;
        cv_tag red;
        map<int,int> sviTiketi;     //mapa svih tiketa tiket-id
    public:
        Rasporedjivac() {
            aktivniProces = -1;
        }
        void izvrsi(Proces p) {
            unique_lock<mutex> l(m);
            for(auto it = p.tiketi->begin(); it != p.tiketi->end(); it++)
                sviTiketi[*it] = p.id;

            for(int i = 0; i < p.broj_naredbi; i++) {
                if(aktivniProces == -1)
                    aktivniProces = p.id;

                while(aktivniProces != p.id)
                    red.wait(l,p.id);

                l.unlock();
                this_thread::sleep_for(milliseconds(300));
                l.lock();

                if(i == p.broj_naredbi - 1) {   //izbacivanje tiketa procesa
                    for(auto it = p.tiketi->begin(); it != p.tiketi->end(); it++) {
                        auto zaBrisanje = sviTiketi.find(*it);
                        sviTiketi.erase(zaBrisanje);
                    }
                }

                if(!sviTiketi.empty()) {
                    int pozicija = rand() % sviTiketi.size();   //naredni tiket
                    map<int,int>::iterator it = sviTiketi.begin();
                    for(int k = 0; k < pozicija; k++, it++);
                    aktivniProces = it->second;
                    red.notify(aktivniProces);
                } else
                    aktivniProces = -1;
                cout << "Proces " << p.id << " zavrsio izvrsavanjem naredbe " << i << endl;
                cout << "Sledeci proces je " << aktivniProces << endl;
            }
        }
};


void ispisi_tikete(Proces& p) {
    for (auto it = p.tiketi->begin(); it != p.tiketi->end(); it++)
        cout << *it << " ";
}

mutex term_mx;

void proces(Rasporedjivac& r, vector<int>& baza_tiketa, int id_procesa, int broj_naredbi) {

    //na slucajan nacin dodeljujemo odredjeni broj tiketa procesu
    vector<int> tiketi_procesa;
    {
        unique_lock<mutex> l(term_mx);
        int br_tiketa = rand() % 10 + 1;
        for (int i = 0; i < br_tiketa; i++) {
            int poz = rand() % baza_tiketa.size(); //bilo koji slobodan tiket
            tiketi_procesa.push_back(baza_tiketa[poz]);
            baza_tiketa.erase(baza_tiketa.begin() + poz); //ovaj tiket je dodeljen pa se izbacuje (ne sme biti dupliranih tiketa)
        }
    }

    Proces p(id_procesa, broj_naredbi, &tiketi_procesa);
    {
        unique_lock<mutex> l(term_mx);
        cout << "Kreira se proces " << id_procesa <<
        " sa brojem naredbi " << broj_naredbi <<
        " i tiketima: ";
        ispisi_tikete(p);
        cout << endl;
    }
    r.izvrsi(p);
    unique_lock<mutex> l(term_mx);
    cout << "Proces " << id_procesa << " se zavrsio." << endl;
}

const int BROJ_PROCESA = 5;
const int BROJ_PRIORITETA = 3;

int main() {
	srand(time(NULL));
	//baza svih mogucih tiketa sadrzi tikete od 1 do 100
	//deo ovih tiketa ce biti dodeljen procesima u funkciji proces
	vector<int> baza_tiketa;
	for (int i = 0; i < 100; i++) {
        baza_tiketa.push_back(i+1);
	}

	Rasporedjivac r;
	thread procesi[BROJ_PROCESA];

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i] = thread(proces, ref(r), ref(baza_tiketa), i+1, rand() % 5 + 1);

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i].join();
}

