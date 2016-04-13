/*
Napraviti konkurentni program koji modeluje klasu brojaÄa. Interfejs klase sadrÅ¾i sledeÄ‡e metode:

class brojac {
   public:
      void inc();
      void dec();
      friend ostream& operator<<(ostream& , brojac& );
};

Metode inc i dec poveÄ‡avaÄ‡u i smanjuju vrednost brojaÄa respektivno. Operator << sluÅ¾i za ispis brojaÄa na ekran.

Klasa treba da zaÅ¡titi konzistentnost brojaÄa u datim metodama uz pomoÄ‡ klasa mutex i unique_lock.

Kreirati 1 globalni objekat brojaÄa kome Ä‡e pristupati 2 niti.

Kreirati 2 niti pri Äemu jedna nit poziva metodu uveÄ‡avanja brojaÄa 1000000 puta
a druga metodu smanjivanja brojaÄa 1000000 puta. Na kraju programa ispisati konaÄnu vrednost
brojaÄa nakon uveÄ‡avanja i smanjivanja.
*/

#include <iostream>
#include <thread>
#include <mutex>


using namespace std;

const int ITERATIONS = 10000;

class brojac {
    private:
        int broj;
        mutex m;
    public:
        brojac() {
            broj = 0;
        }
        void inc();
        void dec();
        friend ostream& operator<<(ostream& , brojac&);
        };

ostream& operator<<(ostream& out , brojac& br) {
            out << br.broj << endl;
            return out;
        }

void brojac::inc() {
    unique_lock<mutex> lock(m);
    broj++;
}

void brojac::dec() {
    unique_lock<mutex> lock(m);
    broj--;
}

void inkrement(brojac &a) {
    for(int i = 0; i < ITERATIONS; i++)
        a.inc();
}

void dekrement(brojac &a) {
    for(int i = 0; i < ITERATIONS; i++)
        a.dec();
}

int main()
{
    brojac br;
    thread nit1(inkrement, ref(br));
    thread nit2(dekrement, ref(br));
    nit1.join();
    nit2.join();
    cout << br << endl;
    return 0;
}
