/*
	Napraviti konkurentni program koji simulira izvrsavanje procesa u racunarskom sistemu sa
	jednim ulazno-izlaznim uredjajem.
	Procesi su predstavljeni nitima. Telo niti je data funkcija proces. Svaki proces izvrsava odredjeni broj
	naredbi pozivom funkcije izvrsi_proces.
	Naredbe se izvrsavaju jedna po jedna. Da bi proces izvrsio naredbu, mora da dobije procesor.
	Nakon izvrsavanja jedne naredbe, proces odlazi u cooldown period od 100 ms, pre nego sto ponovo
	pokusa da koristi procesor i izvrsi narednu naredbu.
	Izvrsavanje jedne naredbe traje 1 sekundu.

	Osim procesa, procesor koristi i ulazno-izlazni uredjaj predstavljen niti cije je telo data
	funkcija ui_uredjaj. Svakih nekoliko sekundi, uredjaj postavlja prekid i trazi
	njegovu obradu od procesora pozivom funkcije "prekini".
	Obrada prekida takodje mora da saceka da procesor postane raspoloziv.
	Ukoliko i procesi i uredjaj cekaju na procesor, prednost u dobijanju procesora ima uredjaj, jer je
	obrada prekida prioritet u odnosu na izvrsavanje naredbi procesa.
	Sama obrada prekida na procesoru traje 300 ms.

	Napomena: Komentari su obavezni.

	Napraviti test scenario sa:
		- 5 procesa koji izvrsavaju slucajan broj naredbi izmedju 1 i 10.
		- 1 ui uredjajem
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

class Procesor {
    private:
        bool zauzetProcesor;
        bool prekidCeka;
        mutex m;
        condition_variable cv_proces_ceka;
        condition_variable cv_prekid_ceka;
    public:
        Procesor() : zauzetProcesor(false), prekidCeka(false) {};
        void izvrsi_proces(int id, int broj_naredbi); //id - id procesa; broj_naredbi - ukupan broj naredbi koje ovaj proces izvrsava
        void prekini();
};

void Procesor::izvrsi_proces(int id, int broj_naredbi) {
    for(int i = 0; i < broj_naredbi; i++) {
        unique_lock<mutex> lock(m);
        while(zauzetProcesor)
            cv_proces_ceka.wait(lock);
        zauzetProcesor = true;
        cout << "Proces " << id << " zapocinje izvrsavanje naredbe " << i+1 << endl;
        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();
        zauzetProcesor = false;
        cout << "Proces " << id << " zavrsio izvrsavanje naredbe " << i+1 << endl;
        if(prekidCeka)
            cv_prekid_ceka.notify_one();
        else
            cv_proces_ceka.notify_one();
        lock.unlock();
        this_thread::sleep_for(milliseconds(100));
    }
}

void Procesor::prekini() {
    unique_lock<mutex> lock(m);
    prekidCeka = true;
    while(zauzetProcesor)
        cv_prekid_ceka.wait(lock);
    zauzetProcesor = true;
    lock.unlock();
    this_thread::sleep_for(milliseconds(300));
    lock.lock();
    zauzetProcesor = false;
    prekidCeka = false;
    cv_proces_ceka.notify_one();
}

mutex term_mx;

void proces(Procesor& p, int id, int br_naredbi) {
    {
        unique_lock<mutex> l(term_mx);
        cout << "Proces " << id << " krenuo sa izvrsavanjem svojih " << br_naredbi << " naredbi." << endl;
    }
    p.izvrsi_proces(id, br_naredbi);
}

void ui_uredjaj(Procesor& p) {
    while (true) {
        this_thread::sleep_for(seconds(rand()%5+1));
        cout << "UI uredjaj postavlja prekid." << endl;
        p.prekini();
        cout << "Procesor obradio prekid." << endl;

    }
}

const int brProcesa = 5;
const int maxBrNaredbi = 10;

int main() {
    srand(time(NULL));
    Procesor p;
    thread procesi[brProcesa];
    thread uiUredaj(ui_uredjaj,ref(p));
    uiUredaj.detach();
    for(int i = 0; i < brProcesa; i++)
        procesi[i] = thread(proces,ref(p), i+1, rand() % maxBrNaredbi + 1);
    for(int i = 0; i < brProcesa; i++)
        procesi[i].join();
    return 0;
}
