/*Modelovati koriscenje racunara u racunarskoj ucionici. Broj racunara u ucionici se prosledjuje pri inicijalizaciji.
Student pozivom operacije zauzmi() dolazi u ucionicu i zauzima prvi slobodan racunar, ukoliko takav racunar postoji.
Ukoliko su svi racunari zauzeti, student mora sacekati da se neki racunar oslobodi.
Operacija zauzmi() vraca redni broj racunara koji je student zauzeo.
Student koristi racunar neki slucajan broj sekundi.
Operacijom oslobodi(), student zavrsava rad u ucionici. Parametar metode je redni broj racunara koji student koristi i koji
treba da se oslobi.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <cmath>

using namespace std;
using namespace chrono;

#define MAXRACUNARA 50
#define BRUCENIKA 10

class RC {
    private:
        bool racunari[MAXRACUNARA];
        int brRacunara;
        int brSlobodnih;
        mutex m;
        condition_variable slobodan;
	public:
		RC(int br) {
            brRacunara = br;
            brSlobodnih = br;
            for(int i = 0; i < brRacunara; i++)
                racunari[i] = true;
		}
		int zauzmi();
		void oslobodi(int id_racunara);
};

int nadjiSlobodan(int brRacunara, bool *niz) {
    int indeks = 0;
    for(int i = 0; i < brRacunara; i++)
        if(niz[i]) {
            indeks = i;
            return indeks;
        }
    return indeks;
}

int RC::zauzmi() {
    unique_lock<mutex> lock(m);
    while(brSlobodnih == 0)
        slobodan.wait(lock);
    int indeks = nadjiSlobodan(brRacunara, racunari);
    --brSlobodnih;
    racunari[indeks] = false;
    return indeks;
}

void RC::oslobodi(int id_racunara) {
    unique_lock<mutex> lock(m);
    ++brSlobodnih;
    racunari[id_racunara] = true;
    slobodan.notify_one();
}
mutex term_mx;

void student(RC &rc, int br_indeksa) {
    {
        unique_lock<mutex> l(term_mx);
        cout << "Student " << br_indeksa << " zeli da koristi ucionicu. " << endl;
    }
    int id_racunara  = rc.zauzmi();
    {
        unique_lock<mutex> l(term_mx);
        cout << "Student " << br_indeksa << " seo za racunar " << id_racunara << endl;
    }
    this_thread::sleep_for(chrono::seconds(rand()%5 + 1));
    rc.oslobodi(id_racunara);
    {
        unique_lock<mutex> l(term_mx);
        cout << "Student " << br_indeksa << " zavrsio rad u ucionici." << endl;
    }
}

const int brRacunaraUcionica = 3;

int main()
{
    RC ucionica(brRacunaraUcionica);
    thread niti[BRUCENIKA];
    for(int i = 0; i < BRUCENIKA; i++)
        niti[i] = thread(student, ref(ucionica), i+1);
    for(int i = 0; i < BRUCENIKA; i++)
        niti[i].join();

    return 0;
}
