// Parking sa 3 ulaza i jednim mestom.
// Na parking automobili ulaze sa ulaza 0, 1 i 2 po round-robin protokolu
// (jedan udje sa prvog, jedan sa drugog, jedan sa treceg i tako u krug).

// Automobilu se pri stvaranju prosledjuje vreme (u sekundama) koje se on
// zadrzave na parkingu.
// U programu uvek ima jednak broj automobila na svim ulazima!

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace chrono;

const int brUlaza = 3;
const int vremeZadrzavanja = 5;
const int brAutomobila = 10;
mutex term_m;

class Parking {
    private:
        mutex m;
        condition_variable ulazi[brUlaza];
        bool slobodno;
        int aktivanUlaz;
    public:
        Parking() : aktivanUlaz(0), slobodno(true) {};
        void udji(int ulaz);
        void izadji();
};

void Parking::udji(int ulaz) {
    unique_lock<mutex> l(m);
    while(!slobodno || (aktivanUlaz != ulaz))
        ulazi[ulaz].wait(l);
    slobodno = false;
    aktivanUlaz = (aktivanUlaz+1)%3;
}

void Parking::izadji() {
    unique_lock<mutex> l(m);
    slobodno = true;
    ulazi[aktivanUlaz].notify_one();
}

void automobil(Parking& p, int ulaz, int ostajem_na_parkingu, int rba) {
    {
        unique_lock<mutex> l(term_m);
        cout << "Automobil "<< rba <<" pokusava da udje na parking na ulaz "<< ulaz <<endl;
    }
    p.udji(ulaz);
    {
        unique_lock<mutex> l(term_m);
        cout << "Automobil "<< rba <<" usao na parking na ulaz " << ulaz << endl;
    }
    this_thread::sleep_for(seconds(ostajem_na_parkingu));
    p.izadji();
    unique_lock<mutex> l(term_m);
    cout << "Automobil "<< rba << " izasao sa parkinga." << endl;
}

int main()
{
    Parking p;
    thread niti[brAutomobila];
    for(int i = 0; i < brAutomobila; i++)
        niti[i] = thread(automobil, ref(p), i%3, rand()%vremeZadrzavanja + 1, i+1);
    for(int i = 0; i < brAutomobila; i++)
        niti[i].join();
    return 0;
}
