/*
Napraviti konkurentni program koji simulira dinamicku dodelu memorijskog prostora procesima.
Memorija je podeljena na memorijske lokacije.
Procesu je potreban odredjeni broj slobodnih UZASTOPNIH memorijskih lokacija da bi se smestio u memoriju.
Proces zauzima prvi slobodan blok memorije koji pronadje.
Kada proces zavrsi sa radom, oslobadjaju se lokacije koje je koristio.

U opisanom scenariju, kako procesi zavrsavaju sa radom, dolazi do pojave "rupa" u memoriji (slobodni blokovi memorije).
Moze se desiti da ukupan broj slobodnih memorijskih lokacija bude dovoljan da se novi proces smesti,
ali da se te slobodne lokacije ne nalaze u sukcesivnim delovima memorije.
Zato je potrebno izvrsiti SAZIMANJE memorije. Pri sazimanju memorije, trenutni procesi u memoriji se pomeraju
tako da zauzimaju uzastopne lokacije pocevsi od prve. Na taj nacin, na kraju memorije ostaju sve
slobodne lokacije kao jedan blok uzastopnih lokacija.

Memorija je predstavljena datom klasom Radna_memorija. Pri instanciranju se prosledjuje broj lokacija u memoriji.
Procesi su predstavljeni nitima. Telo niti je funkcija proces.
Svaki proces dobija kao parametar broj lokacija koje su mu potrebne da se smesti u memoriju.
Procesi zauzimaju memoriju pozivom operacije "koristi" klase Radna_memorija.
Ukoliko ne postoji dovoljno uzastopnih slobodnih memorijskih lokacija koliko procesu treba,
proces mora da ceka.
Kada ima dovoljno lokacija, proces zauzima lokacije. Nakon toga se izvrsava.
Izvrsavanje simulirati uspavljivanjem niti na broj sekundi definisan parametrom "trajanje".
Nakon toga, proces oslobadja lokacije koje je koristio i trazi sazimanje memorije.

Sazimanje memorije vrsi posebna nit predstavljena funkcijom os.
Samo sazimanje vrsi se u metodi "sazimanje" klase radna memorija.
U ovoj metodi, mehanizam zaduzen za sazimanje ceka dok neki proces
ne zatrazi sazimanje.
Kada se sazimanje zatrazi, vrsi se sazimanje memorije na ranije opisani nacin.
Nakon sazimanja, moguce je da u memoriji postoji veci slobodan blok nego ranije,
pa se neki od procesa koji su cekali na memoriju sada moze smestiti.

U okviru sazimanja NIJE dozvoljeno vrsiti sortiranje memorijskih lokacija kao skraceni postupak
za izbacivanje slobodnih lokacija na kraj.

Npr. ukoliko je izgled memorije pre sazimanja:

	-1 -1 3 3 3 1 1 -1 -1 2 2

	Nakon sazimanja izgled memorije treba da bude:

	3 3 3 1 1 2 2 -1 -1 -1 -1

	a NE:

	1 1 2 2 3 3 3 -1 -1 -1 -1 ili 3 3 3 2 2 1 1 -1 -1 -1

Napomena: Komentari su obavezni.
*/


#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;
using namespace chrono;

class Radna_memorija {
    private:
        vector<int> lokacije;
        bool aktiviranoSazimanje;
        mutex m;
        condition_variable cv_sazimanje,cv_slobodan;
        int pocetak_bloka(int broj_lokacija_procesa);
	public:
		Radna_memorija(int ukupno_lokacija);
		void koristi(int id_procesa, int br_lokacija_procesa, int trajanje);
		void sazimanje();
		void ispisiLokacije();
};


