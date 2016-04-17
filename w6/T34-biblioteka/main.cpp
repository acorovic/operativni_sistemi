/*
	Modelovati iznajmljivanje jedne knjige u biblioteci. Biblioteka poseduje N primeraka ove knjige.
	Ovaj parametar se prosledjuje pri inicijaliziciji biblioteke.
	Clan iznajmljuje primerak pozivom metode iznajmi().
	Ukoliko su svi primerci trenutno na citanju, clan biblioteke mora da saceka da neki primerak bude vracen u biblioteku.
	Primerak moze da bude kod clana na citanju neki slucajan vremenski period, koji nije duzi od 4 sekunde.
	Nakon citanja, clan vraca primerak u biblioteku pozivom metode vrati().
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

using namespace std;
using namespace chrono;

const int vremeCitanja = 4;
mutex term_m;

class Biblioteka{
    private:
        int brPrimeraka;
        mutex m;
        condition_variable slobodno;
    public:
        Biblioteka(int n) : brPrimeraka(n) {}
        void iznajmi();
        void vrati();
};

void Biblioteka::iznajmi() {
    unique_lock<mutex> l(m);
    while(brPrimeraka == 0)
        slobodno.wait(l);
    --brPrimeraka;
}

void Biblioteka::vrati() {
    unique_lock<mutex> l(m);
    ++brPrimeraka;
    slobodno.notify_one();
}

void clan(Biblioteka &b, int rbr) {
    b.iznajmi();
    {
        unique_lock<mutex> terminal(term_m);
        cout << "clan br " << rbr << " uzeo knjigu" << endl;
    }
    this_thread::sleep_for(seconds(rand()%vremeCitanja + 1));
    {
        unique_lock<mutex> terminal(term_m);
        b.vrati();
        cout << "clan br " << rbr << " vratio knjigu" << endl;
    }
}

const int brPrimeraka = 5;
const int brClanova = 20;

int main()
{
    Biblioteka b(brPrimeraka);
    thread niti[brClanova];
    for(int i = 0; i < brClanova; i++)
        niti[i] = thread(clan,ref(b), i+1);
    for(int i = 0; i < brClanova; i++)
        niti[i].join();
    return 0;
}
