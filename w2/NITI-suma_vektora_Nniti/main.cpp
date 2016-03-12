/*Napraviti konkrentni program koji izraÄunava
sumu svih elemenata vektora, koristeÄ‡i funkciju f():

f(ci begin, ci end, double& zbir) pri cemu je
ci typedef vector<double>::const_iterator

Podeliti racunanje sume na N delova tako svaka nit sabira
duzina vektora/broj niti elemenata vektora.
*/

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

#define BROJ_NITI 4

typedef vector<double>::const_iterator ci;

void f(ci a_begin, ci a_end, double &zbir) {
    zbir = 0;
    for( ; a_begin != a_end; a_begin++)
        zbir += *a_begin;
}

int main()
{
    vector<double> a = {1,2,3,4,5,6,7,8,9};
    double zbir[BROJ_NITI];
    thread niz_niti[BROJ_NITI];
    ci a_begin = a.begin();
    ci a_end = a_begin + a.size()/BROJ_NITI;

    for(int i = 0; i < BROJ_NITI - 1; i++) {
        niz_niti[i] = thread(f, a_begin, a_end, ref(zbir[i]));
        a_begin = a_end;
        a_end = a_begin + a.size()/BROJ_NITI;
    }
   niz_niti[BROJ_NITI - 1] = thread(f, a_begin, a.end(), ref(zbir[BROJ_NITI - 1]));

    for(int i = 0; i < BROJ_NITI; i++)
        niz_niti[i].join();

    for(int i = 0; i < BROJ_NITI; i++)
        cout << "zbirovi " << zbir[i] << endl;
    return 0;
}
