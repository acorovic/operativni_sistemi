/*
Napraviti program koji simulira prenos novca sa jednog bankovnog racuna na drugi.
Iznosi na racunima su predstavljeni datim nizom racuni.

Data funkcija transfer() predstavlja telo niti koje vrse prenos novca.
Funkcija 10 puta na slucajan nacin bira dva racuna i neki iznos novca i poziva funkciju prebaci() koja
skida novac sa prvog racuna i dodaje ga na drugi racun.
U funkciji prebaci(), nakon skidanja novca sa prvog racuna potrebno je jedna sekunda da
se novac uplati na drugi racun.
Povratna vrednost funkcije prebaci je struktura retVal koja sadrzi iznos na prvom racunu pre i
posle transakcije.

U glavnom programu potrebno je kreirati dve niti koje izvrsavaju funkciju transfer().
Ispisati ukupnu kolicinu novca na svim racunima u banci pre i posle transakcija.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;
using namespace chrono;

mutex m;
mutex term_m;
const int UKUPNO_RACUNA = 3;
double racuni[UKUPNO_RACUNA];


struct retVal{
    double pre;
    double posle;
};

retVal prebaci(int izvor, int cilj, int iznos) {
    retVal r;
    unique_lock<mutex> lock(m);
    r.pre = racuni[izvor];
    racuni[izvor] -= iznos;
    r.posle = racuni[izvor];
    this_thread::sleep_for(seconds(1));
    racuni[cilj] += iznos;
    return r;
}

void transfer() {
    for (int i = 0; i < 10; i++) {
        int izvor = rand()%UKUPNO_RACUNA;
        int cilj = (izvor+5)%UKUPNO_RACUNA;
        int iznos = rand()%10 + 1;
        retVal r = prebaci(izvor, cilj, iznos);
        unique_lock<mutex> lock(term_m);
        if ((r.pre - r.posle != iznos))
            cout << "Prebaceno " << iznos << " sa racuna " << izvor << " na racun "
                << cilj << ". Na izvoru bilo " << r.pre << ", a ostalo " << r.posle << endl;
    }
}

double ukupno() {
    double suma = 0;
    for(int i = 0; i < UKUPNO_RACUNA; i++)
        suma += racuni[i];

    return suma;
}

int main()
{
    for(int i = 0; i < UKUPNO_RACUNA; i++)
        racuni[i] = 10;
    cout << "suma u banci pre transakcija " << ukupno() << endl;
    thread t1(transfer);
    thread t2(transfer);
    t1.join();
    t2.join();
    cout << "suma u banci posle transakcija " << ukupno() << endl;
    return 0;
}
