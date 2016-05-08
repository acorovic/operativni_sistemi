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
#define BR_ELEM 9000
#define BR_NITI 3

using namespace std;
using namespace chrono;

void f(float *niz, float &najblizi, int i) {
    system_clock::time_point pocetak = system_clock::now();
    najblizi = niz[0];
    for(int i = 1; i < BR_ELEM/3; i++) {
        if(abs(najblizi) > abs(niz[i]))
            najblizi = niz[i];
    }
    system_clock::time_point kraj = system_clock::now();
    duration<double,milli> miliS = kraj - pocetak;
    cout << "nit " << i << "se izvrsavala " << miliS.count() << "milisekundi" << endl;
    duration<double> sec= kraj - pocetak;
    cout << "nit " << i << "se izvrsavala " << sec.count() << "sekundi" << endl;
    duration<double,ratio<60>> minutes = kraj - pocetak;
    cout << "nit " << i << "se izvrsavala " << minutes.count() << "minuta" << endl;
}

int main()
{
    float niz[BR_ELEM];
    float najblizi[BR_NITI];
    for(int i = 0; i < BR_ELEM; i++)
        niz[i] = rand();
    thread niti[BR_NITI];
    for(int i = 0; i < BR_NITI; i++)
        niti[i] = thread(f,niz + i*BR_ELEM/BR_NITI, ref(najblizi[i]),i+1);
    for(int i = 0; i < BR_NITI; i++)
        niti[i].join();
    float najbliziBR = najblizi[0];
    for(int i = 1; i < BR_NITI; i++)
        if(abs(najbliziBR) > abs(najblizi[i]))
            najbliziBR = najblizi[i];
    cout << najbliziBR << endl;

    return 0;
}
