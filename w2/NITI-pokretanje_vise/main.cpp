/*
Napraviti konkurentni program koji pokrece vise niti u petlji.
Svakoj niti proslediti njen redni broj prilikom kreiranja i
svaka nit treba da ispise dati broj u okviru tela niti.
*/



#include <iostream>
#include <thread>
using namespace std;

void f(int a) {
    cout << a;
}

int main()
{
    int a[] = {1,2,3,4,5};
    int velicina = sizeof(a) / sizeof(int);
    thread niti[velicina];

    for(int i = 0; i < velicina; i++)
        niti[i] = thread(f,a[i]);

    for(int i = 0; i < velicina; i++)
        niti[i].join();

    return 0;
}
