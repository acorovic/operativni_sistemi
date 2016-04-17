// Modelovati salter salu u posti u kojoj postoje 2 saltera.

// Vremensko trajanje uplate (boravak klijenta na salteru) je srazmerno
// velicini uplate. Za svaku uplacenu hiljadu dinara klijent ceka 1 sec.
// Na salteru se moze uplatiti maksimalno 4 hiljada dinara.
// (podrazumeva se da je ispravna vrednost prosledjena klijentu
// pri stvaranju niti).

// Kada klijent zeli da uplati sredstava, on poziva operaciju uplati(),
// cime prakticno ulazi u postu i staje u red.
// Povratna vrednost ove operacije je broj saltera na kojem je klijent
// izvrsio uplatu i svota koja je do tog trenutka na salteru uplacena.

// Treba stvoriti 7 klijenata.

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <cmath>

using namespace std;
using namespace chrono;

const int brSaltera = 2;
const int brKlijenata = 7;
const int maxUplata = 4;
mutex term_m;

struct Povratna_vrednost {
   int salter;
   int uplaceno;
   Povratna_vrednost(int s, int u) : salter{s}, uplaceno{u} {}
};

class Posta {
    private:
        mutex m;
        int uplate[brSaltera];
        bool slobodan[brSaltera];
        condition_variable slobodanSalter;
    public:
        Posta() {
            for(int i = 0; i < brSaltera; i++)
                uplate[i] = 0;
            for(int i = 0; i < brSaltera; i++)
                slobodan[i] = true;
        };
        Povratna_vrednost uplati(int svota);
};

Povratna_vrednost Posta::uplati(int svota) {
    unique_lock<mutex> lock(m);
    Povratna_vrednost pov(0,0);
    while(!(slobodan[0] || slobodan[1]))
        slobodanSalter.wait(lock);
    pov.salter = (slobodan[0] ? 0 : 1);
    slobodan[pov.salter] = false;
    lock.unlock();
    this_thread::sleep_for(seconds(svota));
    lock.lock();
    uplate[pov.salter] += svota;
    pov.uplaceno = uplate[pov.salter];
    slobodan[pov.salter] = true;
    slobodanSalter.notify_one();
    return pov;
}

void klijent(Posta& p, int svota, int rbk) {
   static mutex term_m;
   {
      unique_lock<mutex> l(term_m);
      cout << "Klijent broj: " << rbk
           << " zeli da uplati " << svota
           << " hiljada dinara." << endl;
   }
   auto ret = p.uplati(svota);
   {
      unique_lock<mutex> l(term_m);
      cout << "Klijent broj: " << rbk << " ("
           << svota << ") salter " << ret.salter
           << " (" << ret.uplaceno << ")"<< endl;
   }
}

int main()
{
    srand(time(NULL));
    Posta p;
    thread niti[brKlijenata];
    for(int i = 0; i < brKlijenata; i++)
        niti[i] = thread(klijent, ref(p), rand()%maxUplata + 1, i+1);
    for(int i = 0; i < brKlijenata; i++)
        niti[i].join();
    return 0;
}
