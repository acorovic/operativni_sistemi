// Modelovati Skladiste koje ima dve identicne rampe za istovar robe iz kamiona.

// Nosivost kamiona je maksimalno 7 tona.
// Kamioni nose obicnu ili zapaljivu robu.
// Kamioni sa zapaljivom robom imaju prednost pri istovaru.

// Kamion koji zeli da ostavi robu u skladistu poziva operaciju istovari().
// Kamion ceka ispred skadista dok jedna od rampi ne postane slobodna.
// Istovar traje onoliko sekundi koliko u kamionu ima tona robe.
// Operacija istovar() vraca pozivaocu informaciju o tome na kojoj rampi je
// kamion istovaren.

// Stvoriti 5 kamiona sa obicnom i 5 sa zapaljivom robom.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace chrono;

const int brRampi = 2;

class Skladiste {
    private:
        bool zauzeteRampe[brRampi];
        mutex m;
        int brZapaljivihCeka;
        int brObicnihCeka;
        condition_variable zapaljivi;
        condition_variable obicni;
public:
   Skladiste();
   int istovari(int kolicina, bool zapaljivo);
};

Skladiste::Skladiste() {
    brZapaljivihCeka = 0;
    brObicnihCeka = 0;
    zauzeteRampe[0] = false;
    zauzeteRampe[1] = false;
}

int Skladiste::istovari(int kolicina, bool zapaljivo) {
    unique_lock<mutex> lock(m);
    while(zauzeteRampe[0] && zauzeteRampe[1]) {
        if(zapaljivo) {
            brZapaljivihCeka++;
            zapaljivi.wait(lock);
             brZapaljivihCeka--;
        } else {
            brObicnihCeka++;
            obicni.wait(lock);
            brObicnihCeka--;
        }
    }
    int rampa = (zauzeteRampe[0] ? 1 : 0);
    zauzeteRampe[rampa] = true;
    lock.unlock();
    this_thread::sleep_for(seconds(kolicina));
    lock.lock();
    zauzeteRampe[rampa] = false;
    if(brZapaljivihCeka > 0)
        zapaljivi.notify_one();
    else
        obicni.notify_one();
    return rampa;

}

mutex term_m;

void kamion(Skladiste& s, int kolicina, bool zapaljivo, int rbk) {
   {
      unique_lock<mutex> l(term_m);
      cout << "Kamion broj: " << rbk << " nosi "
           << kolicina << " tona ";
      if(zapaljivo) cout << "zapaljive robe" << endl;
      else          cout << "obicne robe" << endl;
   }
   int rampa = s.istovari(kolicina, zapaljivo);
   {
      unique_lock<mutex> l(term_m);
      cout << "Kamion broj: " << rbk
           << " istovaren na rampi " << rampa << " (nosio " << kolicina;
     if(zapaljivo) cout << "; zapaljivo)." << endl;
     else          cout << "; obicno)." << endl;
   }
}

const int brZapaljivih = 5;
const int brObicnih = 5;
const int maxNosivost = 7;

int main()
{
    Skladiste s;
    thread niti[brZapaljivih + brObicnih];
    for(int i = 0; i < brZapaljivih + brObicnih; i++)
        niti[i] = thread(kamion, ref(s), rand()%maxNosivost + 1, i%2, i+1);
    for(int i = 0; i < brZapaljivih + brObicnih; i++)
        niti[i].join();
    return 0;
}