void Radna_memorija::sazimanje() {
    unique_lock<mutex> l(m);
    while(!aktiviranoSazimanje)
        cv_sazimanje.wait(l);

    aktiviranoSazimanje = false;
    cout << "sazimanje aktivirano" << endl;

    vector<int> kopijaLokacija;
    for(int i = 0; i != lokacije.size(); i++)
        kopijaLokacija.push_back(-1);

    int i = 0;
    for(auto it = lokacije.begin(); it != lokacije.end(); it++)
        if(*it != -1)
            kopijaLokacija[i++] = *it;

    lokacije = kopijaLokacija;
    ispisiLokacije();
    cv_slobodan.notify_all();
}


void Radna_memorija::koristi(int id_procesa, int br_lokacija_procesa, int trajanje) {
    unique_lock<mutex> l(m);

    while(pocetak_bloka(br_lokacija_procesa) == -1) {
        cout << "Proces " << id_procesa << " ceka da se oslobode lokacije" << endl;
        cv_slobodan.wait(l);
        }

    int iteratorLokacija = pocetak_bloka(br_lokacija_procesa);
    for(int i = 0; i < br_lokacija_procesa; i++)
        lokacije[iteratorLokacija++] = id_procesa;

    cout << "Proces " << id_procesa << " zauzeo lokacije " << endl;
    ispisiLokacije();

    l.unlock();
    this_thread::sleep_for(seconds(trajanje));
    l.lock();

    cout << "Proces " << id_procesa << " se izvrsio" << endl;

    for(auto it = lokacije.begin(); it != lokacije.end(); it++)
        if(*it == id_procesa)
            *it = -1;

    cout << "Proces " << id_procesa << " oslobodio memoriju" << endl;
    ispisiLokacije();

    aktiviranoSazimanje = true;
    cv_sazimanje.notify_one();
}


void Radna_memorija::ispisiLokacije() {
    for(auto it = lokacije.begin(); it != lokacije.end(); it++)
        cout << *it << " ";
    cout << endl;
}


int Radna_memorija::pocetak_bloka(int broj_lokacija_procesa) {
    int pronadjenoLokacija = 0;
    int i = 0;
    for(auto it = lokacije.begin(); it != lokacije.end(); it++, i++)
        if(*it == -1) {
            pronadjenoLokacija++;
            if(pronadjenoLokacija == broj_lokacija_procesa)
                return i-pronadjenoLokacija+1;
        } else
            pronadjenoLokacija = 0;
    return -1;
}


Radna_memorija::Radna_memorija(int ukupno_lokacija) {
    for(int i = 0; i < ukupno_lokacija; i++)
        lokacije.push_back(-1);
    aktiviranoSazimanje = false;
}


void os(Radna_memorija& rm) {
	while (true) {
        rm.sazimanje();
	}
}

mutex term_m;


void proces(Radna_memorija& rm, int id_procesa, int br_lokacija_procesa, int trajanje, int kasnjenje) {
    this_thread::sleep_for(seconds(kasnjenje));
    {
        unique_lock<mutex> l(term_m);
        cout << "Proces " << id_procesa << " sa " << br_lokacija_procesa << " stranica krenuo sa radom." << endl;
    }
	rm.koristi(id_procesa, br_lokacija_procesa, trajanje);
}

const int BROJ_PROCESA = 5;
const int UKUPNO_OKVIRA = 54;

//scenario je napravljen tako da proces 5 ne bi mogao da se ubaci u memoriju da ne postoji sazimanje
//memorijskih lokacija
int main() {
	Radna_memorija rm(UKUPNO_OKVIRA);

	int velicine[] = {20, 14, 18, 8, 16};
	int trajanje[] = {5, 2, 5, 4, 4};
	int kasnjenje[] = {0, 1, 2, 4, 5};

    thread os_thread(os, ref(rm));
	thread procesi[BROJ_PROCESA];

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i] = thread(proces, ref(rm), i+1, velicine[i], trajanje[i], kasnjenje[i]);

	for (int i = 0; i < BROJ_PROCESA; i++)
		procesi[i].join();

    os_thread.detach();
    exit(0);   //exit 0 - predstavlja signal detach-ovanoj niti da prekine rad jer vise nije neophodna
}
