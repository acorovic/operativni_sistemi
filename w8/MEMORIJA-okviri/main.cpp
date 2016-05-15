/*
Napisati konkurentni program koji simulira jednostavno zauzimanje radne memorije od strane procesa.
Radna memorija je predstavljena datom klasom Radna_memorija i podeljena je na N okvira.
(N se prosledjuje pri instanciranju objekta klase Radna_memorija).
Okvir predstavlja skup memorijskih lokacija u koji se mogu smestiti podaci ili instrukcije procesa.

Procesi su predstavljeni nitima. Podaci i instrukcije procesa su podeljeni u stranice.
Stranice procesa su jednake velicine kao okviri radne memorije.
Da bi se proces izvrsio, mora sve svoje stranice da ucita u okvire radne memorije.
Nije obavezno da stranice jednog procesa budu smestene u susedne okvire radne memorije.
Ucitavanje stranica u okvire vrsi se u metodi "ucitaj" klase Radna_memorija.
Ukoliko u radnoj memoriji ne postoji dovoljan broj slobodnih okvira, proces ce sacekati dok se potreban broj okvira ne oslobodi.
Slobodnim okvirom se smatra okvir kojeg nijedan proces ne koristi.

Nakon izvrsavanja procesa, proces treba da oslobodi okvire koje je koristio, sto je potrebno implementirati
u metodi "oslobodi".

Metoda "ispisi_okvire_procesa" ispisuje na standardni izlaz redne brojeve svih okvira u koje je proces smestio svoje stranice.

Napomena: Komentari su obavezni.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

class Radna_memorija {
    private:
        int brojOkvira;
        int brojSlobodnihOkvira;
        int *okviri;
        mutex m;
        condition_variable slobodno;
	public:
		Radna_memorija(int ukupno_okvira);  //ukupan broj okvira u memoriji
		~Radna_memorija();
		void ucitaj(int broj_stranica, int id_procesa); //parametri su ukupan broj stranica koje proces sadrzi i identifikator procesa
		void oslobodi(int id_procesa); //identifikator procesa koji oslobadja memoriju
		void ispisi_okvire_procesa(int id_procesa); //ispisuje na standardni izlaz indekse okvira koje zauzima proces sa identifikatorom id_procesa
};

Radna_memorija::Radna_memorija(int ukupno_okvira) {
    brojOkvira = ukupno_okvira;
    brojSlobodnihOkvira = brojOkvira;
    okviri = new int[brojOkvira];
    for(int i = 0; i < brojOkvira; i++)
        okviri[i] = -1;
}

Radna_memorija::~Radna_memorija() {
    delete[] okviri;
}

void Radna_memorija::ucitaj(int broj_stranica, int id_procesa) {
    unique_lock<mutex> l(m);

    while(brojSlobodnihOkvira < broj_stranica)
        slobodno.wait(l);

    int brojZauzetihStranica = 0;
    for(int i = 0; i < brojOkvira; i++)
        if(okviri[i] == -1) {
            okviri[i] = id_procesa;
            brojSlobodnihOkvira--;
            if(++brojZauzetihStranica == broj_stranica)
                break;
        }
}

void Radna_memorija::oslobodi(int id_procesa) {
    unique_lock<mutex> l(m);

    for(int i = 0; i < brojOkvira; i++)
        if(okviri[i] == id_procesa) {
            okviri[i] = -1;
            ++brojSlobodnihOkvira;
        }

    slobodno.notify_all();
}

void Radna_memorija::ispisi_okvire_procesa(int id_procesa) {
    unique_lock<mutex> l(m);

    for(int i = 0; i < brojOkvira; i++)
        if(okviri[i] == id_procesa)
            cout << i << " ";
}

mutex term_mx;

void proces(Radna_memorija& mem, int id_procesa, int broj_stranica_procesa) {
	{
		unique_lock<mutex> l(term_mx);
		cout << "Proces " << id_procesa << ", koji ima " << broj_stranica_procesa
            << " stranica, pokusava da se ucita u radnu memoriju..." << endl;
	}
	mem.ucitaj(broj_stranica_procesa, id_procesa);
	{
		unique_lock<mutex> l(term_mx);
		cout << "Proces " << id_procesa << " se ucitao u radnu memoriju i krece sa izvrsavanjem."
			<< " Proces je zauzeo okvire: ";
			mem.ispisi_okvire_procesa(id_procesa);
			cout << endl;
	}
	this_thread::sleep_for(seconds(rand()%3+1));
	{
		unique_lock<mutex> l(term_mx);
		cout << "Proces " << id_procesa << " se zavrsio, suspenduje se..." << endl;
	}
	mem.oslobodi(id_procesa);
	unique_lock<mutex> l(term_mx);
	cout << "Proces " << id_procesa << " zamenjen iz memorije." << endl;
}

int BROJ_OKVIRA = 4;
int UKUPNO_PROCESA = 10;

int main() {
	Radna_memorija rm(BROJ_OKVIRA);

	thread procesi[UKUPNO_PROCESA];
	for (int i = 0; i < UKUPNO_PROCESA; ++i)
		procesi[i] = thread(proces, ref(rm), i + 1, rand() % BROJ_OKVIRA + 1);

	for (int i = 0; i < UKUPNO_PROCESA; ++i)
		procesi[i].join();
}
