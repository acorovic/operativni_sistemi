/*
    Napraviti konkurentni program koji pronalazi element najblizi 0 iz zadatog niza brojeva.
    Posao podeliti u 3 toka izvrsavanja.
    Niz treba da sadrzi 900000 elemenata. Elementi su pseudoslucajni brojevi dobijeni
    pozivom funkcije rand().

    Za svaki tok izvrsavanja, evidentirati trenutke kada je izvrsavanje pocelo i kada je zavrsilo.
    Na kraju programa, ispisati koliko je trajalo izvrsavanje svakog toka izvrsavanja.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;
using namespace chrono;

struct vreme{
    system_clock::time_point pocetak;
    system_clock::time_point kraj;};

void f(double *start, int brIteracija, double &najblizi, vreme &vremeIzvrsavanja) {
    najblizi = *start;
    vremeIzvrsavanja.pocetak = system_clock::now();
    for(int i = 1; i < brIteracija; i++)
        if(abs(najblizi) > abs(start[i]))
            najblizi = start[i];
    vremeIzvrsavanja.kraj = system_clock::now();

}

const int brNiti = 3;
const int brClanova = 90000;

int main()
{
    vector<vreme> vremenaIzvrsavanja(brNiti);
    thread niti[brNiti];
    double niz[brClanova];
    for(int i = 0; i < brClanova; i++)
        niz[i] = rand();
    double najblizi[brNiti];
    for(int i = 0; i < brNiti; i++)
        niti[i] = thread(f, niz + i*brClanova/brNiti, brClanova/brNiti, ref(najblizi[i]), ref(vremenaIzvrsavanja[i]));
    for(int i = 0; i < brNiti; i++)
        niti[i].join();
    for(int i = 0; i < brNiti; i++) {
        duration<double,milli> temp = vremenaIzvrsavanja[i].kraj - vremenaIzvrsavanja[i].pocetak;
        cout << "nit " << i+1 << " se izvrsavala " << temp.count() << " "<< najblizi[i] <<  endl;
    }
    return 0;
}
