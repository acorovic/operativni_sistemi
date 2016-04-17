/*
Modelovati prvo kolo teniskog turnira koje se odrzava u teniskom klubu sa n terena.
Svaki teren ima svoj broj. (Brojevi su od 1 do n).

Na terenima se igraju teniski mecevi prvog kola teniskog turnira. U prvom kolu ucestvuje X takmicara,
tako da ima M = X/2 teniska meca. Za svaki mec u startu se definise na kojem terenu ce biti odigran.
Raspored meceva po terenima pravi se tako da se mecevi ravnomerno rasporede po terenima.
Znaci, na svakom terenu se u proseku igraju n/M meca. Svaki mec ima svoj identifikator (broj).

Svaki mec traje slucajan vremenski period izmedju 1 i 5 sekundi. Naredni mec na terenu ne moze da pocne dok
se prethodni mec na tom terenu ne zavrsi.

Za svaki mec potrebno je evidentirati kada su takmicari dosli na teren, kada je mec poceo i koliko je mec trajao.

*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace chrono;

const int brTerena = 3;
const int brTakmicara = 20;
const int brMeceva = brTakmicara/2;

struct podaci {
    int broj_meca;
    duration<double, milli> trajanje;
    system_clock::time_point dosao;
    system_clock::time_point pocetak;
};

class Teniski_klub {
    private:
        int brojTerena;
        mutex m;
        condition_variable *slobodanTeren;
        bool *slobodniTereni;
    public:
        Teniski_klub(int ukupno_terena) : brojTerena(ukupno_terena) {
            slobodanTeren = new condition_variable[brojTerena];
            slobodniTereni = new bool[brojTerena];
            for(int i = 0; i < brojTerena; i++)
                slobodniTereni[i] = true;
        };
        ~Teniski_klub() {
            delete []slobodanTeren;
            delete []slobodniTereni;
        }
        podaci odigraj_mec(int, int);
};

podaci Teniski_klub::odigraj_mec(int broj_meca, int na_terenu) {
    podaci pov;
    pov.broj_meca = broj_meca;
    pov.dosao = system_clock::now();
    {
        unique_lock<mutex> lock(m);
        while(slobodniTereni[na_terenu] == false)
            slobodanTeren[na_terenu].wait(lock);
        pov.pocetak = system_clock::now();
        slobodniTereni[na_terenu] = false;
    }
    this_thread::sleep_for(seconds(rand()%5+1));
    pov.trajanje = system_clock::now() - pov.pocetak;
    {
        unique_lock<mutex> lock(m);
        slobodniTereni[na_terenu] = true;
        slobodanTeren[na_terenu].notify_one();
    }
    return pov;
}

mutex term_mx;

void mec(Teniski_klub& tk, int broj_meca, int na_terenu) {
    podaci v = tk.odigraj_mec(broj_meca, na_terenu);

    duration<double, milli> cekao = v.pocetak - v.dosao;
    unique_lock<mutex> l(term_mx);
    cout << "Mec " << v.broj_meca + 1 << " odigran na terenu " << na_terenu + 1 << " trajao " << v.trajanje.count() << " milisekundi. Takmicari su na pocetak meca cekali " << cekao.count()
        << " milisekundi. " << endl;
}


int main()
{
    Teniski_klub tk(brTerena);
    thread niti[brMeceva];
    for(int i = 0; i < brMeceva; i++)
        niti[i] = thread(mec, ref(tk), i, i%3);
    for(int i = 0; i < brMeceva; i++)
        niti[i].join();
    return 0;
}
