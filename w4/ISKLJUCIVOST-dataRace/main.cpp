/*
Kreirati globalnu celobrojnu promeljivu brojaÄ. Nakon toga kreirati 2 funkcije
inkrement i dekrement koje poveÄ‡avaju i smanjuju dati brojaÄ ITERACIJA puta, pri Äemu je
 ITERACIJA konstanta koja predstavlja proizvoljno velik broj (npr. 100000000). Kreirati jednu nit od
  funkcije inkrement i jednu nit od funkcije dekrement.

Nakon zavrÅ¡atka rada niti ispisati vrednosti brojaÄa. Da li je data vrednost oÄekivana?
*/

#include <iostream>
#include <thread>

using namespace std;

const int ITERATIONS = 100000;

void inkrement(int &a) {
    for(int i = 0; i < ITERATIONS; i++)
        a++;
}

void dekrement(int &a) {
    for(int i = 0; i < ITERATIONS; i++)
        a--;
}



int main()
{
    int a = 0;
    thread nit1(inkrement, ref(a));
    thread nit2(dekrement, ref(a));
    nit1.join();
    nit2.join();

    cout << a << endl;
    return 0;
}
