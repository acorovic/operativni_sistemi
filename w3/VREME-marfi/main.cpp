/*
    Napraviti program koji ispituje ispravnost jednog od Marfijevih zakona, koji glasi:
    "Vas red je uvek najsporiji.".

    Simulirati placanje robe na tri kase. Rad svake kase predstavljen je jednom niti.
    Svaka nit dobija parametar koji predstavlja broj kupaca koje kasa treba da opsluzi.
    Broj kupaca je slucajan broj izmedju 1 i 30.
    Opsluzivanje svakog kupca simulirati uspavljivanjem niti na slucajan vremenski period izmedju 1 i 100 ms.

    Izracunati trajanje kupovine na svakoj od kasa.

    Na pocetku programa potrebno je pitati korisnika da unese redni broj kase na
    kojoj zeli da izvrsi kupovinu.
    Na kraju ispisati da li je Marfijev zakon potvrdjen.
    Ukoliko je kupovina na kasi koju je kupac izabrao trajala najduze, tada je Marfijev zakon potvrdjen.
*/

#include <iostream>
#include <thread>
#include <cmath>
#include <vector>

using namespace std;
using namespace chrono;

const int brKasa = 3;
const int MAX_MILLISECONDS = 100;
const int MAX_KUPACA = 30;

void f(int brKupaca, duration<double> &trajanje) {
    system_clock::time_point pocetak = system_clock::now();
    for(int i = 0; i < brKupaca; i++)
        this_thread::sleep_for(milliseconds(rand()%MAX_MILLISECONDS + 1));
    system_clock::time_point kraj = system_clock::now();
    trajanje = kraj - pocetak;
}

int main()
{
     srand(time(NULL));
    int n;
    do{
        cout << "unesite redni broj kase koju zelite, postoje 3" << endl;
        cin >> n;
    } while (n < 0 || n > 3);
    thread niti[brKasa];
    vector<duration<double>> trajanja(brKasa);
    for(int i = 0; i < brKasa; i++)
        niti[i] = thread(f, rand()%MAX_KUPACA + 1 ,ref(trajanja[i]));
    for(int i = 0; i < brKasa; i++)
        niti[i].join();

    for(int i = 0; i < brKasa; i++)
        cout << "nit " << i << " trajala " << trajanja[i].count() << endl;
    int najduze = 0;
    for(int i = 1; i < brKasa; i++)
        if(trajanja[i] > trajanja[najduze])
            najduze = i;
    if(najduze + 1 == n)
        cout << "Marfijev zakon je potvrdjen" << endl;
    else
        cout << "Marfijev zakon nije potvrdjen" << endl;

    return 0;
}
