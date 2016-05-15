/*
Napisati konkurentni program koji modeluje pristupanje vise procesa deljenoj memoriji.

Deljena memorija je predstavljena klasom Memorija.
Prilikom instanciranja se zadaje velicina memorije.
Upis vrednosti u memoriju se obavlja pozivom operacije pisi(),
a citanje pozivom operacije citaj().

I citanje i pisanje svake vrednosti traju po 1 sekundu.

Svaka adresa u memoriji adresira lokaciju velicine jednog bajta.
Tokom pisanja u memorijsku lokaciju ekskluzivno se zakljucava
cela rec kojoj ta lokacija (bajt) pripada.

Tokom citanja memorijske lokacije rec kojoj ta lokacija (bajt) pripada
se zakljucava, tako da i drugi procesori koji zele da CITAJU mem. lokaciju
iz te reci to mogu istovremeno da urade.

Treba omoguciti istovremeno pristupanje memorijskim lokacijama koje ne pripadaju istoj reci.

Ako istovremeno postoje i procesi koji zeli da citaju i procesi koji zeli da pisu u istu rec,
treba prvo pristup dozvoliti procesima koji zele da citaju.

Stvoriti nekoliko niti koje pokazuju da program radi ispravno.

Dat je primer scenarija pristupanja, zarad lakseg testiranja.

Napomena: Komentari su obavezni.
*/

#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

class Memorija {
public:
    enum Stanje {SLOBODAN, CITANJE, PISANJE };
    Memorija(int bajtova);
    char citaj(int adresa);
    void pisi(int adresa, char vrednost);
    ~Memorija();
private:
    const int VELICINA_RECI = 4;
    mutex m;
    vector<char> mem;
    struct Pristup{
        Stanje stanje;
        condition_variable citanje;
        condition_variable pisanje;
        int aktivnih_citaca;
        int citaca_ceka;

        Pristup() : stanje(SLOBODAN), aktivnih_citaca(0), citaca_ceka(0) {}
    };
    vector<Pristup*> pristupi;
};

Memorija::Memorija(int bajtova) : mem(bajtova) {
    int brReci = (bajtova + VELICINA_RECI-1) / VELICINA_RECI;
    for(int i = 0; i < brReci; i++)
        pristupi.push_back(new Pristup);
}

Memorija::~Memorija() {
    for(auto it = pristupi.begin(); it != pristupi.end(); it++)
        delete *it;
}

char Memorija::citaj(int adresa) {
    int rec = adresa / VELICINA_RECI;
    Pristup* pristupReci = pristupi[rec];
    unique_lock<mutex> lock(m);

    while(pristupReci->stanje == PISANJE) {
        (pristupReci->citaca_ceka)++;
        pristupReci->citanje.wait(lock);
        (pristupReci->citaca_ceka)--;
    }

    (pristupReci->aktivnih_citaca)++;
    pristupReci->stanje == CITANJE;

    lock.unlock();
    this_thread::sleep_for(seconds(1));
    lock.lock();

    (pristupReci->aktivnih_citaca)--;
    if(pristupReci->aktivnih_citaca == 0) {
        pristupReci->stanje == SLOBODAN;
        pristupReci->pisanje.notify_one();
    }

    return mem[adresa];
}

void Memorija::pisi(int adresa, char vrednost) {
    int rec = adresa / VELICINA_RECI;
    Pristup* pristupReci = pristupi[rec];
    unique_lock<mutex> lock(m);

    while(pristupReci->stanje != SLOBODAN)
        pristupReci->pisanje.wait(lock);

    pristupReci->stanje = PISANJE;

    lock.unlock();
    this_thread::sleep_for(seconds(1));
    lock.lock();

    mem[adresa] = vrednost;
    pristupReci->stanje = SLOBODAN;

    if(pristupReci->citaca_ceka != 0)
        pristupReci->citanje.notify_all();
    else
        pristupReci->pisanje.notify_one();
}

struct Pristup_memoriji {                       //Strukture koje ce se koristiti radi formiranja testnog scenarija upisa ili
    Memorija::Stanje tip_pristupa;               //citanja iz memorije. Zadaje se i adresa kao i vrednost. Fakticki naredba.
    int adresa;
    char vrednost;
    Pristup_memoriji(Memorija::Stanje tip, int adr, char v = 0) : tip_pristupa(tip), adresa(adr), vrednost(v) {}
};

mutex term_m;

void proces(Memorija& mem, vector<Pristup_memoriji>& pristupi, int rbp) {

    for(auto it = pristupi.begin(); it != pristupi.end(); it++) {
        if(it->tip_pristupa == Memorija::CITANJE) {
            it->vrednost = mem.citaj(it->adresa);
            unique_lock<mutex> l(term_m);
            cout << "Proces " << rbp
                 << " procitao sa adrese " << it->adresa
                 << " vrednost = " << it->vrednost << endl;
        } else {
            mem.pisi(it->adresa, it->vrednost);
            unique_lock<mutex> l(term_m);
            cout << "Proces " << rbp
                 << " zapisao na adresu " << it->adresa
                 << " vrednost = " << it->vrednost << endl;
        }
    }
}

int main() {
    Memorija mem(10);
    vector<Pristup_memoriji> I  = {
        {Memorija::PISANJE, 0, '0'},
        {Memorija::PISANJE, 4, '4'},
        {Memorija::CITANJE, 0},
        {Memorija::PISANJE, 8, '8'}
    };
    vector<Pristup_memoriji> II = {
        {Memorija::PISANJE, 5, '5'},
        {Memorija::PISANJE, 1, '1'},
        {Memorija::CITANJE, 1},
        {Memorija::PISANJE, 9, '9'}
    };
    vector<Pristup_memoriji> III = {
        {Memorija::PISANJE, 8, '8'},
        {Memorija::PISANJE, 9, '9'},
        {Memorija::PISANJE, 2, '2'},
    };

    thread P1(proces, ref(mem), ref(I), 1);
    thread P2(proces, ref(mem), ref(II), 2);
    thread P3(proces, ref(mem), ref(III), 3);
    P1.join();
    P2.join();
    P3.join();
}
