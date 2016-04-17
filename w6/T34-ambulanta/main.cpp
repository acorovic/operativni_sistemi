/*Napraviti konkurentni program koji modeluje rad studentske ambulante pri sistematskom pregledu.
	Ambulanta ima operacije udji i izadji.
	Studenti i studentkinje pozivaju operaciju udji prilikom ulaska u ambulantu i izadji prilikom izlaska iz nje.
	Svaki student se u ambulanti zadrzava do 3 sekunde.

Ambulanta prime studente i studentkinje u odvojene prostorije.
Muska prostorija prima najvise 1 studenta a zenska 2 studentkinje.
Ukoliko je kapacitet prostorije popunjen studenti cekaju na slobodna mesta u datoj prostoriji.

Napomena: Indeks studenta je redni broj niti student pri stvaranju. Stvoriti 3 muska i 5 zenskih studenata.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

mutex term_m;

const int brMestaM = 1;
const int brMestaZ = 2;

enum pol {muski, zenski};

class Ambulanta{
    private:
        mutex m;
        condition_variable slobodnoMesto[2];
        int brMesta[2];
    public:
        Ambulanta() {
            brMesta[0] = brMestaM;
            brMesta[1] = brMestaZ;
        }
        void udji(pol);
        void izadji(pol);
};

void Ambulanta::udji(pol oznaka) {
    unique_lock<mutex> lock(m);
    while(brMesta[oznaka] == 0)
        slobodnoMesto[oznaka].wait(lock);
    --brMesta[oznaka];
}

void Ambulanta::izadji(pol oznaka) {
    unique_lock<mutex> lock(m);
    ++brMesta[oznaka];
    slobodnoMesto[oznaka].notify_one();
}

string oznakaPola(pol oznaka) {
    return oznaka ? "zenski" : "muski";
}

void student(Ambulanta &a, pol oznaka, int rbr) {
    {
        unique_lock<mutex> terminal(term_m);
        cout << "ucenik " << rbr << " pola " << oznakaPola(oznaka) << " pokusava da udje" << endl;
    }
    a.udji(oznaka);
    {
        unique_lock<mutex> terminal(term_m);
        cout << "ucenik " << rbr << " pola " << oznakaPola(oznaka) << " je usao" << endl;
    }
    this_thread::sleep_for(seconds(3));
     {
        unique_lock<mutex> terminal(term_m);
        a.izadji(oznaka);
        cout << "ucenik " << rbr << " pola " << oznakaPola(oznaka) << " je izasao" << endl;
    }

}

const int brMuskih = 3;
const int brZenskih = 5;

int main()
{
    Ambulanta sa;
   thread t[brMuskih+brZenskih];

   for(int i = 0; i < brMuskih; i++)
      t[i] = thread(student, ref(sa), muski, i);

   for(int i = brMuskih; i < brMuskih + brZenskih; i++)
      t[i] = thread(student, ref(sa), zenski, i);

   for(int i = 0; i < brMuskih + brZenskih; i++)
      t[i].join();
    return 0;
}
