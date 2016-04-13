/*
    Napraviti konkurentni program koji pronalazi element najblizi 0 iz zadatog niza brojeva.
    Niz treba da sadrzi 900000 elemenata. Elementi su pseudoslucajni brojevi dobijeni
    pozivom funkcije rand().
    Posao podeliti u 3 toka izvrsavanja.
    Potrebno je ispisati (u sekundama, milisekundama i minutima) trajanje izvrsavanja svakog od tokova izvrsavanja .
*/

#include <iostream>
#include <thread>
#include <cmath>

using namespace std;
using namespace chrono;

const int brElem = 9;
const int brNiti = 3;

void f(double *pocetak, int brIteracija, double &najblizi, int rbr) {
    double mini = *pocetak;
    system_clock::time_point pocetakVremena = system_clock::now();
    for(int i = 0; i < brIteracija; i++, pocetak++)
        if(abs(mini) > abs(*pocetak))
            mini = *pocetak;
    najblizi = mini;
    system_clock::time_point krajVremena = system_clock::now();
    duration<double> sec = krajVremena - pocetakVremena;
    cout << "nit " << rbr << "se izvrsavala " << sec.count() << endl;
    duration<double,milli> milisec = krajVremena - pocetakVremena;
    cout << "nit " << rbr << "se izvrsavala " << milisec.count() << endl;
    duration<double,ratio<60>> mins = krajVremena - pocetakVremena;
    cout << "nit " << rbr << "se izvrsavala " << mins.count() << endl;
    cout << "najblizi iz niti " << rbr << " " << najblizi << endl;
}

int main()
{
    double niz[brElem] = {1,2,3,4,5,6,7,0.01,12};
    //for(int i = 0; i < brElem; i++)
       // niz[i] = rand();
    thread niti[brNiti];
    double najblizi[brNiti];
    for(int i = 0; i < brNiti; i++)
        niti[i] = thread(f, niz + i*brElem/brNiti, brElem/brNiti, ref(najblizi[i]), i+1);
    for(int i = 0; i < brNiti; i++)
        niti[i].join();


    return 0;
}
